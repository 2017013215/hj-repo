#ifndef ROADWIDGET_H
#define ROADWIDGET_H

#include "operateWidget.h"
#include <osgEarthAnnotation/FeatureNode>
#include "ui_roadWidget.h"

class ByyRoadPick;

class ByyRoadWidget : public ByyOperateWidget
{
	Q_OBJECT

public:
	ByyRoadWidget(ByySceneController* sceneController,QWidget *parent = 0);
	~ByyRoadWidget();

	void addRoad(osg::ref_ptr<osgEarth::Annotation::FeatureNode> roadLine,double width,const QString& texture);

	void init();
	void release();

	void writeToMapNode();

protected slots:
	void removeRoad();
	void rename();
	void zoomTo();
	void picking(int index);

private:
	Ui::ByyRoadWidget ui;
	osg::ref_ptr<ByyRoadPick> roadPick;

};

#endif // ROADWIDGET_H
