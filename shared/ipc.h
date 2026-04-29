#pragma once
/* ipc.h — shared memory layout between daemon and GNOME extension.
 * The JS side mirrors this struct manually in shared/ipc.js.
 *
 * Lock-free single-writer (daemon) / single-reader (extension).
 * Writer increments seq before and after writing; reader retries if seq is odd
 * (write in progress) or changes between two reads (torn read).
 */

#include <stdint.h>

#define XR_SHM_PATH  "/xr-desktop"
#define XR_SHM_SIZE  sizeof(struct xr_ipc)

struct xr_ipc {
    uint32_t seq;           /* sequence number — odd means write in progress */
    uint32_t _pad;
    double   timestamp;     /* monotonic seconds */
    float    qw, qx, qy, qz; /* orientation quaternion (unit) */
    uint32_t flags;         /* reserved */
    uint32_t _pad2;
};
