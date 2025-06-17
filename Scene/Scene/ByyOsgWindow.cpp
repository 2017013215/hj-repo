#include "StdAfx.h"
#include "ByyOsgWindow.h"
#include "ByyChannel.h"
#include "ByyDisplaySignaler.h"
#include "ByyViewWidget.h"

#include <osgQt/GraphicsWindowQt>

ByyOsgWindow::ByyOsgWindow( ByyIG& ig,const QString& name,ByyOsgWindow* shareWindow/*=0*/,QObject *parent/*=0*/ )
	: QObject(parent)
	,myIG(ig)
	,myName(name)
{
	osg::DisplaySettings* ds = osg::DisplaySettings::instance().get();
	osg::ref_ptr<osgQt::GraphicsWindowQt::Traits> traits=new osgQt::GraphicsWindowQt::Traits(ds);

	traits->windowDecoration=false;
	traits->x=0;
	traits->y=0;
	traits->width=100;
	traits->height=100;
	traits->doubleBuffer=true;
	traits->alpha = ds->getMinimumNumAlphaBits();
	traits->stencil = ds->getMinimumNumStencilBits();
	traits->sampleBuffers = ds->getMultiSamples();
	traits->samples = ds->getNumMultiSamples();
	traits->sharedContext=shareWindow?shareWindow->getGraphicsContext():0;
	traits->windowName=name.toStdString();
	traits->quadBufferStereo=ds->getStereo()&&ds->getStereoMode()==osg::DisplaySettings::QUAD_BUFFER;

	Qt::WindowFlags flags = Qt::Window | Qt::CustomizeWindowHint;
	if ( traits->windowDecoration )
		flags |= Qt::WindowTitleHint | Qt::WindowMinMaxButtonsHint | Qt::WindowSystemMenuHint
#if (QT_VERSION_CHECK(4, 5, 0) <= QT_VERSION)
		| Qt::WindowCloseButtonHint
#endif
		;

	myViewWidget=new ByyViewWidget(myIG,this,traits,0,shareWindow?shareWindow->getAdapterWidget():0,flags);
//	osgQt::GraphicsWindowQt *context=new osgQt::GraphicsWindowQt(traits);
//	myViewWidget=context->getGLWidget();

	addChannel();
}

ByyOsgWindow::~ByyOsgWindow()
{
	myViewWidget->close();
	myViewWidget->deleteLater();

	while (myChannels.size()>0)
	{
		destoryChannel(myChannels.last());
	}
}

osgQt::GLWidget* ByyOsgWindow::getAdapterWidget()
{
	return myViewWidget;
}

osg::GraphicsContext* ByyOsgWindow::getGraphicsContext()
{
	return myViewWidget->getGraphicsWindow();
}

ByyChannel* ByyOsgWindow::addChannel()
{
	static int index=1;

	QString channelName=QString::fromLocal8Bit("Í¨µÀ %1").arg(index++);

	ByyChannel *channel=new ByyChannel(myIG,*this,channelName);

	myChannels.append(channel);

	ByyDisplaySignaler::instance()->emitChannelCreated(channel);

	return channel;
}

void ByyOsgWindow::destoryChannel( ByyChannel* channel )
{
	ByyDisplaySignaler::instance()->emitChannelToBeDestroyed(channel);
	myChannels.removeOne(channel);
	delete channel;
}

int ByyOsgWindow::width() const
{
	return myViewWidget->width();
}

int ByyOsgWindow::height() const
{
	return myViewWidget->height();
}

ByyOsgWindow::ChannelList& ByyOsgWindow::channels()
{
	return myChannels;
}

const QString& ByyOsgWindow::getName()
{
	return myName;
}
