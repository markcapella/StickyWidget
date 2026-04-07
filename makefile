
# *****************************************************
# Variables to control Compile / Link.
#
APP_NAME="StickyWidget"
APP_VERSION="2026-04-07 17:00"
APP_AUTHOR="Mark James Capella"

# Color styling.
COLOR_NORMAL := $(shell tput sgr0)

COLOR_BLACK := $(shell tput setaf 0)
COLOR_RED := $(shell tput setaf 1)
COLOR_GREEN := $(shell tput setaf 2)
COLOR_YELLOW := $(shell tput setaf 3)
COLOR_BLUE := $(shell tput setaf 4)
COLOR_MAGENTA := $(shell tput setaf 5)
COLOR_CYAN := $(shell tput setaf 6)
COLOR_WHITE := $(shell tput setaf 7)

CPP = g++

CFLAGS = -std=c++17 -fPIC -I. \
	-I/usr/include/X11 \
	-I/usr/include/x86_64-linux-gnu/qt6 \
	-I/usr/include/x86_64-linux-gnu/qt6/QtCore \
	-I/usr/include/X11/extensions \
	`fltk-config --cxxflags`

LFLAGS = -lX11 -lXext -lXfixes -lXrender -lfltk_images \
	`pkg-config --libs libpng` \
	-L/usr/lib/x86_64-linux-gnu -lQt6Core \
	`fltk-config --ldflags`


# ****************************************************
# make
#
all:
	@if [ "$(shell id -u)" = 0 ]; then \
		echo; \
		echo "$(COLOR_RED)Error!$(COLOR_NORMAL) You must not"\
			"be root to perform this action."; \
		echo; \
		echo  "Please re-run with:"; \
		echo "   $(COLOR_GREEN)make$(COLOR_NORMAL)"; \
		echo; \
		exit 1; \
	fi

	@echo
	@echo "$(COLOR_BLUE)Build Starts.$(COLOR_NORMAL)"
	@echo

	$(CPP) $(CFLAGS) -c SettingsHelper.cpp -o SettingsHelper.o \
		$(pkg-config --cflags --libs QtCore)
	$(CPP) $(CFLAGS) -c DisplayHelper.cpp -o DisplayHelper.o \
		$(pkg-config --cflags --libs QtCore)
	$(CPP) $(CFLAGS) -c XHelper.cpp -o XHelper.o \
		$(pkg-config --cflags --libs QtCore)

	$(CPP) $(CFLAGS) -c StickyWindow.cpp -o StickyWindow.o \
		$(pkg-config --cflags --libs QtCore)
	$(CPP) $(CFLAGS) -c StickyWidget.cpp -o StickyWidget.o \
		$(pkg-config --cflags --libs QtCore)

	@echo
	$(CPP) SettingsHelper.o DisplayHelper.o XHelper.o \
		StickyWindow.o StickyWidget.o \
		-o StickyWidget \
		${LFLAGS}

	@echo "true" > "BUILD_COMPLETE"

	@echo
	@echo "$(COLOR_BLUE)Build Done.$(COLOR_NORMAL)"

# ****************************************************
# make run
#
run:
	@if [ ! -f BUILD_COMPLETE ]; then \
		echo; \
		echo "$(COLOR_RED)Error!$(COLOR_NORMAL) Nothing"\
			"currently built to run."; \
		echo; \
		echo "Please make this project first, with:"; \
		echo "   $(COLOR_GREEN)make$(COLOR_NORMAL)"; \
		echo; \
		exit 1; \
	fi

	@if [ "$(shell id -u)" = 0 ]; then \
		echo; \
		echo "$(COLOR_RED)Error!$(COLOR_NORMAL) You must not"\
			"be root to perform this action."; \
		echo; \
		echo  "Please re-run with:"; \
		echo "   $(COLOR_GREEN)make run$(COLOR_NORMAL)"; \
		echo; \
		exit 1; \
	fi

	@echo
	@echo "$(COLOR_BLUE)Run Starts.$(COLOR_NORMAL)"
	@echo

	rm -f '$(HOME)/.local/StickyWidget.ini'
	./StickyWidget

	@echo
	@echo "$(COLOR_BLUE)Run Done.$(COLOR_NORMAL)"

# ****************************************************
# sudo make install
#
install:
	@if [ ! -f BUILD_COMPLETE ]; then \
		echo; \
		echo "$(COLOR_RED)Error!$(COLOR_NORMAL) Nothing"\
			"currently built to install."; \
		echo; \
		echo "Please make this project first, with:"; \
		echo "   $(COLOR_GREEN)make$(COLOR_NORMAL)"; \
		echo; \
		exit 1; \
	fi

	@if ! [ "$(shell id -u)" = 0 ]; then \
		echo; \
		echo "$(COLOR_RED)Error!$(COLOR_NORMAL) You must"\
			"be root to perform this action."; \
		echo; \
		echo  "Please re-run with:"; \
		echo "   $(COLOR_GREEN)sudo make install$(COLOR_NORMAL)"; \
		echo; \
		exit 1; \
	fi

	@echo
	@echo "$(COLOR_BLUE)Install Starts.$(COLOR_NORMAL)"
	@echo

	cp 'StickyWidget' '/usr/local/bin'
	chmod +x '/usr/local/bin/StickyWidget'
	@echo

	cp 'StickyWidget.desktop' '/usr/share/applications/'
	cp 'StickyWidget.png' '/usr/share/icons/hicolor/64x64/apps/'
	@echo

	@echo
	@echo "$(COLOR_BLUE)Install Done.$(COLOR_NORMAL)"

# ****************************************************
# sudo make uninstall
#
uninstall:
	@if ! [ "$(shell id -u)" = 0 ]; then \
		echo; \
		echo "$(COLOR_RED)Error!$(COLOR_NORMAL) You must"\
			"be root to perform this action."; \
		echo; \
		echo  "Please re-run with:"; \
		echo "   $(COLOR_GREEN)sudo make uninstall$(COLOR_NORMAL)"; \
		echo; \
		exit 1; \
	fi

	@echo
	@echo "$(COLOR_BLUE)Uninstall Starts.$(COLOR_NORMAL)"
	@echo

	rm -f '/usr/local/bin/StickyWidget'
	@echo

	rm -f '/usr/share/applications/StickyWidget.desktop'
	rm -f '/usr/share/icons/hicolor/64x64/apps/StickyWidget.png'
	@echo

	@echo
	@echo "$(COLOR_BLUE)Uninstall Done.$(COLOR_NORMAL)"

# ****************************************************
# make clean
#
clean:
	@if [ "$(shell id -u)" = 0 ]; then \
		echo; \
		echo "$(COLOR_RED)Error!$(COLOR_NORMAL) You must not"\
			"be root to perform this action."; \
		echo; \
		echo  "Please re-run with:"; \
		echo "   $(COLOR_GREEN)make clean$(COLOR_NORMAL)"; \
		echo; \
		exit 1; \
	fi

	@echo
	@echo "$(COLOR_BLUE)Clean Starts.$(COLOR_NORMAL)"
	@echo

	rm -f DisplayHelper.o
	rm -f SettingsHelper.o
	rm -f XHelper.o

	rm -f StickyWindow.o
	rm -f StickyWidget.o

	rm -f StickyWidget

	@rm -f "BUILD_COMPLETE"

	@echo
	@echo "$(COLOR_BLUE)Clean Done.$(COLOR_NORMAL)"
