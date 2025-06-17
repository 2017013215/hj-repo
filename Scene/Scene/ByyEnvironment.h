#ifndef ENVIRONMENTCOMPONENT_H
#define ENVIRONMENTCOMPONENT_H

#include "scene_global.h"

#include <QObject>
#include <QDateTime>

#include <osg/Group>
#include <osg/Fog>

namespace osgViewer
{
	class CompositeViewer;
}

class ByyTritonNode;
class ByySilverLiningNode;
class SilverLiningUserInterface;
class TritonUserInterface;

namespace osgEarth
{
	class MapNode;
}

class ByyIG;
class ByyChannel;

class SCENE_EXPORT ByyEnvironment :public QObject
{
	Q_OBJECT
public:
	ByyEnvironment(ByyIG& ig);
	~ByyEnvironment();

	void initiate();

public slots:
	void setDateTime(const QDateTime& datetime=QDateTime::currentDateTime());
	void setTimeScale(double scale);
	void setAmbient(double value);
	void setSeaLevel(int value);
	void setWind( double speed, double direction );

	void setPrecipitation(int precipitationType,double precipitationRate,double  nearClip = -1, double  farClip = -1, bool  bUseDepthBuffer = false  );

	void setVisibility( double range );
	
	void setCloudLayerType(int type);
	void removeCloudLayer();

	void setOceanVisible(bool b);

public:
	QDateTime dateTime(){return d_dateTime;}
	double ambient(){return d_ambient;}
	double timeScale(){return d_timeScale;}
	int seaLevel();

private slots:
	void timeout();
	void attachChannel(ByyChannel* channel);

	void onTerrainLoaded(osgEarth::MapNode* mapNode);

	void set2D(bool is2d);

private:
	ByyIG& myIG;

	osg::ref_ptr<ByyTritonNode> tritonOcean;
	osg::ref_ptr<ByySilverLiningNode> silverLiningSky;
	osg::ref_ptr<SilverLiningUserInterface> mySilverLiningUserInterface;
	osg::observer_ptr<TritonUserInterface> myTritonUserInterface;

	QDateTime d_dateTime;
	double d_ambient;
	double d_timeScale;

	osg::ref_ptr<osg::Fog> myFog;

	QTimer timer;
};

#endif // ENVIRONMENTCOMPONENT_H
