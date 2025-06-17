#ifndef CHANNEL_H
#define CHANNEL_H

#include <QObject>
#include "scene_global.h"
#include "snapshoot.h"

namespace osgEarth
{
	namespace Util
	{
		namespace Controls
		{
			class ControlCanvas;
			class Control;
		}
	}
}

class ByyIG;
class ByyOsgWindow;
class ByyObserver;

class SCENE_EXPORT ByyChannel : public QObject
{
	Q_OBJECT
public:
	ByyChannel(ByyIG& ig,ByyOsgWindow& window,const QString& name);
	~ByyChannel();

	static ByyChannel* instance();
	void initiate();

	osgViewer::View* view();

	void setViewport(int x,int y,int width,int height);

	void addToScene(osg::Node* node);
	void removeFromScene(osg::Node* node);

	void addToCanvas(osgEarth::Util::Controls::Control* c);

	void addEventHandler(osgGA::GUIEventHandler* handler);
	void removeEventHandler(osgGA::GUIEventHandler* handler);

	void setObserver(ByyObserver* observer);
	ByyObserver* getObserver();
	
	ByySnapshoot* snapshoot();

	osg::Vec3d worldToScreen(const osg::Vec3d& world,bool invertY=false);

	const QString& getName();

	ByyOsgWindow& window();
	osg::Group*	channelRoot(){return myChannelRoot;}
	osgEarth::Util::Controls::ControlCanvas*	controlCanvas(){return myCanvas;}
public slots:
	void set2D(bool is2d=true);

protected:
	ByyIG& myIG;
	ByyOsgWindow& myWindow;

	QString myName;
	osg::ref_ptr<osgViewer::View> myView;
	osg::ref_ptr<osg::Group> myChannelRoot;
	osg::ref_ptr<osgEarth::Util::Controls::ControlCanvas> myCanvas;

	osg::ref_ptr<ByySnapshoot> mySnapshoot;

	ByyObserver* myObserver;
};

#endif // CHANNEL_H
