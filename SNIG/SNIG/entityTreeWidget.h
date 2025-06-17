#ifndef ENTITYTREEWIDGET_H
#define ENTITYTREEWIDGET_H

#include <QTreeWidget>

class ByyEntityTreeWidget : public QTreeWidget
{
	Q_OBJECT

public:
	ByyEntityTreeWidget(QWidget *parent=0);
	~ByyEntityTreeWidget();

protected:
	void dragEnterEvent(QDragEnterEvent *event);
	void dragMoveEvent(QDragMoveEvent *event);
	void dropEvent(QDropEvent *event);
	void startDrag(Qt::DropActions supportedActions);

signals:
	void levelChanged(QTreeWidgetItem*);	
};

#endif // ENTITYTREEWIDGET_H
