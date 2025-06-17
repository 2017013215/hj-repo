#include "StdAfx.h"
#include "entityTreeWidget.h"

Q_DECLARE_METATYPE(ByyEntityObject*)

ByyEntityTreeWidget::ByyEntityTreeWidget(QWidget *parent)
	: QTreeWidget(parent)
{
	setDragEnabled(true);
	setAcceptDrops(true);
}

ByyEntityTreeWidget::~ByyEntityTreeWidget()
{

}

void ByyEntityTreeWidget::dragEnterEvent(QDragEnterEvent *event)
{
	if (event->mimeData()->hasFormat("image/y-puzzle-piece"))
		event->accept();
	else
		event->ignore();
}

void ByyEntityTreeWidget::dragMoveEvent(QDragMoveEvent *event)
{
	if (event->mimeData()->hasFormat("image/y-puzzle-piece")) {
		event->setDropAction(Qt::MoveAction);
		event->accept();
	} else
		event->ignore();
}

void ByyEntityTreeWidget::dropEvent(QDropEvent *event)
{
	QTreeWidgetItem *parentItem=this->itemAt(this->viewport()->mapFromGlobal(QCursor::pos()));


	if (event->mimeData()->hasFormat("image/y-puzzle-piece")) 
	{
		QByteArray pieceData = event->mimeData()->data("image/y-puzzle-piece");
		QDataStream dataStream(&pieceData, QIODevice::ReadOnly);
		
		int t;

		dataStream >> t;

		QTreeWidgetItem *item=(QTreeWidgetItem*)t;

		if (parentItem)
		{
			parentItem->addChild(item);
			parentItem->setExpanded(true);
		}
		else
			addTopLevelItem(item);

		event->setDropAction(Qt::MoveAction);
		event->accept();

		emit levelChanged(item);
	} 
	else
		event->ignore();
}

void ByyEntityTreeWidget::startDrag(Qt::DropActions /*supportedActions*/)
{
	QTreeWidgetItem *item = currentItem();

	ByyEntityObject *ent=item->data(0,Qt::UserRole).value<ByyEntityObject*>();

	if (!ent->isLocal())
		return;

	QByteArray itemData;

	QDataStream dataStream(&itemData, QIODevice::WriteOnly);

	dataStream << (int)item;

	QMimeData *mimeData = new QMimeData;
	mimeData->setData("image/y-puzzle-piece", itemData);

	QDrag *drag = new QDrag(this);
	drag->setMimeData(mimeData);

	if (item->parent())
	{
		item->parent()->removeChild(item);
	}
	else
	{
		takeTopLevelItem(indexOfTopLevelItem(item));
	}

	drag->exec(Qt::MoveAction);
}
