/* gen_video_y4m.c — generates tst/video.y4m
 * 10x10 pixels, 5s, 10fps, bouncing ball
 * Ball path: top-left -> bottom-middle -> top-right
 */

#include <stdio.h>
#include <string.h>
#include <math.h>

#define W      10
#define H      10
#define FPS    10
#define SECS   5
#define FRAMES (FPS * SECS)

int main (void) {
    FILE* fp = fopen("tst/video.y4m", "wb");
    if (!fp) {
        fprintf(stderr, "Could not create tst/video.y4m\n");
        return 1;
    }

    /* Y4M file header */
    fprintf(fp, "YUV4MPEG2 W%d H%d F%d:1 Ip A1:1 C420\n",
        W, H, FPS);

    unsigned char y_plane[W * H];
    unsigned char u_plane[(W/2) * (H/2)];
    unsigned char v_plane[(W/2) * (H/2)];

    for (int f = 0; f < FRAMES; f++) {
        /* Ball position:
         *   x: linear from 0 to W-1
         *   y: parabola 0 -> H-1 -> 0
         */
        double t = (double)f / (FRAMES - 1);
        int bx = (int)round((W - 1) * t);
        int by = (int)round((H - 1) * (1.0 - pow(2.0*t - 1.0, 2)));

        /* Black background */
        memset(y_plane, 0, sizeof(y_plane));
        memset(u_plane, 128, sizeof(u_plane));
        memset(v_plane, 128, sizeof(v_plane));

        /* White ball (single pixel) */
        y_plane[by * W + bx] = 255;

        /* Frame marker */
        fprintf(fp, "FRAME\n");

        /* Y, U, V planes */
        fwrite(y_plane, 1, W * H, fp);
        fwrite(u_plane, 1, (W/2) * (H/2), fp);
        fwrite(v_plane, 1, (W/2) * (H/2), fp);
    }

    fclose(fp);
    printf("Generated tst/video.y4m: %dx%d, %dfps, "
        "%d frames (%ds)\n", W, H, FPS, FRAMES, SECS);
    return 0;
}
