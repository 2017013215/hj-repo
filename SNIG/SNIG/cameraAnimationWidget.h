#ifndef VIEWPOINTSWIDGET_H
#define VIEWPOINTSWIDGET_H

#include <QWidget>
#include <osg/ref_ptr>

#include "ui_cameraAnimationWidget.h"

class ByyIG;
class ByyCameraAnimationComponent;

class ByyCameraAnimationWidget : public QWidget
{
	Q_OBJECT

public:
	ByyCameraAnimationWidget(ByyIG& ig,QWidget *parent = 0);
	~ByyCameraAnimationWidget();

	void stopAll();

public slots:
	void stop();

protected slots:
	void closePath();
	void loadPath();

	void enablePath(bool b);
	void enablePoints(bool b);
	void enableLoop(bool b);

	void playOrPause();
	
	void reset();

	void speedChanged(int value);

	void autoFlyChanged(int index);
	void autoFlyScaleChanged(int value);

private slots:
	void currentItemChanged(QTreeWidgetItem * current, QTreeWidgetItem * previous); 
	void itemClicked(QTreeWidgetItem *item,int column);

	void currentPointChanged(int path,int point);

private:
	void updateTime();

signals:
	void runningChanged(bool);

private:
	Ui::ByyCameraAnimationWidget ui;

	ByyCameraAnimationComponent* d_vf;

	QTreeWidgetItem *playItem;
	QIcon playItemIcon;
};

#endif // VIEWPOINTSWIDGET_H
