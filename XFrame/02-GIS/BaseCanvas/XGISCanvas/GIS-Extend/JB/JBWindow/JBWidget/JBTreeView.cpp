#include "JBTreeView.h"

#include "JBTreeModel.h"

CJBTreeView::CJBTreeView(QWidget *parent)
    : QTreeView(parent)
{
	connect(this,SIGNAL(clicked(QModelIndex)),this,SLOT(slotClicked(QModelIndex)));
	connect(this,SIGNAL(doubleClicked(QModelIndex)),this,SLOT(slotDoubleClicked(QModelIndex)));
}

CJBTreeView::~CJBTreeView()
{

}

void CJBTreeView::setTopItemsExpend(bool bExpended)
{
	for (int i = 0;i < model()->rowCount(); ++i)
	{
        setExpanded(model()->index(i, 0), bExpended);
	}
}

void CJBTreeView::slotClicked(const QModelIndex &index)
{
	CJBTreeItem *pItem = (CJBTreeItem*)model()->data(index, CJBTreeModel::DataRole).value<void*>();
	if (pItem != NULL)
	{
		emit signalItemClicked(pItem);
	}
}

void CJBTreeView::slotDoubleClicked(const QModelIndex &index)
{
	CJBTreeItem *pItem = (CJBTreeItem*)model()->data(index, CJBTreeModel::DataRole).value<void*>();
	if (pItem != NULL)
	{
		emit signalItemDoubleClicked(pItem);
	}
}
