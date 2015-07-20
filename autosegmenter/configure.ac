dnl Process this file with autoconf to produce a configure script.

AC_INIT([DermShare Autosegmenter], [1])
AC_CONFIG_SRCDIR([src])
AC_CONFIG_AUX_DIR([build-aux])
AM_INIT_AUTOMAKE([1.11 foreign subdir-objects])
AM_SILENT_RULES([yes])
AM_CONFIG_HEADER([config.h])

AC_PROG_CC
AM_PROG_CC_STDC
AC_PROG_CXX

PKG_CHECK_MODULES([GLIB], [glib-2.0])
PKG_CHECK_MODULES([OPENDIAMOND], [opendiamond])

if test "x$STATIC_KIT_CFLAGS" = x ; then
	STATIC_KIT_CFLAGS=-I/opt/diamond-filter-kit/include
fi
if test "x$STATIC_KIT_LIBS" = x ; then
	STATIC_KIT_LIBS=-L/opt/diamond-filter-kit/lib
fi
if test "x$IPP_CFLAGS" = x ; then
	IPP_CFLAGS=-I/opt/intel/ipp/include
fi
if test "x$IPP_LIBS" = x ; then
	IPP_LIBS="-lippcc_l -lippcv_l -lippi_l -lipps_l -lippcore_l"
fi
if test "x$OPENCV_CFLAGS" = x ; then
	OPENCV_CFLAGS=-I/opt/diamond-filter-kit/include/opencv
fi
if test "x$OPENCV_LIBS" = x ; then
	OPENCV_LIBS="-lopencv_highgui -lopencv_core -ltiff -lz -lrt"
fi
AC_ARG_VAR([STATIC_KIT_CFLAGS], [static library kit CFLAGS])
AC_ARG_VAR([STATIC_KIT_LIBS], [static library kit LDFLAGS])
AC_ARG_VAR([IPP_CFLAGS], [IPP CFLAGS])
AC_ARG_VAR([IPP_LIBS], [IPP LDFLAGS])
AC_ARG_VAR([OPENCV_CFLAGS], [OpenCV CFLAGS])
AC_ARG_VAR([OPENCV_LIBS], [OpenCV LDFLAGS])

AC_SEARCH_LIBS([sqrt], [m],, AC_MSG_FAILURE([cannot find math library]))
AC_SEARCH_LIBS([pthread_getspecific], [pthread],, AC_MSG_FAILURE([cannot find threading library]))

AC_OUTPUT([Makefile])
