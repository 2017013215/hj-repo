#ifndef ABSTRACTMANAGEWIDGET_H
#define ABSTRACTMANAGEWIDGET_H

#include "operateWidget.h"
#include <osgEarth/Map>

class ByyVariantManager;
class QtVariantEditorFactory;
class QtTreePropertyBrowser;
class QAction;
class QtProperty;

class ByyAbstractManageWidget : public ByyOperateWidget
{
	Q_OBJECT

public:
	ByyAbstractManageWidget(ByySceneController *scene,QWidget *parent);
	~ByyAbstractManageWidget();

protected:
	void createWidgets();

protected:
	virtual bool eventFilter(QObject *obj, QEvent *event);

protected slots:
	virtual void moveUp()=0;
	virtual void moveDown()=0;
	virtual void removeToggle()=0;
	virtual void valueChanged(QtProperty *property, const QVariant &val)=0;

protected:
	osg::ref_ptr<osgEarth::Map> d_map;
	ByyVariantManager *variantManager;
	QtVariantEditorFactory *variantFactory;
	QtTreePropertyBrowser *variantEditor;
	QAction *moveUpAction;
	QAction *moveDownAction;
	QAction *removeAction;
};

#endif // ABSTRACTMANAGEWIDGET_H
