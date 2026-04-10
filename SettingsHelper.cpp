
#include "Global.h"

/**
 * SettingsHelper provides a permanant keyed values
 * store for user preferences in a file tied to appName.
 *
 */
SettingsHelper::SettingsHelper(const QString appName) {

    mSettingsApp = appName;
}

/**
 * Getter for Hello World message.
 */
QString
SettingsHelper::getHelloMessage() {
    return getQSettings()->value("helloMessage",
        "Hello world !").toString();
}

/**
 * Getter & setters for window minimum Width.
 */
double
SettingsHelper::getMinimumWindowWidth() {
    return getQSettings()->value(
        "minimumWindowWidth", 48).toDouble();
}

void
SettingsHelper::setMinimumWindowWidth(const double w) {
    return getQSettings()->setValue(
        "minimumWindowWidth", w);
}

/**
 * Getter & setters for window minimum Height.
 */
double
SettingsHelper::getMinimumWindowHeight() {
    return getQSettings()->value(
        "minimumWindowHeight", 48).toDouble();
}
void
SettingsHelper::setMinimumWindowHeight(const double h) {
    return getQSettings()->setValue(
        "minimumWindowHeight", h);
}

/**
 * Getters & setter for window x, y, w, & h.
 */
double
SettingsHelper::getWindowXPos() {
    return getQSettings()->value("windowXPos",
        -1).toDouble();
}

double
SettingsHelper::getWindowYPos() {
    return getQSettings()->value("windowYPos",
        -1).toDouble();
}

double
SettingsHelper::getWindowWidth() {
    return getQSettings()->value("windowWidth",
        300).toDouble();
}

double
SettingsHelper::getWindowHeight() {
    return getQSettings()->value("windowHeight",
        125).toDouble();
}

void
SettingsHelper::setWindowPosAndSize(const int xPos,
    const int yPos, const int width, const int height) {
    if (xPos != getWindowXPos()) {
        getQSettings()->setValue("windowXPos", xPos);
    }
    if (yPos != getWindowYPos()) {
        getQSettings()->setValue("windowYPos", yPos);
    }
    if (width != getWindowWidth()) {
        getQSettings()->setValue("windowWidth", width);
    }
    if (height != getWindowHeight()) {
        getQSettings()->setValue("windowHeight", height);
    }
}

/**
 * Getters & setters of window border & title
 * visibility state & attributes.
 */
bool
SettingsHelper::isWindowBorderVisible() {
    return getQSettings()->value("borderVisibile",
        true).toBool();
}

void
SettingsHelper::setWindowBorderVisible(const bool state) {
    getQSettings()->setValue("borderVisibile", state);
}

double
SettingsHelper::getWindowBorderWidth() {
    return isWindowBorderVisible() ? 1 : 0;
}

/**
 * Helper to return a new QSettings object
 * for pref access based on our appName.
 */
QSettings*
SettingsHelper::getQSettings() {
    return new QSettings(getQSettingsFile(),
        QSettings::IniFormat);
}

/**
 * Helper to return a QSettings filename
 * from appName.
 */
QString
SettingsHelper::getQSettingsFile() {
    return getenv("HOME") + QStringLiteral(
        "/.local/") + mSettingsApp + ".ini";
}

