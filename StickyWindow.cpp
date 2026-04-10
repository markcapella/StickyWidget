
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

    // Set app minimum window size.
    size_range(mSettingsHelper->getMinimumWindowWidth(),
        mSettingsHelper->getMinimumWindowHeight(), 0, 0);

    // Create floating SettingsButton, clickBack &
    // visibility state.
    const int BORDER_WIDTH = mSettingsHelper->
        getWindowBorderWidth();
    mSettingsButton = new Fl_Button(width -
        BUTTON_WIDTH - BORDER_WIDTH, BORDER_WIDTH,
        BUTTON_WIDTH, BUTTON_HEIGHT, "O");

    mSettingsButton->callback(onSettingsButtonClicked, this);
    if (!mSettingsButtonVisibile) {
        mSettingsButton->hide();
    }

    // Don't use toolkit draw.
    Fl_PNG_Image* icon = new Fl_PNG_Image("/usr/share/icons/"
        "hicolor/64x64/apps/StickyWidget.png");
    if (!icon || icon->fail()) {
        icon = new Fl_PNG_Image("StickyWidget.png");
    }
    if (icon && !icon->fail()) {
        this->icon(icon);
    }

    this->box(FL_NO_BOX);
    this->end();

    // Set initial app size & position centered on screen.
    if (mSettingsHelper->getWindowXPos() == -1 &&
        mSettingsHelper->getWindowYPos() == -1) {
        mSettingsHelper->setWindowPosAndSize(
            (WidthOfScreen(DefaultScreenOfDisplay(
                fl_display)) - mSettingsHelper->
                getWindowWidth()) / 2,
            (HeightOfScreen(DefaultScreenOfDisplay(
                fl_display)) - mSettingsHelper->
                getWindowHeight()) / 2,
            mSettingsHelper->getWindowWidth(),
            mSettingsHelper->getWindowHeight()
        );
        this->position(mSettingsHelper->getWindowXPos(),
            mSettingsHelper->getWindowYPos());
    }
}

/**
 * Overriden show() method ensures we stay on bottom
 * when app is "stuck", app border state != visible.
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
 * Overriden draw() method ensures we have a
 * transparent window with SettingsButton visible
 * on mouse hover.
 */
void
StickyWindow::draw() {
    const XRenderPictFormat* RENDER_FORMAT =
        XRenderFindVisualFormat(fl_display, fl_visual->visual);
    if (!RENDER_FORMAT) { return; }

    // Create a 1x1 pixmap to act as a solid color source.
    const Pixmap RENDER_PIXMAP = XCreatePixmap(fl_display,
        fl_xid(this), 1, 1, 32);
    const Picture RENDER_COLOR_RED = XRenderCreatePicture(
        fl_display, RENDER_PIXMAP, RENDER_FORMAT, 0, nullptr);

    const Picture RENDER_PICTURE = XRenderCreatePicture(
        fl_display, fl_xid(this), RENDER_FORMAT, 0, nullptr);

    XRenderColor RED_COLOR;
    RED_COLOR.red = 0xffff;
    RED_COLOR.green = 0x0000;
    RED_COLOR.blue = 0x0000;
    RED_COLOR.alpha = 0xffff;
    XRenderFillRectangle(fl_display, PictOpSrc, RENDER_COLOR_RED,
        &RED_COLOR, 0, 0, 1, 1);

    // Set entire draw canvas transparent.
    XRenderFillRectangle(fl_display, PictOpSrc, RENDER_PICTURE,
        &TRANSPARENT, 0, 0, w(), h());

    // Red background in window.
    XRenderFillRectangle(fl_display, PictOpOver, RENDER_PICTURE,
        &RED_COLOR, 10, 10, w() - 20, h() - 20);

    // Use font color green.
    XftColor greenFontColor;
    greenFontColor.color.red = 0x0000;
    greenFontColor.color.green = 0xffff;
    greenFontColor.color.blue = 0x0000;
    greenFontColor.color.alpha = 0xffff;
    XftColorAllocValue(fl_display, fl_visual->visual,
        fl_colormap, &greenFontColor.color, &greenFontColor);

    // Say hello message in the font.
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

    // Define & draw settings button
    if (mSettingsButtonVisibile) {
        const XRenderColor BUTTON_COLOR =
            mSettingsButton->value() ? LIGHT_GRAY : WHITE;

        const int BUTTON_X = w() - BUTTON_WIDTH -
            mSettingsHelper->getWindowBorderWidth();
        const int BUTTON_Y = mSettingsHelper->
            getWindowBorderWidth();

        const XRenderPictureAttributes PIC_ATTR = {
            .poly_edge = PolyEdgeSmooth };
        const Picture CANVAS = XRenderCreatePicture(fl_display,
            fl_xid(this), XRenderFindStandardFormat(fl_display,
            PictStandardARGB32), CPPolyEdge, &PIC_ATTR);

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

    // Cleanup
    XRenderFreePicture(fl_display, RENDER_PICTURE);
    XRenderFreePicture(fl_display, RENDER_COLOR_RED);
    XFreePixmap(fl_display, RENDER_PIXMAP);
}

/**
 * Overriden resize() method ensures we remember
 * window position & size on restarts, and adjusts
 * clickable SettingsButton position & its Input
 * Shape Region to the new top-right location.
 */
void
StickyWindow::resize(const int xPos, const int yPos,
    const int width, const int height) {

    Fl_Double_Window::resize(xPos, yPos, width, height);

    // Save new position of StickyWidget.
    mSettingsHelper->setWindowPosAndSize(xPos, yPos,
        width, height);

    // Calc border size for settingsButton
    // position & size.
    const int BORDER_WIDTH = mSettingsHelper->
        getWindowBorderWidth();

    // Re-position clickable SettingsButton.
    mSettingsButton->position(width - mSettingsButton->w() -
        BORDER_WIDTH, BORDER_WIDTH);

    // Reposition the Input Shape Region under the
    // SettingsButton to allow small clickable area.
    XRectangle settingsButtonInputRegion {
        .x = static_cast<short> (width -
            BUTTON_WIDTH - BORDER_WIDTH),
        .y = static_cast<short> (BORDER_WIDTH),
        .width = BUTTON_WIDTH,
        .height = BUTTON_HEIGHT,
    };
    const XserverRegion region = XFixesCreateRegion(
        fl_display, &settingsButtonInputRegion, 1);
    XFixesSetWindowShapeRegion(fl_display,
        fl_xid(this), ShapeInput, 0, 0, region);
    XFixesDestroyRegion(fl_display, region);

    // Convienience to expose the Settings Button
    // visually during drags.
    setSettingsButtonVisibility(true);
    this->redraw();
}

/**
 * Settings button is visible when cursor hovers
 * the app window, & enables clicking to opposite
 * app border state.
 */
void
StickyWindow::setSettingsButtonVisibility(
    const bool visibility) {
    if (mSettingsButtonVisibile == visibility) {
        return;
    }

    mSettingsButtonVisibile = visibility;
    if (!mSettingsButtonVisibile) {
        mSettingsButton->hide();
    } else {
        mSettingsButton->show();
    }
}

/**
 * Toggle app visibility state when settings button
 * clicked. Switch between StickyWidget app border
 * state (titlebar & border displayed -vs- none).
 */
void
StickyWindow::onSettingsButtonClicked(Fl_Widget* w,
    void* data) {
    StickyWindow* canvas = (StickyWindow*) data;
    canvas->hide();

    // Toggle app border state.
    mSettingsHelper->setWindowBorderVisible(
        !mSettingsHelper->isWindowBorderVisible());

    // Set position & state.
    canvas->position(mSettingsHelper->
        getWindowXPos(), mSettingsHelper->
        getWindowYPos());

    // Set titlebar & border state.
    canvas->border(mSettingsHelper->
        getWindowBorderWidth());

    canvas->show();
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
