#include "StdAfx.h"
#include "detectIndicateVisualizer.h"
#include "ByyDetectObject.h"
#include "entitySettings.h"
#include "ByyIG.h"
#include "osgFileCache.h"
#include "placeNode.h"
#include "utils.h"
#include "observerSettings.h"
#include "ByyNodeBank.h"
#include "terrain.h"

ByyDetectIndicateVisualizer::ByyDetectIndicateVisualizer( ByyDetectObject& obj,ByyIG& ig )
	:myObj(obj)
	,myIG(ig)
	,myMapNode(ig.terrain()->mapNode())
{
	myRoot=new osg::Group;

	osgEarth::Symbology::Style style;
	style.getOrCreate<osgEarth::Symbology::TextSymbol>()->encoding()=osgEarth::Symbology::TextSymbol::ENCODING_UTF8;
	style.getOrCreate<osgEarth::Symbology::TextSymbol>()->font()="../data/fonts/msyh.ttf";
	style.getOrCreate<osgEarth::Symbology::IconSymbol>()->alignment()=osgEarth::Symbology::IconSymbol::ALIGN_CENTER_CENTER;
	style.getOrCreate<osgEarth::Symbology::IconSymbol>()->declutter()=false;

	myIndicateNode=new osgEarth::Annotation::ByyPlaceNode(myMapNode.get(),osgEarth::GeoPoint(myMapNode->getMapSRS(),myObj.position()),loadIcon(),myObj.name().toUtf8().data(),style);

	myIndicateNode->setDynamic(true);

	updateForceType();

	myRoot->addChild(myIndicateNode);

	//ByyNodeBank::instance().addPlaceNode(myIndicateNode,entObj.objectId());

	setIconVisible(ByyEntitySettings::instance().entityLabelEnabled());
	setTextVisible(ByyEntitySettings::instance().entityNameEnabled());

	//setVisible(myEntObj.visible());

	connect(&ByyEntitySettings::instance(),SIGNAL(entityLabelEnabledChanged(bool)),SLOT(setIconVisible(bool)));
	connect(&ByyEntitySettings::instance(),SIGNAL(entityNameEnabledChanged(bool)),SLOT(setTextVisible(bool)));

	connect(&myObj,SIGNAL(posChanged(osg::Vec3d)),this,SLOT(setPosition(osg::Vec3d)));
	connect(&myObj,SIGNAL(forceTypeChanged(ByyForceType)),this,SLOT(updateForceType()));
	connect(&myObj,SIGNAL(nameChanged(const QString&)),this,SLOT(setText(const QString&)));
	connect(&myObj,SIGNAL(visibleChanged(bool)),SLOT(setVisible(bool)));

	connect(&ByyObserverSettings::instance(),SIGNAL(projectionChanged(bool)),this,SLOT(set2D(bool)));
}

ByyDetectIndicateVisualizer::~ByyDetectIndicateVisualizer()
{
	//ByyNodeBank::instance().removePlaceNode(myIndicateNode);
}

void ByyDetectIndicateVisualizer::setIconVisible( bool v )
{
	myIndicateNode->setIconVisible(v);
}

void ByyDetectIndicateVisualizer::setTextVisible( bool v )
{
	myIndicateNode->setTextVisible(v);
}

void ByyDetectIndicateVisualizer::setText( const QString& text )
{
	myIndicateNode->setText(text.toUtf8().data());
}

void ByyDetectIndicateVisualizer::updateForceType()
{
	myIndicateNode->setIconColor(forceTypeColorAlpha(myObj.forceType(),0.6));
}

void ByyDetectIndicateVisualizer::setPosition( osg::Vec3d pos )
{
	const ByyEntityType& et=myObj.entityType();

	bool clampGround=ByyEntitySettings::instance().groundClampEnabled()&&et.kind==1&&et.domain==1;

	osgEarth::GeoPoint gp=osgEarth::GeoPoint(myMapNode->getMapSRS(),pos);

	if (clampGround)
	{
		gp.z()=0;
		gp.altitudeMode()=osgEarth::ALTMODE_RELATIVE;
	}

	myIndicateNode->setPosition(gp);
}

void ByyDetectIndicateVisualizer::set2D( bool is2d )
{
	myIndicateNode->setIconImage(loadIcon());
}

const QString& ByyDetectIndicateVisualizer::theType()
{
	static QString _type("DetectIndicate");
	return _type;
}

const QString& ByyDetectIndicateVisualizer::type()
{
	return theType();
}

osg::Image* ByyDetectIndicateVisualizer::loadIcon()
{
	osg::Image *image;

	if (ByyObserverSettings::instance().is2D())
		image=loadEntityIcon(myObj.entityType().string(),myIG);
	else
		image=myIG.fileCache().readImageFile("../data/Overlays/circle.png");

	return image;
}
//必须把place的nodemask设置为0或者1 ，屏幕点选的要是要做这个判断
void ByyDetectIndicateVisualizer::setVisible( bool bl )
{
	myIndicateNode->setNodeMask(bl);
}

osg::Group * ByyDetectIndicateVisualizer::root()
{
	return myRoot;
}
