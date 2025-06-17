#ifndef DISPLAY_H
#define DISPLAY_H

#include <QObject>
#include <QTimer>

#include "scene_global.h"

class ByyCore;
class ByyDisplay;
class ByyInputController;
class ByySceneObjectManager;
class ByyOsgFileCache;
class ByyOsgRenderer;
class ByyTerrain;
class ByyProp;
class ByyEnvironment;
class ByySoundManager;
class ByyTacticalGraphicsProcesser;
class ByySceneRadioCommsManager;
class ByyEntityLevelDisplayManager;
class ByyMeasureProcesser;
class ByyLogCanvas;
class ByyHdose;

#include "ScaleBar.h"

#include <osgEarthUtil/Controls>
class SCENE_EXPORT ByyIG :  public QObject
{
	Q_OBJECT
public:
	ByyIG(ByyCore& committee);
	~ByyIG();

	static ByyIG* IGInstance(); //xks
	void initiate(ByyHdose* byyhdose);

	inline ByyDisplay& display();
	inline ByyInputController& inputController();
	
	inline ByySceneObjectManager* sceneObjectManager();
	inline ByyOsgFileCache& fileCache();
	inline ByyOsgRenderer* renderer();
	inline ByyTerrain* terrain();
	inline ByyEnvironment& environment();
	inline ByySoundManager& soundManager();
	inline ByyCore& core();
	inline ByyTacticalGraphicsProcesser& tacticalGraphicsProcesser();
	inline ByyMeasureProcesser& measureProcesser();

	inline ByySceneRadioCommsManager& radioCommsManager();

	inline ByyEntityLevelDisplayManager& entityLevelDisplayManager();

	void block(bool bl);

	void initLogCanvas(QObject* src,const char* signal);

	ByyLogCanvas* logCanvas();

	ByyHdose& hdose();

	QWidget*	sceneWidget(); //xks

	void createScaleBar();
public slots:
	void tick();

protected slots:
	void set2D(bool is2d);

signals:
	void preTick();
	void postTick();

protected:
	ByyDisplay *myDisplayEngine;
	ByyInputController *myInputController;
	ByySceneObjectManager* mySceneObjManager;
	ByyOsgFileCache* myOsgFileCache;
	ByyOsgRenderer* myRenderer;
	ByyTerrain* myTerrain;
	ByyProp* myProp;
	ByyEnvironment* myEnvironment;
	ByySoundManager* mySoundManager;
	ByyTacticalGraphicsProcesser* myTacticalGraphicsProcesser;
	ByyMeasureProcesser* myMeasureProcesser;
	ByySceneRadioCommsManager* myRadioCommsManager;

	ByyEntityLevelDisplayManager* myEntityLevelDisplayManager;

	ByyLogCanvas* myLogCanvas;

	ByyCore& myCgf;
	ByyHdose* myHdose;

	QTimer timer;

	bool myBlock;

	osg::ref_ptr<ScaleBar> _scaleBar;  // MapScaleBar

	osg::ref_ptr<osgEarth::Util::Controls::ControlCanvas> _controlCanvas;
};

#endif // DISPLAY_H
