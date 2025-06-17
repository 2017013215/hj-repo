#ifndef VIEWPOINTMANAGEWIDGET_H
#define VIEWPOINTMANAGEWIDGET_H

#include "abstractManageWidget.h"

#include <osgEarth/MapNode>
#include <osgEarth/Viewpoint>

class ByyViewpointWidget : public ByyAbstractManageWidget
{
	Q_OBJECT
public:
	ByyViewpointWidget(ByySceneController *scene,QWidget *parent);
	~ByyViewpointWidget();

	void init();
	void release();
	void add(const osgEarth::Viewpoint& viewPoint);

protected:
	 virtual bool eventFilter(QObject *obj, QEvent *event);

private:
	void addViewpointToWidget(  const osgEarth::Config& conf );

protected slots:
	void moveUp();
	void moveDown();
	void removeToggle();
	void valueChanged(QtProperty *property, const QVariant &val);
	void gotoToggled();

private:
	osg::ref_ptr<osgEarth::MapNode> d_mapNode;
	QAction *gotoAction;

};

#endif // VIEWPOINTMANAGEWIDGET_H
