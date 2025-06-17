#include "stdafx.h"
#include "ByyRenderer.h"

ByyOsgRenderer::ByyOsgRenderer(QObject *parent)
	: QObject(parent)
{
	mySceneRoot=new osg::Group;
	myEnvironmentRoot=mySceneRoot;

	myTerrainRoot=new osg::Group;
	myEntityRoot=new osg::Group;
	myControlObjectRoot=new osg::Group;
	myPropRoot=new osg::Group;
	//myCanvas=new osgEarth::Util::Controls::ControlCanvas;

	myEnvironmentRoot->addChild(myTerrainRoot);
	myEnvironmentRoot->addChild(myEntityRoot);
	myEnvironmentRoot->addChild(myPropRoot);
	myEnvironmentRoot->addChild(myControlObjectRoot);
	//myEnvironmentRoot->addChild(myCanvas);
}

ByyOsgRenderer::~ByyOsgRenderer()
{
	myTerrainRoot=0;
	myControlObjectRoot=0;
	myEntityRoot=0;
	myPropRoot=0;
	myEnvironmentRoot=0;
	mySceneRoot=0;
//	myCanvas=0;
}

void ByyOsgRenderer::initiate()
{
	
}

void ByyOsgRenderer::setEnvironmentRootNode( osg::Group* root )
{
	for (int i=0;i!=myEnvironmentRoot->getNumChildren();++i)
	{
		root->addChild(myEnvironmentRoot->getChild(i));
	}

	myEnvironmentRoot->removeChildren(0,myEnvironmentRoot->getNumChildren());
	mySceneRoot->removeChild(myEnvironmentRoot);

	myEnvironmentRoot=root;

	mySceneRoot->addChild(myEnvironmentRoot);
}

osg::Group* ByyOsgRenderer::getSceneRoot()
{
	return mySceneRoot;
}

osg::Group* ByyOsgRenderer::getEnvironmentRoot()
{
	return myEnvironmentRoot;
}

osg::Group* ByyOsgRenderer::getTerrainRoot()
{
	return myTerrainRoot;
}

osg::Group* ByyOsgRenderer::getEntityRoot()
{
	return myEntityRoot;
}

osg::Group* ByyOsgRenderer::getControlObjectRoot()
{
	return myControlObjectRoot;
}

osg::Group* ByyOsgRenderer::getPropRoot()
{
	return myPropRoot;
}

void ByyOsgRenderer::addNodeToRoot( osg::Node* node,RootDomain rd )
{
	domainToRoot(rd)->addChild(node);
}

void ByyOsgRenderer::removeNodeFromRoot( osg::Node* node,RootDomain rd )
{
	domainToRoot(rd)->removeChild(node);
}

osg::Group* ByyOsgRenderer::domainToRoot( RootDomain rd )
{
	osg::Group* result=0;

	switch(rd)
	{
	case SceneRoot:
		result=mySceneRoot;
		break;
	case Environment:
		result=myEnvironmentRoot;
		break;
	case Terrain:
		result=myTerrainRoot;
		break;
	case Entity:
		result=myEntityRoot;
		break;
	case ControlObject:
		result=myControlObjectRoot;
		break;
	case Prop:
		result=myPropRoot;
		break;
	}

	return result;
}

/*
void ByyOsgRenderer::addOverlayControl( osgEarth::Util::Controls::Control *control )
{
	myCanvas->addControl(control);
}
*/
