#include "StdAfx.h"
#include "sceneEntity.h"
#include "ByyIG.h"
#include "entityObject.h"
#include "fltEntityVisitor.h"
#include "utils.h"
#include "entitySettings.h"
#include "audioSettings.h"
#include "mappingQuery.h"
#include "osgFileCache.h"
#include "visualizerFactory.h"
#include "stateVisualizer.h"
#include "observerSettings.h"
#include "ByyNodeBank.h"
#include "indicateVisualizer.h"


#include <osgAudio/SoundState.h>
#include <osgAudio/SoundUpdateCB.h>
#include <osgAudio/SoundManager.h>

ByySceneEntity::ByySceneEntity( ByyEntityObject* entObj,ByyIG& ig ) 
	:ByySceneObject(entObj,ig)
,myEntityObject(entObj)
{
	connect(&ByyEntitySettings::instance(),SIGNAL(groundClampEnabledChanged(bool)),SLOT(update()));
	connect(&ByyEntitySettings::instance(),SIGNAL(autoScaleEnabledChanged(bool)),SLOT(setAutoScaleEnabled(bool)));

	connect(&ByyAudioSettings::instance(),SIGNAL(enableChanged(bool)),SLOT(enableSound(bool)));
	connect(&ByyAudioSettings::instance(),SIGNAL(volumeChanged(float)),SLOT(setSoundVolume(float)));
	connect(&ByyAudioSettings::instance(),SIGNAL(rangeChanged(int)),SLOT(setSoundRange(int)));

	connect(&ByyObserverSettings::instance(),SIGNAL(projectionChanged(bool)),this,SLOT(set2D(bool)));

	connect(myEntityObject,SIGNAL(positionChanged(const osg::Vec3d&)),this,SLOT(setPosition(const osg::Vec3d&)));
	connect(myEntityObject,SIGNAL(orientationChanged(const osg::Vec3d&)),this,SLOT(setOrientation(const osg::Vec3d&)));
	connect(myEntityObject,SIGNAL(damageStateChanged(ByyDamageState)),this,SLOT(setDamageState(ByyDamageState)));

	myLocalizedGroup=new osg::Group;
}

ByySceneEntity::~ByySceneEntity()
{
	if (soundState)
	{
		osgAudio::SoundManager::instance()->removeSoundState(soundState);
	}

	for (int i=0;i!=myStateVisualizerList.size();++i)
	{
		myRoot->removeChild(myStateVisualizerList[i]->root());
	}

	qDeleteAll(myStateVisualizerList);

	ByyNodeBank::instance().removeNode(myModel);
}

void ByySceneEntity::initiate()
{
	if(!myEntityObject)
		return;

	osg::Node*node=loadEntityModel(myEntityObject->entityType().string(),myIG);

	if (!node)
		return;

	ByyFltEntityVisitor v;
	node->accept(v);

	damageSwitch=v.getDamageSwith();
	healthSwitch=v.getHealthSwith();
	artMaps=v.getArticulatedParts();
	movingSwitch=v.getMovingSwith();

	myModel=node;

	ByyNodeBank::instance().addNode(myModel,ByyNodeBank::Entity,ByyNodeBank::MainObject,myEntityObject->objectId());

	myAutoScaleTransform = new osgEarth::PixelAutoTransform;
	myAutoScaleTransform->addChild(node);
	myAutoScaleTransform->setMinPixelWidthAtScaleOne(300);

	myAutoScaleTransform->setAutoScaleToScreen(ByyEntitySettings::instance().autoScaleEnabled());
	myAutoScaleTransform->setPosition(osg::Vec3d(0,0,0));

	myLocalizedGroup->addChild(myAutoScaleTransform);

	myLocalNode=new osgEarth::Annotation::LocalGeometryNode(mapNode(),myLocalizedGroup);

	myLocalNodeMatrixTransfrom=dynamic_cast<osg::MatrixTransform*>(myLocalNode->getChild(0));

	myRoot->addChild(myLocalNode);


	myStateVisualizerList=ByyVisualizerFactory::instance().createVisualizer(myEntityObject->entityType(),*myEntityObject,myIG);

	for (int i=0;i!=myStateVisualizerList.size();++i)
	{
		if(myStateVisualizerList[i]->isLocalized())

			myLocalizedGroup->addChild(myStateVisualizerList[i]->root());
		else

			myRoot->addChild(myStateVisualizerList[i]->root());
	}

	set2D(ByyObserverSettings::instance().is2D());

	setPosition(myEntityObject->position());
	setOrientation(myEntityObject->orientation());
	setDamageState(myEntityObject->damageState());
	setWake(); // xks
}

void ByySceneEntity::setPosition( const osg::Vec3d& pos )
{
	if(myEntityObject == NULL)
		return;

	const ByyEntityType& et=myEntityObject->entityType();

	bool clampGround=ByyEntitySettings::instance().groundClampEnabled()&&et.kind==1&&et.domain==1;

	osgEarth::GeoPoint gp=osgEarth::GeoPoint(srs(),pos);

	if (clampGround)
	{
		gp.z()=0;
		gp.altitudeMode()=osgEarth::ALTMODE_RELATIVE;
	}
	if(myLocalNode)
		myLocalNode->setPosition(gp);
}

void ByySceneEntity::setOrientation( const osg::Vec3d& ori )
{
	if(myLocalNode == NULL  || myEntityObject == NULL)
		return;

	myLocalNode->setLocalRotation(osg::Quat(
		osg::inDegrees(ori.z()),osg::Y_AXIS,
		osg::inDegrees(ori.y()),osg::X_AXIS,
		osg::inDegrees(-ori.x()),osg::Z_AXIS));

	if(myEntityObject->entityType().kind == 3)
		return;

	QList<ByyStateVisualizer*>& stateVisualizers=this->getStateVisualizerList();
	for (int i=0;i!=stateVisualizers.size();++i)
	{
		ByyIndicateVisualizer* indicateVisualizer=dynamic_cast<ByyIndicateVisualizer*>(stateVisualizers[i]);

		if (!indicateVisualizer)
			continue;

		if (&indicateVisualizer->getEntObj() == myEntityObject)
		{
			if(!indicateVisualizer->isLocalized())
			{
				indicateVisualizer->setOrientation(ori);
			}
		}
	}
}

void ByySceneEntity::setDamageState( ByyDamageState ds )
{
	if(ds<ByyDamageDestroyed&&mySmoke.valid())
	{
		if(myAutoScaleTransform.valid())
		{
			myAutoScaleTransform->removeChild(mySmoke);
			mySmoke=0;

			myAutoScaleTransform->removeChild(myFlames);
			myFlames=0;
		}
	}
	else if (ds==ByyDamageDestroyed&&!mySmoke.valid())
	{
		mySmoke=/*myIG.fileCache().*/osgDB::readNodeFile("../data/models/SnModels/Vehicles/Smoke.osgb"/*,osg::CopyOp::DEEP_COPY_ALL*/);
		osgEarth::Registry::shaderGenerator().run(mySmoke);
		if(myAutoScaleTransform.valid())
		{
			myAutoScaleTransform->addChild(mySmoke);

			//myFlames=/*myIG.fileCache().*/osgDB::readNodeFile("../data/models/SnModels/Vehicles/Flames.osgb"/*,osg::CopyOp::DEEP_COPY_ALL*/);
			myFlames=/*myIG.fileCache().*/osgDB::readNodeFile("../data/models/Effect/explosion.osg"/*,osg::CopyOp::DEEP_COPY_ALL*/);
			osgEarth::Registry::shaderGenerator().run(myFlames);
			myAutoScaleTransform->addChild(myFlames);
		}
	}

	if (damageSwitch.valid())
	{
		damageSwitch->setSingleChildOn(0,ds==0||ds==1?0:1);
	}
	else if (healthSwitch.valid())
	{
		healthSwitch->setSingleChildOn(0,ds==0||ds==1||ds==2?0:1);
	}
}

//xkx
void ByySceneEntity::setWake()
{
	if(myWake==0 && myEntityObject && myEntityObject->entityType().kind == 2 && 
		(myEntityObject->entityType().category == 1 || myEntityObject->entityType().domain ==10 || myEntityObject->entityType().domain ==11) )
	{
		myWake=/*myIG.fileCache().*/osgDB::readNodeFile("../data/models/Effect/weiyan.osg"/*,osg::CopyOp::DEEP_COPY_ALL*/);
		osgEarth::Registry::shaderGenerator().run(myWake);
		myAutoScaleTransform->addChild(myWake);
	}
}

void ByySceneEntity::update()
{
	//updateArtParts();
	//updateMoveState();
}

void ByySceneEntity::updateArtParts()
{
	if(myEntityObject == NULL)
		return;

	ByyArticulatedPartCollection *parts=myEntityObject->artPartList();

	if (parts && parts->partCount()!=0)
	{
		ByyArticulatedPartCollection::PartTypeSet partTypeSet;
		parts->getPartTypes(partTypeSet);
		ByyArticulatedPartCollection::PartTypeSet::iterator setItr = partTypeSet.begin();
		for(;setItr != partTypeSet.end();setItr++)
		{
			int currentPartType =*setItr;
			ByyArticulatedPart* currentPart = parts->findPart(currentPartType);
			if(currentPart==NULL)
			{
				continue;
			}

			QMultiMap<int,osgSim::DOFTransform*>::const_iterator it = artMaps.find(currentPartType);
			for (;it != artMaps.end() && it.key() ==currentPartType;++it) 
			{
				QVector<ByyArticulatedPart::ParameterMetric> metrics;
				currentPart->getParameterMetrics(metrics);
				osg::Vec3 hpr;
				osg::Vec3 translation;
				osg::Vec3 hprIncrement;

				for(int i=0;i<metrics.size();i++)
				{
					int parameterType = (int)metrics[i];
					float value = currentPart->getParameterValue(metrics[i]);
					switch(parameterType)
					{
						//disenums.h DtArtParamType
					case 1://DtApPosition
						break;
					case 2://DtApPositionRate
						break;
					case 3://DtApExtension
						break;
					case 4://DtApExtensionRate
						break;
					case 5://DtApX
						break;
					case 6://DtApXRate
						break;
					case 7://DtApY
						break;

					case 8://DtApYRate
						break;
					case 9://DtApZ
						break;
					case 10://DtApZRate
						break;
					case 11://DtApAzimuth ·½Î»½Ç£¨ÅÚËþ£©
						hpr._v[0] = value;
						break;
					case 12://DtApAzimuthRate
						hprIncrement._v[0]=value;
						break;
					case 13://DtApElevation Ñö½Ç£¨ÅÚËþ£©
						hpr._v[1] = value;
						break;
					case 14://DtApElevationRate
						hprIncrement._v[1] = value;
						break;

					case 15://DtApRotation
						break;
					case 16://DtApRotationRate
						break;


					default:
						break;

					}
				}

				it.value()->updateCurrentHPR(hpr);


			}
		}
	}
}

void ByySceneEntity::updateMoveState()
{
	if (movingSwitch.valid())
	{
		movingSwitch->setSingleChildOn(0,myEntityObject->velocity()==osg::Vec3d()?0:1);
	}
}

osg::Vec3d ByySceneEntity::center()
{
	if(myEntityObject)
		return myEntityObject->position();

	return osg::Vec3d(0,0,0);
}

double ByySceneEntity::radius()
{
	if(myModel)
		return myModel->getBound().radius();

	return 0;
}

void ByySceneEntity::setForceType( ByyForceType side )
{
	
}

void ByySceneEntity::setAutoScaleEnabled( bool e )
{
	if(myAutoScaleTransform)
		myAutoScaleTransform->setAutoScaleToScreen(e);

	if (!e)
	{
		myAutoScaleTransform->setScale(1);
	}
}

osg::Node* ByySceneEntity::modelNode()
{
	return myModel;
}

void ByySceneEntity::setSoundVolume( float value )
{
	if (soundState)
	{
		soundState->setGain(value);
	}
}

void ByySceneEntity::setSoundRange( int range )
{
	if (soundState)
	{
		soundState->setReferenceDistance(range);
	}
}

void ByySceneEntity::enableSound( bool enable )
{
	if (enable)
	{
		soundCB = new osgAudio::SoundUpdateCB;

		soundState=createSoundState(ByyMappingQuery::instance()->getEntityAudio(myEntityObject->entityType().string().toLocal8Bit().data()).toStdString());

		soundCB->setSoundState(soundState);

		myModel->setUpdateCallback(soundCB);
	}
	else
	{
		myModel->removeUpdateCallback(soundCB);
		soundCB=NULL;

		osgAudio::SoundManager::instance()->removeSoundState(soundState);

		soundState=NULL;
	}
}

osg::ref_ptr<osgAudio::SoundState> ByySceneEntity::createSoundState( const std::string& file )
{
	// Create a sample, load a .wav file.
	osg::ref_ptr<osgAudio::Sample> sample = osgAudio::SoundManager::instance()->getSample(file, true);

	// Create a named sound state.
	osg::ref_ptr<osgAudio::SoundState> sound_state = new osgAudio::SoundState(myEntityObject->name().toStdString());

	// Let the soundstate use the sample we just created
	sound_state->setSample(sample);

	// Set its gain (volume) to 0.9
	sound_state->setGain(0.9f);

	// Set its pitch to 1 (normal speed)
	sound_state->setPitch(1);

	// Make it play
	sound_state->setPlay(true);

	// The sound should loop over and over again
	sound_state->setLooping(true);

	// Allocate a hardware soundsource to this soundstate (priority 10)
	//
	sound_state->allocateSource(10, false);

	// At 70 the gain will be half of full!
	sound_state->setReferenceDistance(100);
	sound_state->setRolloffFactor(4);
	sound_state->apply();

	// Add the soundstate to the sound manager, so we can find it later on if we want to
	osgAudio::SoundManager::instance()->addSoundState(sound_state);

	return sound_state;
}

ByyEntityObject* ByySceneEntity::entityObject()
{
	return myEntityObject;
}

osg::Vec3d ByySceneEntity::worldPosition()
{
	if(myLocalNodeMatrixTransfrom.valid())
		return myLocalNodeMatrixTransfrom->getMatrix().getTrans();

	return osg::Vec3d(0,0,0);
}

void ByySceneEntity::set2D( bool is2d )
{
	if(myAutoScaleTransform)
		myAutoScaleTransform->setNodeMask(!is2d);
}

QList<ByyStateVisualizer*>& ByySceneEntity::getStateVisualizerList()
{
	return myStateVisualizerList;
}
