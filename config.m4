dnl $Id$
dnl config.m4 for extension xml2array

dnl Comments in this file start with the string 'dnl'.
dnl Remove where necessary. This file will not work
dnl without editing.

dnl If your extension references something external, use with:

PHP_ARG_WITH(xml2array, for xml2array support,
dnl Make sure that the comment is aligned:
[  --with-xml2array             Include xml2array support])

dnl Otherwise use enable:

dnl PHP_ARG_ENABLE(xml2array, whether to enable xml2array support,
dnl Make sure that the comment is aligned:
dnl [  --enable-xml2array           Enable xml2array support])

if test "$PHP_XML2ARRAY" != "no"; then
  dnl Write more examples of tests here...

  dnl # --with-xml2array -> check with-path
  dnl SEARCH_PATH="/usr/local /usr"     # you might want to change this
  dnl SEARCH_FOR="/include/xml2array.h"  # you most likely want to change this
  dnl if test -r $PHP_XML2ARRAY/$SEARCH_FOR; then # path given as parameter
  dnl   XML2ARRAY_DIR=$PHP_XML2ARRAY
  dnl else # search default path list
  dnl   AC_MSG_CHECKING([for xml2array files in default path])
  dnl   for i in $SEARCH_PATH ; do
  dnl     if test -r $i/$SEARCH_FOR; then
  dnl       XML2ARRAY_DIR=$i
  dnl       AC_MSG_RESULT(found in $i)
  dnl     fi
  dnl   done
  dnl fi
  dnl
  dnl if test -z "$XML2ARRAY_DIR"; then
  dnl   AC_MSG_RESULT([not found])
  dnl   AC_MSG_ERROR([Please reinstall the xml2array distribution])
  dnl fi

  dnl # --with-xml2array -> add include path
  dnl PHP_ADD_INCLUDE($XML2ARRAY_DIR/include)

  dnl # --with-xml2array -> check for lib and symbol presence
  dnl LIBNAME=xml2array # you may want to change this
  dnl LIBSYMBOL=xml2array # you most likely want to change this 

  dnl PHP_CHECK_LIBRARY($LIBNAME,$LIBSYMBOL,
  dnl [
  dnl   PHP_ADD_LIBRARY_WITH_PATH($LIBNAME, $XML2ARRAY_DIR/$PHP_LIBDIR, XML2ARRAY_SHARED_LIBADD)
  dnl   AC_DEFINE(HAVE_XML2ARRAYLIB,1,[ ])
  dnl ],[
  dnl   AC_MSG_ERROR([wrong xml2array lib version or lib not found])
  dnl ],[
  dnl   -L$XML2ARRAY_DIR/$PHP_LIBDIR -lm
  dnl ])
  dnl
  dnl PHP_SUBST(XML2ARRAY_SHARED_LIBADD)

  PHP_NEW_EXTENSION(xml2array, xml2array.c, $ext_shared,, -DZEND_ENABLE_STATIC_TSRMLS_CACHE=1)
fi
