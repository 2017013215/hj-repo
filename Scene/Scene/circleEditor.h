#ifndef CIRCLEEDITOR_H
#define CIRCLEEDITOR_H

#include "tacticalGraphEditor.h"

class ByyCircle;

class ByyCircleEditor : public ByyTacticalGraphEditor
{
public:
	ByyCircleEditor(ByyIG& ig,ByyCircle* circle);
	~ByyCircleEditor();

	virtual ByyControlObject* accept();

	static ByyTacticalGraphicsCreator* create(ByyIG& ig,ByyControlObject* obj);

private:
	ByyCircle* myCircle;

	osg::ref_ptr<osgEarth::Annotation::CircleNode> myCircleNode;
	osg::ref_ptr<osgEarth::Annotation::CircleNodeEditor> myCircleEditor;
};

#endif // CIRCLEEDITOR_H
