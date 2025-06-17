#include "StdAfx.h"
#include "indicateVisualizer.h"
#include "entityObject.h"
#include "entitySettings.h"
#include "ByyIG.h"
#include "osgFileCache.h"
#include "placeNode.h"
#include "utils.h"
#include "observerSettings.h"
#include "ByyNodeBank.h"

ByyIndicateVisualizer::ByyIndicateVisualizer( ByyEntityObject& entObj,ByyIG& ig )
	: ByyStateVisualizer(entObj,ig)
{
	m_isSelected=false;
	osgEarth::Symbology::Style style;
	style.getOrCreate<osgEarth::Symbology::TextSymbol>()->encoding()=osgEarth::Symbology::TextSymbol::ENCODING_UTF8;
	style.getOrCreate<osgEarth::Symbology::TextSymbol>()->font()="../data/fonts/msyh.ttf";
	style.getOrCreate<osgEarth::Symbology::IconSymbol>()->alignment()=osgEarth::Symbology::IconSymbol::ALIGN_CENTER_CENTER;
	style.getOrCreate<osgEarth::Symbology::IconSymbol>()->declutter()=false;

	myIndicateNode=new osgEarth::Annotation::ByyPlaceNode(mapNode(),osgEarth::GeoPoint(mapNode()->getMapSRS(),entObj.position()),loadIcon(),myEntObj.name().toUtf8().data(),style);

	myIndicateNode->setDynamic(true);

	osg::Image* img=myIG.fileCache().readImageFile("../data/symbols/kuang.png");	
	mySelectionNode=new osgEarth::Annotation::ByyPlaceNode(mapNode(),
		osgEarth::GeoPoint(mapNode()->getMapSRS(),entObj.position()),
		img,"",style);
	mySelectionNode->setDynamic(true);

	updateForceType();

	myRoot->addChild(myIndicateNode);
	myRoot->addChild(mySelectionNode);

	ByyNodeBank::instance().addPlaceNode(myIndicateNode,entObj.id());
	//ByyNodeBank::instance().addPlaceNode(mySelectionNode,entObj.objectId());

	setIconVisible(ByyEntitySettings::instance().entityLabelEnabled());
	setTextVisible(ByyEntitySettings::instance().entityNameEnabled());

	setVisible(myEntObj.visible());

	connect(&ByyEntitySettings::instance(),SIGNAL(entityLabelEnabledChanged(bool)),SLOT(setIconVisible(bool)));
	connect(&ByyEntitySettings::instance(),SIGNAL(entityNameEnabledChanged(bool)),SLOT(setTextVisible(bool)));

	connect(&myEntObj,SIGNAL(positionChanged(const osg::Vec3d&)),this,SLOT(setPosition(const osg::Vec3d&)));
	connect(&myEntObj,SIGNAL(forceTypeChanged(ByyForceType)),this,SLOT(updateForceType()));
	connect(&myEntObj,SIGNAL(nameChanged(const QString&)),this,SLOT(setText(const QString&)));
	connect(&myEntObj,SIGNAL(visibleChanged(bool)),SLOT(setVisible(bool)));
	connect(&ByyObserverSettings::instance(),SIGNAL(projectionChanged(bool)),this,SLOT(set2D(bool)));

	connect(&ByySelectionManager::instance(),SIGNAL(currentSelectionChanged(ByySelectionManager::SelectionType,unsigned int)),this,SLOT(onSelectionChanged(ByySelectionManager::SelectionType,unsigned int)));
	//currentSelectionChanged
}

void ByyIndicateVisualizer::onSelectionChanged(ByySelectionManager::SelectionType type,unsigned int id)
{
	if(type==ByySelectionManager::Entity)
	{
		selectChanged(myEntObj.objectId()==id);
	}
}

void ByyIndicateVisualizer::selectChanged(bool selected)
{
	if(m_isSelected==selected)
		return;
	m_isSelected=selected;
	mySelectionNode->setIconVisible(m_isSelected);
}

ByyIndicateVisualizer::~ByyIndicateVisualizer()
{
	ByyNodeBank::instance().removePlaceNode(myIndicateNode);
}

void ByyIndicateVisualizer::setIconVisible( bool v )
{
	myIndicateNode->setIconVisible(v);
	mySelectionNode->setIconVisible(v&m_isSelected);
}

void ByyIndicateVisualizer::setTextVisible( bool v )
{
	myIndicateNode->setTextVisible(v);
}

void ByyIndicateVisualizer::setText( const QString& text )
{
	myIndicateNode->setText(text.toUtf8().data());
}

void ByyIndicateVisualizer::updateForceType()
{
	myIndicateNode->setIconColor(forceTypeColorAlpha(myEntObj.forceType(),0.6));
	mySelectionNode->setIconColor(forceTypeColorAlpha(myEntObj.forceType(),0.6));
}

void ByyIndicateVisualizer::setPosition( const osg::Vec3d& pos )
{
	const ByyEntityType& et=myEntObj.entityType();

	bool clampGround=ByyEntitySettings::instance().groundClampEnabled()&&et.kind==1&&et.domain==1;

	osgEarth::GeoPoint gp=osgEarth::GeoPoint(mapNode()->getMapSRS(),pos);

	if (clampGround)
	{
		gp.z()=0;
		gp.altitudeMode()=osgEarth::ALTMODE_RELATIVE;
	}

	myIndicateNode->setPosition(gp);
	mySelectionNode->setPosition(gp);
}

void ByyIndicateVisualizer::set2D( bool is2d )
{
	myIndicateNode->setIconImage(loadIcon());
	myIndicateNode->setTextVisible(ByyEntitySettings::instance().entityNameEnabled());
	updateForceType();
}

const QString& ByyIndicateVisualizer::theType()
{
	static QString _type("Indicate");
	return _type;
}

const QString& ByyIndicateVisualizer::type()
{
	return theType();
}

osg::Image* ByyIndicateVisualizer::loadIcon()
{
	osg::Image *image;

	if (ByyObserverSettings::instance().is2D())
		image=loadEntityIcon(myEntObj.entityType().string(),myIG);
	else
		//image=myIG.fileCache().readImageFile("../data/Overlays/circle.png");
		image=loadEntityIcon(myEntObj.entityType().string(),myIG);

	return image;
}
//必须把place的nodemask设置为0或者1 ，屏幕点选的要是要做这个判断
void ByyIndicateVisualizer::setVisible( bool bl )
{
	myIndicateNode->setNodeMask(bl);
	mySelectionNode->setNodeMask(bl);
}

void ByyIndicateVisualizer::setOrientation(const osg::Vec3d& ori)
{
	myIndicateNode->setOrientation(ori);
	mySelectionNode->setOrientation(ori);
}
