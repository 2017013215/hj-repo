#ifndef ROUTEEDITOR_H
#define ROUTEEDITOR_H

#include "tacticalGraphEditor.h"

class ByyRoute;

class ByyRouteEditor : public ByyTacticalGraphEditor
{
public:
	ByyRouteEditor(ByyIG& ig,ByyRoute* route);
	~ByyRouteEditor();

	virtual ByyControlObject* accept();

	static ByyTacticalGraphicsCreator* create(ByyIG& ig,ByyControlObject* obj);

private:
	osg::ref_ptr<osgEarth::Symbology::LineString>	myPathLine;
	osg::ref_ptr<osgEarth::Features::Feature>		myPathFeature;
	osg::ref_ptr<osgEarth::Annotation::FeatureNode> myPathNode;

	osg::ref_ptr<osgEarth::Annotation::FeatureEditor> myEditor;

	ByyRoute* myRoute;
};

#endif // ROUTEEDITOR_H
