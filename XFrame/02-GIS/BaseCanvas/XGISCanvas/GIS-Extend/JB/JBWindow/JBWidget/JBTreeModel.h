#ifndef CJBTREEMODEL_H
#define CJBTREEMODEL_H

#include <QAbstractItemModel>
#include <QSharedPointer>
#include <QDir>
#include <QList>
#include "JBType.h"
#include "./../../JBSerialize.h"

typedef enum JB_TREE_TYPE
{
	JBTreeRoot = 0,   ///< 树根
	JBTreeTrunk,      ///< 树干
	JBTreeBranch,     ///< 树枝
    JBTreeLeaf,        ///< 军标数据
    JBTreeGroup,
    JBTreeData
} JBTreeType;


class CJBTreeItem
{
public:
	typedef QSharedPointer<QT_GROUP> DATA_PTR;

    CJBTreeItem(const QString &sText, JBTreeType eType, CJBTreeItem *pParent = 0);
    CJBTreeItem(const QString &sText, JBTreeType eType, const QSharedPointer<QT_GROUP> &spData, CJBTreeItem *pParent = 0);
    ~CJBTreeItem();

    CJBTreeItem *child(int nRow);
    CJBTreeItem *parent();

    bool findChildHas(const QString& strItemName);

    CJBTreeItem* findChild(const QString& strItemName);

    void appendChild(CJBTreeItem *pChild);

    int childCount() const;
    int columnCount() const;
    int row() const;

	QT_GROUP *data() const;

    void setText(const QString &sText);
    QString text() const;
    JBTreeType type() const;

	qint32 headId() const;
	void setHeadId(qint32 v);

    qint32 groupId() const;
    void setGroupId(qint32 n) ;

    QString desc() const;
    void setDesc(const QString& strText);


	CJBTreeItem* findLeafNode(qint32 groupID);
private:
    QList<CJBTreeItem *>        m_childItems;
    CJBTreeItem                 *m_pParentItem;
    JBTreeType                  m_eType;

    QString                     m_sText;
    DATA_PTR				    m_spData;
	qint32						m_nHeadId;
    qint32                      m_nGroupID;
    QString                     m_text;
};

class CJBTreeModel : public QAbstractItemModel
{
public:
    CJBTreeModel(const QString &sDirectory = QString::null, QObject *parent = 0);
	~CJBTreeModel();

	enum Role
	{
		TextRole = Qt::DisplayRole,
		DataRole = Qt::UserRole + 1
	};

	/// 设置路径
    void setDirectory(const QString &sDirectory);

	/// 设置树结构标题 
    void setHeaderText(const QString &sText);

public:
    QVariant data(const QModelIndex &index, int role) const;
    Qt::ItemFlags flags(const QModelIndex &index) const;
    QVariant headerData(int section, Qt::Orientation orientation,
                        int role = Qt::DisplayRole) const;
    QModelIndex index(int row, int column,
                      const QModelIndex &parent = QModelIndex()) const;
    QModelIndex parent(const QModelIndex &index) const;
    int rowCount(const QModelIndex &parent = QModelIndex()) const;
    int columnCount(const QModelIndex &parent = QModelIndex()) const;
	CJBTreeItem*   rootItem() const;

	CJBTreeItem*   findItemByGroupAndHead(qint32 headID,qint32 groupID);

    QList<CJBTreeItem*> JBAllItems();
private:
	/// 建立Model
    void setupModel();

    void findChild(QList<CJBTreeItem *> &items, CJBTreeItem *pItem);

private:
	CJBSerialize	m_serializer;
    CJBTreeItem*	m_pRootItem;
    QString         m_sDirectory;
    QString         m_sText;
    bool            m_bLoad;
};

#endif // CJBTREEMODEL_H
