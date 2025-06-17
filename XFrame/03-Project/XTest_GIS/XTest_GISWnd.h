#ifndef XTEST_GISWND_H
#define XTEST_GISWND_H

// 导入头文件

// QT
#include <QtGui/QMainWindow>
#include "ui_XTest_GISWnd.h"

// QGIS
#include <XGISCanvas.h>

class XDynamicTestThread;
class QComboBox;
class XTest_GISWnd : public QMainWindow
{
	Q_OBJECT

public:
	XTest_GISWnd(QWidget *parent = 0, Qt::WFlags flags = 0);
	~XTest_GISWnd();

private slots:
	// 地图漫游响应
	void onMapToolPan();
	// 地图放大响应
	void onMapToolZoomIn();
	// 地图缩小响应
	void onMapToolZoomOut();
	// 地图全貌
	void onMapToolFullExtent();
	// 距离测量
	void onMapToolMeasureDis();
	// 面积测量
	void onMapToolMeasureArea();
	// 角度测量
	void onMapToolMeasureAngle();
	// 添加点
	void onMapToolAddPoint();
	// 添加线段
	void onMapToolAddLine();
	// 添加多边形
	void onMapToolAddPolygon();
	// 添加矩形
	void onMapToolAddRect();
	// 添加圆形
	void onMapToolAddCircle();
	// 添加椭圆形
	void onMapToolAddEllipses();

	// 标绘
	void onMapToolAddJB();
	// 存储态势文件
	void onSaveDrawFile();
	// 打开态势文件
	void onOpenDrawFile();

	// 加载海图
	void onLoadChart();
	// 卸载海图
	void onUnloadChart();
	// 海图查询
	void onChartSearch();

	// 动态测试
	void onDynamicTest();

	// 矢量标绘下拉框事件
	void onVDrawListChange(int _nIdx);

private:
	Ui::XTest_GISWndClass ui;

	// GIS画布
	XGIS::XGISCanvas* m_pGISCanvas;

	// 矢量标绘下拉框
	QComboBox* m_pVDrawComboBox;
	QAction* m_pVDrawAction;

	// 临时存储
	QAction* m_pLastMapTool;

	// 动态测试线程
	XDynamicTestThread* m_pDynamicThread;

};

#endif // XTEST_GISWND_H
