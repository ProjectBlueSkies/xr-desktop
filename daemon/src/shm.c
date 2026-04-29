/* shm.c — POSIX shared memory writer for xr-desktop IPC */

#include "shm.h"
#include "ipc.h"

#include <fcntl.h>
#include <stdatomic.h>
#include <stdint.h>
#include <stdio.h>
#include <string.h>
#include <sys/mman.h>
#include <unistd.h>

static struct xr_ipc *s_shm = MAP_FAILED;
static int            s_fd  = -1;

int shm_init(void) {
    s_fd = shm_open(XR_SHM_PATH, O_CREAT | O_RDWR, 0666);
    if (s_fd < 0) { perror("shm_open"); return -1; }
    if (ftruncate(s_fd, (off_t)XR_SHM_SIZE) != 0) { perror("ftruncate"); return -1; }
    s_shm = mmap(NULL, XR_SHM_SIZE, PROT_READ | PROT_WRITE, MAP_SHARED, s_fd, 0);
    if (s_shm == MAP_FAILED) { perror("mmap"); return -1; }
    memset(s_shm, 0, XR_SHM_SIZE);
    return 0;
}

void shm_write(float qw, float qx, float qy, float qz, double timestamp) {
    _Atomic uint32_t *seq_ptr = (_Atomic uint32_t *)&s_shm->seq;

    /* begin write: bump seq to odd */
    uint32_t seq = atomic_load_explicit(seq_ptr, memory_order_relaxed);
    atomic_store_explicit(seq_ptr, seq + 1, memory_order_release);

    s_shm->qw        = qw;
    s_shm->qx        = qx;
    s_shm->qy        = qy;
    s_shm->qz        = qz;
    s_shm->timestamp = timestamp;

    /* end write: bump seq to even */
    atomic_store_explicit(seq_ptr, seq + 2, memory_order_release);
}

void shm_close(void) {
    if (s_shm != MAP_FAILED) { munmap(s_shm, XR_SHM_SIZE); s_shm = MAP_FAILED; }
    if (s_fd >= 0) { close(s_fd); s_fd = -1; }
    shm_unlink(XR_SHM_PATH);
}
