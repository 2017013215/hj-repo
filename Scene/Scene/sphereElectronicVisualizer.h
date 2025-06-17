#ifndef ELECTRONICVISUALIZER_H
#define ELECTRONICVISUALIZER_H

#include "stateVisualizer.h"
#include "enums.h"

class ByySphereElectronicVisualizer : public ByyStateVisualizer
{
	Q_OBJECT
public:
	ByySphereElectronicVisualizer(ByyEntityObject& entObj,ByyIG& ig);
	virtual ~ByySphereElectronicVisualizer();

	static const QString& theType();
	virtual const QString& type();

	virtual bool isLocalized()const;

public slots:
	void updateForceType(ByyForceType ft);

protected:
	osg::ref_ptr<osg::Vec4Array> mySphereColors;
	osg::ref_ptr<osg::Geometry> myGeom;
	osg::ref_ptr<osg::Group> myHalfMirror;
};

typedef ByyStateVisualizerCreatorTemplate<ByySphereElectronicVisualizer> ByySphereElectronicVisualizerCreator;

#endif // ELECTRONICVISUALIZER_H
