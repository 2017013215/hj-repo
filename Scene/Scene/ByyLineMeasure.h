#ifndef BYYLINEMEASURE_H
#define BYYLINEMEASURE_H

#include "ByyMeasureItem.h"
#include "lineStringTypeCreator.h"

class ByyLineMeasure : public ByyMeasureItem,public ByyLineStringVisualizer
{
public:
	ByyLineMeasure(ByyIG& ig);
	~ByyLineMeasure();

	void addPoint(const osgEarth::GeoPoint& point,bool refresh=true);

	void mouseMove(const osgEarth::GeoPoint& point);

protected:
	ByyVec3dArray myPoints;
	osg::ref_ptr<osgEarth::Annotation::LabelNode> myLabelNode;
};

class ByyLineMeasureCreator : public ByyMeasureItemCreator
{
public:
	ByyMeasureItem* create(ByyIG& ig){return new ByyLineMeasure(ig);}
};

#endif // BYYLINEMEASURE_H
