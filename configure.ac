#
#
AC_INIT([BashCompiler],[0.3.1],[PROJ],[BashCompiler])

AC_CONFIG_HEADERS([config.h])
AC_PROG_CXX
AM_INIT_AUTOMAKE
AC_PROG_CC

AC_SUBST(CFLAGS)
AC_SUBST(CPPFLAGS)
AC_SUBST(LDFLAGS)

CXXFLAGS=-Wall
AC_SUBST(CXXFLAGS)

CFLAGS=-Wall
AC_SUBST(CFLAGS)

QT_MODULES="Qt6Gui >= 6.8.2"
PKG_CHECK_MODULES(QT, $QT_MODULES)
AC_SUBST(QT_CFLAGS)
AC_SUBST(QT_LIBS)

QTWIDGETS_MODULES="Qt6Widgets >= 6.8.2"
PKG_CHECK_MODULES(QTWIDGETS, $QTWIDGETS_MODULES)
AC_SUBST(QTWIDGETS_CFLAGS)
AC_SUBST(QTWIDGETS_LIBS)

QTCORE_MODULES="Qt6Core >= 6.8.2"
PKG_CHECK_MODULES(QTCORE, $QTCORE_MODULES)
AC_SUBST(QTCORE_CFLAGS)
AC_SUBST(QTCORE_LIBS)

AC_CONFIG_FILES(Makefile BashCompiler/app/Makefile)
AC_OUTPUT
