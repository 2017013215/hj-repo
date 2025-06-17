#ifndef MEASUREWIDGET_H
#define MEASUREWIDGET_H

#include <QWidget>
#include <osgEarth/Viewpoint>
#include "ui_measureWidget.h"

class ByyIG;
class ByyAnnotationComponent;
class BaseAnnotationDialog;

namespace osgEarth
{
	namespace Annotation
	{
		class AnnotationNode;
	}
	namespace Symbology
	{
		class Color;
	}
}

class ByyMeasureWidget : public QWidget
{
	Q_OBJECT

public:
	ByyMeasureWidget(ByyIG& ig,QWidget *parent = 0);
	~ByyMeasureWidget();

public slots:
	void init();
	void release();
	void addPath();
	void addPolygon();
	void addEllipse();

	void edit();
	void remove();

private slots:
	void onAddFinished(int result);
	void onEditFinished(int result);

	void slot_itemClicked(QTreeWidgetItem *item,int column=TextColumn,bool scaleTo=true);

	void itemChanged(QTreeWidgetItem * item,int column);

	void itemClickedFrom3D(osg::Node* node);

	void colorItemClicked( QListWidgetItem * item);

	void currentColorItemChanged( QListWidgetItem * current, QListWidgetItem * previous );

protected:
	bool eventFilter(QObject *obj, QEvent *event);
	void showEvent(QShowEvent *e);
	void hideEvent(QHideEvent *e);

private:
	void setEnables(bool enable);

	QTreeWidgetItem* addItem(osgEarth::Annotation::AnnotationNode *annoNode);

	osgEarth::Viewpoint getItemCenter(QTreeWidgetItem * item);

	void showOrHideLabel(QTreeWidgetItem* item);
	void showContextMenu();

	void setCurrentColor(const osgEarth::Symbology::Color& color);

signals:
	void itemClicked();

private:
	Ui::ByyMeasureWidget ui;

	ByyIG& myIG;
	ByyAnnotationComponent *d_annoComponent;

	QMap<QTreeWidgetItem*,osgEarth::Annotation::AnnotationNode*> _annoNodeMap;

	QPointer<BaseAnnotationDialog>  _activeDialog;

	QTreeWidgetItem *pathTopItem;
	QTreeWidgetItem *polygonTopItem;
	QTreeWidgetItem *ellipseTopItem;

	QAction* editAction;
	QAction* removeAction;

	QTreeWidgetItem* clickCurrentItem;

	enum Column{TextColumn};

	QIcon currentColorIcon;
};

#endif // MEASUREWIDGET_H
