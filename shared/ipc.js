// ipc.js — JS mirror of shared/ipc.h for the GNOME extension.
// Reads the mmap'd file using a DataView over a Uint8Array.

const SHM_PATH = '/dev/shm/xr-desktop';

// Byte offsets matching struct xr_ipc (x86-64, no padding surprises)
const OFF_SEQ       = 0;   // uint32
const OFF_TIMESTAMP = 8;   // double  (8-byte aligned)
const OFF_QW        = 16;  // float
const OFF_QX        = 20;  // float
const OFF_QY        = 24;  // float
const OFF_QZ        = 28;  // float
const OFF_FLAGS     = 32;  // uint32
const SHM_SIZE      = 40;

export { SHM_PATH, SHM_SIZE, OFF_SEQ, OFF_TIMESTAMP, OFF_QW, OFF_QX, OFF_QY, OFF_QZ, OFF_FLAGS };
