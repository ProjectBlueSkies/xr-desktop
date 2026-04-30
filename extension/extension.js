// extension.js — XR Virtual Desktop GNOME Shell extension
// Phase 2: reads head orientation from the IMU daemon's shared memory and
// world-locks the window group so windows appear fixed in space as you move.

import GLib from 'gi://GLib';
import * as Main from 'resource:///org/gnome/shell/ui/main.js';

// Shared memory layout — mirrors shared/ipc.h
const SHM_PATH       = '/dev/shm/xr-desktop';
const OFF_SEQ        = 0;
const OFF_QW         = 16;
const OFF_QX         = 20;
const OFF_QY         = 24;
const OFF_QZ         = 28;
const SHM_MIN_BYTES  = 32;

const POLL_MS        = 16;    // ~60 Hz
const PIXELS_PER_RAD = 2400;  // screen pixels per radian — tune to taste

export default class XRDesktopExtension {
    enable() {
        this._refQ = null;
        this._pollId = GLib.timeout_add(GLib.PRIORITY_DEFAULT, POLL_MS, () => {
            this._tick();
            return GLib.SOURCE_CONTINUE;
        });
        console.log('[xr-desktop] enabled');
    }

    disable() {
        if (this._pollId) {
            GLib.source_remove(this._pollId);
            this._pollId = null;
        }
        global.window_group.set_position(0, 0);
        console.log('[xr-desktop] disabled');
    }

    // ── IPC ────────────────────────────────────────────────────────────────

    _readShm() {
        try {
            const [ok, raw] = GLib.file_get_contents(SHM_PATH);
            if (!ok || raw.byteLength < SHM_MIN_BYTES) return null;

            const dv = new DataView(raw.buffer, raw.byteOffset, raw.byteLength);
            const seq = dv.getUint32(OFF_SEQ, true);
            if (seq === 0 || (seq & 1)) return null; // not yet written / write in progress

            const q = {
                w: dv.getFloat32(OFF_QW, true),
                x: dv.getFloat32(OFF_QX, true),
                y: dv.getFloat32(OFF_QY, true),
                z: dv.getFloat32(OFF_QZ, true),
            };

            if (dv.getUint32(OFF_SEQ, true) !== seq) return null; // torn read
            return q;
        } catch (_) {
            return null;
        }
    }

    // ── Quaternion math ────────────────────────────────────────────────────

    _conj(q) {
        return { w: q.w, x: -q.x, y: -q.y, z: -q.z };
    }

    _mul(a, b) {
        return {
            w: a.w*b.w - a.x*b.x - a.y*b.y - a.z*b.z,
            x: a.w*b.x + a.x*b.w + a.y*b.z - a.z*b.y,
            y: a.w*b.y - a.x*b.z + a.y*b.w + a.z*b.x,
            z: a.w*b.z + a.x*b.y - a.y*b.x + a.z*b.w,
        };
    }

    // Extract yaw (Z) and pitch (Y) from a relative quaternion (NWU, ZXY order)
    _yawPitch(rel) {
        const yaw = Math.atan2(
            2 * (rel.w * rel.z + rel.x * rel.y),
            1 - 2 * (rel.y * rel.y + rel.z * rel.z)
        );
        const sinP = Math.max(-1, Math.min(1, 2 * (rel.w * rel.y - rel.z * rel.x)));
        const pitch = Math.asin(sinP);
        return { yaw, pitch };
    }

    // ── Per-frame update ───────────────────────────────────────────────────

    _tick() {
        const q = this._readShm();
        if (!q) return;

        // Capture reference on first valid frame
        if (!this._refQ) {
            this._refQ = q;
            return;
        }

        const rel = this._mul(this._conj(this._refQ), q);
        const { yaw, pitch } = this._yawPitch(rel);

        // Head turns right  → windows shift left  (negative x)
        // Head tilts up     → windows shift down  (positive y)
        global.window_group.set_position(
            -yaw   * PIXELS_PER_RAD,
             pitch * PIXELS_PER_RAD
        );
    }
}
