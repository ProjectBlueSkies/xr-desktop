/* shm.c — shared memory writer (Phase 1 stub)
 * Writes IMU data to /dev/shm/xr-desktop matching the layout in shared/ipc.h
 */

#include <stdio.h>

/* TODO Phase 1: mmap /dev/shm/xr-desktop, write quaternion + timestamp. */

void shm_init(void)  { fprintf(stderr, "shm_init: stub\n"); }
void shm_close(void) { fprintf(stderr, "shm_close: stub\n"); }
