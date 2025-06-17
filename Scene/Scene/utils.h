#ifndef UTILS_H
#define UTILS_H

#include "scene_global.h"
#include "enums.h"

class ByyIG;

SCENE_EXPORT osg::Node* loadEntityModel(const QString& type,ByyIG& ig);
SCENE_EXPORT osg::Image* loadEntityIcon(const QString& type,ByyIG& ig);
SCENE_EXPORT QPixmap loadEntityIcon(const QString& type,ByyForceType ft);
SCENE_EXPORT osg::Vec3 forceTypeColor(const ByyForceType& ft);
SCENE_EXPORT osg::Vec4 forceTypeColorAlpha(const ByyForceType& ft,float alpha);

#define LINE_WIDTH	3
#endif // UTILS_H
