
#include "Global.h"

#include "StickyWidget.h"

SettingsHelper* mSettingsHelper = nullptr;
DisplayHelper* mDisplayHelper = nullptr;
XHelper* mXHelper = nullptr;
XftFont* mFont = nullptr;

bool mWaitingForUnClick = false;
Clock::time_point mClickStart{};
Clock::time_point mClickEnd{};

const double MAIN_THREAD_MS = 0.55;


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
 * transparency), and has a transparent visual background.
 *
 * While "Stuck" to the desktop, the widgets window
 * button in the panels Task manager is removed.
 *
 */
int main(int argc, char** argv) {
    // Some Qt6 setup.
    QCoreApplication::setOrganizationName(ORG_NAME);
    QCoreApplication::setApplicationName(APP_NAME);
    QCoreApplication app(argc, argv);

    // Init fl_display.
    fl_open_display();

    // Init helpers.
    mSettingsHelper = new SettingsHelper(APP_NAME);

    mDisplayHelper = new DisplayHelper(fl_display);
    if (mDisplayHelper->getDisplay() == nullptr) {
        cout << endl << XCOLOR_RED << "StickyWidget: X11 "
            "Windows are unavailable with this Desktop - "
            "FATAL." << XCOLOR_NORMAL << endl;
        return true;
    }

    mXHelper = new XHelper();

    // Set X Error handler (quiets non-errors).
    XSetErrorHandler(mXHelper->handleX11ErrorEvent);

    // Set font for layouts.
    mFont = XftFontOpenName(fl_display, DefaultScreen(
        fl_display), "Sans-16:bold");

    // Ensure we have a compositor running.
    if (!mXHelper->isACompositorRunning()) {
        cout << endl << XCOLOR_RED << "StickyWidget: A "
            "Compositor isn't running, widgets lose "
            "transparency - FATAL." << XCOLOR_NORMAL << endl;
        return true;
    }

    // Enable the compositor has true, per-pixel,
    // alpha-blended transparency.
    XVisualInfo vinfo;
    if (XMatchVisualInfo(fl_display, fl_screen, 32,
        TrueColor, &vinfo)) {
        fl_visual = &vinfo;
        fl_colormap = XCreateColormap(fl_display,
            RootWindow(fl_display, fl_screen), vinfo.visual,
                AllocNone);
    } else {
        cout << endl << XCOLOR_RED << "StickyWidget: A "
            "Compositor is running, but is incapable of "
            "transparency - FATAL." << XCOLOR_NORMAL << endl;
        return true;
    }

    // Create initial window & set border state.
    StickyWindow* canvas = new StickyWindow(
        mSettingsHelper->getWindowXPos(),
        mSettingsHelper->getWindowYPos(),
        mSettingsHelper->getWindowWidth(),
        mSettingsHelper->getWindowHeight(), APP_NAME);
    canvas->border(mSettingsHelper->
        getWindowBorderWidth());
    canvas->end();
    canvas->show();

    // Start mainThread timer & done.
    Fl::add_timeout(MAIN_THREAD_MS, mainThread, canvas);
    return Fl::run();
}

/**
 * Timer callback to toggle Window Visible, if required
 * due to user right-clicking.
 */
void mainThread(void* data) {
    StickyWindow* canvas = static_cast<StickyWindow*> (data);
    const Window THIS_X_WINDOW(fl_xid(canvas));

    // If canvas not-right clicked, exit.
    Window root, child;
    int rootX, rootY, winX, winY;
    unsigned int mask;

    if (!XQueryPointer(fl_display, THIS_X_WINDOW, &root,
        &child, &rootX, &rootY, &winX, &winY, &mask)) {
        mWaitingForUnClick = false;
        Fl::repeat_timeout(MAIN_THREAD_MS, mainThread, data);
        return;
    }

    // Get window hovered state.
    const bool IS_WINDOW_HOVERED = mXHelper->isWindowHovered(
        THIS_X_WINDOW, QPoint(rootX, rootY));
    canvas->setSettingsButtonVisibility(IS_WINDOW_HOVERED);

    // Restart mainThread timer & done.
    mClickStart = Clock::now();
    mWaitingForUnClick = true;

    Fl::repeat_timeout(MAIN_THREAD_MS, mainThread, data);
}
