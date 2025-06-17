#include "StdAfx.h"
#include "stateVisualizer.h"
#include "ByyIG.h"
#include "terrain.h"

ByyStateVisualizer::ByyStateVisualizer( ByyEntityObject& entObj,ByyIG& ig ) 
	:myEntObj(entObj)
	,myIG(ig)
	,myMapNode(ig.terrain()->mapNode())
{
	myRoot=new osg::Group;
}

ByyStateVisualizer::~ByyStateVisualizer()
{

}

osg::Group* ByyStateVisualizer::root()
{
	return myRoot;
}

osgEarth::MapNode* ByyStateVisualizer::mapNode()
{
	return myMapNode.get();
}

bool ByyStateVisualizer::contributesToBound() const
{
	return false;
}

bool ByyStateVisualizer::isLocalized() const
{
	return false;
}

ByyEntityObject& ByyStateVisualizer::getEntObj()
{
	return myEntObj;
}

void ByyStateVisualizer::setVisible( bool v )
{
	root()->setNodeMask(v);
}
