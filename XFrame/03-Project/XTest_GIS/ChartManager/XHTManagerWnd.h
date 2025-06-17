#ifndef XHTMANAGERWND_H
#define XHTMANAGERWND_H

#include <QtGui/QDialog>
#include "ui_XHTManagerWnd.h"

namespace XGIS
{
	class XGISCanvas;
}
using namespace XGIS;
class XHTManagerWnd : public QDialog
{
	Q_OBJECT

public:
	XHTManagerWnd(QWidget *_pParent,XGISCanvas* _pGISCanvas);
	~XHTManagerWnd();

private slots:
	// 响应卸载海图文件按钮
	void OnUploadBtn();
	// 响应数据列表选中项改变事件
	void OnItemCheck(QTreeWidgetItem *_pItem, int _nColumn);

private:
	Ui::XHTManagerWnd ui;			// 界面文件
	QStringList m_strCurSelFiles;	// 当前选中需要卸载的文件路径
	XGISCanvas* m_pxCanvas;			// 地图画布
};

#endif // XHTMANAGERWND_H
