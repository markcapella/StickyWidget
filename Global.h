
#pragma once

/**
 * Global headers, order important.
 *
 */
// Standard C libraries.
#include <chrono>
#include <cstdint>
#include <iostream>
#include <thread>
#include <unistd.h>
#include <vector>
using namespace std;

// Qt libraries.
#include <QCoreApplication>
#include <QSettings>
#include <QPoint>
#include <QRect>
#include <QSize>
#include <QString>

// FL Window Toolkit libraries.
#include <FL/Fl.H>
#include <FL/Fl_Double_Window.H>
#include <FL/Fl_Box.H>
#include <FL/Fl_Button.H>
#include <FL/fl_draw.H>
#include <FL/Fl_PNG_Image.H>
#include <FL/Fl_RGB_Image.H>
#include <FL/x.H>

// x11 libraries.
#include <X11/Xlib.h>
#include <X11/extensions/shape.h>
#include <X11/extensions/Xfixes.h>
#include <X11/extensions/Xrender.h>
#include <X11/Xft/Xft.h>

// Libpng libraries.
#include <png.h>

// Application libraries.
#include "SettingsHelper.h"
#include "DisplayHelper.h"
#include "WinInfo.h"
#include "XHelper.h"
#include "StickyWindow.h"

/**
 * Global consts.
 */
#define APP_NAME "StickyWidget"
#define ORG_NAME "skyrmion.inc"

/**
 * Common charsets to enable colorized cout messages.
 * "Ansii escape sequences for cursor color controls".
 */
#define XCOLOR_NORMAL  "\033[0m"

#define XCOLOR_BLACK   "\033[0;30m"
#define XCOLOR_WHITE   "\033[0;37m"

#define XCOLOR_RED     "\033[0;31m"
#define XCOLOR_GREEN   "\033[1;32m"
#define XCOLOR_YELLOW  "\033[1;33m"
#define XCOLOR_BLUE    "\033[1;34m"
#define XCOLOR_MAGENTA "\033[1;35m"
#define XCOLOR_CYAN    "\033[1;36m"

/**
 * Global type-defs.
 */
typedef std::chrono::steady_clock Clock;


/**
 * Global method decl's.
 */
void mainThread(void* data);
