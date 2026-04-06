
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

const char* ICON_PNGFILE = "StickyWidget.png";
const char* ICON_PNGPATH =
    "/usr/share/icons/hicolor/64x64/apps/";


SettingsHelper* mSettingsHelper = nullptr;
DisplayHelper* mDisplayHelper = nullptr;
XHelper* mXHelper = nullptr;


bool mWaitingForUnClick = false;
Clock::time_point mClickStart{};
Clock::time_point mClickEnd{};


const double MAIN_THREAD_MS = 0.55;
