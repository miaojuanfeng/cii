dnl $Id$
dnl config.m4 for extension cii

dnl Comments in this file start with the string 'dnl'.
dnl Remove where necessary. This file will not work
dnl without editing.

dnl If your extension references something external, use with:

dnl PHP_ARG_WITH(cii, for cii support,
dnl Make sure that the comment is aligned:
dnl [  --with-cii             Include cii support])

dnl Otherwise use enable:

PHP_ARG_ENABLE(cii, whether to enable cii support,
Make sure that the comment is aligned:
[  --enable-cii           Enable cii support])

if test "$PHP_CII" != "no"; then
  dnl Write more examples of tests here...

  dnl # --with-cii -> check with-path
  dnl SEARCH_PATH="/usr/local /usr"     # you might want to change this
  dnl SEARCH_FOR="/include/cii.h"  # you most likely want to change this
  dnl if test -r $PHP_CII/$SEARCH_FOR; then # path given as parameter
  dnl   CII_DIR=$PHP_CII
  dnl else # search default path list
  dnl   AC_MSG_CHECKING([for cii files in default path])
  dnl   for i in $SEARCH_PATH ; do
  dnl     if test -r $i/$SEARCH_FOR; then
  dnl       CII_DIR=$i
  dnl       AC_MSG_RESULT(found in $i)
  dnl     fi
  dnl   done
  dnl fi
  dnl
  dnl if test -z "$CII_DIR"; then
  dnl   AC_MSG_RESULT([not found])
  dnl   AC_MSG_ERROR([Please reinstall the cii distribution])
  dnl fi

  dnl # --with-cii -> add include path
  dnl PHP_ADD_INCLUDE($CII_DIR/include)

  dnl # --with-cii -> check for lib and symbol presence
  dnl LIBNAME=cii # you may want to change this
  dnl LIBSYMBOL=cii # you most likely want to change this 

  dnl PHP_CHECK_LIBRARY($LIBNAME,$LIBSYMBOL,
  dnl [
  dnl   PHP_ADD_LIBRARY_WITH_PATH($LIBNAME, $CII_DIR/$PHP_LIBDIR, CII_SHARED_LIBADD)
  dnl   AC_DEFINE(HAVE_CIILIB,1,[ ])
  dnl ],[
  dnl   AC_MSG_ERROR([wrong cii lib version or lib not found])
  dnl ],[
  dnl   -L$CII_DIR/$PHP_LIBDIR -lm
  dnl ])
  dnl
  dnl PHP_SUBST(CII_SHARED_LIBADD)

  PHP_NEW_EXTENSION(cii, cii.c, $ext_shared)
fi
