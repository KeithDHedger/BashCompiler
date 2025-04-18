AUTOMAKE_OPTIONS = subdir-objects
include ../flagsandlibs
include ../sources

bashcompiler_SOURCES = $(COMMONSRC)

bashcompiler_CFLAGS = -O3
bashcompiler_CXXFLAGS = -O3

pixfilesdir = $(pkgdatadir)/pixmaps
pixfiles_DATA = ../resources/pixmaps/*

scriptfilesdir = $(pkgdatadir)/scripts
scriptfiles_SCRIPTS = ../resources/scripts/README

bin_PROGRAMS = bashcompiler

man1_MANS = ../resources/man/bashcompiler.1

install-exec-hook:
	install -D ../resources/pixmaps/BashCompiler.png $(DESTDIR)${prefix}/share/pixmaps/BashCompiler.png
	install -D ../resources/pixmaps/BashCompiler.desktop $(DESTDIR)${prefix}/share/applications/BashCompiler.desktop

uninstall-hook:
	rm -f $(DESTDIR)${prefix}/share/applications/BashCompiler.desktop
	rm -f $(DESTDIR)${prefix}/share/pixmaps/BashCompiler.png
	rm -fr $(pkgdatadir)

