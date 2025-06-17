#include "QtDrawJBWidget.h"
#include "./../QtDrawJB.h"
#include "QTreeWidget"
#include "QDebug"
#include "./../LoadJB.h"


#include <QScrollArea>
#include <QVBoxLayout>
#include <QList>
#include <QDir>
#include <QLineEdit>
#include <QLabel>
#include "JBWidget/JBTreeView.h"
#include "JBWidget/JBTreeModel.h"
//#include "../../Message/MessageManager.h"
#include "QPushButton"
#include "./../JBPixmapHelper.h"
#include "JBWidget/JBSetWidget.h"
//#include "../../ByyAppManager.h"
#include "./../Common_Base.h"
#include "./../ParseJunBiao.h"
#include <QApplication>

#include "XAddJBMapTool.h"
#include <qgsmapcanvas.h>


ByyQtDrawJBWidget::ByyQtDrawJBWidget(XAddJBMapTool* _pAddJBTool)
	: QDialog(_pAddJBTool->canvas()->topLevelWidget(),nullptr)
    , m_parseJunBiaoNew(new ParseJunBiao)
{
	createUI();
	
	connect(m_treeView,SIGNAL(signalItemDoubleClicked(CJBTreeItem*)),
		this,SLOT(slotItemClicked(CJBTreeItem*)));
	connect(m_drawJBWidget,SIGNAL(currentJBChange(int)),this,SLOT(slotSelectJBChange(int)));

    connect(m_edSreach,SIGNAL(textChanged(const QString&)),this,SLOT(slotSreach(const QString&)));
	std::string sPath = QApplication::applicationDirPath().toLocal8Bit();
    sPath += "/Data/JB";
    m_pModel->setDirectory(tr8(sPath.c_str()));
    m_parseJunBiaoNew->addJunBiaoNew(m_pModel->rootItem());
}

/// 析构
ByyQtDrawJBWidget::~ByyQtDrawJBWidget()
{
    delete m_parseJunBiaoNew;
}

///军标树被点击后响应，用于绘制军标组
void ByyQtDrawJBWidget::slotItemClicked(CJBTreeItem *pItem)
{
	QVector<QT_GROUP> vGroups;
	switch (pItem->type())
	{
	case TREE_Trunk:
		return;
	case TREE_Branch:
		m_drawJBWidget->SetSelID(-1);
		for (int i=0; i < pItem->childCount(); ++i)
		{
			vGroups << *pItem->child(i)->data();
		}
		break;
	case TREE_LEAF:
		m_drawJBWidget->SetSelID(-1);
		vGroups << *pItem->data();
		break;
        ///点击信号或目标组
    case JBTreeGroup:
		m_drawJBWidget->SetSelID(-1);
        drawGroup(pItem);
        m_currentDisplay = pItem;
        return;
        ///点击具体军标
    case TREE_DATA:
		m_drawJBWidget->SetSelID(-1);
        drawJBByData(pItem);
        m_currentDisplay = pItem;
        return;
	default:
		return;
	}
	m_currentDisplay = pItem;
	m_drawJBWidget->setCurrentUnIDs(vGroups);
}

///选中军标发生变化消息
void ByyQtDrawJBWidget::slotSelectJBChange(int gUid)
{
    /// 获取树控件
	CJBTreeItem* parentItem = m_currentDisplay;

	while(parentItem->parent() != NULL)
	{
		parentItem = parentItem->parent();
		if(parentItem->parent() == m_pModel->rootItem())
			break;
	}
	QString itemText = parentItem->text();
	QString strUid ;
	int beginIndex = itemText.indexOf('(');
	int endIndex = itemText.indexOf(')');
    strUid = itemText.mid(beginIndex + 1,endIndex - beginIndex - 1);

    if(m_parseJunBiaoNew->JBTagID() == strUid.toInt())
    {
        currentJBChange(0,gUid);
    }
    else
    {
        currentJBChange(strUid.toUInt(),gUid);
    }
}

void ByyQtDrawJBWidget::slotSreach(const QString &text)
{
    QList<CJBTreeItem*> items = m_pModel->JBAllItems();
    QList<CJBTreeItem*>::iterator it = items.begin();
    for(;it != items.end();++it)
    {
        QT_GROUP * pQtGroup = (*it)->data();
    }

}

///获取到一个军标图片
QPixmap ByyQtDrawJBWidget::getJBPixMap(int headID,int groupID, int nwidth, int nheight, const QColor& color, int nLineWidth)
{
    if(headID != 0)
    {
        QT_GROUP* jbGroup = m_pModel->findItemByGroupAndHead(headID,groupID)->data();
        QPixmap pixmap = CJBPixmapHelper::Instance()->getPixmap(*jbGroup,nwidth,nheight,color,nLineWidth);
        m_pixmap = pixmap;
    }
    else
    {
        JB_GROUP  group= m_parseJunBiaoNew->getJunBiaoGroup(groupID);
        QPixmap pixmap =  CJBPixmapHelper::Instance()->getPixmap(group,QSize(nwidth,nheight),color,nLineWidth);
        m_pixmap = pixmap;
    }
    return m_pixmap;
}

/// 当前有选中的军标，并弹出对话框供用户选择属性
void ByyQtDrawJBWidget::currentJBChange(int headID, int groupID)
{
    QT_GROUP* jbGroup = NULL;
    if(headID != 0)
    {
        jbGroup = m_pModel->findItemByGroupAndHead(headID,groupID)->data();
    }
    ByyJBSetDialog dia(0,this);
    if(!jbGroup)
    {
        JB_GROUP stGroup = m_parseJunBiaoNew->getJunBiaoGroup(groupID);
        dia.setJBGroup(&stGroup);
    }
    else
    {
        dia.setJBGroup(jbGroup);
    }
	if(QDialog::Accepted == dia.exec())
	{
        //GetAppManager()->SetMouseState(STATE_PLOT);
       // GetMsgManager()->SelectJBChange(headID, groupID, dia.getSize(), dia.getSize(), dia.getColor(), dia.getLineWidth());
	}
}

///创建界面
void ByyQtDrawJBWidget::createUI()
{
	if (this->objectName().isEmpty())
		this->setObjectName(QString::fromUtf8("DockWidget"));
	//m_contens = new QWidget();
	//m_contens->setObjectName(QString::fromUtf8("m_contens"));

    QVBoxLayout* vBox = new QVBoxLayout(this);

    QHBoxLayout* hBox_2 = new QHBoxLayout(this);

    QHBoxLayout*  horizontalLayout = new QHBoxLayout();
	horizontalLayout->setObjectName(QString::fromUtf8("horizontalLayout"));
	m_treeView = new CJBTreeView(this);
	m_treeView->setObjectName(QString::fromUtf8("treeView"));
    m_treeView->setSizePolicy(QSizePolicy::Preferred,QSizePolicy::Expanding);

    m_edSreach = new QLineEdit(this);
	horizontalLayout->addWidget(m_treeView);
    QLabel* label_2 = new QLabel(this);
    label_2->setText(tr8("搜索："));
    hBox_2->addWidget(label_2);
    hBox_2->addWidget(m_edSreach);
    vBox->addLayout(hBox_2);
    vBox->addLayout(horizontalLayout);

	m_drawJBWidget = new CQtDrawJB(this);
	m_drawJBWidget->setObjectName(QString::fromUtf8("widget"));
    QSizePolicy sizePolicy1(QSizePolicy::MinimumExpanding, QSizePolicy::MinimumExpanding);
    sizePolicy1.setHorizontalStretch(0);
    sizePolicy1.setVerticalStretch(0);
    sizePolicy1.setHeightForWidth(m_drawJBWidget->sizePolicy().hasHeightForWidth());
    m_drawJBWidget->setSizePolicy(sizePolicy1);

    m_scroll = new QScrollArea(this);
	m_scroll->setWidgetResizable(true);
	m_scroll->setVerticalScrollBarPolicy(Qt::ScrollBarAlwaysOn);
	m_scroll->setHorizontalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
	m_scroll->setWidgetResizable(true);
	m_scroll->setBackgroundRole(QPalette::Dark);
    m_scroll->setWidget(m_drawJBWidget);

	horizontalLayout->addWidget(m_scroll);
    //horizontalLayout->addWidget(m_edSreach);

	//this->setWidget(m_contens);

    m_treeView->setModel(m_pModel = new CJBTreeModel());
}

void ByyQtDrawJBWidget::drawGroup(CJBTreeItem *pItem)
{
    QVector<JB_GROUP> vGroups;
    QList<CJBTreeItem*> items;
    for(int i = 0;i < pItem->childCount();++i)
    {
        items.append(pItem->child(i));
    }
    foreach (CJBTreeItem* item, items)
    {
          vGroups.append(m_parseJunBiaoNew->getJunBiaoGroup(item->groupId()));
    }
    m_drawJBWidget->setCurrentUnIDs(vGroups);
}

void ByyQtDrawJBWidget::drawJBByData(CJBTreeItem *pItem)
{
    QVector<JB_GROUP> vGroups;
    quint32 nGroupID = pItem->groupId();
    vGroups.append(m_parseJunBiaoNew->getJunBiaoGroup(nGroupID));
    m_drawJBWidget->setCurrentUnIDs(vGroups);
}

void ByyQtDrawJBWidget::clearSel()
{
	if (nullptr != m_drawJBWidget)
		m_drawJBWidget->SetSelID(-1);
}

const int ByyQtDrawJBWidget::GetCurSelJBImg(QPixmap& _refImg, int _nW, int _nH, const QColor& _clr, int _nLineW)
{
	if (nullptr == m_drawJBWidget)
	{
		return -1;
	}

	int nCurGroupId = m_drawJBWidget->GetSelID();
	if (-1 == nCurGroupId)
	{
		return -1;
	}

	/// 获取树控件
	CJBTreeItem* parentItem = m_currentDisplay;

	while (parentItem->parent() != NULL)
	{
		parentItem = parentItem->parent();
		if (parentItem->parent() == m_pModel->rootItem())
			break;
	}
	QString itemText = parentItem->text();
	QString strUid;
	int beginIndex = itemText.indexOf('(');
	int endIndex = itemText.indexOf(')');
	strUid = itemText.mid(beginIndex + 1, endIndex - beginIndex - 1);
	int nHeadId = strUid.toUInt();

	if (nHeadId != 0)
	{
		QT_GROUP* jbGroup = m_pModel->findItemByGroupAndHead(nHeadId, nCurGroupId)->data();
		_refImg = CJBPixmapHelper::Instance()->getPixmap(*jbGroup, _nW, _nH, _clr, _nLineW);
		return nCurGroupId;
	}
	
	return -1;
}

void ByyQtDrawJBWidget::closeEvent(QCloseEvent *event)
{
	emit slgCloseWnd();
	hide();
}

const bool ByyQtDrawJBWidget::IsCheckedItem()
{
	int nCurGroupId = m_drawJBWidget->GetSelID();
	if (-1 == nCurGroupId)
	{
		return false;
	}

	/// 获取树控件
	CJBTreeItem* parentItem = m_currentDisplay;

	while (parentItem->parent() != NULL)
	{
		parentItem = parentItem->parent();
		if (parentItem->parent() == m_pModel->rootItem())
			break;
	}
	QString itemText = parentItem->text();
	QString strUid;
	int beginIndex = itemText.indexOf('(');
	int endIndex = itemText.indexOf(')');
	strUid = itemText.mid(beginIndex + 1, endIndex - beginIndex - 1);
	int nHeadId = strUid.toUInt();

	if(-1 == nHeadId)
	{
		return false;
	}

	return true;
}

const bool ByyQtDrawJBWidget::GetCheckItem(quint32& _unRefLibID, quint32& _unRefItemID)
{
	int nItemID = m_drawJBWidget->GetSelID();
	if (-1 == nItemID)
	{
		return false;
	}

	/// 获取树控件
	CJBTreeItem* parentItem = m_currentDisplay;

	while (parentItem->parent() != NULL)
	{
		parentItem = parentItem->parent();
		if (parentItem->parent() == m_pModel->rootItem())
			break;
	}
	QString itemText = parentItem->text();
	QString strUid;
	int beginIndex = itemText.indexOf('(');
	int endIndex = itemText.indexOf(')');
	strUid = itemText.mid(beginIndex + 1, endIndex - beginIndex - 1);
	int nHeadId = strUid.toUInt();

	if(-1 == nHeadId)
	{
		return false;
	}

	_unRefLibID = nHeadId;
	_unRefItemID = nItemID;
	return true;
}


