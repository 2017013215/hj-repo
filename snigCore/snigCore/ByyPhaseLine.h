#ifndef PHASELINE_H
#define PHASELINE_H

#include "ByyControlObject.h"

class SNIGCORE_EXPORT ByyPhaseLine : public ByyControlObject
{
	Q_OBJECT
public:
	ByyPhaseLine();
	virtual ~ByyPhaseLine();

	void setStart(const osg::Vec3d& p);
	osg::Vec3d& start();

	void setEnd(const osg::Vec3d& p);
	osg::Vec3d& end();

	virtual const QString& type()const;
	static const QString& theType();

	virtual ByyKeyValueList getConfig();
	virtual void fromConfig(ByyKeyValueList& keyValues);

	virtual QString getAttributeValueSet();

	virtual ByyVec3dArray points()const;
	void setPoints(const ByyVec3dArray& ps);
	void setPoints_xy(const ByyVec3dArray& ps);

signals:
	void startChanged(const osg::Vec3d&);
	void endChanged(const osg::Vec3d&);

private:
	osg::Vec3d myStart;
	osg::Vec3d myEnd;
};

#endif // PHASELINE_H
