#ifndef XCHARTSEARCHRESWND_H
#define XCHARTSEARCHRESWND_H

#include <QDialog>

class QTextEdit;
namespace XGIS
{
	class XChartSearchResWnd : public QDialog
	{
		Q_OBJECT

	public:
		XChartSearchResWnd(QWidget *parent);
		~XChartSearchResWnd();

		// 显示当前窗口-自动调整位置
		void ShowThis();
		// 隐藏当前窗口-自动保存位置
		void HideThis();
		// 更新当前显示HTML文本
		void UpdateInfo(const QStringList& _strDetailInfo);

	protected:
		virtual void closeEvent(QCloseEvent *_pEvent);
		virtual void resizeEvent(QResizeEvent *_pEvent);

	private:
		QPoint m_cLastPos;
		QTextEdit* m_pShowEdit;
	};
}

#endif // XCHARTSEARCHRESWND_H
