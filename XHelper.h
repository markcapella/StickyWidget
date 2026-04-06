
#pragma once

/**
 * XHelper provides common X related methods.
 *
 */
class XHelper {
    public:
        XHelper(Display* displayHelper);

        /**
         * This method traps and handles X11 errors.
         */
        static int handleX11ErrorEvent(Display* display,
            XErrorEvent* event);

        /**
         * This method determines if a compositor is running.
         */
        bool isACompositorRunning();

        /**
         * This method checks if the desktop is currently
         * being shown, (which hides all windows).
         */
        bool isDesktopShowing();

        /**
         * This method returns the number of the current workspace,
         * where the OS allows multiple / virtual workspaces.
         * result == -1 is all.
         */
        long getVisibleWorkspace();

        /**
         * Method returns a a list of active X11 windows
         * in stacking order.
         */
        vector<Window> getWindowsStackedList();

        /**
         * Method returns if a window is in the list
         * of active X11 windows.
         */
        bool isWindowInStackedList(const Display* display,
            const Window window);

        /**
         * This method waits until a window is in the list
         * of active X11 windows.
         */
        bool waitForWindowInStackedList(const Window window,
            const int maxWaitTimeMS);

        /**
         * Getter to return WinInfo* for a Window.
         */
        int getWindowStackNumber(const Window window);

        /**
         * Getter for Window size attributes.
         */
        XWindowAttributes getWindowAttributes(
            const Window window);

        /**
         * This method returns a 40-char window title string.
         *    Replace unprintables with SPACE.
         */
        string getWindowTitle(const Window window);

        /**
         * This method determines which workspace a
         * window is visible on. result == -1 means all.
         */
        long getWindowWorkspace(const Window window);

        /**
         * This method checks if a window is hidden.
         */
        bool isWindowHidden(const Window window);

        /**
         * This method checks "_NET_WM_STATE" for
         * window HIDDEN attribute.
         */
        bool isWindowHiddenByNetWMState(const Window window);

        /**
         * This method checks "WM_STATE" for
         * window HIDDEN attribute.
         */
        bool isWindowHiddenByWMState(const Window window);

        /**
         * This method checks if a window is sticky.
         */
        bool isWindowSticky(const Window window);

        /**
         * This method checks is a window is a dock.
         */
        bool isWindowDock(const Window window);

        /**
         * This method scans the desktop windows list in
         * stacked order to determine if a requested window
         * is the one the mouse is over.
         */
        bool isWindowHovered(const Window window,
            const QPoint pos);

        /**
         * Place window in stack order to be immediately
         * above desktop, yet below all other windows.
         */
        void makeWindowStayOnBottom(const Window window);

        /**
         * Helper to return Window as Hex value.
         */
        string getWindowAsHexString(const Window window);

        /**
         * Private initializer to create raw list of
         * currently active x11 windows.
         */
        vector<WinInfo*> getWinInfoList();

        /**
         * Getter to return WinInfo* for a Window.
         */
        WinInfo* getWinInfoForWindow(const Window window);

        /**
         * Debug method prints all WinInfo structs.
         */
        void logAllWinInfoStructs();

        /**
         * Debug method prints column headings for
         * WinInfo structs.
         */
        void logWinInfoStructColumns();

        /**
         * Debug method prints a requested windows
         * WinInfo struct.
         */
        void logWinInfo(const WinInfo* winInfo);

    private:
        // Members.
        Display* mDisplay = nullptr;
};
