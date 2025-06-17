#ifndef VIEWPOINTEDITWIDGET_H
#define VIEWPOINTEDITWIDGET_H

#include <QWidget>
#include <osgEarth/Viewpoint>
#include "ui_viewpointEditWidget.h"

class ByyIG;

class ByyVprFile
{
public:
	bool read(const QString& filename,QList<osgEarth::Viewpoint>& out_vps);
	bool write(const QString& filename,const QList<osgEarth::Viewpoint>& out_vps);
};

class ByyViewpointEditWidget : public QWidget
{
	Q_OBJECT

public:
	ByyViewpointEditWidget(ByyIG& ig,QWidget * parent = 0, Qt::WindowFlags f = 0);
	~ByyViewpointEditWidget();

public slots:
	void newFile();
	void openFile();

	void clear();

	bool saveFile();
	bool saveAsFile();

	void add();
	void remove();

	void moveUp();
	void moveDown();

private:
	bool okToContinue();

	bool saveFile(const QString& filename);

	void addToTable(const osgEarth::Viewpoint& vp);

	osgEarth::Viewpoint getViewpointFromTable(int row);

protected:
	void closeEvent(QCloseEvent *e);

private slots:
	void cellClicked(int row,int column);

private:
	Ui::ByyViewpointEditWidget ui;

	bool needSave;

	QString currentFilename;

	ByyIG& myIG;

	osg::ref_ptr<const osgEarth::SpatialReference> _srs;
};

#endif // VIEWPOINTEDITWIDGET_H
