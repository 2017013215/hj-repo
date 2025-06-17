#include "model.h"
#include "math.h"

#include <osgEarth/Registry>
#include <QStringList>

ByyModel::ByyModel( osgEarth::MapNode* mapNode,const osgEarth::Config& conf )
	:d_width(400),
	d_height(300),
	entheightValue(0),
	_backgroundColor(Qt::blue),
	myUseLOD(false),
	myLodRange(5),
	myMonitorCount(0),
	myIsTerrain(false)
{
	myModelNode=new osgEarth::Annotation::ModelNode(mapNode,conf,osgEarth::Registry::instance()->cloneOrCreateOptions());
	_backgroundColor.setAlpha(125);

	myGlintCallback=new GlintCallback(myModelNode);
	myModelNode->addUpdateCallback(myGlintCallback);

	myName=QString::fromUtf8(conf.value("name").c_str());

	entheight=conf.hasChild("entityheight");

	conf.getIfSet("uselod",myUseLOD);
	conf.getIfSet("lodrange",myLodRange);

	conf.getIfSet("terrain",myIsTerrain);

	if (entheight)
	{
		entheightValue=QString::fromUtf8(conf.child("entityheight").value("height").c_str()).toInt();
	}

	building=conf.hasChild("building");

	if (building)
	{
		osgEarth::Config buildingConf=conf.child("building");
		_title=QString::fromUtf8(buildingConf.value("title").c_str());
		_content=QString::fromUtf8(buildingConf.value("content").c_str());
		_backgroundColor=QColor(buildingConf.value("backgroundcolor").c_str());
		_backgroundColorAlpha=buildingConf.value("backgroundcoloralpha").c_str();
		_backgroundColor.setAlpha(_backgroundColorAlpha.toInt());
		d_width=QString::fromUtf8(buildingConf.value("width").c_str()).toInt();
		d_height=QString::fromUtf8(buildingConf.value("height").c_str()).toInt();
	}

	video=conf.hasChild("videos");

	if (video)
	{
		osgEarth::Config videos=conf.child("videos");
		const osgEarth::ConfigSet& children = videos.children();

		for(osgEarth::ConfigSet::const_iterator i = children.begin(); i != children.end(); ++i )
		{
			if (!i->empty())
			{
				VideoInfo info;

				info.name=QString::fromUtf8(i->value("name").c_str());
				info.url=QString::fromUtf8(i->value("url").c_str());
				info.port=QString::fromUtf8(i->value("port").c_str()).toInt();
				info.accout=QString::fromUtf8(i->value("accout").c_str());
				info.password=QString::fromUtf8(i->value("password").c_str());
				info.allCamera=QString::fromUtf8(i->value("allcamera").c_str()).toLower()=="true";
				info.cameras=QString::fromUtf8(i->value("cameras").c_str());

				videoInfos.append(info);
			}
		}
	}
}

ByyModel::~ByyModel()
{

}

osgEarth::Config ByyModel::getConfig()
{
	osgEarth::Config conf=myModelNode->getConfig();
	conf.add("name",myName.toUtf8().data());

	conf.add("uselod",myUseLOD);
	conf.add("lodrange",myLodRange);
	conf.add("terrain",myIsTerrain);

	if (conf.hasChild("position"))
	{
		if (!conf.child("position").hasChild("alt"))
		{
			conf.mutable_child("position")->add("alt",0);
		}
	}

	if (building)
	{
		osgEarth::Config buildingConf("building");

		buildingConf.add("title",_title.toUtf8().data());
		buildingConf.add("content",_content.toUtf8().data());
		buildingConf.add("backgroundcolor",_backgroundColor.name().toUtf8().data());
		buildingConf.add("backgroundcoloralpha",QString::number(_backgroundColor.alpha()).toUtf8().data());
		buildingConf.add("width",QString::number(d_width).toUtf8().data());
		buildingConf.add("height",QString::number(d_height).toUtf8().data());
		conf.add(buildingConf);
	}

	if (entheight)
	{
		osgEarth::Config entHeightConf("entityheight");
		entHeightConf.add("height",entheightValue);

		conf.add(entHeightConf);
	}

	if (video)
	{
		osgEarth::Config videosConf("videos");

		for (int i=0;i!=videoInfos.count();++i)
		{
			osgEarth::Config videoConf("video");
			videoConf.add("name",videoInfos[i].name.toUtf8().data());
			videoConf.add("url",videoInfos[i].url.toUtf8().data());
			videoConf.add("port",videoInfos[i].port);
			videoConf.add("accout",videoInfos[i].accout.toUtf8().data());
			videoConf.add("password",videoInfos[i].password.toUtf8().data());
			videoConf.add("allcamera",videoInfos[i].allCamera);
			videoConf.add("cameras",videoInfos[i].cameras.toUtf8().data());

			videosConf.add(videoConf);
		}
		conf.add(videosConf);
	}

	return conf;
}

void ByyModel::setPositionX( double value )
{
	osgEarth::GeoPoint geopoint=myModelNode->getPosition();
	geopoint.x()=value;
	myModelNode->setPosition(geopoint);
}

void ByyModel::setPositionY( double value )
{
	osgEarth::GeoPoint geopoint=myModelNode->getPosition();
	geopoint.y()=value;
	myModelNode->setPosition(geopoint);
}

void ByyModel::setPositionZ( double value )
{
	osgEarth::GeoPoint geopoint=myModelNode->getPosition();
	geopoint.z()=value;
	myModelNode->setPosition(geopoint);
}

void ByyModel::setRotation( double x,double y,double z )
{
	osg::Quat rotation(x,osg::Vec3(1,0,0),y,osg::Vec3(0,1,0),z,osg::Vec3(0,0,1));
	myModelNode->setLocalRotation(rotation);
}

void ByyModel::setScaleX( double value )
{
	osg::Vec3f scale=myModelNode->getScale();
	scale.x()=value;
	myModelNode->setScale(scale);
}

void ByyModel::setScaleY( double value )
{
	osg::Vec3f scale=myModelNode->getScale();
	scale.y()=value;
	myModelNode->setScale(scale);
}

void ByyModel::setScaleZ( double value )
{
	osg::Vec3f scale=myModelNode->getScale();
	scale.z()=value;
	myModelNode->setScale(scale);
}

void ByyModel::setWindowSize( int w,int h )
{
	d_width=w;
	d_height=h;
}

int ByyModel::windowWidth()
{
	return d_width;
}

int ByyModel::windowHeight()
{
	return d_height;
}

QString ByyModel::backgroundColorName()
{
	return backgroundColor().name()+QString::number(backgroundColor().alpha(),16);
}

void ByyModel::monitor()
{
	myGlintCallback->setEnable(++myMonitorCount);
}

void ByyModel::unMonitor()
{
	if (myMonitorCount>0)
	{
		myGlintCallback->setEnable(--myMonitorCount);
	}
}

GlintCallback::GlintCallback(osgEarth::Annotation::ModelNode* modelNode)
	:myEnable(false),
myFrameNumber(0),
myModelNode(modelNode)
{

}

void GlintCallback::setEnable( bool b )
{
	myEnable=b;

	if ((!myEnable)&&myModelNode.valid())
	{
		myModelNode->getChild(0)->setNodeMask(0x1);
	}
}

void GlintCallback::operator()( osg::Node *node, osg::NodeVisitor *nv )
{
	if (!myEnable)
		return;

	if (!myModelNode)
		return;

	if (myFrameNumber==0)
	{
		myModelNode->getChild(0)->setNodeMask(0x1);
	}
	else if (myFrameNumber==40)
	{
		myModelNode->getChild(0)->setNodeMask(0x0);
	}

	++myFrameNumber;

	if (myFrameNumber==80)
	{
		myFrameNumber=0;
	}
}
