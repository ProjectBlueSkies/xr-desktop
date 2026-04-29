// extension.js — XR Virtual Desktop GNOME Shell extension
// Phase 2 stub: registers enable/disable lifecycle only.

import * as Main from 'resource:///org/gnome/shell/ui/main.js';

export default class XRDesktopExtension {
    enable() {
        // TODO Phase 2: create Clutter actor, attach repaint callback
        // TODO Phase 3: read shared memory, push quaternion to shader
        console.log('[xr-desktop] enabled (stub)');
    }

    disable() {
        // TODO Phase 2: destroy actor, disconnect signals
        console.log('[xr-desktop] disabled');
    }
}
