#ifndef TEXTEDIT_GLOBAL_H
#define TEXTEDIT_GLOBAL_H

#include <QtCore/qglobal.h>

#ifdef TEXTEDIT_LIB
# define TEXTEDIT_EXPORT Q_DECL_EXPORT
#else
# define TEXTEDIT_EXPORT Q_DECL_IMPORT
#endif

#endif // TEXTEDIT_GLOBAL_H
