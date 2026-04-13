
#include "Global.h"

#include "StickyWidget.h"


/**
 * Main canvas is derived from FLTK Fl_Double_Window.
 *
 */
StickyWindow::StickyWindow(const double xPos,
    const double yPos, const double width,
    const double height, const char* title) :
    Fl_Double_Window(xPos, yPos, width, height, title) {

    // Set window icon.
    Fl_PNG_Image* icon = new Fl_PNG_Image(
        ICON_INSTALL_LOCATION);
    if (!icon || !icon->data()) {
        icon = new Fl_PNG_Image(ICON_LOCAL_LOCATION);
    }
    if (icon && icon->data()) {
        this->icon(icon);
    }

    // Don't use toolkit draw.
    this->box(FL_NO_BOX);
    this->end();

    // Set initial window size & position centered on screen.
    if (mSettingsHelper->getWindowXPos() == -1 &&
        mSettingsHelper->getWindowYPos() == -1) {
        mSettingsHelper->setWindowPosAndSize(
            (WidthOfScreen(DefaultScreenOfDisplay(fl_display)) -
                mSettingsHelper->getWindowWidth()) / 2,
            (HeightOfScreen(DefaultScreenOfDisplay(fl_display)) -
                mSettingsHelper->getWindowHeight()) / 2,
            mSettingsHelper->getWindowWidth(),
            mSettingsHelper->getWindowHeight()
        );
        this->position(mSettingsHelper->getWindowXPos(),
            mSettingsHelper->getWindowYPos());
    }
}

/**
 * Overriden show() method ensures we stay on bottom
 * when window is "stuck", window border state != visible.
 */
void
StickyWindow::show() {
    // TODO: Suppress bad window result first time.
    Fl_Double_Window::show();

    // Set window below all while "stuck" (borderless).
    if (!mSettingsHelper->isWindowBorderVisible()) {
        mXHelper->makeWindowStayOnBottom(fl_xid(this));
    }
}

/**
 * Overriden hide() method.
 */
void
StickyWindow::hide() {
    Fl_Double_Window::hide();
}

/**
 * Overriden draw() method ensures we have a transparent
 * window with SettingsButton visible on mouse hover.
 */
void
StickyWindow::draw() {
    const XRenderPictFormat* RENDER_FORMAT =
        XRenderFindVisualFormat(fl_display, fl_visual->visual);
    if (!RENDER_FORMAT) { return; }

    // Create a 1x1 pixmap to act as a solid color source.
    const Pixmap RENDER_PIXMAP = XCreatePixmap(fl_display,
        fl_xid(this), 1, 1, 32);
    const Picture RENDER_PICTURE = XRenderCreatePicture(
        fl_display, fl_xid(this), RENDER_FORMAT, 0, nullptr);

    // Draw entire draw canvas transparent.
    XRenderFillRectangle(fl_display, PictOpSrc,
        RENDER_PICTURE, &TRANSPARENT, 0, 0, w(), h());

    // Draw red background in window.
    const Picture RENDER_COLOR_RED = XRenderCreatePicture(
        fl_display, RENDER_PIXMAP, RENDER_FORMAT, 0, nullptr);
    XRenderColor backgroundColor = {
        0xffff, 0x0000, 0x0000, 0xffff };
    XRenderFillRectangle(fl_display, PictOpSrc,
        RENDER_COLOR_RED, &backgroundColor,
        0, 0, 1, 1);
    XRenderFillRectangle(fl_display, PictOpOver,
        RENDER_PICTURE, &backgroundColor,
        10, 10, w() - 20, h() - 20);

    // Select font color green for Hello message.
    XftColor greenFontColor;
    greenFontColor.color.red = 0x0000;
    greenFontColor.color.green = 0xffff;
    greenFontColor.color.blue = 0x0000;
    greenFontColor.color.alpha = 0xffff;
    XftColorAllocValue(fl_display, fl_visual->visual,
        fl_colormap, &greenFontColor.color, &greenFontColor);

    // Draw hello message in the font.
    const QString MESSAGE = mSettingsHelper->getHelloMessage();
    const int MESSAGE_WIDTH = getStringPixelWidth(MESSAGE);
    const int MESSAGE_HEIGHT = getStringPixelHeight(MESSAGE);
    const int MESSAGE_X = (w() - MESSAGE_WIDTH) / 2;
    const int MESSAGE_Y = (h() - MESSAGE_HEIGHT) / 2 +
        MESSAGE_HEIGHT;

    const string MESSAGE_STD_STRING = MESSAGE.toStdString();
    const char* MESSAGE_CHARS = MESSAGE_STD_STRING.c_str();

    XftDraw* xft_draw = XftDrawCreate(fl_display,
        fl_xid(this), fl_visual->visual, fl_colormap);
    XftDrawStringUtf8(xft_draw, &greenFontColor, mFont,
        MESSAGE_X, MESSAGE_Y, (FcChar8*) MESSAGE_CHARS,
        strlen(MESSAGE_CHARS));

    // Define & draw settings button.
    if (mSettingsButtonVisible) {
        // Get settings button position & color.
        const XRenderColor BUTTON_COLOR =
            isSettingsButtonPressed() ? LIGHT_GRAY : WHITE;
        const int BUTTON_X = w() - BUTTON_WIDTH -1;
        const int BUTTON_Y = 1;

        const XRenderPictureAttributes PIC_ATTR = {
            .poly_edge = PolyEdgeSmooth };
        const Picture CANVAS = XRenderCreatePicture(fl_display,
            fl_xid(this), XRenderFindStandardFormat(fl_display,
            PictStandardARGB32), CPPolyEdge, &PIC_ATTR);

        // Settings button is square white button, indented
        // square black margin, and central settings button
        // square is colored based on pressed state.
        XRenderFillRectangle(fl_display, PictOpOver, CANVAS,
            &WHITE, BUTTON_X, BUTTON_Y, BUTTON_WIDTH,
            BUTTON_HEIGHT);
        XRenderFillRectangle(fl_display, PictOpOver, CANVAS,
            &BLACK, BUTTON_X + 1, BUTTON_Y + 1,
            BUTTON_WIDTH - 2, BUTTON_HEIGHT - 2);
        XRenderFillRectangle(fl_display, PictOpOver, CANVAS,
            &BUTTON_COLOR, BUTTON_X + 3 , BUTTON_Y + 3,
            BUTTON_WIDTH - 6, BUTTON_HEIGHT - 6);

        XRenderFreePicture(fl_display, CANVAS);
    }

    // Cleanup.
    XRenderFreePicture(fl_display, RENDER_PICTURE);
    XRenderFreePicture(fl_display, RENDER_COLOR_RED);
    XFreePixmap(fl_display, RENDER_PIXMAP);
}

/**
 * Overriden resize() method ensures we remember window
 * position & size on restarts.
 *
 * Adjusts clickable settings button position & its Input
 * Shape Region to the new top-right location.
 */
void
StickyWindow::resize(const int xPos, const int yPos,
    const int width, const int height) {

    // Save new position of StickyWidget.
    Fl_Double_Window::resize(xPos, yPos, width, height);
    mSettingsHelper->setWindowPosAndSize(xPos, yPos,
        width, height);

    // Calc border size for settings button position & size.
    const int BORDER_WIDTH = mSettingsHelper->
        getWindowBorderWidth();

    // Set entire draw area as transparent to mouse clicks.
    XRectangle settingsButtonInputRegion { .x = (short) xPos,
        .y = (short) yPos, .width = (unsigned short) width,
        .height = (unsigned short) height };
    const XserverRegion REGION = XFixesCreateRegion(
        fl_display, &settingsButtonInputRegion, 1);
    XFixesSetWindowShapeRegion(fl_display,
        fl_xid(this), ShapeInput, 0, 0, REGION);
    XFixesDestroyRegion(fl_display, REGION);

    // Expose the settings button visually during drags.
    setSettingsButtonVisibility(true);
    redraw();
}

/**
 * Getter for the settings button hover state.
 */
bool
StickyWindow::isSettingsButtonHovered(const QPoint pos) {
    // Get Settings button position in window.
    const QRect* SETTINGS_RECT = new QRect(w() - BUTTON_WIDTH -
        mSettingsHelper->getWindowBorderWidth(),
        mSettingsHelper->getWindowBorderWidth(),
        BUTTON_WIDTH, BUTTON_HEIGHT);

    return SETTINGS_RECT->contains(pos);
}

/**
 * Getter for the settings button press state.
 */
bool
StickyWindow::isSettingsButtonPressed() {
    return mSettingsButtonClicked;
}

/**
 * Setter for the settings button press state.
 */
void
StickyWindow::setSettingsButtonPressed(const bool clicked) {
    if (mSettingsButtonClicked == clicked) {
        return;
    }

    mSettingsButtonClicked = clicked;
    redraw();
}

/**
 * Setter for settings button visibility state.
 */
void
StickyWindow::setSettingsButtonVisibility(
    const bool visibility) {
    if (mSettingsButtonVisible == visibility) {
        return;
    }

    mSettingsButtonVisible = visibility;
    redraw();
}

/**
 * Switch between StickyWidget window border state where
 * titlebar & border displayed or not.
 */
void
StickyWindow::onSettingsButtonClicked() {
    hide();

    // Toggle app border state.
    mSettingsHelper->setWindowBorderVisible(
        !mSettingsHelper->isWindowBorderVisible());

    // Set position & state.
    position(mSettingsHelper->getWindowXPos(),
        mSettingsHelper->getWindowYPos());

    // Set titlebar & border state.
    border(mSettingsHelper->getWindowBorderWidth());

    show();
}

/**
 * This method returns pixel width of a text string.
 */
int
StickyWindow::getStringPixelWidth(const QString textString) {
    const string T = textString.toStdString();

    XGlyphInfo textExtents;
    XftTextExtents8(fl_display, mFont, (const FcChar8*)
        T.c_str(), T.length(), &textExtents);

    return textExtents.width;
}

/**
 * This method returns pixel height of a text string.
 */
int
StickyWindow::getStringPixelHeight(const QString textString) {
    const string T = textString.toStdString();

    XGlyphInfo textExtents;
    XftTextExtents8(fl_display, mFont, (const FcChar8*)
        T.c_str(), T.length(), &textExtents);

    return textExtents.height;
}
