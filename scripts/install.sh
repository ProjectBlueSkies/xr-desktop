#!/usr/bin/env bash
# install.sh — install extension and daemon

set -e
SCRIPT_DIR="$(cd "$(dirname "${BASH_SOURCE[0]}")" && pwd)"
ROOT="$SCRIPT_DIR/.."
EXT_UUID="xr-desktop@projectblueskies"
EXT_DIR="$HOME/.local/share/gnome-shell/extensions/$EXT_UUID"

echo "==> Installing GNOME extension..."
mkdir -p "$EXT_DIR"
cp "$ROOT"/extension/{metadata.json,extension.js,prefs.js} "$EXT_DIR/"
mkdir -p "$EXT_DIR/schemas"
cp "$ROOT/extension/schemas/"*.xml "$EXT_DIR/schemas/"
glib-compile-schemas "$EXT_DIR/schemas/"
echo "    Installed to $EXT_DIR"
echo "    Enable with: gnome-extensions enable $EXT_UUID"

echo "==> Building daemon..."
cmake -S "$ROOT/daemon" -B "$ROOT/daemon/build" -DCMAKE_BUILD_TYPE=Release
cmake --build "$ROOT/daemon/build" --parallel
echo "    Binary: $ROOT/daemon/build/xr-imu-daemon"

echo "==> Done. Log out and back in to activate the extension on Wayland."
