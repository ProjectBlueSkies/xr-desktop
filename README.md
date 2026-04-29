# xr-desktop

Open-source, subscription-free virtual desktop for Viture XR Pro glasses on Linux.

Head-tracked virtual monitors that float in 3D space, anchored to the world rather than your head. Built independently — not a fork of Breezy Desktop.

## Requirements

- Ubuntu 24.04 LTS
- GNOME 46 on **Wayland** (not Xorg)
- Viture XR Pro glasses (USB-C DisplayPort alt-mode)

## Architecture

```
[Viture glasses] → [IMU daemon] → [/dev/shm/xr-desktop] → [GNOME extension] → [display]
   (hardware)        (C, daemon/)      (shared/ipc.h)        (GJS, extension/)
```

```
xr-desktop/
├── daemon/          C IMU daemon — reads quaternions from glasses via USB HID
├── extension/       GNOME Shell extension — reads shm, applies world-lock shader
├── shaders/         GLSL vertex + fragment shaders
├── shared/          IPC struct definition (C header + JS mirror)
├── docs/
└── scripts/         install, dev-reload helpers
```

## Build & Install

```bash
# Install dependencies
sudo apt install -y build-essential cmake pkg-config \
  libusb-1.0-0-dev libhidapi-dev libudev-dev libevdev-dev \
  libjson-c-dev gjs gettext glslang-tools

# Install udev rules (gives your user HID access without sudo)
sudo cp daemon/udev/60-xr-glasses.rules /etc/udev/rules.d/
sudo udevadm control --reload-rules && sudo udevadm trigger

# Build + install extension
bash scripts/install.sh
```

## Development

```bash
# After editing extension files, copy and re-login
bash scripts/dev-reload.sh
```

## Phased Plan

| Phase | Goal | Status |
|-------|------|--------|
| 0 | Pre-flight: Wayland + deps verified | ✅ |
| 1 | IMU daemon — quaternions at 100Hz | 🔲 |
| 2 | Extension scaffold — hardcoded transform | 🔲 |
| 3 | World-locked screen — live quaternion → shader | 🔲 |
| 4 | Virtual displays via Mutter headless API | 🔲 |
| 5 | Polish — settings UI, curved shader, SBS 3D | 🔲 |

## License

MIT — see [LICENSE](LICENSE). Daemon derived from [XRLinuxDriver](https://github.com/wheaney/XRLinuxDriver) (also MIT).
