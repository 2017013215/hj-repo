#ifndef BYYINTERVISIBILITYLINE_H
#define BYYINTERVISIBILITYLINE_H

#include "ByyObject.h"
#include "ByyMeasureItem.h"

class ByyIntervisibilityLine : public ByyMeasureItem
{
public:
	ByyIntervisibilityLine(ByyIG& ig);
	~ByyIntervisibilityLine();

	void addPoint(const osgEarth::GeoPoint& point,bool refresh=true);

	void mouseMove(const osgEarth::GeoPoint& point);

protected:
	void updateLine();
	bool processEvent(const osgGA::GUIEventAdapter& ea,osgGA::GUIActionAdapter& aa);

private:
	osg::ref_ptr<osg::Group> myRoot;

	ByyVec3dArray myPnts;

	float firstAltJustValue;
	float secondAltJustValue;

	bool hasIntersect;
	osg::Vec3d intersetPoint;

	osg::ref_ptr<osgEarth::Symbology::LineString>	myFirstLine;
	osg::ref_ptr<osgEarth::Features::Feature>		myFirstFeature;
	osg::ref_ptr<osgEarth::Annotation::FeatureNode> myFirstNode;

	osg::ref_ptr<osgEarth::Symbology::LineString>	mySecondLine;
	osg::ref_ptr<osgEarth::Features::Feature>		mySecondFeature;
	osg::ref_ptr<osgEarth::Annotation::FeatureNode> mySecondNode;
};

class ByyIntervisibilityLineCreator : public ByyMeasureItemCreator
{
public:
	ByyMeasureItem* create(ByyIG& ig){return new ByyIntervisibilityLine(ig);}
};

#endif // BYYINTERVISIBILITYLINE_H
