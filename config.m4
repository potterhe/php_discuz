dnl $Id$
dnl config.m4 for extension discuz

dnl Comments in this file start with the string 'dnl'.
dnl Remove where necessary. This file will not work
dnl without editing.

dnl If your extension references something external, use with:

dnl PHP_ARG_WITH(discuz, for discuz support,
dnl Make sure that the comment is aligned:
dnl [  --with-discuz             Include discuz support])

dnl Otherwise use enable:

PHP_ARG_ENABLE(discuz, whether to enable discuz support,
[  --enable-discuz           Enable discuz support])

if test "$PHP_DISCUZ" != "no"; then
  dnl Write more examples of tests here...

  dnl # --with-discuz -> check with-path
  dnl SEARCH_PATH="/usr/local /usr"     # you might want to change this
  dnl SEARCH_FOR="/include/discuz.h"  # you most likely want to change this
  dnl if test -r $PHP_DISCUZ/$SEARCH_FOR; then # path given as parameter
  dnl   DISCUZ_DIR=$PHP_DISCUZ
  dnl else # search default path list
  dnl   AC_MSG_CHECKING([for discuz files in default path])
  dnl   for i in $SEARCH_PATH ; do
  dnl     if test -r $i/$SEARCH_FOR; then
  dnl       DISCUZ_DIR=$i
  dnl       AC_MSG_RESULT(found in $i)
  dnl     fi
  dnl   done
  dnl fi
  dnl
  dnl if test -z "$DISCUZ_DIR"; then
  dnl   AC_MSG_RESULT([not found])
  dnl   AC_MSG_ERROR([Please reinstall the discuz distribution])
  dnl fi

  dnl # --with-discuz -> add include path
  dnl PHP_ADD_INCLUDE($DISCUZ_DIR/include)

  dnl # --with-discuz -> check for lib and symbol presence
  dnl LIBNAME=discuz # you may want to change this
  dnl LIBSYMBOL=discuz # you most likely want to change this 

  dnl PHP_CHECK_LIBRARY($LIBNAME,$LIBSYMBOL,
  dnl [
  dnl   PHP_ADD_LIBRARY_WITH_PATH($LIBNAME, $DISCUZ_DIR/lib, DISCUZ_SHARED_LIBADD)
  dnl   AC_DEFINE(HAVE_DISCUZLIB,1,[ ])
  dnl ],[
  dnl   AC_MSG_ERROR([wrong discuz lib version or lib not found])
  dnl ],[
  dnl   -L$DISCUZ_DIR/lib -lm
  dnl ])
  dnl
  dnl PHP_SUBST(DISCUZ_SHARED_LIBADD)

  PHP_NEW_EXTENSION(discuz, discuz.c, $ext_shared)
fi
