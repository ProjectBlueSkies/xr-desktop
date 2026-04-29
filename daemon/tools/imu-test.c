/* imu-test.c — read the xr-desktop shared memory and print live quaternion data
 * Usage: run xr-imu-daemon first, then run this in a second terminal.
 */

#include <fcntl.h>
#include <stdatomic.h>
#include <stdint.h>
#include <stdio.h>
#include <sys/mman.h>
#include <time.h>
#include <unistd.h>

#include "ipc.h"

int main(void) {
    int fd = shm_open(XR_SHM_PATH, O_RDONLY, 0);
    if (fd < 0) {
        perror("shm_open — is xr-imu-daemon running?");
        return 1;
    }

    struct xr_ipc *p = mmap(NULL, XR_SHM_SIZE, PROT_READ, MAP_SHARED, fd, 0);
    close(fd);
    if (p == MAP_FAILED) { perror("mmap"); return 1; }

    fprintf(stderr, "imu-test: connected to %s\n", XR_SHM_PATH);

    uint32_t last_seq = 0;
    struct timespec sleep_ts = { .tv_sec = 0, .tv_nsec = 1000000 }; /* 1ms */

    while (1) {
        uint32_t seq = atomic_load_explicit(
            (_Atomic uint32_t *)&p->seq, memory_order_acquire);

        /* skip if write in progress (odd) or unchanged */
        if (seq == last_seq || (seq & 1)) {
            nanosleep(&sleep_ts, NULL);
            continue;
        }

        float qw = p->qw, qx = p->qx, qy = p->qy, qz = p->qz;
        double ts = p->timestamp;

        /* re-read seq to detect torn read */
        uint32_t seq2 = atomic_load_explicit(
            (_Atomic uint32_t *)&p->seq, memory_order_acquire);
        if (seq != seq2) {
            nanosleep(&sleep_ts, NULL);
            continue;
        }

        printf("ts=%10.3f  qw=%+.4f  qx=%+.4f  qy=%+.4f  qz=%+.4f\n",
               ts, qw, qx, qy, qz);
        fflush(stdout);
        last_seq = seq;

        nanosleep(&sleep_ts, NULL);
    }

    return 0;
}
