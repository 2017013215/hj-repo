#ifndef LAYERSCONTROLWIDGET_H
#define LAYERSCONTROLWIDGET_H

#include <QWidget>
#include <osgEarth/MapNode>
#include "ui_layersControlWidget.h"

class QTreeWidget;
class QTreeWidgetItem;
class ByyIG;
class QAction;

class ByyLayersControlWidget : public QWidget
{
	Q_OBJECT

public:
	ByyLayersControlWidget(ByyIG& ig,QWidget * parent = 0, Qt::WindowFlags f = 0);
	~ByyLayersControlWidget();

	void refresh();

protected:
	virtual bool eventFilter(QObject *obj, QEvent *event);

private slots:
	void itemChanged(QTreeWidgetItem * item,int column);
	void moveUp();
	void moveDown();

	void remove();

	void addImageToggled();
	void addElevationToggled();

private:
	void createWidgets();
	void addImageLayer(osgEarth::ImageLayer* layer);
	void addElevationLayer(osgEarth::ElevationLayer* layer);

private:
	ByyIG& myIG;

	QAction *moveUpAction;
	QAction *moveDownAction;
	QAction *removeAction;

	Ui::ByyLayersControlWidget *ui;

	QTreeWidgetItem* imageTopItem;
	QTreeWidgetItem* elevationTopItem;
	QTreeWidgetItem* modelTopItem;

	QMap<QTreeWidgetItem*,osgEarth::Layer*> layersMap;
};

#endif // LAYERSCONTROLWIDGET_H
