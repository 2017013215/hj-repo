#pragma once
#include "tacticalgrapheditor.h"
#include "ByyArc.h"
class ByyArcEditor :	public ByyTacticalGraphEditor
{
public:
	ByyArcEditor(ByyIG& ig,ByyArc* arc);
	~ByyArcEditor(void);

	virtual ByyControlObject* accept();

	static ByyTacticalGraphicsCreator* create(ByyIG& ig,ByyControlObject* obj);
protected:
	osg::ref_ptr<osgEarth::Symbology::LineString>		myArcNode;
	osg::ref_ptr<osgEarth::Features::Feature>		myFeature;
	osg::ref_ptr<osgEarth::Annotation::FeatureNode> myNode;

	osg::ref_ptr<osgEarth::Annotation::FeatureEditor> myEditor;

	ByyArc* myArc;
};

