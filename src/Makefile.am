AM_CFLAGS = -Wall
INCLUDES = -I$(top_builddir)/lib

aetos_libs = $(top_builddir)/lib

bin_PROGRAMS = aetos
aetos_SOURCES = main.c mem.c event.c modules.c options.c utility.c error.c
aetos_LDADD = $(aetos_libs)/irc/libirc.a
aetos_LDFLAGS = `pth-config --libs` `pth-config --ldflags`
