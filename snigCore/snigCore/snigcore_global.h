#ifndef SNIGCORE_GLOBAL_H
#define SNIGCORE_GLOBAL_H

#include <QtCore/qglobal.h>

#ifdef SNIGCORE_LIB
# define SNIGCORE_EXPORT Q_DECL_EXPORT
#else
# define SNIGCORE_EXPORT Q_DECL_IMPORT
#endif

#endif // SNIGCORE_GLOBAL_H
