/* main.c — XR IMU daemon entry point
 * Reads head orientation quaternions from Viture XR Pro via USB HID
 * and writes them to a shared memory file for the GNOME extension to consume.
 */

#include <stdio.h>
#include <stdlib.h>
#include <signal.h>

/* TODO Phase 1: include imu.h, shm.h */

static volatile int running = 1;

static void handle_signal(int sig) {
    (void)sig;
    running = 0;
}

int main(void) {
    signal(SIGINT,  handle_signal);
    signal(SIGTERM, handle_signal);

    fprintf(stderr, "xr-imu-daemon: stub — Phase 1 not yet implemented\n");

    /* TODO Phase 1:
     *   shm_init();
     *   imu_open();
     *   while (running) { imu_read(&q); shm_write(&q); }
     *   imu_close();
     *   shm_close();
     */

    return 0;
}
