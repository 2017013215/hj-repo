#ifndef MASKWIDGET_H
#define MASKWIDGET_H

#include "operateWidget.h"
#include "ui_maskWidget.h"

#include <osgEarth/Config>
#include <osgEarth/MaskLayer>

struct MaskInfo
{
	MaskInfo()
		:forceAltitude(false),
		altitude(0),
		maskEnable(true)
	{

	}

	std::string geometry;
	bool maskEnable;
	bool forceAltitude;
	double altitude;

	osg::ref_ptr<osgEarth::MaskLayer> myMaskLayer;
};

class ByyMaskWidget : public ByyOperateWidget
{
	Q_OBJECT

public:
	ByyMaskWidget(ByySceneController *scene,QWidget *parent = 0);
	~ByyMaskWidget();

	void init();
	void release();

	void writeToMapNode();

	void addArea();

	void addMask(const osgEarth::Config& conf);

protected slots:
	void edit();
	void find();
	void remove();
	void rename();

	void addAreaFinished(int result);
	void editFinished(int result);

	void currentRowChanged(int currentRow);

	void forceAltitudeChanged(bool checked);
	void altitudeChanged(double value);

	void maskEnableChanged(int checkState);

private:
	Ui::ByyMaskWidget ui;

	QVector<MaskInfo> myMaskInfoList;
};

#endif // MASKWIDGET_H
