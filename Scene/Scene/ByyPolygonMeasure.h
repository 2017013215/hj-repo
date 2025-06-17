#ifndef BYYPOLYGONMEASURE_H
#define BYYPOLYGONMEASURE_H

#include "ByyMeasureItem.h"
#include "polygonTypeCreator.h"

class ByyPolygonMeasure : public ByyMeasureItem,public ByyPolygonVisualizer
{
public:
	ByyPolygonMeasure(ByyIG& ig);
	~ByyPolygonMeasure();

	void addPoint(const osgEarth::GeoPoint& point,bool refresh=true);

	void mouseMove(const osgEarth::GeoPoint& point);

protected:
	ByyVec3dArray myPoints;
	osg::ref_ptr<osgEarth::Annotation::LabelNode> myLabelNode;
	
};

class ByyPolygonMeasureCreator : public ByyMeasureItemCreator
{
public:
	ByyMeasureItem* create(ByyIG& ig){return new ByyPolygonMeasure(ig);}
};

#endif // BYYPOLYGONMEASURE_H
