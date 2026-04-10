
#pragma once

/**
 * SettingsHelper provides a permanant keyed values
 * store for user preferences in a file tied to appName.
 *
 */
class SettingsHelper {

    public:
        SettingsHelper(const QString appName);

        /**
         * Getter for Hello World message.
         */
        QString getHelloMessage();

        /**
         * Getter & setters for window minimum Width.
         */
        double getMinimumWindowWidth();
        void setMinimumWindowWidth(const double w);

        /**
         * Getter & setters for window minimum Height.
         */
        double getMinimumWindowHeight();
        void setMinimumWindowHeight(const double h);

        /**
         * Getters & setter for window x, y, w, & h.
         */
        double getWindowXPos();
        double getWindowYPos();
        double getWindowWidth();
        double getWindowHeight();
        void setWindowPosAndSize(const int xPos,
            const int yPos, const int width, const int height);

        /**
         * Getters & setters of window border & title
         * visibility state & attributes.
         */
        bool isWindowBorderVisible();
        void setWindowBorderVisible(const bool state);
        double getWindowBorderWidth();

    private:
        /**
         * Helper to return a new QSettings object
         * for pref access based on our appName.
         */
        QSettings* getQSettings();

        /**
         * Helper to return a QSettings filename
         * from appName.
         */
        QString getQSettingsFile();

        // Members.
        QString mSettingsApp = "";
        bool mBorderVisible = false;
};
