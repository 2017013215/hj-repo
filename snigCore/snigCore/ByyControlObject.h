#ifndef CONTROLOBJECT_H
#define CONTROLOBJECT_H

#include "ByyObject.h"

class SNIGCORE_EXPORT ByyControlObject : public ByyObject
{
	Q_OBJECT
public:
	ByyControlObject();
	virtual ~ByyControlObject();

	virtual const QString& type()const=0;

	virtual ByyKeyValueList getConfig();
	virtual void fromConfig(ByyKeyValueList& keyValues);

	virtual ByyVec3dArray points()const=0;
	virtual void setPoints(const ByyVec3dArray& ps)=0;

	virtual void setClampToTerrain(bool clamp);
	virtual bool clampToTerrain();


signals:
	void clampToTerrainChanged(bool);

private:
	bool myClampToTerrain;
};

#endif // CONTROLOBJECT_H
