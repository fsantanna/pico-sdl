#include "pico.h"

int main (void) {
    pico_init(1);

    // Test 1: filter matches within timeout
    // Move the mouse during the 5s window
    puts("TEST 1: move mouse within 5s");
    puts("  filter=MOUSE_MOTION, timeout=5000");
    {
        Pico_Event e;
        int ok = pico_input_event_timeout(
            &e, PICO_EVENT_MOUSE_MOTION, 5000
        );
        if (ok) {
            printf("  OK: got mouse motion\n");
        } else {
            printf("  FAIL: timed out\n");
        }
    }

    // Test 2: filter does NOT match within timeout
    // Do NOT press any key during the 3s window
    puts("TEST 2: do NOT press keys for 3s");
    puts("  filter=KEY_DOWN, timeout=3000");
    puts("  (move mouse to trigger non-matching events)");
    {
        Pico_Event e;
        int ok = pico_input_event_timeout(
            &e, PICO_EVENT_KEY_DOWN, 3000
        );
        if (ok) {
            printf("  FAIL: should have timed out\n");
        } else {
            printf("  OK: timed out correctly\n");
        }
    }

    // Test 3: filter match after non-matching events
    // Move mouse first, THEN press a key within 5s
    puts("TEST 3: move mouse, then press key within 5s");
    puts("  filter=KEY_DOWN, timeout=5000");
    puts("  (this is the actual bug scenario)");
    {
        Pico_Event e;
        int ok = pico_input_event_timeout(
            &e, PICO_EVENT_KEY_DOWN, 5000
        );
        if (ok) {
            printf("  OK: got key down\n");
        } else {
            printf("  FAIL: timed out (bug!)\n");
        }
    }

    pico_init(0);
    return 0;
}
