#include "StdAfx.h"
#include "ByySceneDetect.h"
#include "ByyIG.h"
#include "ByyDetectObject.h"
#include "fltEntityVisitor.h"
#include "utils.h"
#include "entitySettings.h"
#include "mappingQuery.h"
#include "osgFileCache.h"
#include "visualizerFactory.h"
#include "stateVisualizer.h"
#include "observerSettings.h"
#include "ByyNodeBank.h"
#include "detectIndicateVisualizer.h"

ByySceneDetectObject::ByySceneDetectObject( ByyDetectObject* entObj,ByyIG& ig ) 
	:ByySceneObject(entObj,ig)
,myDetectObject(entObj)
,myIndicateVisualizer(0)
{
	connect(&ByyObserverSettings::instance(),SIGNAL(projectionChanged(bool)),this,SLOT(set2D(bool)));

	connect(myDetectObject,SIGNAL(posChanged(osg::Vec3d)),this,SLOT(updatePositionAndOri()));
	connect(myDetectObject,SIGNAL(courseChanged(double)),this,SLOT(updatePositionAndOri()));
/*
	connect(myDetectObject,SIGNAL(distanceChanged(double)),this,SLOT(updatePositionAndOri()));
	connect(myDetectObject,SIGNAL(elChanged(double)),this,SLOT(updatePositionAndOri()));
	connect(myDetectObject,SIGNAL(azChanged(double)),this,SLOT(updatePositionAndOri()));*/

	//connect(myDetectObject,SIGNAL(orientationChanged(const osg::Vec3d&)),this,SLOT(updatePositionAndOri()));


//	connect(myDetectObject,SIGNAL(damageStateChanged(ByyDamageState)),this,SLOT(setDamageState(ByyDamageState)));

	myLocalizedGroup=new osg::Group;
}

ByySceneDetectObject::~ByySceneDetectObject()
{
	if (myIndicateVisualizer)
	{
		myRoot->removeChild(myIndicateVisualizer->root());

		delete myIndicateVisualizer;
	}
	

	//ByyNodeBank::instance().removeNode(myModel);
}

void ByySceneDetectObject::initiate()
{
	osg::Node*node=loadEntityModel(myDetectObject->entityType().string(),myIG);

	if (!node)
		return;

	myModel=node;

	//ByyNodeBank::instance().addNode(myModel,ByyNodeBank::Detect,ByyNodeBank::MainObject,myDetectObject->objectId());

	myAutoScaleTransform = new osgEarth::PixelAutoTransform;
	myAutoScaleTransform->addChild(node);
	myAutoScaleTransform->setMinPixelWidthAtScaleOne(300);

	myAutoScaleTransform->setAutoScaleToScreen(ByyEntitySettings::instance().autoScaleEnabled());
	myAutoScaleTransform->setPosition(osg::Vec3d(0,0,0));

	myLocalizedGroup->addChild(myAutoScaleTransform);

	myLocalNode=new osgEarth::Annotation::LocalGeometryNode(mapNode(),myLocalizedGroup);

	myLocalNodeMatrixTransfrom=dynamic_cast<osg::MatrixTransform*>(myLocalNode->getChild(0));

	myRoot->addChild(myLocalNode);


	myIndicateVisualizer=new ByyDetectIndicateVisualizer(*myDetectObject,myIG);

	myRoot->addChild(myIndicateVisualizer->root());

	set2D(ByyObserverSettings::instance().is2D());

	updatePositionAndOri();
}

void ByySceneDetectObject::updatePositionAndOri()
{
	myLocalNode->setLocalRotation(osg::Quat(0,osg::Y_AXIS,0,osg::X_AXIS,
		osg::inDegrees(-myDetectObject->course()),osg::Z_AXIS));


	osgEarth::GeoPoint gp=osgEarth::GeoPoint(srs(),myDetectObject->position());

	myLocalNode->setPosition(gp);
}

void ByySceneDetectObject::update()
{
	
}

osg::Vec3d ByySceneDetectObject::center()
{
	return myDetectObject->position();
}

double ByySceneDetectObject::radius()
{
	return myModel->getBound().radius();
}

void ByySceneDetectObject::setForceType( ByyForceType side )
{
	
}

void ByySceneDetectObject::setAutoScaleEnabled( bool e )
{
	myAutoScaleTransform->setAutoScaleToScreen(e);

	if (!e)
	{
		myAutoScaleTransform->setScale(1);
	}
}

osg::Node* ByySceneDetectObject::modelNode()
{
	return myModel;
}

ByyDetectObject* ByySceneDetectObject::detectObject()
{
	return myDetectObject;
}

osg::Vec3d ByySceneDetectObject::worldPosition()
{
	return myLocalNodeMatrixTransfrom->getMatrix().getTrans();
}

void ByySceneDetectObject::set2D( bool is2d )
{
	myAutoScaleTransform->setNodeMask(!is2d);
}