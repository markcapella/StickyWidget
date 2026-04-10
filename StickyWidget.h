
#pragma once

/**
 * StickyWidget is an X-based prototype for windows that
 * act like KDE Plasmoids.
 *
 * Desktop widgets with various views can be stuck in
 * place to the desktop below other windows.
 *
 * Widget remembers position, size, settings etc.
 *
 * Cursor hover reveals SettingsButton that toggles widget
 * in & out of "Stuck" state.
 *
 * When stuck to the desktop, widget ignores clicks &
 * passes all mouse actions to desktop (input
 * transparency), and has a transparent visual
 * background.
 *
 * While "Stuck" to the desktop, the widgets window
 * button in the panels Task manager is removed.
 *
 */
extern SettingsHelper* mSettingsHelper;

extern DisplayHelper* mDisplayHelper;

extern XHelper* mXHelper;

extern XftFont* mFont;
