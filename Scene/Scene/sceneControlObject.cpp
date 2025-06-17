#include "StdAfx.h"
#include "sceneControlObject.h"

#include "ByyControlObject.h"
#include "ByyNodeBank.h"
#include "observerSettings.h"

ByySceneControlObject::ByySceneControlObject( ByyControlObject* ctrlObj,ByyIG& ig )
	:ByySceneObject(ctrlObj,ig)
{
	osgEarth::Symbology::Style style;
	style.getOrCreate<osgEarth::Symbology::TextSymbol>()->encoding()=osgEarth::Symbology::TextSymbol::ENCODING_UTF8;
	style.getOrCreate<osgEarth::Symbology::TextSymbol>()->font()="../data/fonts/msyh.ttf";
	style.getOrCreate<osgEarth::Symbology::TextSymbol>()->declutter()=true;

	myLabelNode=new osgEarth::Annotation::LabelNode(mapNode(),osgEarth::GeoPoint(srs(),osg::Vec3()),ctrlObj->name().toUtf8().data(),style);
	myLabelNode->setDynamic(true);
	myLabelNode->setNodeMask(ByyObserverSettings::instance().ctrlObjLableVisible());
	setForceType(ctrlObj->forceType());

	myRoot->addChild(myLabelNode);
	
	ByyNodeBank::instance().addNode(myRoot,ByyNodeBank::ControlObject,ByyNodeBank::MainObject,ctrlObj->objectId());

	connect(ctrlObj,SIGNAL(clampToTerrainChanged(bool)),this,SLOT(setClampToTerrain(bool)));
	connect(&ByyObserverSettings::instance(),SIGNAL(ctrlObjLableVisibleChanged(bool)),this,SLOT(setCtrlObjLableVisible(bool)));
	setClampToTerrain(ctrlObj->clampToTerrain());
}

ByySceneControlObject::~ByySceneControlObject()
{
	ByyNodeBank::instance().removeNode(myRoot);
}

void ByySceneControlObject::setCtrlObjLableVisible(bool visible)
{
	if(myLabelNode==0)
		return;
	myLabelNode->setNodeMask(visible);
}

void ByySceneControlObject::setName( const QString& text )
{
	myLabelNode->setText(text.toUtf8().data());
}

void ByySceneControlObject::setForceType( ByyForceType side )
{
	osgEarth::Symbology::Style style=myLabelNode->getStyle();

	updateTextStyle(side,style);

	myLabelNode->setStyle(style);
}

void ByySceneControlObject::setClampToTerrain( bool clamp )
{
	osgEarth::Symbology::Style style=myLabelNode->getStyle();

	updateClampStyle(clamp,style);

	myLabelNode->setStyle(style);
}

void ByySceneControlObject::updateClampStyle( bool clamp,osgEarth::Symbology::Style& style )
{
	if (clamp)
	{
		style.getOrCreate<osgEarth::Symbology::AltitudeSymbol>()->clamping()=osgEarth::Symbology::AltitudeSymbol::CLAMP_TO_TERRAIN;
	}
	else
		style.getOrCreate<osgEarth::Symbology::AltitudeSymbol>()->clamping()=osgEarth::Symbology::AltitudeSymbol::CLAMP_NONE;
}

void ByySceneControlObject::update()
{

}
