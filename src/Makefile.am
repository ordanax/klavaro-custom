## Process this file with automake to produce Makefile.in

bin_PROGRAMS = klavaro

klavaro_SOURCES = \
	main.c main.h\
	auxiliar.c auxiliar.h \
	callbacks.c callbacks.h \
	translation.c translation.h \
	keyboard.c keyboard.h \
	tutor.c tutor.h \
	cursor.c cursor.h \
	plot.c plot.h \
	basic.c basic.h \
	adaptability.c adaptability.h \
	velocity.c velocity.h \
	fluidness.c fluidness.h \
	accuracy.c accuracy.h \
	top10.c top10.h 

AM_CPPFLAGS = \
	      @GTK_CFLAGS@ \
	      @GTKDATABOX_CFLAGS@ \
	      -DPACKAGE_DATA_DIR=\""$(datadir)"\" \
	      -DPACKAGE_LOCALE_DIR=\""$(localedir)"\"

AM_CFLAGS = -Wextra -Wformat -Werror=format-security

klavaro_LDADD = \
		@GTK_LIBS@ \
		@GTKDATABOX_LIBS@ \
		-lgmodule-2.0

if IS_POSIX
AM_CPPFLAGS += -export-dynamic
AM_LDFLAGS = -Wl,--export-dynamic
endif

if IS_WIN
AM_CFLAGS += -mwindows
endif

