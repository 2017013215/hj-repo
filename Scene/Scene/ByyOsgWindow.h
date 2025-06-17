#ifndef BYYOSGWINDOW_H
#define BYYOSGWINDOW_H

#include <QObject>
#include "scene_global.h"

namespace osgQt
{
	class GLWidget;
	class GraphicsWindowQt;
}

class ByyChannel;
class ByyIG;
class ByyViewWidget;

class SCENE_EXPORT ByyOsgWindow : public QObject
{
	Q_OBJECT

public:
	ByyOsgWindow(ByyIG& ig,const QString& name,ByyOsgWindow* shareWindow=0,QObject *parent=0);
	~ByyOsgWindow();

	osgQt::GLWidget* getAdapterWidget();
	osg::GraphicsContext* getGraphicsContext();

	ByyChannel* addChannel();
	void destoryChannel(ByyChannel* channel);

	int width()const;
	int height()const;

	typedef QList<ByyChannel*>  ChannelList;

	ChannelList& channels();

	const QString& getName();

protected:
	ByyIG& myIG;

	QString myName;
	ChannelList myChannels;
	osgQt::GLWidget *myViewWidget;
};

#endif // BYYOSGWINDOW_H
