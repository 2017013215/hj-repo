#include "stdafx.h"
#include "XTest_GISWnd.h"

#include <QtGui/QFileDialog>
#include <QtGui/QMessageBox>
#include <QtGui/QComboBox>
#include "XCommonLib.h"
#include "ChartManager/XHTManagerWnd.h"
#include "XDynamicTestThread.h"

using namespace XGIS;
XTest_GISWnd::XTest_GISWnd(QWidget *parent, Qt::WFlags flags)
	: QMainWindow(parent, flags)
	, m_pDynamicThread(nullptr)
{
	ui.setupUi(this);
	ui.mainToolBar->setVisible(false);

	// 创建界面
	XGIS::XGISCanvas::InitGISEnvi();
	m_pGISCanvas = new XGIS::XGISCanvas(this);
	setCentralWidget(m_pGISCanvas);

	// 设置画布颜色
	m_pGISCanvas->setCanvasColor(Qt::lightGray);

	m_pLastMapTool = nullptr;

	// 动态测试线程
	m_pDynamicThread = new XDynamicTestThread(this,m_pGISCanvas);

	// 矢量标绘工具栏
	m_pVDrawComboBox = new QComboBox(ui.mainToolBar);
	m_pVDrawComboBox->setIconSize(ui.mainToolBar->iconSize());
	m_pVDrawComboBox->addItem(QIcon(":/XTest_GISWnd/Resources/Icons/mActionAddBasicShape.svg"),QString::fromLocal8Bit("矢量绘制"));
	m_pVDrawComboBox->addItem(QIcon(":/XTest_GISWnd/Resources/Icons/mActionCapturePolygon.svg"),QString::fromLocal8Bit("多边形"));
	m_pVDrawComboBox->addItem(QIcon(":/XTest_GISWnd/Resources/Icons/mActionAddBasicRectangle.svg"),QString::fromLocal8Bit("矩形"));
	m_pVDrawComboBox->addItem(QIcon(":/XTest_GISWnd/Resources/Icons/mActionAddBasicCircle.svg"),QString::fromLocal8Bit("圆形"));
	m_pVDrawComboBox->addItem(QIcon(":/XTest_GISWnd/Resources/Icons/mActionAddBasicCircle.svg"),QString::fromLocal8Bit("椭圆形"));
	m_pVDrawAction = ui.mainToolBar->addWidget(m_pVDrawComboBox);

	// 信号链接
	QObject::connect(ui.pAcMapPan,SIGNAL(triggered()),this,SLOT(onMapToolPan()));
	QObject::connect(ui.pAcZoomIn,SIGNAL(triggered()),this,SLOT(onMapToolZoomIn()));
	QObject::connect(ui.pAcZoomOut,SIGNAL(triggered()),this,SLOT(onMapToolZoomOut()));
	QObject::connect(ui.pAcFullExtent,SIGNAL(triggered()),this,SLOT(onMapToolFullExtent()));
	QObject::connect(ui.pAcMeasureDis,SIGNAL(triggered()),this,SLOT(onMapToolMeasureDis()));
	QObject::connect(ui.pAcMeasureArea,SIGNAL(triggered()),this,SLOT(onMapToolMeasureArea()));
	QObject::connect(ui.pAcMeasureAngle,SIGNAL(triggered()),this,SLOT(onMapToolMeasureAngle()));
	QObject::connect(ui.pAcAddPoint,SIGNAL(triggered()),this,SLOT(onMapToolAddPoint()));
	QObject::connect(ui.pAcAddLine,SIGNAL(triggered()),this,SLOT(onMapToolAddLine()));
	//QObject::connect(ui.pAcAddPolygon,SIGNAL(triggered()),this,SLOT(onMapToolAddPolygon()));

	QObject::connect(ui.pAcAddJB,SIGNAL(triggered()),this,SLOT(onMapToolAddJB()));
	QObject::connect(ui.pAcSaveDraw,SIGNAL(triggered()),this,SLOT(onSaveDrawFile()));
	QObject::connect(ui.pAcOpenDraw,SIGNAL(triggered()),this,SLOT(onOpenDrawFile()));

	QObject::connect(ui.pAcLoadChart,SIGNAL(triggered()),this,SLOT(onLoadChart()));
	QObject::connect(ui.pAcUnloadChart,SIGNAL(triggered()),this,SLOT(onUnloadChart()));
	QObject::connect(ui.pAcChartSearch,SIGNAL(triggered()),this,SLOT(onChartSearch()));

	QObject::connect(ui.pAcDynamicTest,SIGNAL(triggered()),this,SLOT(onDynamicTest()));

	QObject::connect(m_pVDrawComboBox,SIGNAL(currentIndexChanged(int)),this,SLOT(onVDrawListChange(int)));

	ui.pAcMapPan->trigger();
}

XTest_GISWnd::~XTest_GISWnd()
{
	if(nullptr != m_pDynamicThread)
	{
		m_pDynamicThread->Stop();
		delete m_pDynamicThread;
		m_pDynamicThread = nullptr;
	}
	if(nullptr != m_pGISCanvas)
	{
		m_pLastMapTool = nullptr;
		delete m_pGISCanvas;
		m_pGISCanvas = nullptr;
	}

	if(nullptr != m_pVDrawComboBox)
	{
		delete m_pVDrawComboBox;
		m_pVDrawComboBox = nullptr;
		m_pVDrawAction = nullptr;
	}
}

void XTest_GISWnd::onMapToolPan()
{
	if(nullptr != m_pGISCanvas)
	{
		m_pGISCanvas->SetCurOpModel(XGISCanvas::Op_Pan);
		if(nullptr != m_pLastMapTool && m_pLastMapTool != ui.pAcMapPan)
		{
			m_pLastMapTool->setChecked(false);
		}
		m_pLastMapTool = ui.pAcMapPan;
	}
}

void XTest_GISWnd::onMapToolZoomIn()
{
	if(nullptr != m_pGISCanvas)
	{
		m_pGISCanvas->SetCurOpModel(XGISCanvas::Op_ZoomIn);
		if(nullptr != m_pLastMapTool && m_pLastMapTool != ui.pAcZoomIn)
		{
			m_pLastMapTool->setChecked(false);
		}
		m_pLastMapTool = ui.pAcZoomIn;
	}
}

void XTest_GISWnd::onMapToolZoomOut()
{
	if(nullptr != m_pGISCanvas)
	{
		m_pGISCanvas->SetCurOpModel(XGISCanvas::Op_ZoomOut);
		if(nullptr != m_pLastMapTool && m_pLastMapTool != ui.pAcZoomOut)
		{
			m_pLastMapTool->setChecked(false);
		}
		m_pLastMapTool = ui.pAcZoomOut;
	}
}

void XTest_GISWnd::onMapToolFullExtent()
{
	if(nullptr != m_pGISCanvas)
	{
		m_pGISCanvas->SetCurOpModel(XGISCanvas::Op_None);
		m_pGISCanvas->MapFullExtent();
		if(nullptr != m_pLastMapTool)
		{
			m_pLastMapTool->setChecked(false);
		}
		m_pLastMapTool = nullptr;
	}
}

void XTest_GISWnd::onMapToolMeasureDis()
{
	if(nullptr != m_pGISCanvas)
	{
		m_pGISCanvas->SetCurOpModel(XGISCanvas::Op_MeasureDis);
		if(nullptr != m_pLastMapTool && m_pLastMapTool != ui.pAcMeasureDis)
		{
			m_pLastMapTool->setChecked(false);
		}
		m_pLastMapTool = ui.pAcMeasureDis;
	}
}

void XTest_GISWnd::onMapToolAddLine()
{
	if(nullptr != m_pGISCanvas)
	{
		m_pGISCanvas->SetCurOpModel(XGISCanvas::Op_AddLine);
		if(nullptr != m_pLastMapTool && m_pLastMapTool != ui.pAcAddLine)
		{
			m_pLastMapTool->setChecked(false);
		}
		m_pLastMapTool = ui.pAcAddLine;
	}
}

void XTest_GISWnd::onMapToolAddPoint()
{
	if(nullptr != m_pGISCanvas)
	{
		m_pGISCanvas->SetCurOpModel(XGISCanvas::Op_AddPoint);
		if(nullptr != m_pLastMapTool && m_pLastMapTool != ui.pAcAddPoint)
		{
			m_pLastMapTool->setChecked(false);
		}
		m_pLastMapTool = ui.pAcAddPoint;
	}
}

void XTest_GISWnd::onMapToolAddPolygon()
{
	if(nullptr != m_pGISCanvas)
	{
		m_pGISCanvas->SetCurOpModel(XGISCanvas::Op_AddPolygon);
		if(nullptr != m_pLastMapTool && m_pLastMapTool != m_pVDrawAction)
		{
			m_pLastMapTool->setChecked(false);
		}
		m_pLastMapTool = m_pVDrawAction;
	}
}

void XTest_GISWnd::onMapToolMeasureArea()
{
	if(nullptr != m_pGISCanvas)
	{
		m_pGISCanvas->SetCurOpModel(XGISCanvas::Op_MeasureArea);
		if(nullptr != m_pLastMapTool && m_pLastMapTool != ui.pAcMeasureArea)
		{
			m_pLastMapTool->setChecked(false);
		}
		m_pLastMapTool = ui.pAcMeasureArea;
	}
}

void XTest_GISWnd::onMapToolMeasureAngle()
{
	if(nullptr != m_pGISCanvas)
	{
		m_pGISCanvas->SetCurOpModel(XGISCanvas::Op_MeasureAngle);
		if(nullptr != m_pLastMapTool && m_pLastMapTool != ui.pAcMeasureAngle)
		{
			m_pLastMapTool->setChecked(false);
		}
		m_pLastMapTool = ui.pAcMeasureAngle;
	}
}

void XTest_GISWnd::onMapToolAddJB()
{
	if(nullptr != m_pGISCanvas)
	{
		m_pGISCanvas->SetCurOpModel(XGISCanvas::Op_AddJB);
		if(nullptr != m_pLastMapTool && m_pLastMapTool != ui.pAcAddJB)
		{
			m_pLastMapTool->setChecked(false);
		}
		m_pLastMapTool = ui.pAcAddJB;
	}
}

void XTest_GISWnd::onLoadChart()
{
	QString strDir = sfnGetAbsPath("./Data/S57Chart/");
	QStringList strFileList = QFileDialog::getOpenFileNames(this, QString::fromLocal8Bit("选取海图文件"), strDir, QString::fromLocal8Bit("S57海图文件(*.000)"));
	if (strFileList.isEmpty())
		return;	// 没有选择任何文件

	// 使用S57海图适配器加载海图文件-存在多个数据文件时开启多线程加载数据
	int nRet = 0;
	foreach(QString strPath,strFileList)
	{
		nRet = m_pGISCanvas->LoadEChartFile(strPath);
		if(0 != nRet)
		{
			if(3 == nRet)
			{
				// 重复加载
				QMessageBox::warning(this,QString::fromLocal8Bit("提示"),QString::fromLocal8Bit("重复加载(%1)").arg(strPath));
			}
		}
	}
}

void XTest_GISWnd::onUnloadChart()
{
	XHTManagerWnd uploadDlg(this, m_pGISCanvas);
	uploadDlg.exec();
}

void XTest_GISWnd::onChartSearch()
{
	if(nullptr != m_pGISCanvas)
	{
		m_pGISCanvas->SetCurOpModel(XGISCanvas::Op_ChartAttSearch);
		if(nullptr != m_pLastMapTool && m_pLastMapTool != ui.pAcChartSearch)
		{
			m_pLastMapTool->setChecked(false);
		}
		m_pLastMapTool = ui.pAcChartSearch;
	}
}

void XTest_GISWnd::onDynamicTest()
{
	if(nullptr == m_pDynamicThread)
		return ;

	if(nullptr != m_pLastMapTool && m_pLastMapTool != ui.pAcDynamicTest)
	{
		m_pLastMapTool->setChecked(false);
	}
	m_pLastMapTool = ui.pAcDynamicTest;


	if(nullptr != m_pDynamicThread)
	{
		if(ui.pAcDynamicTest->isChecked())
		{
			if(false == m_pDynamicThread->isRunning())
				m_pDynamicThread->start();
			else
				m_pDynamicThread->Continue();
		}
		else
			m_pDynamicThread->Pause();
	}
}

void XTest_GISWnd::onVDrawListChange(int _nIdx)
{
	if(_nIdx < 0)
		return;

	switch(_nIdx)
	{
	case 0:
		{
			// 取消矢量标绘
			if(nullptr != m_pGISCanvas)
			{
				m_pGISCanvas->SetCurOpModel(XGISCanvas::Op_None);
				if(nullptr != m_pLastMapTool)
				{
					m_pLastMapTool->setChecked(false);
				}
				m_pLastMapTool = nullptr;
			}
		}break;
	case 1:
		{
			// 绘制多边形
			onMapToolAddPolygon();
		}break;
	case 2:
		{
			// 绘制矩形
			onMapToolAddRect();
		}break;
	case 3:
		{
			// 绘制圆形
			onMapToolAddCircle();
		}break;
	case 4:
		{
			// 绘制椭圆形
			onMapToolAddEllipses();
		}break;
	default:
		break;
	}
}

void XTest_GISWnd::onMapToolAddRect()
{
	if(nullptr != m_pGISCanvas)
	{
		m_pGISCanvas->SetCurOpModel(XGISCanvas::Op_AddRectangle);
		if(nullptr != m_pLastMapTool && m_pLastMapTool != m_pVDrawAction)
		{
			m_pLastMapTool->setChecked(false);
		}
		m_pLastMapTool = m_pVDrawAction;
	}
}

void XTest_GISWnd::onMapToolAddCircle()
{
	if(nullptr != m_pGISCanvas)
	{
		m_pGISCanvas->SetCurOpModel(XGISCanvas::Op_AddCircle);
		if(nullptr != m_pLastMapTool && m_pLastMapTool != m_pVDrawAction)
		{
			m_pLastMapTool->setChecked(false);
		}
		m_pLastMapTool = m_pVDrawAction;
	}
}

void XTest_GISWnd::onMapToolAddEllipses()
{
	if(nullptr != m_pGISCanvas)
	{
		m_pGISCanvas->SetCurOpModel(XGISCanvas::Op_AddEllipses);
		if(nullptr != m_pLastMapTool && m_pLastMapTool != m_pVDrawAction)
		{
			m_pLastMapTool->setChecked(false);
		}
		m_pLastMapTool = m_pVDrawAction;
	}
}

void XTest_GISWnd::onSaveDrawFile()
{
	QString strPath = QFileDialog::getSaveFileName(this,QString::fromLocal8Bit("保存态势文件"),sfnGetAbsPath("./Data/StateFile"),QString::fromLocal8Bit("态势文件(*.STF)"));
	if(false == strPath.isEmpty())
	{
		bool bRes = m_pGISCanvas->SaveStateFile(strPath);
		if(true == bRes)
		{
			QMessageBox::about(this,QString::fromLocal8Bit("提示"),QString::fromLocal8Bit("存储成功"));
			return ;
		}
	}
	QMessageBox::about(this,QString::fromLocal8Bit("提示"),QString::fromLocal8Bit("存储失败"));
}

void XTest_GISWnd::onOpenDrawFile()
{
	QString strPath = QFileDialog::getOpenFileName(this,QString::fromLocal8Bit("打开态势文件"),sfnGetAbsPath("./Data/StateFile"),QString::fromLocal8Bit("态势文件(*.STF)"));
	if(false == strPath.isEmpty())
	{
		bool bRes = m_pGISCanvas->OpenStateFile(strPath);
		if(true == bRes)
		{
			QMessageBox::about(this,QString::fromLocal8Bit("提示"),QString::fromLocal8Bit("打开成功"));
			return ;
		}
	}
	QMessageBox::about(this,QString::fromLocal8Bit("提示"),QString::fromLocal8Bit("打开失败"));
}

void XTest_GISWnd::showEvent( QShowEvent *e )
{
	ui.mainToolBar->setVisible(true);

	__super::showEvent(e);
}

void XTest_GISWnd::hideEvent( QHideEvent *e )
{
	ui.mainToolBar->setVisible(false);

	__super::hideEvent(e);
}
