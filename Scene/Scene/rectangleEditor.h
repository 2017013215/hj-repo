#ifndef RECTANGLEEDITOR_H
#define RECTANGLEEDITOR_H

#include "tacticalGraphEditor.h"
#include <osgEarthAnnotation/RectangleNode>
#include <osgEarthAnnotation/AnnotationEditing>

class ByyRectangle;

class SCENE_EXPORT ByyRectangleEditor : public ByyTacticalGraphEditor
{
public:
	ByyRectangleEditor(ByyIG& ig,ByyRectangle *rectangle);
	~ByyRectangleEditor();

	virtual ByyControlObject* accept();

	static ByyTacticalGraphicsCreator* create(ByyIG& ig,ByyControlObject* obj);

private:
	ByyRectangle *myRectangle;

	osg::ref_ptr<osgEarth::Annotation::RectangleNode> myRectNode;

	osg::ref_ptr<osgEarth::Annotation::RectangleNodeEditor> myEditor;
};

#endif // RECTANGLEEDITOR_H
