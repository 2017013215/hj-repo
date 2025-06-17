#ifndef BYYADAPTERQTWIDGET_H
#define BYYADAPTERQTWIDGET_H

#include <osgQt/GraphicsWindowQt>

class ByyIG;
class ByyOsgWindow;

class ByyViewWidget : public osgQt::GLWidget
{
public:
	ByyViewWidget(ByyIG& ig,ByyOsgWindow* window,osg::GraphicsContext::Traits *contextTraits,QWidget *parent=NULL,const QGLWidget *shareWidget=NULL,Qt::WindowFlags f=0);
	~ByyViewWidget();

	osgViewer::GraphicsWindow* graphicsWindow(); 

protected:
	void init(osg::GraphicsContext::Traits *contextTraits);
	void closeEvent(QCloseEvent *event);

protected:
	ByyIG& myIG;
	osg::ref_ptr<osgQt::GraphicsWindowQt> myContext;
	ByyOsgWindow* myWindow;
};

#endif // BYYADAPTERQTWIDGET_H
