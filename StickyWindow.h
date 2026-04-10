
#pragma once

/**
 * Main canvas is derived from FLTK Fl_Double_Window.
 *
 */
class StickyWindow : public Fl_Double_Window {

    public:
        // Define window icon.
        static inline const char* ICON_LOCAL_LOCATION =
            "StickyWidget.png";
        static inline const char* ICON_INSTALLED_LOCATION =
            "/usr/share/icons/hicolor/64x64/apps/StickyWidget.png";

        // Define settings button.
        static inline const int BUTTON_WIDTH = 24;
        static inline const int BUTTON_HEIGHT = 24;

        // Define draw colors.
        static inline constexpr XftColor mFontColor = {
            .pixel = 0x0, .color = { .red = 0xff, .green = 0xff,
                .blue = 0xff, .alpha = 0xffff } };

        static inline constexpr XRenderColor TRANSPARENT = {
            0x0000, 0x0000, 0x0000, 0x0000 };

        static inline constexpr XRenderColor BLACK = {
            0x0000, 0x0000, 0x0000, 0xffff };
        static inline constexpr XRenderColor WHITE = {
            0xffff, 0xffff, 0xffff, 0xffff };

        static inline constexpr XRenderColor LIGHT_GRAY = {
            0xd800, 0xd500, 0xd200, 0xffff };


        // Constructors.
        StickyWindow(const double width, const double height,
            const char* title = 0);

        StickyWindow(const double xPos, const double yPos,
            const double width, const double height,
            const char* title = 0);

        /**
         * Overriden show() method ensures we stay on bottom
         * when app is "stuck", app border state != visible.
         */
        void show() override;

        /**
         * Overriden draw() method ensures we have a
         * transparent window with SettingsButton visible
         * on mouse hover.
         */
        void draw() override;

        /**
         * Overriden hide() method.
         */
        void hide() override;

        /**
         * Overriden resize() method ensures we remember
         * window position & size on restarts, and adjusts
         * clickable SettingsButton position & its Input
         * Shape Region to the new top-right location.
         */
        void resize(const int xPos, const int yPos,
            const int width, const int height) override;

        /**
         * Settings button is visible when cursor hovers
         * the app window, & enables clicking to opposite
         * app border state.
         */
        void setSettingsButtonVisibility(const bool visibility);

        /**
         * Toggle app visibility state when settings button
         * clicked. Switch between StickyWidget app border
         * state (titlebar & border displayed -vs- none).
         */
        static void onSettingsButtonClicked(Fl_Widget* w,
            void* data);

    private:
        // Members.
        Fl_Button* mSettingsButton = nullptr;

        bool mSettingsButtonVisibile = false;

        /**
         * This method returns pixel width of a text string.
         */
        int getStringPixelWidth(const QString textString);

        /**
         * This method returns pixel height of a text string.
         */
        int getStringPixelHeight(const QString textString);
};

