#ifndef LINESTRINGTYPECREATOR_H
#define LINESTRINGTYPECREATOR_H

#include "pointSetCreator.h"

class ByyLineStringVisualizer : public ByySceneObjCreateVisualizer
{
public:
	ByyLineStringVisualizer(ByyIG& ig);
	virtual ~ByyLineStringVisualizer();

	virtual void updateLineString(const ByyVec3dArray& pnts);

protected:
	osg::ref_ptr<osgEarth::Symbology::LineString>	myPathLine;
	osg::ref_ptr<osgEarth::Features::Feature>		myPathFeature;
	osg::ref_ptr<osgEarth::Annotation::FeatureNode> myPathNode;
};

class ByyLineStringTypeCreator: public ByyPointSetCreator,public ByyLineStringVisualizer
{
public:
	ByyLineStringTypeCreator(ByyIG& ig);
	virtual ~ByyLineStringTypeCreator();

protected:
	virtual void update(const QVector<osg::Vec3d>& pts);
	virtual ByyVec3dArray calcPlots(const ByyVec3dArray& srcPoints);
	
};


class ByyFixedLineStringTypeCreator : public ByyFixedPointSetCreator,public ByyLineStringVisualizer
{
public:
	ByyFixedLineStringTypeCreator(ByyIG& ig);
	virtual ~ByyFixedLineStringTypeCreator();

protected:
	virtual void update(const QVector<osg::Vec3d>& pts);
	virtual ByyVec3dArray calcPlots(const ByyVec3dArray& srcPoints);
};

template<class T>
class ByyLineStringTypeCreatorTemplate : public ByyLineStringTypeCreator
{
public:
	ByyLineStringTypeCreatorTemplate(ByyIG& ig)
		:ByyLineStringTypeCreator(ig)
	{

	}

	virtual ~ByyLineStringTypeCreatorTemplate()
	{

	}

	virtual ByyControlObject* accept()
	{
		T *obj=new T;
		obj->setPoints(myPoints);

		return obj;
	}
};

template<class T>
class ByyFixedLineStringTypeCreatorTemplate : public ByyFixedLineStringTypeCreator
{
public:
	ByyFixedLineStringTypeCreatorTemplate(ByyIG& ig)
		:ByyFixedLineStringTypeCreator(ig)
	{

	}

	virtual ~ByyFixedLineStringTypeCreatorTemplate()
	{

	}

	virtual ByyControlObject* accept()
	{
		T *obj=new T;
		obj->setPoints(myPoints);

		return obj;
	}
};
#endif // LINESTRINGTYPECREATOR_H
