#!/usr/bin/env bash
# dev-reload.sh — copy extension files to ~/.local and prompt for re-login
# On Wayland, GNOME Shell cannot be restarted in-place; a logout/login is required.

set -e
EXT_UUID="xr-desktop@projectblueskies"
EXT_DIR="$HOME/.local/share/gnome-shell/extensions/$EXT_UUID"
ROOT="$(cd "$(dirname "${BASH_SOURCE[0]}")/.." && pwd)"

cp "$ROOT"/extension/{metadata.json,extension.js,prefs.js} "$EXT_DIR/"
cp "$ROOT/extension/schemas/"*.xml "$EXT_DIR/schemas/"
glib-compile-schemas "$EXT_DIR/schemas/"

echo "Extension files updated."
echo "Log out and back in to reload (Wayland limitation)."
