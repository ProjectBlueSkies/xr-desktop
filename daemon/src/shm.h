#pragma once

int  shm_init(void);
void shm_write(float qw, float qx, float qy, float qz, double timestamp);
void shm_close(void);
