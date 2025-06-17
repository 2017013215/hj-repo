#ifndef BYYINTERVISIBILITYFAN_H
#define BYYINTERVISIBILITYFAN_H

#include "ByyObject.h"
#include "ByyMeasureItem.h"

class ByyIntervisibilityFan : public ByyMeasureItem
{
public:
	ByyIntervisibilityFan(ByyIG& ig);
	~ByyIntervisibilityFan();

	void addPoint(const osgEarth::GeoPoint& point,bool refresh=true);

	void mouseMove(const osgEarth::GeoPoint& point);

protected:
	void updateFan();
	bool processEvent(const osgGA::GUIEventAdapter& ea,osgGA::GUIActionAdapter& aa);

private:
	ByyVec3dArray myPnts;

	float myJustValue;

	osg::ref_ptr<osg::Group> myRoot;
	osg::ref_ptr<osgEarth::MapNode> myMapNode;

	osg::ref_ptr<osgEarth::Annotation::PlaceNode> myPlaceNode;
	osg::ref_ptr<osgEarth::Annotation::CircleNode> myCircleNode;

	struct  LineData
	{
		osg::ref_ptr<osgEarth::Symbology::LineString> myFirstLine;
		osg::ref_ptr<osgEarth::Features::Feature> myFirstFeature;
		osg::ref_ptr<osgEarth::Annotation::FeatureNode> myFirstNode;

		osg::ref_ptr<osgEarth::Symbology::LineString> mySecondLine;
		osg::ref_ptr<osgEarth::Features::Feature> mySecondFeature;
		osg::ref_ptr<osgEarth::Annotation::FeatureNode> mySecondNode;
	};

	QVector<LineData> myLineData;
};

class ByyIntervisibilityFanCreator : public ByyMeasureItemCreator
{
public:
	ByyMeasureItem* create(ByyIG& ig){return new ByyIntervisibilityFan(ig);}
};

#endif // BYYINTERVISIBILITYFAN_H
