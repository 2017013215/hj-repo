#include "xchartsearchreswnd.h"

#include <QtGui/QTextEdit>
#include <QtGui/QResizeEvent>
namespace XGIS
{
	XChartSearchResWnd::XChartSearchResWnd(QWidget *parent)
		: QDialog(parent,nullptr)
	{
		m_cLastPos = QPoint(0,0);

		m_pShowEdit = new QTextEdit(this);
	}

	XChartSearchResWnd::~XChartSearchResWnd()
	{
		m_pShowEdit->clear();
		delete m_pShowEdit;
		m_pShowEdit = nullptr;
	}

	void XChartSearchResWnd::ShowThis()
	{
		move(m_cLastPos);
		show();
	}

	void XChartSearchResWnd::HideThis()
	{
		m_cLastPos = pos();
		hide();
	}

	void XChartSearchResWnd::closeEvent(QCloseEvent *_pEvent)
	{
		m_cLastPos = pos();
		__super::closeEvent(_pEvent);
	}

	void XChartSearchResWnd::UpdateInfo(const QStringList& _strDetailInfo)
	{
		m_pShowEdit->clear();
		QString strShow;
		foreach(QString strLine,_strDetailInfo)
		{
			strShow += strLine;
			strShow += "\r\n";
		}
		m_pShowEdit->setText(strShow);
	}

	void XChartSearchResWnd::resizeEvent(QResizeEvent *_pEvent)
	{
		if(nullptr != m_pShowEdit)
		{
			m_pShowEdit->move(0,0);
			m_pShowEdit->resize(_pEvent->size());
		}
	}

}