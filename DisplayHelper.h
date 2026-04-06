
#pragma once

/**
 * Display helper to check and/or return a valid x11
 * display & session type.
 *
 */
class DisplayHelper {

    public:
        DisplayHelper(Display* display);

        /**
         * Return the initialized, or pre-provided-then
         * verified Display*.
         */
        Display* getDisplay();

    private:
        // Members.
        Display* mDisplay;
};
