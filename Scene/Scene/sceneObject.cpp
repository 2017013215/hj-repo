#include "StdAfx.h"
#include "sceneObject.h"
#include "ByyObject.h"
#include "ByyIG.h"
#include "terrain.h"
#include "utils.h"

ByySceneObject::ByySceneObject( ByyObject* obj,ByyIG& ig ) 
	:myIG(ig)
	,myMapNode(ig.terrain()->mapNode())
	,mySrs(ig.terrain()->srs())
{
	myRoot=new osg::Group;

	setVisible(obj->visible());

	connect(obj,SIGNAL(nameChanged(const QString&)),SLOT(setName(const QString&)));
	connect(obj,SIGNAL(visibleChanged(bool)),SLOT(setVisible(bool)));
	connect(obj,SIGNAL(forceTypeChanged(ByyForceType)),SLOT(setForceType(ByyForceType)));
}

ByySceneObject::~ByySceneObject()
{

}

osg::Group* ByySceneObject::root()
{
	return myRoot;
}

void ByySceneObject::updateTextStyle( ByyForceType side,osgEarth::Symbology::Style& style )
{
	if (side==1)
	{
		style.getOrCreate<osgEarth::Symbology::TextSymbol>()->halo()->color()=osgEarth::Symbology::Color::Red;
	}
	else if (side==2)
	{
		style.getOrCreate<osgEarth::Symbology::TextSymbol>()->halo()->color()=osgEarth::Symbology::Color::Blue;
	}
	else
		style.getOrCreate<osgEarth::Symbology::TextSymbol>()->halo()->color()=osgEarth::Symbology::Color::White;
}

void ByySceneObject::updateLineStyle( ByyForceType side,osgEarth::Symbology::Style& style )
{
	if (side==1)
	{
		style.getOrCreate<osgEarth::Symbology::LineSymbol>()->stroke()->color()=osgEarth::Symbology::Color::Red;
	}
	else if (side==2)
	{
		style.getOrCreate<osgEarth::Symbology::LineSymbol>()->stroke()->color()=osgEarth::Symbology::Color::Blue;
	}
	else
		style.getOrCreate<osgEarth::Symbology::LineSymbol>()->stroke()->color()=osgEarth::Symbology::Color::White;
}

void ByySceneObject::updatePolygonStyle( ByyForceType side,osgEarth::Symbology::Style& style )
{
	style.getOrCreate<osgEarth::Symbology::LineSymbol>()->stroke()->color() = forceTypeColorAlpha(side,0.9);
}

void ByySceneObject::setVisible( bool b )
{
	myRoot->setNodeMask((int)b);
}

const osgEarth::SpatialReference* ByySceneObject::srs()
{
	return mySrs.get();
}

osgEarth::MapNode* ByySceneObject::mapNode()
{
	return myMapNode.get();
}

bool ByySceneObject::visible()
{
	return myRoot->getNodeMask();
}
