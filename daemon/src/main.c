/* main.c — XR IMU daemon
 * Reads head orientation from Viture XR glasses and writes to shared memory
 * for the GNOME extension to consume.
 */

#include <signal.h>
#include <stdio.h>
#include <time.h>

#include "imu.h"
#include "shm.h"

static volatile sig_atomic_t g_running = 1;

static void handle_signal(int sig) { (void)sig; g_running = 0; }

static void on_pose(float qw, float qx, float qy, float qz, double timestamp) {
    shm_write(qw, qx, qy, qz, timestamp);
}

int main(void) {
    signal(SIGINT,  handle_signal);
    signal(SIGTERM, handle_signal);

    if (shm_init() != 0) {
        fprintf(stderr, "xr-daemon: shm_init failed\n");
        return 1;
    }

    if (imu_open(on_pose) != 0) {
        fprintf(stderr, "xr-daemon: failed to open IMU — is the device connected?\n");
        shm_close();
        return 1;
    }

    fprintf(stderr, "xr-daemon: running (Ctrl-C to stop)\n");
    struct timespec ts = { .tv_sec = 0, .tv_nsec = 10000000 }; /* 10ms poll */
    while (g_running)
        nanosleep(&ts, NULL);

    imu_close();
    shm_close();
    fprintf(stderr, "xr-daemon: stopped\n");
    return 0;
}
