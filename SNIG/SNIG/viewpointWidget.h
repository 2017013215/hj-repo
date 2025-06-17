#ifndef VIEWPOINTWIDGET_H
#define VIEWPOINTWIDGET_H

#include <QWidget>
#include "ui_viewpointWidget.h"

class ByyIG;

namespace osgEarth
{
	class Viewpoint;
}

class ByyViewpointWidget : public QWidget
{
	Q_OBJECT

public:
	ByyViewpointWidget(ByyIG& ig,QWidget *parent = 0);
	~ByyViewpointWidget();

public slots:
	void remove();
	void importFile();

	void loadFile( const QString& fileName );

	void manualListCurrentChanged(int current);
	void itemClicked(QListWidgetItem *item);

private:
	void addToManual(osgEarth::Viewpoint& vp);

signals:
	void viewpointClicked();

private:
	Ui::ByyViewpointWidget ui;
	ByyIG& myIG;
	QVector<osgEarth::Viewpoint> manualViewpoints;
};

#endif // VIEWPOINTWIDGET_H
