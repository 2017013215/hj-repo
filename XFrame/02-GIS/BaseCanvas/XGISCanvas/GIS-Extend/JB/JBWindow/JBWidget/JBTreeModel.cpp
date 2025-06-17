#include "JBTreeModel.h"
#include "./../../JBSerialize.h"
#include "./../../Common_Base.h"

#include <QDebug>

CJBTreeModel::CJBTreeModel(const QString &sDirectory, QObject *parent)
    : QAbstractItemModel(parent)
    , m_pRootItem(NULL)
    , m_bLoad(false)
    , m_sText(tr8("军标"))

{
	m_pRootItem = new CJBTreeItem(m_sText, JBTreeRoot);

	setDirectory(sDirectory);
}

CJBTreeModel::~CJBTreeModel()
{
    if (NULL != m_pRootItem)
    {
        delete (m_pRootItem);
        m_pRootItem = NULL;
    }
}

void CJBTreeModel::setDirectory(const QString &sDirectory)
{
	if (sDirectory.isEmpty())
		return;

    /// 如果已加载且文件路径相同，则直接返回
    if(m_sDirectory == sDirectory && m_bLoad)
        return;

    QDir dir(sDirectory);

    if (!dir.exists())
        return ;

    m_sDirectory = sDirectory;

    setupModel();

	//reset();
	beginResetModel();
	endResetModel();
}

void CJBTreeModel::setHeaderText(const QString &sText)
{
    m_sText = sText;
    if (NULL != m_pRootItem)
	{
        m_pRootItem->setText(sText);
    }
}

QVariant CJBTreeModel::data(const QModelIndex &index, int role) const
{
    if (!index.isValid())
        return QVariant();

    switch (role) {
	case TextRole:
        {
            CJBTreeItem *pItem = static_cast<CJBTreeItem*>(index.internalPointer());
            return pItem->text();
        }
	case DataRole:
		{
			CJBTreeItem *pItem = static_cast<CJBTreeItem*>(index.internalPointer());
			return QVariant::fromValue<void*>(pItem);
		}
        default:
            break;
    }

    return QVariant();
}

Qt::ItemFlags CJBTreeModel::flags(const QModelIndex &index) const
{
    if (!index.isValid())
        return 0;

    return Qt::ItemIsEnabled | Qt::ItemIsSelectable;
}

QVariant CJBTreeModel::headerData(int section, Qt::Orientation orientation, int role) const
{
    if (orientation == Qt::Horizontal && role == Qt::DisplayRole)
    {
        return m_pRootItem->text();
    }

    return QAbstractItemModel::headerData(section, orientation, role );
}

QModelIndex CJBTreeModel::index(int row, int column, const QModelIndex &parent) const
{
    if (!hasIndex(row, column, parent))
        return QModelIndex();

    CJBTreeItem *pParentItem = NULL;

    if (!parent.isValid())
        pParentItem = m_pRootItem;
    else
        pParentItem = static_cast<CJBTreeItem*>(parent.internalPointer());

    CJBTreeItem *pChildItem = pParentItem->child(row);
    if (pChildItem != NULL)
        return createIndex(row, column, pChildItem);
    else
        return QModelIndex();
}

QModelIndex CJBTreeModel::parent(const QModelIndex &index) const
{
    if (!index.isValid())
        return QModelIndex();

    CJBTreeItem *pChildItem = static_cast<CJBTreeItem*>(index.internalPointer());
    CJBTreeItem *pParentItem = pChildItem->parent();

    if (pParentItem == m_pRootItem)
        return QModelIndex();

    return createIndex(pParentItem->row(), 0, pParentItem);
}

int CJBTreeModel::rowCount(const QModelIndex &parent) const
{
    CJBTreeItem *pParentItem = NULL;
    if (parent.column() > 0)
        return 0;

    if (!parent.isValid())
        pParentItem = m_pRootItem;
    else
        pParentItem = static_cast<CJBTreeItem*>(parent.internalPointer());

    return pParentItem->childCount();
}

int CJBTreeModel::columnCount(const QModelIndex &parent) const
{
//    if (parent.isValid())
//        return static_cast<CJBTreeItem*>(parent.internalPointer())->columnCount();
//    else
//        return m_pRootItem->columnCount();

    Q_UNUSED(parent);
    return 1;
}

CJBTreeItem* CJBTreeModel::rootItem() const
{
	return m_pRootItem;
}

CJBTreeItem* CJBTreeModel::findItemByGroupAndHead(qint32 headID,qint32 groupID)
{
	for(int i  = 0; i < m_pRootItem->childCount();++i)
	{
		if(CJBTreeItem* topItem = m_pRootItem->child(i)->headId() == headID ? m_pRootItem->child(i) : NULL)
		{
			if(!topItem)
			{
				return NULL;
			}
			else
			{
				return topItem->findLeafNode(groupID);
			}
		}
	}
    return NULL;
}

QList<CJBTreeItem *> CJBTreeModel::JBAllItems()
{
    QList<CJBTreeItem*> items;
    findChild(items,m_pRootItem);
    return items;
}


void CJBTreeModel::setupModel()
{
    if (NULL != m_pRootItem)
    {
        delete (m_pRootItem);
        m_pRootItem = NULL;
    }

    m_pRootItem = new CJBTreeItem(m_sText, JBTreeRoot);

    if (m_serializer.Execute(m_pRootItem, m_sDirectory))
    {
        m_bLoad = true;
    }
}

void CJBTreeModel::findChild(QList<CJBTreeItem*> &items,CJBTreeItem* pItem)
{
    if(pItem->childCount() == 0)
    {
        items.push_back(pItem);
    }
    else
    {

        QList<CJBTreeItem*> parents ;
        for(int i = 0;i < pItem->childCount();++i)
        {
            parents.push_back(pItem->child(i));
        }
        for(int i = 0;i < parents.count();++i)
        {
            findChild(items,pItem->child(i));
        }
    }
}

///==============================================

CJBTreeItem::CJBTreeItem(const QString &sText, JBTreeType eType, CJBTreeItem *pParent):
    m_sText(sText),
    m_eType(eType),
    m_pParentItem(pParent),
    m_nHeadId(0),
    m_nGroupID(0)
{
    if(pParent)
    {
        pParent->appendChild(this);
    }
}

CJBTreeItem::CJBTreeItem(const QString &sText, JBTreeType eType, const QSharedPointer<QT_GROUP> &spData, CJBTreeItem *pParent):
    m_sText(sText),
    m_eType(eType),
    m_pParentItem(pParent),
    m_spData(spData),
    m_nHeadId(0),
    m_nGroupID(0)
{
    if(pParent)
    {
        pParent->appendChild(this);
    }
}

CJBTreeItem::~CJBTreeItem()
{
    qDeleteAll(m_childItems);
}

void CJBTreeItem::appendChild(CJBTreeItem *pChild)
{
    if(m_childItems.indexOf(pChild) != -1)
    {
        return;
    }
    m_childItems.append(pChild);
}

CJBTreeItem *CJBTreeItem::child(int nRow)
{
#if 0
    if (nRow < m_childItems.size() && nRow >= 0)
        return m_childItems.at(nRow);
    else
        return NULL;
#else
   return m_childItems.at(nRow);
#endif
}

CJBTreeItem *CJBTreeItem::parent()
{
    return m_pParentItem;
}

bool CJBTreeItem::findChildHas(const QString &strItemName)
{
    for(QList<CJBTreeItem *>::iterator itr = m_childItems.begin();
        itr != m_childItems.end();++itr)
    {
        if((*itr)->text() == strItemName)
        {
            return true;
        }
    }
    return false;
}

CJBTreeItem *CJBTreeItem::findChild(const QString &strItemName)
{
    for(QList<CJBTreeItem *>::iterator itr = m_childItems.begin();
        itr != m_childItems.end();++itr)
    {
        if(strItemName == (*itr)->text())
        {
            return *itr;
        }
    }
    return NULL;
}

int CJBTreeItem::childCount() const
{
    return m_childItems.size();
}

int CJBTreeItem::columnCount() const
{
    return 1;
}

int CJBTreeItem::row() const
{
    if (m_pParentItem)
        return m_pParentItem->m_childItems.indexOf(const_cast<CJBTreeItem*>(this));

    return 0;
}

QT_GROUP * CJBTreeItem::data() const
{
    if(m_spData.isNull())
    {
        return NULL;
    }
	return m_spData.data();
}

void CJBTreeItem::setText(const QString &sText)
{
	m_sText = sText;
}

QString CJBTreeItem::text() const
{
	return m_sText;
}

JBTreeType CJBTreeItem::type() const
{
	return m_eType;
}

qint32 CJBTreeItem::headId() const
{
	return m_nHeadId;
}

void CJBTreeItem::setHeadId(qint32 v)
{
	m_nHeadId = v;
}

qint32 CJBTreeItem::groupId() const
{
    return m_nGroupID;
}

void CJBTreeItem::setGroupId(qint32 n)
{
    m_nGroupID = n;
}

QString CJBTreeItem::desc() const
{
    return m_text;
}

void CJBTreeItem::setDesc(const QString &strText)
{
    m_text = strText;
}

CJBTreeItem* CJBTreeItem::findLeafNode(qint32 groupID)
{
	if(childCount() < 1)
	{
        if(m_nGroupID == groupID)
        {
            return this;
        }
        if(m_spData.isNull())
        {
            return NULL;
        }
        if(m_spData->unID == groupID)
		{
			return this;
		}

	}
	else
	{
		for(int i = 0;i < childCount();++i)
		{
			CJBTreeItem* item = child(i)->findLeafNode(groupID);
			if(item != NULL)
			{
				return item;
			}
		}
	}

	return NULL;
}
