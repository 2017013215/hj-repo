#include "StdAfx.h"
#include "halfSphereElectronicVisualizer.h"

ByyHalfSphereElectronicVisualizer::ByyHalfSphereElectronicVisualizer( ByyEntityObject& entObj,ByyIG& ig )
	:ByySphereElectronicVisualizer(entObj,ig)
{
	myHalfMirror->setNodeMask(0);
}

ByyHalfSphereElectronicVisualizer::~ByyHalfSphereElectronicVisualizer()
{

}

const QString& ByyHalfSphereElectronicVisualizer::theType()
{
	static QString _type("HalfSphereElectronic");
	return _type;
}

const QString& ByyHalfSphereElectronicVisualizer::type()
{
	return theType();
}
