/* imu.c — Viture XR glasses IMU source via vendor SDK */

#include "imu.h"

#include <libudev.h>
#include <stdbool.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>

#include "viture_device.h"
#include "viture_glasses_provider.h"

#define IMU_MODE_POSE   1
#define IMU_FREQ_240HZ  3   /* viture::protocol::imu::Frequency */

static XRDeviceProviderHandle s_provider = NULL;
static imu_pose_fn            s_pose_cb  = NULL;

static double monotonic_now(void) {
    struct timespec ts;
    clock_gettime(CLOCK_MONOTONIC, &ts);
    return (double)ts.tv_sec + ts.tv_nsec * 1e-9;
}

static void viture_pose_cb(float *pose, uint64_t ts_ns) {
    (void)ts_ns;
    if (!pose || !s_pose_cb) return;
    /* pose layout: [roll, pitch, yaw, qw, qx, qy, qz] (NWU) */
    s_pose_cb(pose[3], pose[4], pose[5], pose[6], monotonic_now());
}

static int find_product_id(void) {
    struct udev *udev = udev_new();
    if (!udev) return 0;

    struct udev_enumerate *en = udev_enumerate_new(udev);
    udev_enumerate_add_match_subsystem(en, "usb");
    udev_enumerate_add_match_property(en, "ID_VENDOR_ID", "35ca");
    udev_enumerate_scan_devices(en);

    int product_id = 0;
    struct udev_list_entry *devices = udev_enumerate_get_list_entry(en);
    struct udev_list_entry *entry;
    udev_list_entry_foreach(entry, devices) {
        struct udev_device *dev = udev_device_new_from_syspath(
            udev, udev_list_entry_get_name(entry));
        if (!dev) continue;
        const char *devtype = udev_device_get_devtype(dev);
        if (devtype && strcmp(devtype, "usb_device") == 0) {
            const char *pid_str = udev_device_get_property_value(dev, "ID_MODEL_ID");
            if (pid_str) {
                product_id = (int)strtol(pid_str, NULL, 16);
                udev_device_unref(dev);
                break;
            }
        }
        udev_device_unref(dev);
    }

    udev_enumerate_unref(en);
    udev_unref(udev);
    return product_id;
}

int imu_open(imu_pose_fn cb) {
    s_pose_cb = cb;

    int product_id = find_product_id();
    if (product_id == 0) {
        fprintf(stderr, "imu: no Viture device found (vendor 0x35ca)\n");
        return -1;
    }
    if (!xr_device_provider_is_product_id_valid(product_id)) {
        fprintf(stderr, "imu: product 0x%04x rejected by SDK\n", product_id);
        return -1;
    }
    fprintf(stderr, "imu: found device 0x%04x\n", product_id);

    s_provider = xr_device_provider_create(product_id);
    if (!s_provider) {
        fprintf(stderr, "imu: xr_device_provider_create failed\n");
        return -1;
    }

    if (register_pose_callback(s_provider, viture_pose_cb) != 0) {
        fprintf(stderr, "imu: register_pose_callback failed\n");
        goto fail;
    }
    if (xr_device_provider_initialize(s_provider, NULL) != 0) {
        fprintf(stderr, "imu: xr_device_provider_initialize failed\n");
        goto fail;
    }
    if (xr_device_provider_start(s_provider) != 0) {
        fprintf(stderr, "imu: xr_device_provider_start failed\n");
        goto fail;
    }
    if (open_imu(s_provider, IMU_MODE_POSE, IMU_FREQ_240HZ) != 0) {
        fprintf(stderr, "imu: open_imu failed\n");
        goto fail;
    }

    return 0;

fail:
    xr_device_provider_shutdown(s_provider);
    xr_device_provider_destroy(s_provider);
    s_provider = NULL;
    return -1;
}

void imu_close(void) {
    if (!s_provider) return;
    close_imu(s_provider, IMU_MODE_POSE);
    xr_device_provider_stop(s_provider);
    xr_device_provider_shutdown(s_provider);
    xr_device_provider_destroy(s_provider);
    s_provider = NULL;
}
