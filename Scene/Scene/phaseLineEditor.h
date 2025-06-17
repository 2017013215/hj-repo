#ifndef PHASELINEEDITOR_H
#define PHASELINEEDITOR_H

#include "tacticalGraphEditor.h"

class ByyPhaseLine;

class ByyPhaseLineEditor : public ByyTacticalGraphEditor
{
public:
	ByyPhaseLineEditor(ByyIG& ig,ByyPhaseLine* phaseLine);
	~ByyPhaseLineEditor();

	virtual ByyControlObject* accept();

	static ByyTacticalGraphicsCreator* create(ByyIG& ig,ByyControlObject* obj);

private:
	osg::ref_ptr<osgEarth::Symbology::LineString>	myPathLine;
	osg::ref_ptr<osgEarth::Features::Feature>		myPathFeature;
	osg::ref_ptr<osgEarth::Annotation::FeatureNode> myPathNode;

	osg::ref_ptr<osgEarth::Annotation::FeatureEditor> myEditor;

	ByyPhaseLine* myPhaseLine;
};

#endif // PHASELINEEDITOR_H
