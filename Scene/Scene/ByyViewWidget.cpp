#include "StdAfx.h"
#include "ByyViewWidget.h"
#include "ByyIG.h"
#include "ByyDisplay.h"

ByyViewWidget::ByyViewWidget( ByyIG& ig,ByyOsgWindow* window,osg::GraphicsContext::Traits *contextTraits,
	QWidget *parent/*=NULL*/,const QGLWidget *shareWidget/*=NULL*/,Qt::WindowFlags f/*=0*/ ) 
	:osgQt::GLWidget(osgQt::GraphicsWindowQt::traits2qglFormat(contextTraits),parent,shareWidget,f)
	,myIG(ig)
	,myWindow(window)
{
	setWindowTitle(contextTraits->windowName.c_str());
	move(contextTraits->x,contextTraits->y);

	if (!contextTraits->supportsResize ) 
		setFixedSize( contextTraits->width, contextTraits->height );
	else
		resize( contextTraits->width, contextTraits->height );

	init(contextTraits);
}

ByyViewWidget::~ByyViewWidget()
{

}

void ByyViewWidget::init( osg::GraphicsContext::Traits *contextTraits )
{
	contextTraits->inheritedWindowData=new osgQt::GraphicsWindowQt::WindowData(this);

	myContext=new osgQt::GraphicsWindowQt(contextTraits);
}

osgViewer::GraphicsWindow* ByyViewWidget::graphicsWindow()
{
	return myContext;
}

void ByyViewWidget::closeEvent( QCloseEvent *event )
{
	myIG.display().destroyWindow(myWindow);
	__super::closeEvent(event);
}