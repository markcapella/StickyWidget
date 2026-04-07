
#include "Global.h"

/**
 * XHelper provides common X related methods.
 *
 */
XHelper::XHelper(Display* displayHelper) {

    mDisplay = displayHelper;
}

/**
 * This method traps and handles X11 errors.
 */
int
XHelper::handleX11ErrorEvent(Display* display,
    XErrorEvent* event) {
    if (event->error_code == BadWindow ||
        event->error_code == BadAccess ||
        event->error_code == BadMatch) {
        return 0;
    }

    // Print the error message of the event.
    const int MAX_MESSAGE_BUFFER_LENGTH = 4096;
    char msg[MAX_MESSAGE_BUFFER_LENGTH];
    XGetErrorText(display, event->error_code, msg,
        sizeof(msg));

    printf("%sfltkWidget5: handleX11ErrorEvent() %s.%s\n",
        XCOLOR_RED, msg, XCOLOR_NORMAL);
    return 1;
}

/**
 * This method determines if a compositor is running.
 */
bool
XHelper::isACompositorRunning() {
    return XGetSelectionOwner(fl_display, XInternAtom(
        fl_display, "_NET_WM_CM_S0", False)) != None;
}

/**
 * This method checks if the desktop is currently
 * being shown, (which hides all windows).
 */
bool
XHelper::isDesktopShowing() {
    Atom type; int format;
    unsigned long nItems, unusedBytes;
    unsigned char* properties = nullptr;

    if (XGetWindowProperty(mDisplay, DefaultRootWindow(
        mDisplay), XInternAtom(mDisplay,
        "_NET_SHOWING_DESKTOP", False), 0, (~0L), False,
        AnyPropertyType, &type, &format, &nItems,
        &unusedBytes, &properties) == Success &&
        format == 32 && nItems > 0 &&
        (*(long*) (void*) properties == 1)) {
        XFree(properties);
        return true;
    }
    XFree(properties);

    return false;
}

/**
 * This method returns the number of the current workspace,
 * where the OS allows multiple / virtual workspaces.
 * result == -1 is all windows are visible.
 */
long
XHelper::getVisibleWorkspace() {
    Atom type; int format;
    unsigned long nItems, unusedBytes;
    unsigned char* properties = nullptr;

    if (XGetWindowProperty(mDisplay, DefaultRootWindow(
        mDisplay), XInternAtom(mDisplay, "_NET_CURRENT_DESKTOP",
        False), 0, 1, False, AnyPropertyType, &type, &format,
        &nItems, &unusedBytes, &properties) == Success &&
        type == XA_CARDINAL) {
        const long result = *(long*) (void*) properties;
        XFree(properties);
        return result;
    }
    XFree(properties);

    if (XGetWindowProperty(mDisplay, DefaultRootWindow(
        mDisplay), XInternAtom(mDisplay, "_WIN_WORKSPACE",
        False), 0, 1, False, AnyPropertyType, &type, &format,
        &nItems, &unusedBytes, &properties) == Success &&
        type == XA_CARDINAL) {
        const long result = *(long*) (void*) properties;
        XFree(properties);
        return result;
    }
    XFree(properties);

    return -1;
}

/**
 * Method returns a a list of active X11 windows
 * in stacking order.
 */
vector<Window>
XHelper::getWindowsStackedList() {
    Atom type; int format;
    unsigned long nItems, unusedBytes;
    unsigned char* properties = nullptr;

    vector<Window> result;
    if (XGetWindowProperty(mDisplay, DefaultRootWindow(mDisplay),
        XInternAtom(mDisplay, "_NET_CLIENT_LIST_STACKING", False),
        0L, 1024, False, XA_WINDOW, &type, &format, &nItems,
        &unusedBytes, &properties) == Success) {
        Window* windows = (Window*) properties;
        // 0 is desktop, add up.
        for (int i = 0; i < nItems; i++) {
            result.push_back(windows[i]);
        }
    }
    XFree(properties);

    return result;
}

/**
 * Method returns if a window is in the list
 * of active X11 windows.
 */
bool
XHelper::isWindowInStackedList(const Display* display,
    const Window window) {
    const vector<Window> windows = getWindowsStackedList();
    const int WINDOWS_SIZE = windows.size();

    // 0 is desktop, search order unimportant.
    for (int i = 0; i < WINDOWS_SIZE; i++) {
        if (windows[i] == window) {
            return true;
        }
    }
    return false;
}

/**
 * This method waits until a window is in the list
 * of active X11 windows.
 */
bool
XHelper::waitForWindowInStackedList(const Window window,
    const int maxWaitTimeMS) {
    const chrono::milliseconds MAX_WAIT_TIME(maxWaitTimeMS);
    const chrono::milliseconds DELAY_TIME(25);

    chrono::milliseconds waitTime(0);
    while (waitTime < MAX_WAIT_TIME) {
        if (isWindowInStackedList(mDisplay, window)) {
            return true;
        }
        this_thread::sleep_for(DELAY_TIME);
        waitTime += DELAY_TIME;
    }

    return false;
}

/**
 * Getter to return WinInfo* for a Window.
 */
int
XHelper::getWindowStackNumber(const Window window) {
    const vector<WinInfo*> winInfos = getWinInfoList();
    const int WININFO_SIZE = winInfos.size();

    // 0 is desktop, search order unimportant.
    for (int i = 0; i < WININFO_SIZE; i++) {
        WinInfo* winInfo = winInfos[i];
        if (winInfo->window == window) {
            return i;
        }
    }
    return -1;
}

/**
 * Getter for Window size attributes.
 */
XWindowAttributes
XHelper::getWindowAttributes(const Window window) {
    XWindowAttributes wAttrs;
    wAttrs.map_state = -1;
    wAttrs.width = -1;
    wAttrs.height = -1;
    wAttrs.x = -1;
    wAttrs.y = -1;

    XGetWindowAttributes(mDisplay, window, &wAttrs);
    return wAttrs;
}

/**
 * This method returns a 40-char window title string.
 *    Replace unprintables with SPACE.
 */
string
XHelper::getWindowTitle(const Window window) {
    const int TITLE_STRING_LENGTH = 40;
    char mTitleOfWindow[TITLE_STRING_LENGTH + 1] = "";

    XTextProperty titleBarName;
    int outP = 0;
    if (XGetWMName(mDisplay, window, &titleBarName)) {
        const char* NAME_PTR = (char*)
            titleBarName.value;
        const int NAME_LEN = strlen(NAME_PTR);

        for (; outP < NAME_LEN && outP <
            TITLE_STRING_LENGTH; outP++) {
            mTitleOfWindow[outP] = isprint(
                *(NAME_PTR + outP)) ?
                    *(NAME_PTR + outP) : ' ';
        }
        XFree(titleBarName.value);
    }

    for (; outP < TITLE_STRING_LENGTH; outP++) {
        mTitleOfWindow[outP] = ' ';
    }
    mTitleOfWindow[outP] = '\0';
    return string(mTitleOfWindow);
}

/**
 * This method determines which workspace a
 * window is visible on. result == -1 means all.
 */
long
XHelper::getWindowWorkspace(const Window window) {
    Atom type; int format;
    unsigned long nItems, unusedBytes;
    unsigned char* properties = nullptr;

    if (XGetWindowProperty(mDisplay, window, XInternAtom(
        mDisplay, "_NET_WM_DESKTOP", False), 0, 1, False,
        AnyPropertyType, &type, &format, &nItems,
        &unusedBytes, &properties) == Success &&
        type == XA_CARDINAL && properties) {
        long result = *(long*) (void*) properties;
        XFree(properties);
        return result;
    }
    XFree(properties);

    properties = nullptr;
    if (XGetWindowProperty(mDisplay, window, XInternAtom(
        mDisplay, "_WIN_WORKSPACE", False), 0, 1, False,
        AnyPropertyType, &type, &format, &nItems,
        &unusedBytes, &properties) == Success &&
        type == XA_CARDINAL && properties) {
        const long result = *(long*) (void*) properties;
        XFree(properties);
        return result;
    }
    XFree(properties);

    return -1;
}

/**
 * This method checks if a window is hidden.
 */
bool
XHelper::isWindowHidden(const Window window) {
    if (isWindowHiddenByNetWMState(window)) {
        return true;
    }
    if (isWindowHiddenByWMState(window)) {
        return true;
    }
    if (isDesktopShowing()) {
        return true;
    }

    return false;
}

/**
 * This method checks "_NET_WM_STATE" for
 * window HIDDEN attribute.
 */
bool
XHelper::isWindowHiddenByNetWMState(const Window window) {

    Atom type; int format;
    unsigned long nItems, unusedBytes;
    unsigned char* properties = nullptr;

    if (XGetWindowProperty(mDisplay, window, XInternAtom(
        mDisplay, "_NET_WM_STATE", False), 0, (~0L), False,
        AnyPropertyType, &type, &format, &nItems,
        &unusedBytes, &properties) == Success &&
        format == 32) {
        for (unsigned long i = 0; i < nItems; i++) {
            char* nameString = XGetAtomName(mDisplay,
                ((Atom*) (void*) properties) [i]);
            if (strcmp("_NET_WM_STATE_HIDDEN",
                nameString) == 0) {
                XFree(nameString);
                XFree(properties);
                return true;
            }
            XFree(nameString);
        }
    }
    XFree(properties);

    return false;
}

/**
 * This method checks "WM_STATE" for
 * window HIDDEN attribute.
 */
bool
XHelper::isWindowHiddenByWMState(const Window window) {
    Atom type; int format;
    unsigned long nItems, unusedBytes;
    unsigned char* properties = nullptr;

    if (XGetWindowProperty(mDisplay, window, XInternAtom(
        mDisplay, "WM_STATE", False), 0, (~0L), False,
        AnyPropertyType, &type, &format, &nItems,
        &unusedBytes, &properties) == Success &&
        format == 32 && nItems > 0 &&
        (*(long*) (void*) properties != NormalState)) {
        XFree(properties);
        return true;
    }
    XFree(properties);

    return false;
}

/**
 * This method checks if a window is sticky.
 */
bool
XHelper::isWindowSticky(const Window window) {
    Atom type; int format;
    unsigned long nItems, unusedBytes;
    unsigned char* properties = nullptr;

    if (XGetWindowProperty(mDisplay, window, XInternAtom(
        mDisplay, "_NET_WM_STATE", False), 0, (~0L), False,
        AnyPropertyType, &type, &format, &nItems,
        &unusedBytes, &properties) == Success &&
        type == XA_ATOM) {
        for (unsigned long int i = 0; i < nItems; i++) {
            char* nameString = XGetAtomName(mDisplay,
                ((Atom*) (void*) properties) [i]);
            if (strcmp(nameString,
                "_NET_WM_STATE_STICKY") == 0) {
                XFree(nameString);
                XFree(properties);
                return true;
            }
            XFree(nameString);
        }
    }
    XFree(properties);

    return false;
}

/**
 * This method checks is a window is a dock.
 */
bool
XHelper::isWindowDock(const Window window) {
    Atom type; int format;
    unsigned long nItems, unusedBytes;
    unsigned char* properties = nullptr;

    if (XGetWindowProperty(mDisplay, window, XInternAtom(
        mDisplay, "_NET_WM_WINDOW_TYPE", False), 0, (~0L),
        False, AnyPropertyType, &type, &format, &nItems,
        &unusedBytes, &properties) == Success &&
        format == 32) {
        for (int i = 0; (unsigned long)i < nItems; i++) {
            char* nameString = XGetAtomName(mDisplay,
                ((Atom*) (void*) properties) [i]);
            if (strcmp(nameString,
                "_NET_WM_WINDOW_TYPE_DOCK") == 0) {
                XFree(nameString);
                XFree(properties);
                return true;
            }
            XFree(nameString);
        }
    }
    XFree(properties);

    return false;
}

/**
 * This method scans the desktop windows list in
 * stacked order to determine if a requested window
 * is the one the mouse is over.
 */
bool
XHelper::isWindowHovered(const Window window, const QPoint pos) {
    // Get info for the request.
    const long VISIBLE_WS = getVisibleWorkspace();
    const WinInfo* WININFO = getWinInfoForWindow(window);

    // Get candidate windows.
    const vector<WinInfo*> winInfos = getWinInfoList();
    const int WININFO_SIZE = winInfos.size();

    // Search windows down from the top.
    for (int i = WININFO_SIZE - 1; i >= 0; i--) {
        // Hidden windows can't be hovered.
        const WinInfo* EACH = winInfos[i];
        if (EACH->isHidden) {
            continue;
        }

        // If the window is on a specific workspace & its not
        // the one we're displaying, we're not hovering it.
        if (VISIBLE_WS != -1 && EACH->onWorkspace != -1 &&
            VISIBLE_WS != EACH->onWorkspace) {
            continue;
        }

        // If the window doesn't contain the cursor x,y
        // position, we're not hovering it.
        if (!EACH->windowBorder.contains(pos)) {
            continue;
        }

        // We found the hovered window, is it the requested one?
        return EACH->window == window;
    }

    // No windows hovered.
    return false;
}

/**
 * Place window in stack order to be immediately
 * above desktop, yet below all other windows.
 */
void
XHelper::makeWindowStayOnBottom(const Window window) {
    const Atom NET_WM_STATE = XInternAtom(
        mDisplay, "_NET_WM_STATE", False);
    const Atom NET_WM_STATE_BELOW = XInternAtom(
        mDisplay, "_NET_WM_STATE_BELOW", False);

    XEvent xev;
    memset(&xev, 0, sizeof(xev));
    xev.type = ClientMessage;
    xev.xclient.window = window;
    xev.xclient.message_type = NET_WM_STATE;
    xev.xclient.format = 32;
    xev.xclient.data.l[0] = 1; // _NET_WM_STATE_ADD
    xev.xclient.data.l[1] = NET_WM_STATE_BELOW;
    xev.xclient.data.l[2] = 0; // No second property
    xev.xclient.data.l[3] = 0;
    xev.xclient.data.l[4] = 0;

    XSendEvent(mDisplay, DefaultRootWindow(mDisplay),
        False, SubstructureRedirectMask |
        SubstructureNotifyMask, &xev);
    XFlush(mDisplay);
}

/**
 * Helper to return Window as Hex value.
 */
string
XHelper::getWindowAsHexString(const Window window) {
    char buffer[20];
    sprintf(buffer, "[0x%08lx]", window);
    return string(buffer);
}

/**
 * Private initializer to create raw list of
 * currently active x11 windows.
 */
vector<WinInfo*>
XHelper::getWinInfoList() {
    const vector<Window> windows = getWindowsStackedList();
    const int WINDOWS_SIZE = windows.size();

    // 0 is desktop, add up.
    vector<WinInfo*> winInfoVector;
    for (int i = 0; i < WINDOWS_SIZE; i++) {
        WinInfo* winInfo = new WinInfo();
        winInfo->window = windows[i];

        // Set StickyWindow client rect x-y Pos.
        int xCoord = 0;
        int yCoord = 0;
        Window unused;
        XTranslateCoordinates(mDisplay, winInfo->window,
            DefaultRootWindow(mDisplay), 0, 0, &xCoord,
                &yCoord, &unused);
        winInfo->windowBorder.setX(xCoord);
        winInfo->windowBorder.setY(yCoord);

        // Set StickyWindow client rect w-h QSize.
        const XWindowAttributes wAttrs =
            getWindowAttributes(winInfo->window);
        winInfo->windowBorder.setWidth(wAttrs.width);
        winInfo->windowBorder.setHeight(wAttrs.height);

        // Set all other WinInfo fields.
        winInfo->onWorkspace = getWindowWorkspace(
            winInfo->window);
        winInfo->mapState = wAttrs.map_state;

        winInfo->isSticky = (winInfo->onWorkspace == -1) ?
            true : isWindowSticky(winInfo->window);
        winInfo->isDock = isWindowDock(winInfo->window);
        winInfo->isHidden = isWindowHidden(
            winInfo->window);

        // Push final object.
        winInfoVector.emplace_back(winInfo);
    }

    return winInfoVector;
}

/**
 * Getter to return WinInfo* for a Window.
 */
WinInfo*
XHelper::getWinInfoForWindow(const Window window) {
    const vector<WinInfo*> winInfos = getWinInfoList();
    const int WININFO_SIZE = winInfos.size();

    // 0 is desktop, search order unimportant.
    for (int i = 0; i < WININFO_SIZE; i++) {
        WinInfo* winInfo = winInfos[i];
        if (winInfo->window == window) {
            return winInfo;
        }
    }
    return nullptr;
}

/**
 * Debug method prints all WinInfo structs.
 */
void
XHelper::logAllWinInfoStructs() {
    const vector<WinInfo*> winInfos = getWinInfoList();
    const int WININFO_SIZE = winInfos.size();

    // 0 is desktop, print down from top.
    logWinInfoStructColumns();
    for (int i = WININFO_SIZE - 1; i >= 0; i--) {
        logWinInfo(winInfos[i]);
    }
}

/**
 * Debug method prints column headings for
 * WinInfo structs.
 */
void
XHelper::logWinInfoStructColumns() {
    cout << endl << XCOLOR_GREEN <<
        "---window---  Titlebar Name"
        "                             WS   "
        "M  ---Position--  -----QSize----  "
        "Attributes" <<
        XCOLOR_NORMAL << endl;
}

/**
 * Debug method prints a requested windows
 * WinInfo struct.
 */
void
XHelper::logWinInfo(const WinInfo* winInfo) {
    printf("[0x%08lx]  %s  %2i  %2i  %5i , %-5i  "
        "%5i , %-5i  %s%s%s\n",
        winInfo->window,
        getWindowTitle(
            winInfo->window).c_str(),
        winInfo->onWorkspace,
        winInfo->mapState,
        (int) winInfo->windowBorder.x(),
        (int) winInfo->windowBorder.y(),
        (int) winInfo->windowBorder.width(),
        (int) winInfo->windowBorder.height(),
        winInfo->isDock ? "dock " : "",
        winInfo->isSticky ? "sticky " : "",
        winInfo->isHidden ? "hidden" : "");
}
