#ifndef BYYDISPLAY_H
#define BYYDISPLAY_H

#include <QObject>
#include "scene_global.h"

class ByyIG;
class ByyOsgWindow;

class SCENE_EXPORT ByyDisplay : public QObject
{
	Q_OBJECT

public:
	ByyDisplay(ByyIG& ig,QObject *parent=0);
	~ByyDisplay();

	void initiate();
	void tick();

	ByyOsgWindow* addWindow();
	void destroyWindow(ByyOsgWindow* window);

	osgViewer::CompositeViewer* getViewer();

	osgViewer::ViewerBase::Views getViews();

	typedef QList<ByyOsgWindow*> WindowList;

	WindowList& windows();
public slots:
	void	tryUpdateNet();
protected:
	ByyIG& myIG;

	osg::ref_ptr<osgViewer::CompositeViewer> myCompositeViewer;
	WindowList myWindowList;

	QTimer timer;
};

#endif // BYYDISPLAY_H
