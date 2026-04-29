#pragma once

typedef void (*imu_pose_fn)(float qw, float qx, float qy, float qz, double timestamp);

int  imu_open(imu_pose_fn cb);
void imu_close(void);
