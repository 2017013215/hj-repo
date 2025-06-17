#include "XJBWidget.h"
#include "XAddJBMapTool.h"
#include <qgsmapcanvas.h>

#include <QHBoxLayout>
#include <QTreeWidget>
#include <QLabel>
#include <QMouseEvent>
#include <QToolTip>
#include "XJBInfoManager.h"
#include "XCommonLib.h"
#include "JBDef.h"
#include "xjbdrawhelper.h"
#include "XJBItemWnd.h"
#include "XCoordinateGeo.h"
#include "GIS-Extend\XShapeDef.h"

using namespace XSpace_Common;
namespace XGIS
{
	XJBWidget::XJBWidget(QWidget* _pParentWnd)
		: QDialog(_pParentWnd,nullptr)
		,m_cDrawJBSize(QSize(40,40))
		,m_pCurShowCate(nullptr)
		,m_pJBInfoManger(nullptr)
		,m_nSelLibID(-1)
		,m_nSelItemID(-1)

	{
		m_pMainVLayout = new QHBoxLayout(this);

		m_pJBNavgationTree = new QTreeWidget(this);
		m_pJBNavgationTree->setWindowTitle(QString::fromLocal8Bit("导航"));
		m_pJBNavgationTree->headerItem()->setText(0,QString::fromLocal8Bit("分类导航"));
		m_pJBNavgationTree->setMinimumSize(230,300);
		m_pJBNavgationTree->setMaximumWidth(350);


		m_pJBDrawRectLabel = new QLabel(this);
		m_pJBDrawRectLabel->setMinimumSize(300,300);
		m_pJBDrawRectLabel->setMouseTracking(true);
		setMouseTracking(true);

		m_pMainVLayout->addWidget(m_pJBNavgationTree,1);
		m_pMainVLayout->addWidget(m_pJBDrawRectLabel,2);
		this->setLayout(m_pMainVLayout);

		// JB信息初始化
		m_pJBInfoManger = (XJBInfoManager*)XJBInfoManager::GetInstance();
		if ( m_pJBInfoManger->ReadJBInfoByDir(sfnGetAbsPath("./Data/JB")) )
		{
			// 初始化JB树
			refreshData();
		}

		QObject::connect(m_pJBNavgationTree,SIGNAL(currentItemChanged(QTreeWidgetItem*, QTreeWidgetItem*)),this,SLOT(onTreeItemChange(QTreeWidgetItem*,QTreeWidgetItem*)));
	}

	XJBWidget::~XJBWidget()
	{
	}

	void XJBWidget::closeEvent(QCloseEvent *_pCloseEvent)
	{
		emit slgCloseWnd();
		hide();
	}

	void XJBWidget::paintEvent(QPaintEvent* _pPaintEvent)
	{
		QDialog::paintEvent(_pPaintEvent);

		// 计算军标Size
		QRect jbDrawRct = m_pJBDrawRectLabel->rect();
		quint16 unJBCol = jbDrawRct.width()/m_cDrawJBSize.width();
		quint16 unJBRow = jbDrawRct.height()/m_cDrawJBSize.height();
		quint16 unWidth = unJBCol*m_cDrawJBSize.width();
		quint16 unHeight = unJBRow*m_cDrawJBSize.height();

		//  绘制网格
		QPainter painter(this);
		painter.translate(m_pJBDrawRectLabel->pos());
		painter.setPen(QPen(Qt::lightGray,1));

		for(int h=0;h<=unJBRow;++h)
		{
			painter.drawLine(QPointF(0,h*m_cDrawJBSize.height()),QPointF(unWidth,h*m_cDrawJBSize.height()));
			for (int i=0;i<=unJBCol;i++)
			{
				painter.drawLine(QPointF(i*m_cDrawJBSize.width(),0),QPointF(i*m_cDrawJBSize.width(),unHeight));
			}
		}

		if(nullptr != m_pCurShowCate )
		{
			QVector<QPointF> emptyPnts;
			// 循环绘制JB
			int nRowIdx = 0;
			int nColIdx = 0;
			int nJBW = m_cDrawJBSize.width();
			int nJBH = m_cDrawJBSize.height();
			foreach(Q_JBItem* pItem,m_pCurShowCate->cJBItems)
			{
				XShapePointJB xPntJB(XCoordinateGeo(0,0,0),m_pCurShowCate->unLibID,pItem->unID);
				xPntJB.SetSize(m_cDrawJBSize);
				XJBDrawHelper::DrawPointJB(&painter,&xPntJB,QPointF(nColIdx*nJBW+nJBW/2.0, nRowIdx*nJBH+nJBH/2.0),false,false);
				
				if(m_pCurShowCate->unLibID == m_nSelLibID
					&& pItem->unID == m_nSelItemID)
				{
					// 绘制选中框
					painter.setPen(QPen(Qt::green,2));
					QRect selRct(nColIdx*nJBW,nRowIdx*nJBH,nJBW,nJBH);
					painter.drawRect(selRct);
				}

				++nColIdx;
				if(nColIdx >= unJBCol)
				{
					nColIdx = 0;
					++nRowIdx;
				}
			}
		}
	}

	void XJBWidget::mouseMoveEvent(QMouseEvent* _pMouseEvent)
	{
		if(nullptr == _pMouseEvent)
		{
			return ;
		}

		if(nullptr == m_pCurShowCate || m_pCurShowCate->cJBItems.isEmpty())
		{
			return ;
		}

		QPoint glPt = _pMouseEvent->globalPos();
		QPoint curPt = _pMouseEvent->pos();
		curPt -= m_pJBDrawRectLabel->pos();
		if(curPt.x() <= 0 || curPt.y() <= 0 )
		{
			QToolTip::hideText();
			return;
		}

		int nJBW = m_cDrawJBSize.width();
		int nJBH = m_cDrawJBSize.height();
		int nColCount = m_pJBDrawRectLabel->width()/nJBW;
		int nRowCount = m_pJBDrawRectLabel->height()/nJBH;

		int nColIdx = curPt.x()/nJBW;
		int nRowIdx = curPt.y()/nJBH;
		if(nColIdx >= nColCount || nRowIdx >= nRowCount)
		{
			QToolTip::hideText();
			return;
		}
		quint16 unTrackIdx = nRowIdx*nColCount+nColIdx;
		if(unTrackIdx < m_pCurShowCate->cJBItems.count() && nullptr != m_pCurShowCate->cJBItems[unTrackIdx])
		{
			QToolTip::showText(glPt,m_pCurShowCate->cJBItems[unTrackIdx]->strName+QString("(%1)").arg(m_pCurShowCate->cJBItems[unTrackIdx]->unID),this);
		}else
			QToolTip::hideText();
	}

	void buildJBCategory(Q_JBCategory* _pCate, QTreeWidgetItem* _pLibItem)
	{
		if(nullptr == _pCate || nullptr == _pLibItem)
		{
			return ;
		}

		QTreeWidgetItem* pCateItem = new QTreeWidgetItem();
		pCateItem->setText(0,_pCate->strName);
		pCateItem->setData(0,Qt::UserRole,qlonglong(_pCate));
		_pLibItem->addChild(pCateItem);
			
		// 循环解析子类别
		foreach(Q_JBCategory* pSubCate,_pCate->cSubCateList)
		{
			buildJBCategory(pSubCate,pCateItem);
		}
		pCateItem = nullptr;
	}
	void XJBWidget::refreshData()
	{
		m_pJBNavgationTree->clear();

		foreach(Q_JBLib* pLib,XJBInfoManager::GetInstance()->m_cJBLibList)
		{
			if(nullptr == pLib)
			{
				continue;
			}
			
			QTreeWidgetItem* pLibItem = new QTreeWidgetItem();
			pLibItem->setText(0,pLib->strDesc+QString("(%1)").arg(pLib->unID));
			pLibItem->setData(0,Qt::UserRole,qlonglong(pLib));
			m_pJBNavgationTree->addTopLevelItem(pLibItem);
			foreach(Q_JBCategory* pCate, pLib->cCategoryList)
			{
				buildJBCategory(pCate,pLibItem);
			}
			pLibItem = nullptr;
		}
	}

	void XJBWidget::onTreeItemChange(QTreeWidgetItem* _pItem, QTreeWidgetItem* _pPreItem)
	{
		if(nullptr == _pItem || nullptr == _pItem->parent() || _pItem == _pPreItem)
		{
			m_pCurShowCate = nullptr;
			update();
			return ;
		}

		m_pCurShowCate = (Q_JBCategory*)_pItem->data(0,Qt::UserRole).toLongLong();

		if(m_pCurShowCate->unID != m_nSelLibID)
		{
			m_nSelLibID = m_nSelItemID = -1;
		}

		update();
	}

	void XJBWidget::mouseDoubleClickEvent(QMouseEvent* _pMouseEvent)
	{
		if(nullptr == _pMouseEvent)
		{
			return ;
		}

		if(nullptr == m_pCurShowCate || m_pCurShowCate->cJBItems.isEmpty())
		{
			return ;
		}

		QPoint glPt = _pMouseEvent->globalPos();
		QPoint curPt = _pMouseEvent->pos();
		curPt -= m_pJBDrawRectLabel->pos();
		if(curPt.x() <= 0 || curPt.y() <= 0 )
		{
			QToolTip::hideText();
			return;
		}

		int nJBW = m_cDrawJBSize.width();
		int nJBH = m_cDrawJBSize.height();
		int nColCount = m_pJBDrawRectLabel->width()/nJBW;
		int nRowCount = m_pJBDrawRectLabel->height()/nJBH;

		int nColIdx = curPt.x()/nJBW;
		int nRowIdx = curPt.y()/nJBH;
		if(nColIdx >= nColCount || nRowIdx >= nRowCount)
		{
			QToolTip::hideText();
			return;
		}
		quint16 unTrackIdx = nRowIdx*nColCount+nColIdx;
		if(unTrackIdx < m_pCurShowCate->cJBItems.count() && nullptr != m_pCurShowCate->cJBItems[unTrackIdx])
		{
			quint64 unKey = m_pCurShowCate->unLibID*10000+m_pCurShowCate->cJBItems[unTrackIdx]->unID;

			XJBItemWnd itemDlg(this);
			itemDlg.ShowJB(m_pCurShowCate->unLibID,m_pCurShowCate->cJBItems[unTrackIdx]->unID);
		}
	}

	void XJBWidget::mouseReleaseEvent(QMouseEvent* _pMouseEvent)
	{
		if(nullptr == _pMouseEvent)
		{
			return ;
		}

		if(nullptr == m_pCurShowCate || m_pCurShowCate->cJBItems.isEmpty())
		{
			return ;
		}

		QPoint glPt = _pMouseEvent->globalPos();
		QPoint curPt = _pMouseEvent->pos();
		curPt -= m_pJBDrawRectLabel->pos();
		if(curPt.x() <= 0 || curPt.y() <= 0 )
		{
			QToolTip::hideText();
			return;
		}

		int nJBW = m_cDrawJBSize.width();
		int nJBH = m_cDrawJBSize.height();
		int nColCount = m_pJBDrawRectLabel->width()/nJBW;
		int nRowCount = m_pJBDrawRectLabel->height()/nJBH;

		int nColIdx = curPt.x()/nJBW;
		int nRowIdx = curPt.y()/nJBH;
		if(nColIdx >= nColCount || nRowIdx >= nRowCount)
		{
			QToolTip::hideText();
			return;
		}
		quint16 unTrackIdx = nRowIdx*nColCount+nColIdx;
		if(unTrackIdx < m_pCurShowCate->cJBItems.count() && nullptr != m_pCurShowCate->cJBItems[unTrackIdx])
		{
			m_nSelLibID = m_pCurShowCate->unLibID;
			m_nSelItemID = m_pCurShowCate->cJBItems[unTrackIdx]->unID;
		}else
		{
			m_nSelLibID = m_nSelItemID = -1;

		}
		update();
	}

	const bool XJBWidget::IsCheckedItem()
	{
		if(m_nSelLibID != -1 && m_nSelItemID != -1)
		{
			return true;
		}

		return false;
	}

	const bool XJBWidget::GetCheckItem(quint32& _unRefLibID, quint32& _unRefItemID)
	{
		if(IsCheckedItem())
		{
			_unRefLibID = m_nSelLibID;
			_unRefItemID = m_nSelItemID;
			return true;
		}

		return false;
	}

}