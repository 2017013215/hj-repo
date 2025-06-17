#ifndef SCENE_GLOBAL_H
#define SCENE_GLOBAL_H

#include <QtCore/qglobal.h>

#ifdef SCENE_LIB
# define SCENE_EXPORT Q_DECL_EXPORT
#else
# define SCENE_EXPORT Q_DECL_IMPORT
#endif

#endif // SCENE_GLOBAL_H
