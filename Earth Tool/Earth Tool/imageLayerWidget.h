#ifndef IMAGELAYERMANAGEWIDGET_H
#define IMAGELAYERMANAGEWIDGET_H

#include "abstractManageWidget.h"
#include <osgEarth/Map>

class ByyImageLayerWidget : public ByyAbstractManageWidget
{
	Q_OBJECT

public:
	ByyImageLayerWidget(ByySceneController *scene,QWidget *parent=NULL);
	~ByyImageLayerWidget();

	void init();
	void release();
	void add(const QStringList& fileNames);

private:
	void addImageLayerToWidget(osgEarth::ImageLayer* imageLayer);

protected slots:
	void moveUp();
	void moveDown();
	void removeToggle();
	void valueChanged(QtProperty *property, const QVariant &val);
};

#endif // IMAGELAYERMANAGEWIDGET_H
