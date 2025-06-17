#ifndef SNIGHDOSE_GLOBAL_H
#define SNIGHDOSE_GLOBAL_H

#include <QtCore/qglobal.h>

#ifdef SNIGHDOSE_LIB
# define SNIGHDOSE_EXPORT Q_DECL_EXPORT
#else
# define SNIGHDOSE_EXPORT Q_DECL_IMPORT
#endif

#endif // SNIGHDOSE_GLOBAL_H
