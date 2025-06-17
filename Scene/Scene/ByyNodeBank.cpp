#include "StdAfx.h"
#include "ByyNodeBank.h"

ByyNodeBank::ByyNodeBank()
{

}

ByyNodeBank::~ByyNodeBank()
{

}

ByyNodeBank& ByyNodeBank::instance()
{
	static ByyNodeBank databack;
	return databack;
}

void ByyNodeBank::addNode( osg::Node* node,MajorType majorType,MinorType minorType,unsigned int objectId )
{
	NodeData nd;
	nd.majorType=majorType;
	nd.minorType=minorType;
	nd.objectId=objectId;

	myNodeDatas[node]=nd;
}

void ByyNodeBank::removeNode( osg::Node* node )
{
	myNodeDatas.remove(node);
}

bool ByyNodeBank::hasNode( osg::Node* node )
{
	return myNodeDatas.contains(node);
}

ByyNodeBank::NodeData ByyNodeBank::getNodeData( osg::Node* node )
{
	QMap<osg::Node*,NodeData>::iterator it=myNodeDatas.find(node);

	if (it!=myNodeDatas.end())
	{
		return it.value();
	}

	return NodeData();
}

void ByyNodeBank::addPlaceNode( osgEarth::Annotation::ByyPlaceNode* node,unsigned int objectId )
{
	myPlaceNodes.insert(node,objectId);
}

void ByyNodeBank::removePlaceNode( osgEarth::Annotation::ByyPlaceNode* node )
{
	myPlaceNodes.remove(node);
}

ByyNodeBank::PlaceNodeList& ByyNodeBank::getPlaceNodeList()
{
	return myPlaceNodes;
}
