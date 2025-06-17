#ifndef ENTITYOBJECTFRAME_H
#define ENTITYOBJECTFRAME_H

#include <QString>
#include <osg/Vec3d>
#include "entityType.h"
#include "snigcore_global.h"
#include "enums.h"

class ByyEntityObject;

class SNIGCORE_EXPORT ByyEntityObjectFrame
{
public:
	ByyEntityObjectFrame(ByyEntityObject* entObj);
	virtual ~ByyEntityObjectFrame();

	void restore(ByyEntityObject* entObj);

private:
	int myId;
	ByyForceType mySide;
	QString myName;
	QString mySuperiorName;

	osg::Vec3d myPosition;   // lat lon high
	osg::Vec3d myOrientation;//  hpr

	double mySpeed;
	osg::Vec3d myVelocity;

	ByyDamageState myDamageState;

	ByyEntityType myEntityType;

	bool myLocalObj;
};

#endif // ENTITYOBJECTFRAME_H
