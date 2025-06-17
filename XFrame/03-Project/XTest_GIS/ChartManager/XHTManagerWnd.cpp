#include "XHTManagerWnd.h"
#include <QtCore/QFileInfo>

#include "XGISCanvas.h"

XHTManagerWnd::XHTManagerWnd(QWidget *_pParent, XGISCanvas* _pCanvas)
	: QDialog(_pParent)
{
	ui.setupUi(this);

	m_strCurSelFiles.clear();
	m_pxCanvas = _pCanvas;

	// 刷新数据列表
	if (m_pxCanvas)
	{
		QStringList strPaths = m_pxCanvas->GetCurCharts();
		foreach(QString strPth, strPaths)
		{
			QFileInfo fInfo(strPth);
			QString strName = fInfo.fileName();
			if (strName.isEmpty())
				continue;

			QTreeWidgetItem* pItem = new QTreeWidgetItem(ui.pTreeData);
			pItem->setText(0,strName);
			pItem->setToolTip(0,strPth);
			pItem->setData(0, Qt::UserRole, strPth);
			pItem->setCheckState(0, Qt::Unchecked);
			ui.pTreeData->addTopLevelItem(pItem);
		}
	}

	// 链接信号槽
	{
		QObject::connect(ui.pBtnUpload, SIGNAL(clicked()),							this, SLOT(OnUploadBtn()));
		QObject::connect(ui.pTreeData,	SIGNAL(itemClicked(QTreeWidgetItem*, int)), this, SLOT(OnItemCheck(QTreeWidgetItem*, int)));
	}
}

XHTManagerWnd::~XHTManagerWnd()
{
	m_strCurSelFiles.clear();
	m_pxCanvas = nullptr;
}

void XHTManagerWnd::OnUploadBtn()
{
	if (m_pxCanvas == nullptr)
	{
		return;
	}

	// 循环卸载海图文件
	foreach(QString strPth, m_strCurSelFiles)
	{
		m_pxCanvas->UnloadEChartFile(strPth);
	}
	m_strCurSelFiles.clear();

	// 重新刷新数据表
	{
		ui.pTreeData->clear();
		QStringList strPaths = m_pxCanvas->GetCurCharts();
		foreach(QString strPth, strPaths)
		{
			QFileInfo fInfo(strPth);
			QString strName = fInfo.fileName();
			if (strName.isEmpty())
				continue;

			QTreeWidgetItem* pItem = new QTreeWidgetItem(ui.pTreeData);
			pItem->setText(0, strName);
			pItem->setToolTip(0, strPth);
			pItem->setData(0, Qt::UserRole, strPth);
			pItem->setCheckState(0, Qt::Unchecked);
			ui.pTreeData->addTopLevelItem(pItem);
			pItem = nullptr;
		}
	}
}

void XHTManagerWnd::OnItemCheck(QTreeWidgetItem *_pItem, int _nColumn)
{
	if (_pItem && _nColumn == 0)
	{
		QString strPath = _pItem->data(0, Qt::UserRole).toString();
		if (_pItem->checkState(0) == Qt::Checked
			&& m_strCurSelFiles.contains(strPath) == false)
		{
			m_strCurSelFiles.append(strPath);
		}
		else if (_pItem->checkState(0) != Qt::Checked)
		{
			m_strCurSelFiles.removeOne(strPath);
		}

		if (m_strCurSelFiles.isEmpty())
			ui.pBtnUpload->setText(QString::fromLocal8Bit("卸载海图文件（0）"));
		else
		{
			ui.pBtnUpload->setText(QString::fromLocal8Bit("卸载海图文件（%1）").arg(m_strCurSelFiles.count()));
			ui.pBtnUpload->setEnabled(true);
		}
	}
}
