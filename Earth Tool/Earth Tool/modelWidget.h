#ifndef MODELWIDGET_H
#define MODELWIDGET_H

#include "operateWidget.h"
#include <osgEarth/GeoData>
#include "modelPicking.h"
#include "ui_modelWidget.h"

class ByyModel;
class ByyBuildingInfoDialog;
class ByyVideoEditDialog;

class ByyModelWidget : public ByyOperateWidget
{
	Q_OBJECT

public:
	ByyModelWidget(ByySceneController* s,QWidget *parent = 0);
	~ByyModelWidget();

	void init();
	void release();
	void writeToMapNode();

	void add( const QString& fileName );
	void addItem(ByyModel* model);

	void enableModelEdit(bool e);
	void enableVideoEdit(bool e);
	void enableInfoEdit(bool e);

protected slots:
	void currentModelChanged(int row);
	void setPostionX(double value);
	void setPostionY(double value);
	void setPostionZ(double value);
	void setRoatate();
	void setScaleX(double value);
	void setScaleY(double value);
	void setScaleZ(double value);

	void setBuilding(int checkState);
	void setVideo(int checkState);

	void setEntityHeight(int checkState);
	void setEntityHeightValue(double value);

	void zoomto();
	void remove();
	void move();

	void newModelPlacePosition(const osgEarth::GeoPoint& geoPoint);
	void newModelPlaceFinished();

	void moveFinish(bool ok);

	void pickingModel(int index);
	void rename();

	void setLod(int checkState);
	void setLodRange(double value);

	void setTerrain(int checkState);

private:
	void blockAllSignals(bool bl);

protected:
	std::string getConfigKey();

private:
	Ui::ByyModelWidget ui;
	QList<ByyModel*> modelList;
	osg::ref_ptr<ByyModelPicking> modelPick;

	ByyBuildingInfoDialog* myBuildingInfoWidget;
	ByyVideoEditDialog* myVideoEditDialog;
};

#endif // MODELWIDGET_H
