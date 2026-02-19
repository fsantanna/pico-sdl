# Plan: select

## Description

Make per-frame video I/O non-blocking so that pipe/stream sources
(e.g., ffmpeg stdout) don't stall the render loop. Use `select()`
on the file descriptor. API stays transparent — show last frame if
new data isn't ready.

## Interview Summary

1. **Scope**: Per-frame only (`fread`/`fseek` in hot path)
2. **Motivation**: Pipe/network source (ffmpeg stdout)
3. **Approach**: `select()`/`poll()` on fd
4. **API**: Transparent — same signatures, show last frame
5. **Buffering**: No read-ahead, just non-blocking current reads

## Analysis

### Blocking calls in hot path (`src/video.h`)

| Call | Lines | Function | Notes |
|------|-------|----------|-------|
| `fseek` | 280, 292 | `pico_set_video` | Seeks to frame offset — **fails on pipes** |
| `fread` x4 | 81, 87-91 | `_y4m_read_frame` | Reads marker + Y/U/V planes — **blocks on pipes** |

### Key constraint: pipes are sequential

- `fseek`/`ftell` don't work on pipes
- Data arrives in order, one frame at a time
- We can only read forward, never backward
- A frame may arrive partially (need to accumulate)

## Plan

### Step 1: Detect pipe vs file

In `_pico_layer_video()`, after `fopen`, detect whether `fp` is a
pipe or a regular file:

```c
#include <sys/stat.h>

int fd = fileno(fp);
struct stat st;
fstat(fd, &st);
int is_pipe = S_ISFIFO(st.st_mode) || !S_ISREG(st.st_mode);
```

Add `is_pipe` field to `Pico_Layer_Video`.

If pipe: skip `ftell`/`fseek` for total-frame calculation (set
`frame.total = -1` meaning unknown).

### Step 2: Add partial-read buffer to `Pico_Layer_Video`

For pipe sources, `fread` may return fewer bytes than requested.
We need to accumulate a full frame before processing:

```c
struct {
    unsigned char* buf;   // frame-sized buffer (6 + Y + U + V)
    int            have;  // bytes accumulated so far
    int            need;  // total bytes for one frame
} partial;
```

This is not read-ahead buffering — it's just handling the fact that
a single frame may arrive across multiple `select()`-ready events.

### Step 3: Non-blocking read function

New function `_y4m_read_frame_nb()` for pipe sources:

```c
static int _y4m_read_frame_nb (Pico_Layer_Video* vs) {
    int fd = fileno(vs->fp);

    // Check if data is available
    fd_set rfds;
    struct timeval tv = {0, 0};  // zero timeout = poll
    FD_ZERO(&rfds);
    FD_SET(fd, &rfds);
    if (select(fd + 1, &rfds, NULL, NULL, &tv) <= 0) {
        return 0;  // no data available
    }

    // Read as much as available (non-blocking portion)
    int remaining = vs->partial.need - vs->partial.have;
    ssize_t n = read(fd,
        vs->partial.buf + vs->partial.have,
        remaining);
    if (n <= 0) {
        return (n == 0) ? -1 : 0;  // EOF or error
    }
    vs->partial.have += n;

    // Full frame?
    if (vs->partial.have < vs->partial.need) {
        return 0;  // still incomplete
    }

    // Parse complete frame from buffer
    unsigned char* p = vs->partial.buf;
    if (strncmp((char*)p, "FRAME\n", 6) != 0) {
        return -1;  // bad marker
    }
    p += 6;
    memcpy(vs->plane.y, p, vs->size.y);   p += vs->size.y;
    memcpy(vs->plane.u, p, vs->size.uv);  p += vs->size.uv;
    memcpy(vs->plane.v, p, vs->size.uv);

    vs->partial.have = 0;  // reset for next frame
    vs->frame.cur++;
    return 1;  // frame ready
}
```

### Step 4: Modify `pico_set_video` for pipe sources

When `vs->is_pipe`:
- **Ignore `frame` parameter** for seeking (can't seek pipes)
- Call `_y4m_read_frame_nb()` to try reading the next frame
- If returns 0 (not ready): keep `frame.cur` unchanged, return 1
  (last frame is still valid — transparent API)
- If returns 1 (frame ready): update texture, advance `frame.cur`
- If returns -1 (EOF/error): set `frame.done = 1`, return 0

When `!vs->is_pipe`: existing behavior unchanged (blocking
`fseek` + `fread`).

### Step 5: Modify `_pico_layer_video` init for pipes

For pipe sources:
- Parse header with `fgets` (still blocking — one-time, small read)
- Skip total-frame calculation (`fseek`/`ftell`), set
  `frame.total = -1`
- Allocate `partial.buf` = `malloc(vs->size.frame)`
- Set `partial.have = 0`, `partial.need = vs->size.frame`

### Step 6: Cleanup

In `_pico_hash_clean_video`: also `free(vs->partial.buf)` if
allocated.

## Files to modify

- `src/video.h` — all changes above (steps 1-6)

## Status

- [x] Interview / requirements gathering
- [x] Analysis of blocking calls in video code
- [x] Plan for non-blocking approach
- [ ] Implementation
