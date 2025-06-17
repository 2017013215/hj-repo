#ifndef OPERATEWIDGET_H
#define OPERATEWIDGET_H

#include <QWidget>
#include <osg/Group>

class ByySceneController;

class ByyOperateWidget : public QWidget
{
	Q_OBJECT

public:
	ByyOperateWidget(ByySceneController *scene,QWidget *parent);
	~ByyOperateWidget();

	virtual void init()=0;
	virtual void release()=0;

	virtual void writeToMapNode();

signals:
	void modified();

protected:
	ByySceneController* d_scene;
	osg::ref_ptr<osg::Group> d_root;
};

#endif // OPERATEWIDGET_H
