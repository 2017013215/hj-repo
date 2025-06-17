#ifndef POLYGONEDITOR_H
#define POLYGONEDITOR_H

#include "tacticalGraphEditor.h"

class ByyPolygon;

class ByyPolygonEditor : public ByyTacticalGraphEditor
{
public:
	ByyPolygonEditor(ByyIG& ig,ByyPolygon* polygon);
	~ByyPolygonEditor();

	virtual ByyControlObject* accept();

	static ByyTacticalGraphicsCreator* create(ByyIG& ig,ByyControlObject* obj);

protected:
	osg::ref_ptr<osgEarth::Symbology::Polygon>		myOEPolygon;
	osg::ref_ptr<osgEarth::Features::Feature>		myPoyFeature;
	osg::ref_ptr<osgEarth::Annotation::FeatureNode> myPoyNode;

	osg::ref_ptr<osgEarth::Annotation::FeatureEditor> myEditor;

	ByyPolygon* myPolygon;
};

#endif // POLYGONEDITOR_H
