#pragma once

#include <QDialog>
namespace XGIS
{
	class XJBItemWnd : public QDialog
	{
	public:
		XJBItemWnd(QWidget* _pParentWnd);
		~XJBItemWnd(void);

	protected:
		virtual void paintEvent(QPaintEvent* _paintEvt);

	public:
		void ShowJB(quint32 _unLibID, quint32 _unItemID, bool _bIsShowDetail = true);

	private:
		quint32 m_unLibID;
		quint32 m_unItemID;
	};
}