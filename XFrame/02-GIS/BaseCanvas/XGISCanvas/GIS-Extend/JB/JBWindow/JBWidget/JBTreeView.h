#ifndef CJBTREEVIEW_H
#define CJBTREEVIEW_H

#include <QTreeView>

class CJBTreeItem;
class CJBTreeModel;

//************************************************************************
// 类名称：CJBTreeView
// 类说明：重写树控件，为保持军标的唯一性，依赖于 CJBTreeModel
//------------------------------------------------------------------------
// 消息说明：signalItemClicked			注：单击事件的抛出
//										==》slotClicked
//										将单击事件中索引转换为item进行抛出
//------------------------------------------------------------------------
// 消息说明：signalItemDoubleClicked	注：双击事件的抛出
//										==》slotDoubleClicked
//										将双击事件中索引转换为item进行抛出
//------------------------------------------------------------------------
// 接口说明：
//------------------------------------------------------------------------
// 重写说明：
//************************************************************************
class CJBTreeView : public QTreeView
{
	Q_OBJECT
public:
    CJBTreeView(QWidget *parent = 0);
    ~CJBTreeView();

    void setTopItemsExpend(bool bExpended);

Q_SIGNALS:
	void signalItemClicked(CJBTreeItem *pItem);
	void signalItemDoubleClicked(CJBTreeItem *pItem);

protected Q_SLOTS:
	void slotClicked(const QModelIndex &index);
	void slotDoubleClicked(const QModelIndex &index);
	
private:

};

#endif // CJBTREEVIEW_H
