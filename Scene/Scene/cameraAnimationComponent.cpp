#include "stdafx.h"
#include "cameraAnimationComponent.h"

#include "ByyIG.h"
#include "ByyObserver.h"
#include "ByyInputController.h"

ByyCameraAnimationComponent::ByyCameraAnimationComponent(ByyIG& ig) 
	:myIG(ig)
,d_running(false),
	d_loop(true),
	currentPath(0),
	currentPoint(0),
	_point_elapse_time(0),
	_lastTickTime(0),
	_time_multiplier(1),
	myAutoFlyScale(1.0),
	myAutoFly(false)
{
	d_pathGroups=new osg::Group;
	d_pointsGroups=new osg::Group;

	d_pathGroups->setNodeMask(0x0);
	d_pointsGroups->setNodeMask(0x0);

	myRoot=new osg::Group;

	myRoot->addChild(d_pathGroups);
	myRoot->addChild(d_pointsGroups);

	connect(&myIG,SIGNAL(preTick()),SLOT(tick()));
}

ByyCameraAnimationComponent::~ByyCameraAnimationComponent()
{

}

ByyAnimationPath& ByyCameraAnimationComponent::addPath( const std::string& filename/*=""*/ )
{
	ByyAnimationPath path;
	path.read(filename);
	animationPathList.push_back(path);

	createPathPointsNode(path);

	return animationPathList.last();
}

void ByyCameraAnimationComponent::closePath( int index )
{
	animationPathList.remove(index);

	d_pathGroups->removeChild(index);
	d_pointsGroups->removeChild(index);
}

void ByyCameraAnimationComponent::enablePath( bool enable )
{
	d_pathGroups->setNodeMask(enable);
}

void ByyCameraAnimationComponent::enablePoints( bool enable )
{
	d_pointsGroups->setNodeMask(enable);
}

void ByyCameraAnimationComponent::setMultiplier( double value )
{
	_time_multiplier=value;
}

void ByyCameraAnimationComponent::enableLoop( bool enable )
{
	d_loop=enable;
}

void ByyCameraAnimationComponent::start()
{
	_lastTickTime=osg::Timer::instance()->time_s();
	d_running=true;
}

void ByyCameraAnimationComponent::pause()
{
	d_running=false;
}

void ByyCameraAnimationComponent::reset()
{
	currentPath=0;
	currentPoint=0;
	_point_elapse_time=0;
	_lastTickTime=osg::Timer::instance()->time_s();
	_time_multiplier=1;
}

void ByyCameraAnimationComponent::tick()
{
	if (d_running)
	{
		double _timer_s=osg::Timer::instance()->time_s();

		ByyAnimationPath& path=animationPathList[currentPath];
		QVector<MyControlPoint>& controlPointList=path.getControlPointList();

		MyControlPoint& first=controlPointList[currentPoint];
		MyControlPoint& second=controlPointList[currentPoint+1];


		_point_elapse_time+=(_timer_s-_lastTickTime)*_time_multiplier;

		double ratio=_point_elapse_time/second.time;

		static osg::Vec3d _position;
		static osg::Quat _rotation;

		double one_minus_ratio = 1.0-ratio;

		_position = first.ctrlPoints.getPosition()*one_minus_ratio + second.ctrlPoints.getPosition()*ratio;
		_rotation.slerp(ratio,first.ctrlPoints.getRotation(),second.ctrlPoints.getRotation());

		osg::Matrixd _matrix;

		_matrix.makeRotate(_rotation);
		_matrix.postMultTranslate(_position);

		myIG.inputController().currentObserver()->manip()->setByMatrix(_matrix);

		if (ratio>=1.0)
		{
			++currentPoint;

			checkPointChange();
		}

		_lastTickTime=_timer_s;
	}
	else if (myAutoFly)
	{
		osgEarth::Util::EarthManipulator *em=dynamic_cast<osgEarth::Util::EarthManipulator*>(myIG.inputController().currentObserver()->manip());
		if (em)
		{
			em->pan(0,myAutoFlyScale*-0.01);
		}
		
	}
}

void ByyCameraAnimationComponent::checkPointChange()
{
	_point_elapse_time=0;
	_time_multiplier=1;

	ByyAnimationPath& path=animationPathList[currentPath];
	QVector<MyControlPoint>& controlPointList=path.getControlPointList();

	if (currentPoint==controlPointList.size()-1)
	{
		++currentPath;
		currentPoint=0;

		if (currentPath==animationPathList.size())
		{
			if (d_loop)
			{
				reset();
				start();
			}
			else
			{
				emit finish();

				pause();
				reset();

				return;
			}
		}

	}

	emit currentPointChanged(currentPath,currentPoint);
}

ByyAnimationPath& ByyCameraAnimationComponent::getAnimationPath( int index )
{
	return animationPathList[index];
}

void ByyCameraAnimationComponent::setCurrentPathPoint( int path,int point )
{
	currentPath=path;
	currentPoint=point;

	checkPointChange();
}

void ByyCameraAnimationComponent::createPathPointsNode( ByyAnimationPath& animationPath )
{
	/*osgEarth::Symbology::Style d_pointsStyle;

	d_pointsStyle.getOrCreate<osgEarth::Symbology::IconSymbol>()->url()->setLiteral( "../data/icons/placemark32.png" );

	osg::Group *pointsGroup=new osg::Group;

	osg::Vec3d out_local;

	QVector<MyControlPoint>& cpList=animationPath.getControlPointList();

	osgEarth::Symbology::LineString* path = new osgEarth::Symbology::LineString();

	for (int i=0;i!=cpList.size();++i)
	{
		d_mapNode->getMapSRS()->transformFromWorld(cpList[i].ctrlPoints.getPosition(),out_local);

		pointsGroup->addChild(
			new osgEarth::Annotation::PlaceNode(d_mapNode.get(), 
			osgEarth::GeoPoint(d_mapNode->getMapSRS(),out_local.x(),out_local.y()),
			cpList[i].name, d_pointsStyle));

		path->push_back(out_local.x(),out_local.y());
	}

	d_pointsGroups->addChild(pointsGroup);

	osgEarth::Symbology::Style pathStyle;
	pathStyle.getOrCreate<osgEarth::Symbology::LineSymbol>()->stroke()->color() = osgEarth::Color::Red;
	pathStyle.getOrCreate<osgEarth::Symbology::LineSymbol>()->stroke()->width() = 3.0f;
	pathStyle.getOrCreate<osgEarth::Symbology::LineSymbol>()->tessellation()=500;
	pathStyle.getOrCreate<osgEarth::Symbology::AltitudeSymbol>()->clamping() = osgEarth::Symbology::AltitudeSymbol::CLAMP_TO_TERRAIN;
	pathStyle.getOrCreate<osgEarth::Symbology::AltitudeSymbol>()->technique() = osgEarth::Symbology::AltitudeSymbol::TECHNIQUE_SCENE;

	osgEarth::Annotation::FeatureNode *d_pathNode=new osgEarth::Annotation::FeatureNode(d_mapNode.get(),new osgEarth::Features::Feature(path, d_mapNode->getMapSRS(), pathStyle));

	d_pathGroups->addChild(d_pathNode);*/
}

void ByyCameraAnimationComponent::setAutoFly( bool enable )
{
	myAutoFly=enable;
}

void ByyCameraAnimationComponent::setAutoFlyScale( float scale )
{
	myAutoFlyScale=scale;
}
