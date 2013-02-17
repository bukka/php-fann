dnl $Id$
dnl config.m4 for extension fann

PHP_ARG_WITH(fann, for fann support,
[  --with-fann             Include fann support])

if test "$PHP_FANN" != "no"; then

  SEARCH_PATH="/usr/local /usr /local /opt"
  SEARCH_FOR="/include/fann.h"

  if test "$PHP_FANN" = "yes"; then
    AC_MSG_CHECKING([for libfann headers in default path])
    for i in $SEARCH_PATH ; do
      if test -r $i/$SEARCH_FOR; then
        FANN_DIR=$i
        AC_MSG_RESULT(found in $i)
      fi
    done
  else 
    AC_MSG_CHECKING([for libfann headers in $PHP_FANN])
    if test -r $PHP_FANN/$SEARCH_FOR; then
      FANN_DIR=$PHP_FANN
      AC_MSG_RESULT([found])
    fi
  fi

  if test -z "$FANN_DIR"; then
    AC_MSG_RESULT([not found])
    AC_MSG_ERROR([Cannot find libfann headers])
  fi

  PHP_ADD_INCLUDE($FANN_DIR/include)

  LIBNAME=fann
  LIBSYMBOL=fann_create_standard

  PHP_CHECK_LIBRARY($LIBNAME,$LIBSYMBOL,
  [
    PHP_ADD_LIBRARY_WITH_PATH($LIBNAME, $FANN_DIR/$PHP_LIBDIR, FANN_SHARED_LIBADD)
    AC_DEFINE(HAVE_FANN,1,[ ])
  ],[
    AC_MSG_ERROR([wrong libfann version or lib not found])
  ],[
    -L$FANN_DIR/$PHP_LIBDIR -lm
  ])
  
  PHP_SUBST(FANN_SHARED_LIBADD)

  PHP_NEW_EXTENSION(fann, fann.c fann_connection.c, $ext_shared)
fi

