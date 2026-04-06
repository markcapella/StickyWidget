
#include "Global.h"

SettingsHelper* StickyWindow::mSettingsHelper = nullptr;

/**
 * Main canvas is derived from FLTK Fl_Double_Window.
 *
 */
StickyWindow::StickyWindow(const double xPos,
    const double yPos, const double width,
    const double height, const char* title) :
    Fl_Double_Window(xPos, yPos, width, height, title) {

    // Init helpers.
    mSettingsHelper = new SettingsHelper(APP_NAME);
    mXHelper = new XHelper(fl_display);

    // Set app minimum window size.
    size_range(mSettingsHelper->getMinimumWindowWidth(),
        mSettingsHelper->getMinimumWindowHeight(), 0, 0);

    // Create floating SettingsButton, clickBack &
    // visibility state.
    const int BORDER_WIDTH = mSettingsHelper->
        getWindowBorderWidth();

    mSettingsButton = new Fl_Button(width -
        SETTINGS_BUTTON_WIDTH - BORDER_WIDTH, BORDER_WIDTH,
        SETTINGS_BUTTON_WIDTH, SETTINGS_BUTTON_HEIGHT, "O");
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
    Fl_Double_Window::show();

    // Set window below all while in "stuck"
    // app border state (borderless).
    if (!mSettingsHelper->isWindowBorderVisible()) {
        mXHelper->makeWindowStayOnBottom(fl_xid(this));
    }
}

/**
 * Overriden draw() method ensures we have a
 * transparent window with SettingsButton visible
 * on mouse hover.
 */
void
StickyWindow::draw() {
    const Window THIS_X_WINDOW(fl_xid(this));

    // Create XRender Picture for the window.
    const XRenderPictureAttributes PIC_ATTR = {
        .poly_edge = PolyEdgeSmooth };
    const Picture CANVAS = XRenderCreatePicture(fl_display,
        THIS_X_WINDOW, XRenderFindStandardFormat(fl_display,
        PictStandardARGB32), CPPolyEdge, &PIC_ATTR);

    // Clear entire window with transparency (0,0,0,0).
    XRenderFillRectangle(fl_display, PictOpSrc, CANVAS,
        &TRANSPARENT, 0, 0, w(), h());

    // Define & draw button.
    if (mSettingsButtonVisibile) {
        const XRenderColor buttonColor = mSettingsButton->
            value() ? LIGHT_GRAY : WHITE;

        const int BORDER_SIZE = mSettingsHelper->
            getWindowBorderWidth();
        const int xPos = w() - SETTINGS_BUTTON_WIDTH -
            BORDER_SIZE;
        const int yPos = BORDER_SIZE;

        XRenderFillRectangle(fl_display, PictOpOver, CANVAS,
            &buttonColor, xPos, yPos, SETTINGS_BUTTON_WIDTH,
            SETTINGS_BUTTON_HEIGHT);
        XRenderFreePicture(fl_display, CANVAS);

        const GC gc = XCreateGC(fl_display,
            THIS_X_WINDOW, 0, NULL);
        XDrawRectangle(fl_display, THIS_X_WINDOW, gc,
            xPos + 1, yPos + 1, SETTINGS_BUTTON_WIDTH - 3,
            SETTINGS_BUTTON_HEIGHT - 3);
    }
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
            SETTINGS_BUTTON_WIDTH - BORDER_WIDTH),
        .y = static_cast<short> (BORDER_WIDTH),
        .width = SETTINGS_BUTTON_WIDTH,
        .height = SETTINGS_BUTTON_HEIGHT,
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
