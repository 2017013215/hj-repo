#ifndef ENTITYOBJECTWIDGET_H
#define ENTITYOBJECTWIDGET_H

#include <QWidget>
#include "ui_entityObjectWidget.h"

class ByyEntityObject;
class ByyApp;
class QAction;
class ByyEntityObserver;

class ByyEntityObjectWidget : public QWidget
{
	Q_OBJECT

public:
	ByyEntityObjectWidget(ByyApp& app,QWidget *parent = 0);
	~ByyEntityObjectWidget();

public slots:
	void appendEntObject(ByyEntityObject* entObj);
	void removeEntObject(ByyEntityObject* entObj);

protected slots:
	void itemDoubleClicked(QTreeWidgetItem *item,int column);
	void onNameChanged(const QString& name);
	void onVisibleChanged(bool b);
	void onLevelChanged(QTreeWidgetItem *item);
	void onForceTypeChanged(ByyForceType type);

	void onItemClicked(QTreeWidgetItem *current, int column ); 

protected:
	bool eventFilter(QObject *obj, QEvent *ev);

	QTreeWidgetItem* findItem(ByyEntityObject* entObj);

private:
	Ui::ByyEntityObjectWidget ui;
	ByyApp& myApp;

	typedef QMultiMap<ByyEntityObject*,QPointer<QWidget>> AttachEntityWidgetMap;
	AttachEntityWidgetMap myAttachEntWidgetMap;

	QList<QTreeWidgetItem*> myUnFindSuperiorItems;
};

#endif // ENTITYOBJECTWIDGET_H
