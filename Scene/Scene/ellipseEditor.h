#ifndef ELLIPSEEDITOR_H
#define ELLIPSEEDITOR_H

#include "tacticalGraphEditor.h"

class ByyEllipse;

class ByyEllipseEditor  : public ByyTacticalGraphEditor
{
public:
	ByyEllipseEditor(ByyIG& ig,ByyEllipse* ellipse);
	~ByyEllipseEditor();

	virtual ByyControlObject* accept();

	static ByyTacticalGraphicsCreator* create(ByyIG& ig,ByyControlObject* obj);

private:
	ByyEllipse* myEllipse;

	osg::ref_ptr<osgEarth::Annotation::EllipseNode> myEllipseNode;

	osg::ref_ptr<osgEarth::Annotation::EllipseNodeEditor> myEditor;
};

#endif // ELLIPSEEDITOR_H
