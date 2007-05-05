bin_PROGRAMS = arkanoidsb
arkanoidsb_SOURCES = arkanoidsb.cpp ball.cpp bullet.cpp exploision.cpp \
			generatelevel.cpp arkanoidsbgame.cpp monster.cpp mystring.cpp mainmenu.cpp bonus.cpp \
			energyhole.cpp coolstring.cpp resource.cpp glSDL.c sinusstring.cpp tutorialdlg.cpp \
		leveleditor.cpp reminderdlg.cpp

# set the include path found by configure
AM_CPPFLAGS = $(all_includes) $(LIBSDL_CFLAGS)

# the library search path.
arkanoidsb_LDFLAGS = $(all_libraries) $(LIBSDL_RPATH)
arkanoidsb_LDADD = $(LIBSDL_LIBS)


noinst_HEADERS = mainmenu.h energyhole.h coolstring.h resource.h glSDL.h \
				sinusstring.h tutorialdlg.h leveleditor.h reminderdlg.h
