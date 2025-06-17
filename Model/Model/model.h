#ifndef MODEL_H
#define MODEL_H

#include <QString>
#include <QVector>
#include <osgEarthAnnotation/ModelNode>
#include <QtGui/QColor>
#include "model_global.h"

struct VideoInfo
{
	QString name;
	QString url;
	int port;
	QString accout;
	QString password;
	bool allCamera;
	QString cameras;
};


class GlintCallback : public osg::NodeCallback
{
public:
	GlintCallback(osgEarth::Annotation::ModelNode* modelNode);

	void setEnable(bool b);

	virtual void  operator() (osg::Node *node, osg::NodeVisitor *nv) ;

protected:
	int myFrameNumber;
	bool myEnable;

	osg::observer_ptr<osgEarth::Annotation::ModelNode> myModelNode;

};

class  MODEL_EXPORT ByyModel
{
public:
	ByyModel(osgEarth::MapNode* mapNode,const osgEarth::Config& conf);
	~ByyModel();

	osgEarth::Config getConfig();

	void setBuilding(bool b){building=b;}
	bool isBuilding(){return building;}

	void setVideo(bool b){video=b;}
	bool isVideo(){return video;}

	bool useLOD(){return myUseLOD;}
	void setLOD(bool b){myUseLOD=b;}

	double lodRange() const { return myLodRange; }
	void setLodRange(double val) { myLodRange = val; }

	QString name(){return myName;}
	void setName(const QString& name){myName=name;}

	osgEarth::Annotation::ModelNode* getModel(){return myModelNode;}

	osg::Vec3d position(){return myModelNode->getPosition().vec3d();}
	void setPosition( const osgEarth::GeoPoint& p ){myModelNode->setPosition(p);}

	double positionX(){return myModelNode->getPosition().x();}
	double positionY(){return myModelNode->getPosition().y();}
	double positionZ(){return myModelNode->getPosition().z();}
	void setPositionX(double value);
	void setPositionY(double value);
	void setPositionZ(double value);

	double rotationX(){return myModelNode->getLocalRotation().x();}
	double rotationY(){return myModelNode->getLocalRotation().y();}
	double rotationZ(){return myModelNode->getLocalRotation().z();}
	void setRotation(double x,double y,double z);

	double scaleX(){return myModelNode->getScale().x();}
	double scaleY(){return myModelNode->getScale().y();}
	double scaleZ(){return myModelNode->getScale().z();}
	void setScaleX(double value);
	void setScaleY(double value);
	void setScaleZ(double value);

	QString title(){return _title;}
	void setTitle(const QString& t){_title=t;}

	QString content(){return _content;}
	void setContent(const QString& c){_content=c;}

	QColor backgroundColor(){return _backgroundColor;}
	void setBackgroundColor(const QColor& c){_backgroundColor=c;}
	QString backgroundColorName();

	void setWindowSize(int w,int h);
	int windowWidth();
	int windowHeight();

	QVector<VideoInfo>& getVideoInfos(){return videoInfos;}
	void setVideoInfos(QVector<VideoInfo>& infos){videoInfos=infos;}

	bool isEntHeight(){return entheight;}
	void setEntHeight(bool b){entheight=b;}

	double getEntHeightValue(){return entheightValue;}
	void setEntHeightValue(double value){entheightValue=value;}

	void monitor();
	void unMonitor();

	bool isTerrain(){return myIsTerrain;}
	void setIsTerrain(bool b){myIsTerrain=b;}

private:
	bool building;
	bool video;
	bool entheight;
	double entheightValue;
	QString _title;
	QString _content;
	QColor _backgroundColor;
	QString _backgroundColorAlpha;
	int d_width,d_height;

	bool myUseLOD;
	double myLodRange;
	
	QVector<VideoInfo> videoInfos;

	QString myName;
	osg::ref_ptr<osgEarth::Annotation::ModelNode> myModelNode;
	osg::ref_ptr<GlintCallback> myGlintCallback;
	int myMonitorCount;

	bool myIsTerrain;
};

#endif // MODEL_H
