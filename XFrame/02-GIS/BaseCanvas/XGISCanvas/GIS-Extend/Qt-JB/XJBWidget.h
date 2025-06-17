#ifndef XJBWIDGET_H
#define XJBWIDGET_H

#include <QDialog>
#include <QMap>

class QHBoxLayout;
class QTreeWidget;
class QLabel;
class QTreeWidgetItem;

namespace XGIS
{
	class XJBInfoManager;
	class Q_JBCategory;
	class XJBWidget : public QDialog
	{
		Q_OBJECT
	public:
		XJBWidget(QWidget* _pParentWnd);
		~XJBWidget();

	public:
		const bool IsCheckedItem();
		const bool GetCheckItem(quint32& _unRefLibID, quint32& _unRefItemID);
	signals:
		void slgCloseWnd();

	protected:
		virtual void closeEvent(QCloseEvent* _pCloseEvent);

		virtual void paintEvent(QPaintEvent* _pPaintEvent);

		virtual void mouseMoveEvent (QMouseEvent* _pMouseEvent);

		virtual void mouseDoubleClickEvent(QMouseEvent* _pMouseEvent);

		virtual void mouseReleaseEvent(QMouseEvent* _pMouseEvent);

	private slots:
		void onTreeItemChange(QTreeWidgetItem* _pItem, QTreeWidgetItem* _pPreItem);

	private:
		void refreshData();

	private:
		QHBoxLayout* m_pMainVLayout;
		QTreeWidget* m_pJBNavgationTree;
		QLabel*		 m_pJBDrawRectLabel;

		QSize		 m_cDrawJBSize;

		XJBInfoManager* m_pJBInfoManger;
		Q_JBCategory* m_pCurShowCate;

		qint32 m_nSelLibID;
		qint32 m_nSelItemID;
	};
}
#endif // XJBWIDGET_H
