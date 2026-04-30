#!/bin/bash
SCRIPT_DIR="$(cd "$(dirname "${BASH_SOURCE[0]}")" && pwd)"
SDK_LIBS="$SCRIPT_DIR/../daemon/upstream/lib/x86_64/viture"
BINARY="$SCRIPT_DIR/../daemon/build/xr-imu-daemon"

export LD_LIBRARY_PATH="$SDK_LIBS${LD_LIBRARY_PATH:+:$LD_LIBRARY_PATH}"
exec "$BINARY" "$@"
