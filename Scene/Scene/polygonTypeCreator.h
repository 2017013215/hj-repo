#ifndef POLYGONCREATETEMPLATE_H
#define POLYGONCREATETEMPLATE_H

#include "pointSetCreator.h"

class ByyPolygonVisualizer : public ByySceneObjCreateVisualizer
{
public:
	ByyPolygonVisualizer(ByyIG& ig);
	virtual ~ByyPolygonVisualizer();

	virtual void updatePolygon(const ByyVec3dArray& pnts);

protected:
	osg::ref_ptr<osgEarth::Symbology::Polygon>		myPolygon;
	osg::ref_ptr<osgEarth::Features::Feature>		myPolygonFeature;
	osg::ref_ptr<osgEarth::Annotation::FeatureNode> myPolygonNode;
};

class ByyPolygonTypeCreator: public ByyPointSetCreator,public ByyPolygonVisualizer
{
public:
	ByyPolygonTypeCreator(ByyIG& ig);
	virtual ~ByyPolygonTypeCreator();
	
protected:
	virtual void update(const QVector<osg::Vec3d>& pts);
	virtual ByyVec3dArray calcPlots(const ByyVec3dArray& srcPoints);
	
};

class ByyFixedPolygonTypeCreator : public ByyFixedPointSetCreator,public ByyPolygonVisualizer
{
public:
	ByyFixedPolygonTypeCreator(ByyIG& ig);
	virtual ~ByyFixedPolygonTypeCreator();

protected:
	virtual void update(const QVector<osg::Vec3d>& pts);
	virtual ByyVec3dArray calcPlots(const ByyVec3dArray& srcPoints);
};

template<class T>
class ByyPolygonTypeCreatorTemplate : public ByyPolygonTypeCreator
{
public:
	ByyPolygonTypeCreatorTemplate(ByyIG& ig)
		:ByyPolygonTypeCreator(ig)
	{

	}

	virtual ~ByyPolygonTypeCreatorTemplate()
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
class ByyFixedPolygonTypeCreatorTemplate : public ByyFixedPolygonTypeCreator
{
public:
	ByyFixedPolygonTypeCreatorTemplate(ByyIG& ig)
		:ByyFixedPolygonTypeCreator(ig)
	{

	}

	virtual ~ByyFixedPolygonTypeCreatorTemplate()
	{

	}

	virtual ByyControlObject* accept()
	{
		T *obj=new T;
		obj->setPoints(myPoints);

		return obj;
	}
};

#endif // POLYGONCREATETEMPLATE_H