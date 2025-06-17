#include "XGISCanvas.h"
#include <XGISVLayer.h>

// QGIS
#include <qgsproviderregistry.h>
#include <qgsmaplayerregistry.h>
#include <qgsmaptoolpan.h>
#include <qgsmaptoolzoom.h>

// GIS-Extend
#include <GIS-Extend/qgsmeasuretool.h>
#include <GIS-Extend/XJBEditMapTool.h>
#include <GIS-Extend/XEditVItemMapTool.h>
#include <GIS-Extend/XShapeDef.h>
#include <GIS-Extend/XChartAttSearchMapTool.h>
#include <GIS-Extend/绘图板/XDrawRubber.h>

// Windows
#ifdef WIN32
#include <windows.h>
#endif

// DependLib
#include <XCommonLib.h>
#include "XVItemRenderV2.h"
#include "qgsmaptoolmeasureangle.h"
#include "Qt-JB/XJBInfoManager.h"
#include <XS57LayerAdapter.h>

namespace XGIS
{
	XGISCanvas::XGISCanvas(QWidget* _pParWnd/* = nullptr*/)
		: QgsMapCanvas(_pParWnd)
		, m_pStaticRubber(nullptr)
		, m_pDynamicRubber(nullptr)
	{
		// JB库初始化
		bool bRes = ((XJBInfoManager*)XJBInfoManager::GetInstance())->ReadJBInfoByDir(sfnGetAbsPath("Data/JB"));
		if(false == bRes)
		{
			Q_ASSERT(false);
			return ;
		}

		// 创建&加载世界地图图层
		{
			m_strWorldSHPPath = sfnGetAbsPath("Data/Shp/Countries.shp");
			m_pWorldLayer = new XGISVLayer(m_strWorldSHPPath, QString::fromLocal8Bit("World图层"), "ogr");
			if ( nullptr == m_pWorldLayer || false == m_pWorldLayer->isValid())
			{
				Q_ASSERT(false);
				return;
			}
			QgsMapLayerRegistry::instance()->addMapLayer(m_pWorldLayer, true);
			m_curLayerDeq.push_front(m_pWorldLayer);
		}

		// 地图工具初始化
		{
			// 地图漫游
			QgsMapTool* pMapTool = new QgsMapToolPan(this);
			m_pMapTools[Op_Pan] = pMapTool;
			// 放大地图
			pMapTool = new QgsMapToolZoom(this,false);
			m_pMapTools[Op_ZoomIn] = pMapTool;
			// 缩小地图
			pMapTool = new QgsMapToolZoom(this,true);
			m_pMapTools[Op_ZoomOut] = pMapTool;

			// 距离测量
			pMapTool = new QgsMeasureTool(this,false);
			m_pMapTools[Op_MeasureDis] = pMapTool;
			// 面积测量
			pMapTool = new QgsMeasureTool(this,true);
			m_pMapTools[Op_MeasureArea] = pMapTool;
			// 角度测量
			pMapTool = new QgsMapToolMeasureAngle(this);
			m_pMapTools[Op_MeasureAngle] = pMapTool;

			// 添加点
			pMapTool = new XEditVItemMapTool(this, XEditVItemMapTool::EVItem_Point);
			m_pMapTools[Op_AddPoint] = pMapTool;

			// 添加线段
			pMapTool = new XEditVItemMapTool(this, XEditVItemMapTool::EVItem_Line);
			m_pMapTools[Op_AddLine] = pMapTool;
			pMapTool = nullptr;

			// 添加多边形标绘
			XEditVItemMapTool* pEditVMapTool = new XEditVItemMapTool(this, XEditVItemMapTool::EVItem_Polygon);
			m_pMapTools[Op_AddPolygon] = pEditVMapTool;
			pEditVMapTool = nullptr;

			// 添加矩形标绘
			pMapTool = new XEditVItemMapTool(this,XEditVItemMapTool::EVItem_Rectangle);
			m_pMapTools[Op_AddRectangle] = pMapTool;
			pMapTool = nullptr;

			// 添加圆形标绘
			pMapTool = new XEditVItemMapTool(this,XEditVItemMapTool::EVItem_Circle);
			m_pMapTools[Op_AddCircle] = pMapTool;
			pMapTool = nullptr;

			// 添加椭圆形标绘
			pMapTool = new XEditVItemMapTool(this,XEditVItemMapTool::EVItem_Ellipses);
			m_pMapTools[Op_AddEllipses] = pMapTool;
			pMapTool = nullptr;

			// 标绘
			XJBEditMapTool* pJBEditMapTool = new XJBEditMapTool(this);
			m_pMapTools[Op_AddJB] = pJBEditMapTool;
			pJBEditMapTool = nullptr;

			// 海图属性查询
			XChartAttSearchMapTool* pChartSerchTool = new XChartAttSearchMapTool(this);
			m_pMapTools[Op_ChartAttSearch] = pChartSerchTool;
			pChartSerchTool = nullptr;
		}

		// 其他数据初始化
		m_eCurOpModel = Op_None;

		// 海图插件初始化
		m_strChartPaths.clear();
		m_pChartAdp = new XSpace_Plugins::XS57LayerAdapter();
		m_pChartAdp->InitAdapter(this);
		QObject::connect(m_pChartAdp,SIGNAL(slgLayerUpdate()),this,SLOT(TouchLayers()));

		// 绘图蒙版
		m_pStaticRubber = new XDrawRubber(this);
		m_pStaticRubber->show();
		QObject::connect( this, SIGNAL( extentsChanged() ), m_pStaticRubber, SLOT(updatePosition()) );
		m_pDynamicRubber = new XDrawRubber(this);
		m_pDynamicRubber->show();
		QObject::connect( this, SIGNAL( extentsChanged() ), m_pDynamicRubber, SLOT(updatePosition()) );
		m_pDynamicRubber->StartAutoUpdate(1500);

		// 设置图层并刷新界面
		//setLayerSet(m_curLayerDeq);
		TouchLayers();
		freeze(false);
		//zoomToFullExtent();
		setExtent(QgsRectangle(73,54,135,4)); // 定位到中国
		refresh();
	}

	XGISCanvas::~XGISCanvas()
	{
		// 删除海图插件
		foreach(QString strPath,m_strChartPaths)
		{
			m_pChartAdp->UnloadS57File(strPath);
		}
		delete m_pChartAdp;
		m_pChartAdp = nullptr;

		// 删除蒙版
		if(nullptr != m_pStaticRubber)
		{
			m_pStaticRubber->ClearAllShape();
			delete m_pStaticRubber;
			m_pStaticRubber = nullptr;
		}
		if(nullptr != m_pDynamicRubber)
		{
			m_pDynamicRubber->ClearAllShape();
			delete m_pDynamicRubber;
			m_pDynamicRubber = nullptr;
		}

		// 卸载地图工具
		{
			QgsMapTool* pMapTool = mapTool();
			if(nullptr != pMapTool)
			{
				unsetMapTool(pMapTool);
				pMapTool = nullptr;
			}

			foreach(QgsMapTool* pMapTool, m_pMapTools)
			{
				delete pMapTool;
				pMapTool = nullptr;
			}
			m_pMapTools.clear();
		}

		stopRendering();

		QgsMapLayerRegistry::instance()->removeAllMapLayers();
		m_pWorldLayer = nullptr;
	}

#pragma region 画布事件
	// 获取画布颜色
	QColor XGISCanvas::canvasColor() const
	{
		return QgsMapCanvas::canvasColor();
	}
	// 设置画布颜色
	void XGISCanvas::setCanvasColor(const QColor & _newVal)
	{
		QgsMapCanvas::setCanvasColor(_newVal);
	}
	// 拖拽进入事件
	void XGISCanvas::dragEnterEvent(QDragEnterEvent* _pEvent)
	{
		QgsMapCanvas::dragEnterEvent(_pEvent);
	}
	// 键盘事件-按下
	void XGISCanvas::keyPressEvent(QKeyEvent* _pEvent)
	{
		QgsMapCanvas::keyPressEvent(_pEvent);
	}
	// 键盘事件-释放
	void XGISCanvas::keyReleaseEvent(QKeyEvent* _pEvent)
	{
		QgsMapCanvas::keyReleaseEvent(_pEvent);
	}
	// 鼠标事件-双击
	void XGISCanvas::mouseDoubleClickEvent(QMouseEvent* _pEvent)
	{
		QgsMapCanvas::mouseDoubleClickEvent(_pEvent);
	}
	// 鼠标事件-移动
	void XGISCanvas::mouseMoveEvent(QMouseEvent* _pEvent)
	{
		QgsMapCanvas::mouseMoveEvent(_pEvent);
	}
	// 鼠标事件-按下
	void XGISCanvas::mousePressEvent(QMouseEvent* _pEvent)
	{
		QgsMapCanvas::mousePressEvent(_pEvent);
	}
	// 鼠标事件-释放
	void XGISCanvas::mouseReleaseEvent(QMouseEvent* _pEvent)
	{
		QgsMapCanvas::mouseReleaseEvent(_pEvent);
	}
	// 重绘
	void XGISCanvas::paintEvent(QPaintEvent* _pEvent)
	{
		QgsMapCanvas::paintEvent(_pEvent);
	}
	// 大小改变事件
	void XGISCanvas::resizeEvent(QResizeEvent* _pEvent)
	{
		QgsMapCanvas::resizeEvent(_pEvent);
	}
	// 鼠标滚轮事件
	void XGISCanvas::wheelEvent(QWheelEvent* _pEvent)
	{
		QgsMapCanvas::wheelEvent(_pEvent);
	}

	void XGISCanvas::SetCurOpModel(EOpModel _eModel)
	{
		QgsMapTool* pCurTool = mapTool();
		if(Op_None == _eModel)
		{
			if(nullptr != pCurTool)
			{
				unsetMapTool(pCurTool);
				pCurTool = nullptr;
			}
			m_eCurOpModel = Op_None;
			return;
		}
		
		// 查询是否包含该操作的处理方式
		if(false == m_pMapTools.contains(_eModel))
		{
			return ;
		}

		// 设置地图工具
		if(pCurTool == m_pMapTools[_eModel])
		{
			unsetMapTool(pCurTool);
			pCurTool = nullptr;
			return;
		}
		setMapTool(m_pMapTools[_eModel]);
	}

	void XGISCanvas::MapFullExtent()
	{
		zoomToFullExtent();
	}

	void XGISCanvas::InitGISEnvi()
	{
		// 初始化环境
		{
			// 初始化插件目录
			QgsProviderRegistry::instance(sfnGetAbsPath("GIS-Plugins"));
			QgsApplication::setPkgDataPath(".");
			// 设置最大线程数
			quint8 unCUPProcessNum = 2;
			if (WIN32)
			{
				SYSTEM_INFO sysInfo;
				GetSystemInfo(&sysInfo);
				unCUPProcessNum = sysInfo.dwNumberOfProcessors;
			}
			QgsApplication::setMaxThreads(unCUPProcessNum);
		}
	}

	bool XGISCanvas::GetScreenPosByGeo(QPointF _pGeoPos, QPointF& _cRefPixPos)
	{
		const QgsMapToPixel* pMap2Pix = getCoordinateTransform();
		if (nullptr == pMap2Pix)
		{
			return false;
		}
		
		_cRefPixPos = pMap2Pix->transform(_pGeoPos.x(),_pGeoPos.y()).toQPointF();

		pMap2Pix = nullptr;
		return true;
	}

	int XGISCanvas::LoadEChartFile(const QString& _strChartPath)
	{
		if(_strChartPath.isEmpty())
		{
			return 1;
		}
		QString strAbsPath = sfnGetAbsPath(_strChartPath);
		if(false == QFile::exists(strAbsPath))
		{
			return 2;
		}

		if(true == m_strChartPaths.contains(strAbsPath))
		{
			return 3;
		}
		m_pChartAdp->LoadS57Files(QStringList()<<strAbsPath);
		m_strChartPaths.append(strAbsPath);

		return 0;
	}

	void XGISCanvas::TouchLayers()
	{
		QList<QgsMapCanvasLayer> layerSet;
		layerSet.push_front(m_pWorldLayer);

		// 获取海图插件图层
		QList<QgsMapLayer*> chartLayers = m_pChartAdp->GetLayers();
		foreach(QgsMapLayer* pLayer, chartLayers)
		{
			layerSet.push_front(pLayer);
		}
		setLayerSet(layerSet);

		freeze(false);
		refresh();
	}

	void XGISCanvas::UnloadEChartFile(const QString& _strChartPath)
	{
		QString strAbsPath = sfnGetAbsPath(_strChartPath);
		if(false == m_strChartPaths.contains(strAbsPath))
			return ;
		m_pChartAdp->UnloadS57File(strAbsPath);
		m_strChartPaths.removeOne(strAbsPath);
	}

	const QStringList& XGISCanvas::GetCurCharts() const
	{
		return m_strChartPaths;
	}

	const QStringList XGISCanvas::GetChartInfoByPt(QPointF _pt)
	{
		QStringList strRet;

		HTFeatureInfoList feaList;
		if(m_pChartAdp)
		{
			m_pChartAdp->GetHTInfo(_pt,feaList);

			// 提取显示字符串
			foreach(HTFeatureInfo fInfo,feaList)
			{
				if(false == strRet.isEmpty())
				{
					strRet.append("");
				}
				strRet.append(QString::fromLocal8Bit("类型：%1").arg(fInfo.strType));
				strRet.append(QString::fromLocal8Bit("属性信息："));
				int nAttCount = fInfo.strAttNames.count();
				for (int nIdx=0;nIdx<nAttCount;nIdx++)
				{
					QString strAtt = QString::fromLocal8Bit("<%1>：").arg(fInfo.strAttNames.at(nIdx));
					if(fInfo.strAttVals.size() > nIdx)
					{
						strAtt += fInfo.strAttVals.at(nIdx);
					}
					strRet.append(strAtt);
				}
				strRet.append("--------------------------");
				strRet.append("");
			}
		}

		return strRet;
	}

	const quint32 XGISCanvas::AddShape(const XShape* _pxShape)
	{
		if(nullptr == _pxShape || nullptr == m_pStaticRubber || nullptr == m_pDynamicRubber)
		{
			return 0;
		}

		quint32 unIDRet = 0;
		if(_pxShape->GetType() == XShape::eType_PointJB)
		{
			unIDRet = m_pDynamicRubber->AddShape(_pxShape);
		}else
		{
			unIDRet = m_pStaticRubber->AddShape(_pxShape);
		}
		return unIDRet;
	}

	const XShape* XGISCanvas::GetShapeByID(quint32 _unID) const
	{
		if(_unID == 0 || nullptr == m_pDynamicRubber || nullptr == m_pStaticRubber)
		{
			return nullptr;
		}

		const XShape* pRet = nullptr;
		pRet = m_pStaticRubber->GetShapeByID(_unID);
		if(nullptr == pRet)
		{
			pRet = m_pDynamicRubber->GetShapeByID(_unID);
		}
		return pRet;
	}

	const bool XGISCanvas::RemoveShapeByID(quint32 _unID) const
	{
		if(_unID == 0 || nullptr == m_pDynamicRubber || nullptr == m_pStaticRubber)
		{
			return nullptr;
		}

		bool bRet = false;
		bRet = m_pStaticRubber->RemoveShapeByID(_unID);
		if(false == bRet)
		{
			bRet = m_pDynamicRubber->RemoveShapeByID(_unID);
		}
		return bRet;
	}

	const bool XGISCanvas::SaveStateFile(const QString& _strFilePath)
	{
		if(true == _strFilePath.isEmpty())
			return false;

		if(true == QFile::exists(_strFilePath))
			return false;

		return m_pStaticRubber->SaveStateFile(_strFilePath);
	}

	const bool XGISCanvas::OpenStateFile(const QString& _strFilePath)
	{
		if(true == _strFilePath.isEmpty())
			return false;
		if(false == QFile::exists(_strFilePath))
			return false;
		
		return m_pStaticRubber->OpenStateFile(_strFilePath);
	}

#pragma endregion
}