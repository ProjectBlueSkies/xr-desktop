// prefs.js — Settings dialog (Phase 5 stub)

import Adw from 'gi://Adw';

export default class XRDesktopPrefs {
    fillPreferencesWindow(window) {
        const page = new Adw.PreferencesPage();
        const group = new Adw.PreferencesGroup({ title: 'XR Virtual Desktop' });
        group.add(new Adw.ActionRow({ title: 'Settings coming in Phase 5' }));
        page.add(group);
        window.add(page);
    }
}
