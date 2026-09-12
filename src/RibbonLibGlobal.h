#ifndef RIBBONLIBGLOBAL_H
#define RIBBONLIBGLOBAL_H

#include <QtGlobal>

#if defined(RIBBONLIB_STATIC)
#  define RIBBONLIB_EXPORT
#else
#  if defined(RIBBONLIB_BUILD_LIBRARY)
#    define RIBBONLIB_EXPORT Q_DECL_EXPORT
#  else
#    define RIBBONLIB_EXPORT Q_DECL_IMPORT
#  endif
#endif

#endif // RIBBONLIBGLOBAL_H
