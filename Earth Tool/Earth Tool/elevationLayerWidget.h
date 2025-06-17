#ifndef ELEVATIONLAYERMANAGEWIDGET_H
#define ELEVATIONLAYERMANAGEWIDGET_H

#include "abstractManageWidget.h"
#include <osgEarth/Map>

class ByyElevationLayerWidget : public ByyAbstractManageWidget
{
	Q_OBJECT

public:
	ByyElevationLayerWidget(ByySceneController *scene,QWidget *parent);
	~ByyElevationLayerWidget();


	void init();
	void release();
	void add(const QStringList& fileNames);

private:
	void addElevationLayerToWidget(osgEarth::ElevationLayer* elevationLayer);

protected slots:
	void moveUp();
	void moveDown();
	void removeToggle();
	void valueChanged(QtProperty *property, const QVariant &val);

};

#endif // ELEVATIONLAYERMANAGEWIDGET_H
