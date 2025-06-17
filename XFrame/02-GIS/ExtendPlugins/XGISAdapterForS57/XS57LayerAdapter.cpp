#include "XS57LayerAdapter.h"
#include <QGIS/qgsvectorlayer.h>
#include <QGIS/qgsmaplayerregistry.h>
#include <QGIS/qgsmarkersymbollayerv2.h>
#include <QGIS/qgspallabeling.h>
#include <QGIS/qgslinesymbollayerv2.h>
#include <QGIS/qgsfillsymbollayerv2.h>
#include <QGIS/qgssymbollayerv2registry.h>
#include <QGIS/qgstextlabelfeature.h>
#include "QGIS/qgsrendererv2.h"
#include "QGIS/qgspallabeling.h"
#include "QGIS/qgsfillsymbollayerv2.h"
#include <QGIS/qgssinglesymbolrendererv2.h>
#include "QGIS/qgsrasterlayer.h"
#include <QGIS/qgsMapcanvas.h>
#include "QGIS/qgscategorizedsymbolrendererv2.h"
#include "XColorTable.h"
#include <QtGui/QImage>
#include <QtGui/QPainter>
#include <QtGui/QScreen>
#include <QApplication>
#include <QDesktopWidget>
#include <QtXml/QDomDocument>
#include "XS52Lib.h"

const GUID GUID_CLASS_MONITOR = { 0x4d36e96e, 0xe325, 0x11ce, 0xbf, 0xc1, 0x08, 0x00, 0x2b, 0xe1, 0x03, 0x18 };
#include <windows.h>
#include <setupapi.h>
#include <tchar.h>
#include <winioctl.h>
#include <initguid.h>
#include "XImageMarkerSymbolLayerV2.h"
//#include "qgsmaptoolselectutils.h"
#include "QMessageBox"
#include "XCommonLib.h"
#include <XS57DataDoc.h>
#include "XLoadLayerFeatureThread.h"
#include <QString>

using namespace XSpace_Plugins;
//using namespace XSpace_Common;

namespace XSpace_Plugins
{
	XS57LayerAdapter::XS57LayerAdapter(QObject* _pxParent/* = NULL*/)
		: QObject(_pxParent)
	{
		m_bIsInit = false;
		m_pxMapCanvas = nullptr;
		m_pxS52Lib = nullptr;
		m_strColorStyleNames.clear();
		m_xeColorStyle = xeNone;
		m_xeShowLev = xeBase;
		m_pxShowLayerSet.clear();
		m_pxWorldLayer = nullptr;
		m_pxS57DocMap.clear();
		m_pxCondFNCs.clear();
		m_xScreenSize = QSize(0, 0);
		m_dXPixPerMM = m_dYPixPerMM = 0.0;
		m_dCanvasScaleFactor = 0.0;
		m_dTideRectangleScale = 0.0;
		m_dCurrentArrowScale = 0.0;
		m_dTideDrawScaler = 0.0;
		m_dCurrentDrawScaler = 0.0;
		m_xS57ClassVec.clear();
		m_xS57ClassCodeToStrs.clear();
		m_xS57ClassStrToCodes.clear();
		m_xS57AttsVec.clear();
		m_xAttsCodeToStrs.clear();
		m_xAttsStrToCodes.clear();
		m_xLookupTABs.clear();
		m_pxCondTypes.clear();
		m_bUseTwoShades = false;
		m_dSafetyContour = 0;
		m_dSafetyDepth = 0;
		m_dShallowContour = 0;
		m_dDeepContour = 0;
		m_xRealScreenSize = QSize(0, 0);
		m_xeDepthUnitDisplay = xeMeter;
		m_unSymbolIndex = 0;

		m_pxAreaNormalLayerMap.clear();
		m_strAreaNormalLayerINSTs.clear();
		m_pxAreaNormalLayerRenderer.clear();

		m_pxCustomLayerMap.clear();
		m_strCustomLayerINSTs.clear();
		m_pxCustomLayerRenderer.clear();

		m_pxAreaCoverLayerMap.clear();
		m_strAreaCoverLayerINSTs.clear();
		m_pxAreaCoverLayerRenderer.clear();

		m_pxAreaTransLayerMap.clear();
		m_strAreaTransLayerINSTs.clear();
		m_pxAreaTransLayerRenderer.clear();

		m_pxLineLayerMap.clear();
		m_strLineLayerINSTs.clear();
		m_pxLineLayerRenderer.clear();

		m_pxPointLayerMap.clear();
		m_strPointLayerINSTs.clear();
		m_pxPointLayerRenderer.clear();

		m_pxHTLayerRenderers.clear();
		m_pxHTLayers.clear();
		m_strHTLayerINSTs.clear();

		m_strLoadS57Files.clear();
	}

	XS57LayerAdapter::~XS57LayerAdapter()
	{
		if (m_bIsInit)
		{
			Desotry();
		}
	}

	// 销毁
	void XS57LayerAdapter::Desotry()
	{
		// 删除画布显示相关
		if (m_pxMapCanvas)
		{
			// 删除所有图层
			m_pxMapCanvas->clear();
			m_pxShowLayerSet.clear();
			m_pxWorldLayer = nullptr;
			m_pxCustomLayerMap.clear();
			m_strCustomLayerINSTs.clear();
			m_pxCustomLayerRenderer.clear();

			m_pxAreaCoverLayerMap.clear();
			m_strAreaCoverLayerINSTs.clear();
			m_pxAreaCoverLayerRenderer.clear();

			m_pxAreaNormalLayerMap.clear();
			m_strAreaNormalLayerINSTs.clear();
			m_pxAreaNormalLayerRenderer.clear();

			m_pxAreaTransLayerMap.clear();
			m_strAreaTransLayerINSTs.clear();
			m_pxAreaTransLayerRenderer.clear();

			m_pxLineLayerMap.clear();
			m_strLineLayerINSTs.clear();
			m_pxLineLayerRenderer.clear();

			m_pxPointLayerMap.clear();
			m_strPointLayerINSTs.clear();
			m_pxPointLayerRenderer.clear();

			m_pxHTLayerRenderers.clear();
			m_pxHTLayers.clear();
			m_strHTLayerINSTs.clear();

			m_strLoadS57Files.clear();
			QgsMapLayerRegistry::instance()->removeAllMapLayers();
			
			m_pxMapCanvas = nullptr;
		}

		// 删除海图符号颜色表
		{
			if (m_pxS52Lib)
			{
				delete m_pxS52Lib;
				m_pxS52Lib = nullptr;
			}
			m_strColorStyleNames.clear();
			m_xeShowLev = xeBase;
			m_xeColorStyle = xeDayBright;
		}

		// 删除海图依赖信息
		{
			m_pxCondFNCs.clear();
			m_xScreenSize = QSize(0, 0);
			m_dXPixPerMM = m_dYPixPerMM = 0.0;
			m_dCanvasScaleFactor = 0.0;
			m_dTideRectangleScale = 0.0;
			m_dCurrentArrowScale = 0.0;
			m_dTideDrawScaler = 0.0;
			m_dCurrentDrawScaler = 0.0;
			m_xS57ClassVec.clear();
			m_xS57ClassCodeToStrs.clear();
			m_xS57ClassStrToCodes.clear();
			m_xS57AttsVec.clear();
			m_xAttsCodeToStrs.clear();
			m_xAttsStrToCodes.clear();
			m_xLookupTABs.clear();
			m_pxCondTypes.clear();
			m_bUseTwoShades = false;
			m_dSafetyContour = 0;
			m_dSafetyDepth = 0;
			m_dShallowContour = 0;
			m_dDeepContour = 0;
			m_xRealScreenSize = QSize(0, 0);
			m_xeDepthUnitDisplay = xeMeter;
			m_unSymbolIndex = 0;
		}

		// 删除海图数据
		{
			foreach(XS57DataDoc* pS57Doc, m_pxS57DocMap)
			{
				if (pS57Doc)
				{
					delete pS57Doc;
					pS57Doc = nullptr;
				}
			}
			m_pxS57DocMap.clear();
		}

		m_bIsInit = false;
	}

	// 初始化适配器
	quint32 XS57LayerAdapter::InitAdapter(QgsMapCanvas* _pTouchCanvas, const QString& _strWorldSHP /*= ""*/)
	{
		if (m_bIsInit)
		{
			return ERROR_CODE_SPQCFCSH;	// 适配器重复初始化
		}
		if (_pTouchCanvas == nullptr)
		{
			return ERROR_CODE_MAPCANVS_NULL;// 画布为空
		}

		// 初始化S52海图色表
		{
			m_strColorStyleNames.insert(xeDayBright, "DAY_BRIGHT");
			m_strColorStyleNames.insert(xeDayBlack, "DAY_BLACKBACK");
			m_strColorStyleNames.insert(xeDayWhite, "DAY_WHITEBACK");
			m_strColorStyleNames.insert(xeDusk, "DUSK");
			m_strColorStyleNames.insert(xeNight, "NIGHT");

			m_pxS52Lib = XS52Lib::fromFile("./Data/S52/S52RAZDS.RLE");
			if (!m_pxS52Lib)
				return ERROR_CODE_S52_FAILD;
		}

		// 初始化S57依赖信息
		{
			// 注册自定义渲染器
			{
				QgsSymbolLayerV2Registry::instance()->addSymbolLayerType(new QgsSymbolLayerV2Metadata("ImageMarker", QObject::tr("IMAGE marker"), QgsSymbolV2::Marker,
					XImageMarkerSymbolLayerV2::create));
			}
			// 条件函数
			{
				m_pxCondFNCs.insert("DEPARE01", &XS57LayerAdapter::DEPARE01);
				m_pxCondFNCs.insert("QUAPOS01", &XS57LayerAdapter::QUAPOS01);
				m_pxCondFNCs.insert("DEPCNT02", &XS57LayerAdapter::DEPCNT02);
				m_pxCondFNCs.insert("OBSTRN04", &XS57LayerAdapter::OBSTRN04);
				m_pxCondFNCs.insert("DATCVR01", &XS57LayerAdapter::DATCVR01);
				m_pxCondFNCs.insert("WRECKS02", &XS57LayerAdapter::WRECKS02);
				m_pxCondFNCs.insert("SLCONS03", &XS57LayerAdapter::SLCONS03);
				m_pxCondFNCs.insert("RESTRN01", &XS57LayerAdapter::RESTRN01);
				m_pxCondFNCs.insert("RESARE02", &XS57LayerAdapter::RESARE02);
			}
			// 初始化S57 Class Info
			{
				// 获取像素DPI
				{
					bool bRes = GetWindowsMonitorSize(m_xRealScreenSize);
					if (!bRes)
					{
						Desotry();
						return ERROR_CODE_GETSCSIZE_FAILD;
					}

					m_xScreenSize = QApplication::desktop()->size();
					m_dXPixPerMM = m_xScreenSize.width() / m_xRealScreenSize.width();
					m_dYPixPerMM = m_xScreenSize.height() / m_xRealScreenSize.height();
					m_dCanvasScaleFactor = m_xScreenSize.width() / (m_xRealScreenSize.width() / 1000.0);
					m_dTideRectangleScale = 100;
					m_dCurrentArrowScale = 100;
					m_dTideDrawScaler = m_dXPixPerMM*0.254*m_dTideRectangleScale / 100.0;
					m_dCurrentDrawScaler = m_dXPixPerMM * 0.254 * m_dCurrentArrowScale / 100.0;
				}

				// S57 Class
				{
					QString strClassPath = sfnGetAbsPath("./Data/S57/s57objectclasses.csv");
					//QString strClassPath = sfnGetAbsPath("./../tem/s57objectclasses.csv");
					QFile xFile(strClassPath);
					if (!xFile.open(QIODevice::ReadOnly))
					{
						Desotry();
						return READ_S57CLA_FAILD;
					}
						
					QString strLine = QString();
					QTextStream stream(&xFile);
					strLine = stream.readLine();
					if (!strLine.contains("20160120")){
						// 识别码错误 用以简单校验文件内容
						xFile.close();
						Desotry();
						return READ_S57CLA_FAILD;
					}
					strLine = stream.readLine();

					QString strTemp = QString();
					while (!stream.atEnd())
					{
						QStringList strOneRec = stream.readLine().split(",");
						if (strOneRec.count() >= 8){
							XS57Class xNewClass;
							xNewClass.unCode = strOneRec.at(0).toUInt();
							xNewClass.strName = strOneRec.at(1);
							xNewClass.strAcronym = strOneRec.at(2);
							xNewClass.strAttsA = strOneRec.at(3).split(";");
							xNewClass.strAttsB = strOneRec.at(4).split(";");
							xNewClass.strAttsC = strOneRec.at(5).split(";");
							xNewClass.strPRIMTypes = strOneRec.at(7).split(";");
							if (strOneRec.at(6).count() == 1)
								xNewClass.ucClassTypes = strOneRec.at(6).at(0).cell();
							if (strOneRec.count() >= 10)
							{
								if(!strOneRec.at(8).isEmpty())
									xNewClass.strLayers = strOneRec.at(8).split(";");
								if (!strOneRec.at(9).isEmpty())
									xNewClass.strRendererLevs = strOneRec.at(9).split(";");
							}
							if (strOneRec.count() >= 11)
							{
								// 获取最小显示比例尺
								if (!strOneRec.at(10).isEmpty())
									xNewClass.strMinScale = strOneRec.at(10).split(";");
							}
							if (strOneRec.count() >= 12)
							{
								// 获取最大显示比例尺
								if (!strOneRec.at(11).isEmpty())
									xNewClass.strMaxScale = strOneRec.at(11).split(";");
							}
							m_xS57ClassVec.insert(xNewClass.unCode, xNewClass);
							m_xS57ClassCodeToStrs.insert(xNewClass.unCode, xNewClass.strAcronym);
							m_xS57ClassStrToCodes.insert(xNewClass.strAcronym, xNewClass.unCode);
						}
					}
					xFile.close();
				}

				// S57 Atts
				{
					QFile xFile("./Data/S57/s57attributes.csv");
					if (!xFile.open(QIODevice::ReadOnly))
					{
						Desotry();
						return READ_S57ATT_FAILD;
					}

					QString strLine = QString();
					QTextStream stream(&xFile);
					strLine = stream.readLine();
					if (!strLine.contains("20160120")){
						// 识别码错误 用以简单校验文件内容
						xFile.close();
						Desotry();
						return READ_S57ATT_FAILD;
					}
					strLine = stream.readLine();

					QString strTemp = QString();
					while (!stream.atEnd())
					{
						QStringList strOneRec = stream.readLine().split(",");
						if (strOneRec.count() == 5){
							XS57Atts xNewAtts;
							xNewAtts.unCode = strOneRec.at(0).toUInt();
							xNewAtts.strName = strOneRec.at(1);
							xNewAtts.strAcronym = strOneRec.at(2);
							if (strOneRec.at(3).count() == 1)
								xNewAtts.ucAttsType = strOneRec.at(3).at(0).cell();
							if (strOneRec.at(4).count() == 1)
								xNewAtts.ucClassType = strOneRec.at(4).at(0).cell();

							m_xS57AttsVec.insert(xNewAtts.unCode, xNewAtts);
							m_xAttsCodeToStrs.insert(xNewAtts.unCode, xNewAtts.strAcronym);
							m_xAttsStrToCodes.insert(xNewAtts.strAcronym, xNewAtts.unCode);
						}
						else
							int md = 0;
					}
					xFile.close();
				}

				// Atts期望值
				{
					QFile xFile("./Data/S57/s57expectedinput.csv");
					if (!xFile.open(QIODevice::ReadOnly))
					{
						Desotry();
						return READ_S57VAL_FAILD;
					}

					QString strLine = QString();
					QTextStream stream(&xFile);
					strLine = stream.readLine();
					if (!strLine.contains("20160120")){
						// 识别码错误 用以简单校验文件内容
						xFile.close();
						Desotry();
						return READ_S57VAL_FAILD;
					}
					strLine = stream.readLine();

					QString strTemp = QString();
					while (!stream.atEnd())
					{
						QStringList strOneRec = stream.readLine().split(",");
						if (strOneRec.count() == 3){
							quint32 unCode = strOneRec.at(0).toUInt();
							qint32  nID = strOneRec.at(1).toInt();
							QString strName = strOneRec.at(2);
							if (m_xS57AttsVec.contains(unCode))
								m_xS57AttsVec[unCode].strExpectedVals.insert(nID, strName);
						}
					}
					xFile.close();
				}
			}
			// 控制参数
			{
				m_bUseTwoShades = false;
				m_dSafetyContour = 8.0;
				m_dShallowContour = 3.0;
				m_dDeepContour = 10.0;
				m_dSafetyDepth = 5.0;
				m_xeDepthUnitDisplay = xeMeter;
			}
		}

		// 海图基础图层创建-这里创建的图层以海图图层Z序排序-如果用户已有其他图层，可能需要调整图层的显示顺序以免影响已有图层的显示
		if (!_strWorldSHP.isEmpty())
		{
			// 获取绝对文件路径
			QString strSHPPath = sfnGetAbsPath(_strWorldSHP);

			if (!sfnFileIsExist(strSHPPath))
			{
				return ERROR_CODE_FILE_NO_EXIST;
			}

			QString strFileSuffix = sfnGetFileSuffix(strSHPPath);
			if (strFileSuffix != "shp" && strFileSuffix != "SHP")
			{
				return ERROR_CODE_FILE_SUFFIX_ERR;
			}

			// 创建图层-基础World图层-该图层Z序为0
			{
				QString strVectorDir = strSHPPath;
				QString strVectorBasename = sfnGetFileBaseNameByPath(_strWorldSHP);
				QString strProviderName = "ogr";

				m_pxWorldLayer = new QgsVectorLayer(strVectorDir, QString::fromLocal8Bit("HT-世界基础图层"), strProviderName, false);
				m_pxWorldLayer->setProviderEncoding("System");
				if (m_pxWorldLayer == nullptr)
					return ERROR_CODE_LAYER_NULL;
				if (!m_pxWorldLayer->isValid())
					return ERROR_CODE_LAYER_FAILD;

				QgsMapLayerRegistry::instance()->addMapLayer(m_pxWorldLayer, true);
				m_pxShowLayerSet.push_back(m_pxWorldLayer);
			}
		}
		
		/*// 003-基础深度区域图层
		{
			QString strName = QString::fromLocal8Bit("深度区域图层");
			QgsPalLayerSettings xLaySettting;
			QStringList strFieldKeys = QStringList();
			QStringList strFieldTypes = QStringList();

			strFieldKeys << "id" << "key" << "symbolNo" << "label";
			strFieldTypes << "integer" << "string(64)" << "integer" << "string(32)";
			xLaySettting.enabled = true;
			QgsVectorLayer* pLayer = createVectorLayer(strName, "polygon", strFieldKeys, strFieldTypes, QPainter::CompositionMode_SourceOver, xLaySettting
				, NULL);

			if (pLayer == NULL)
				return -1;

			pLayer->setScaleBasedVisibility(true);
			pLayer->setMaximumScale(XDEFAULT_VLAYER_SCALE_MAX);
			pLayer->setMinimumScale(XDEFAULT_VLAYER_SCALE_MIN);
			//QgsMapLayerRegistry::instance()->addMapLayer(pLayer, true);
			m_pxBaseAreDepLay = pLayer;
			pLayer = NULL;
		}
		// 005-高层区域图层
		{
			QString strName = QString::fromLocal8Bit("高层区域图层");
			QgsPalLayerSettings xLaySettting;
			QStringList strFieldKeys = QStringList();
			QStringList strFieldTypes = QStringList();

			strFieldKeys << "id" << "key" << "symbolNo" << "label";
			strFieldTypes << "integer" << "string(64)" << "integer" << "string(32)";
			xLaySettting.enabled = true;
			// 			xLaySettting.fieldName = "label";
			// 			xLaySettting.textFont = QFont(QString::fromLocal8Bit("黑体"), 11);
			QgsVectorLayer* pLayer = createVectorLayer(strName, "polygon", strFieldKeys, strFieldTypes, QPainter::CompositionMode_SourceOver, xLaySettting
				, NULL);

			if (pLayer == NULL)
				return -1;

			pLayer->setScaleBasedVisibility(true);
			pLayer->setMaximumScale(XDEFAULT_VLAYER_SCALE_MAX);
			pLayer->setMinimumScale(XDEFAULT_VLAYER_SCALE_MIN);
			//QgsMapLayerRegistry::instance()->addMapLayer(pLayer, true);
			m_pxBaseHFillAreLay = pLayer;
			pLayer = NULL;
		}
		// 009-顶层线图层
		{
			QString strName = QString::fromLocal8Bit("顶层颜色线形图层");
			QgsPalLayerSettings xLaySettting;
			QStringList strFieldKeys = QStringList();
			QStringList strFieldTypes = QStringList();

			strFieldKeys << "id" << "key" << "symbolNo" << "label";
			strFieldTypes << "integer" << "string(64)" << "integer" << "string(32)";
			xLaySettting.enabled = true;
			QgsVectorLayer* pLayer = createVectorLayer(strName, "linestring", strFieldKeys, strFieldTypes, QPainter::CompositionMode_SourceOver, xLaySettting
				, NULL);

			if (pLayer == NULL)
				return -1;

			pLayer->setScaleBasedVisibility(true);
			pLayer->setMaximumScale(XDEFAULT_VLAYER_SCALE_MAX);
			pLayer->setMinimumScale(XDEFAULT_VLAYER_SCALE_MIN);
			//QgsMapLayerRegistry::instance()->addMapLayer(pLayer, true);
			m_pxTopLineLayer = pLayer;
			pLayer = NULL;
		}*/

		m_pxMapCanvas = _pTouchCanvas;
		SetShowLev(xeStand);
		SetColorStyle(XS57LayerAdapter::xeDayBright);

		m_bIsInit = true;
		return 0;
	}
	// 设置显示级别
	void XS57LayerAdapter::SetShowLev(const xeShowLev& _xeLev)
	{
		if (m_xeShowLev != _xeLev)
			m_xeShowLev = _xeLev;

		if (m_pxMapCanvas)
			touchToCanvas();
	}
	// 设置颜色样式
	void XS57LayerAdapter::SetColorStyle(const xEColorStyle& _xeStyle)
	{
		if (m_xeColorStyle == _xeStyle)
			return;

		m_pxS52Lib->m_strCurColorStyle = m_strColorStyleNames[_xeStyle];
		m_xeColorStyle = _xeStyle;
		// 生成样式图片
		updateSymbolsFile();

		updateColorStyle();
	}
	// 加载海图文件
	void XS57LayerAdapter::LoadS57Files(const QStringList& _strFileList)
	{
		if (!m_bIsInit || _strFileList.isEmpty())
			return;

		// 使用S57文档加载器加载海图文档
		foreach(QString strPath, _strFileList)
		{
			if (sfnFileIsExist(strPath) == false)
				continue;
			if (sfnGetFileSuffix(strPath) != "000")
				continue;
			strPath = sfnGetAbsPath(strPath);
			if (m_pxS57DocMap.contains(strPath))
				continue;
			
			XS57DataDoc* pDoc = new XS57DataDoc();
			pDoc->SetDrawStartID(m_pxS57DocMap.count() * 1000000);
			m_pxS57DocMap.insert(strPath, pDoc);

			// 链接信号
			QObject::connect(pDoc, SIGNAL(slgLoadFinish(QString,XS57DataDoc*)), this, SLOT(OnProcNewDoc(QString,XS57DataDoc*)));

			pDoc->LoadFile(strPath);
			pDoc = nullptr;
		}
	}

	// 私有函数
	// 计算当前颜色显示样式Key
	const quint32 XS57LayerAdapter::calcCurStyleKey() const
	{
		if (m_pxS52Lib)
		{
			// 符号样式*1000+边界样式*10+颜色类别
			return m_pxS52Lib->m_xeSymbolStyle * 1000 + m_pxS52Lib->m_xeBoundaryStyle * 10 + m_xeColorStyle;
		}
		return 0;
	}
	// 将图层依附至窗口
	quint32 XS57LayerAdapter::touchToCanvas()
	{
		// 后期的更新中需要在此函数中控制各个显示级别的切换
		QList<QgsMapLayer*> pResultCanvasLays;;

		pResultCanvasLays.push_front(m_pxWorldLayer);
		foreach(QgsVectorLayer* pLayer, m_pxCustomLayerMap)
			pResultCanvasLays.push_front(pLayer);
		foreach(QgsVectorLayer* pLayer, m_pxAreaCoverLayerMap)
			pResultCanvasLays.push_front(pLayer);
		foreach(QgsVectorLayer* pLayer, m_pxAreaNormalLayerMap)
			pResultCanvasLays.push_front(pLayer);
		foreach(QgsVectorLayer* pLayer, m_pxAreaTransLayerMap)
			pResultCanvasLays.push_front(pLayer);
		foreach(QgsVectorLayer* pLayer, m_pxLineLayerMap)
			pResultCanvasLays.push_front(pLayer);
		foreach(QgsVectorLayer* pLayer, m_pxPointLayerMap)
			pResultCanvasLays.push_front(pLayer);
		
		QList<QgsMapLayer*> oldLayer = m_pxMapCanvas->layers();
		foreach(QgsMapLayer* pLayer, oldLayer)
		{
			if (pResultCanvasLays.contains(pLayer) == false)
				pResultCanvasLays.push_front(pLayer);
		}

		QList<QgsMapCanvasLayer> xTouchSet;
		foreach(QgsMapLayer* pLayer, pResultCanvasLays)
		{
			xTouchSet.push_back(pLayer);
		}

		m_pxMapCanvas->setLayerSet(xTouchSet);
		m_pxMapCanvas->freeze(false);
		m_pxMapCanvas->zoomToFullExtent();
		m_pxMapCanvas->refresh();

		return 0;
	}
	// 更新所有图层颜色配置
	quint32 XS57LayerAdapter::updateColorStyle()
	{
		if (m_pxMapCanvas == nullptr)
			return 0;

		//  画布背景色
		{
			QColor xColor = QColor(0, 0, 0);
			if (m_pxS52Lib->GetColorByKey("BASEB", xColor) != 0)
			{
				return ERROR_CODE_S52_FAILD;
			}
			m_pxMapCanvas->setCanvasColor(xColor);
		}

		// 设置图层渲染符号
		quint32 unCurStyle = calcCurStyleKey();
		// 更新世界基础图层渲染器
		if (m_pxWorldLayer)
		{
			// 创建渲染器
			QColor xColor = QColor(0, 0, 0);
			if (m_pxS52Lib->GetColorByKey("BASEF", xColor) != 0)
			{
				Desotry();
				return ERROR_CODE_S52_FAILD;
			}
			QgsSimpleFillSymbolLayerV2* pxSymLay = new QgsSimpleFillSymbolLayerV2();
			pxSymLay->setFillColor(xColor);
			pxSymLay->setOutlineColor(xColor);
			QgsSymbolLayerV2List symbolList;
			symbolList.push_front(pxSymLay);
			m_pxWorldLayer->setRendererV2(new QgsSingleSymbolRendererV2(new QgsFillSymbolV2(symbolList)));
			pxSymLay = nullptr;
			symbolList.clear();
		}

		// 更新海图渲染器
		{
			updateHTRenderer(true);
		}

		m_pxMapCanvas->refresh();

		return 0;
	}
	// 生成当前样式图片数据
	bool XS57LayerAdapter::updateSymbolsFile()
	{
		QString strCurStyle = colorStyleToStr(m_xeColorStyle);
		QString strCurSymbolPath = QString("./Data/S52/Symbols/") + strCurStyle;
		QDir dir(strCurSymbolPath);
		if (!dir.exists(strCurSymbolPath)){
			if (!dir.mkdir(dir.absolutePath()))
				return false;

			QString strTemp = QString();
			foreach(XLineStyle* pSymSt, m_pxS52Lib->m_xLineStyles){
				createImage("LS", pSymSt->m_strName, strTemp);
			}

			foreach(XSymbolStyle* pSymSt, m_pxS52Lib->m_xSymbolStyles){
				createImage("SY", pSymSt->m_strName, strTemp);
			}

			foreach(XPatternsStyle* pSymSt, m_pxS52Lib->m_xPatternsStyles){
				createImage("PA", pSymSt->m_strName, strTemp);
			}
		}
		return true;
	}
	// 创建矢量图层-内存
	QgsVectorLayer* XS57LayerAdapter::createVectorLayer(const QString& _strLayName, const QString& _strLayType, const QStringList& _strFieldNames, const QStringList& _strFieldTypes
		, const QPainter::CompositionMode& _xeBlendMode, const QgsPalLayerSettings* _xLaySeting, QgsFeatureRendererV2* _pxRenderV2)
	{
		QgsVectorLayer* pVecLay = NULL;
		if (_strFieldTypes.count() != _strFieldNames.count())
			return pVecLay;

		QString strFields = QString();
		for (quint32 unIdx = 0; unIdx < _strFieldNames.count(); unIdx++)
		{
			strFields += "field=";
			strFields += _strFieldNames.at(unIdx);
			strFields += ":";
			strFields += _strFieldTypes.at(unIdx);
			strFields += "&";
		}

		QString strLayerParams = _strLayType + "?";
		strLayerParams += "crs=EPSG:4326&";
		strLayerParams += strFields;//"field=id:integer&field=key:integer&field=symbolNo:integer&field=label:string(32)&";
		strLayerParams += "index=no&";
		strLayerParams += QString("memoryid=%1").arg(QUuid::createUuid().toString());
		QgsVectorLayer* pLayer = NULL;
		{
			QList<QgsMapLayer*> pResLays = QgsMapLayerRegistry::instance()->mapLayersByName(_strLayName);
			if (pResLays.count() > 1)
				return NULL;
			if (pResLays.count() == 1)
				pVecLay = (QgsVectorLayer*)pResLays.at(0);

			if (pVecLay == NULL)
			{
				pVecLay = new QgsVectorLayer(strLayerParams, _strLayName, "memory");
				if (pVecLay == NULL || !pVecLay->isValid()){
					if (pVecLay)
						delete pVecLay;
					pVecLay = NULL;
					return NULL;
				}
				QgsMapLayerRegistry::instance()->addMapLayer(pVecLay);
				pVecLay->setBlendMode(_xeBlendMode);
				QgsPalLayerSettings set = *_xLaySeting;
				set.writeToLayer(pVecLay);

				pVecLay->setRendererV2(_pxRenderV2);
			}
		}

		return pVecLay;
	}

	bool CompareLoopTab(XLookupTAB* pFirst, XLookupTAB* pLast){
		if (pFirst && pLast)
		{
			if (pFirst->m_unRCID < pLast->m_unRCID)
				return true;
		}
		return false;
	}
	// 追加新的海图数据
	int XS57LayerAdapter::appendS57Doc(XS57DataDoc* _pDoc)
	{
		if (_pDoc == nullptr)
			return 0;

		// 启用所有海图图层编辑
		{
			foreach(QgsVectorLayer* pLayer, m_pxPointLayerMap)
				pLayer->startEditing();
			foreach(QgsVectorLayer* pLayer, m_pxLineLayerMap)
				pLayer->startEditing();
			foreach(QgsVectorLayer* pLayer, m_pxAreaNormalLayerMap)
				pLayer->startEditing();
			foreach(QgsVectorLayer* pLayer, m_pxAreaCoverLayerMap)
				pLayer->startEditing();
			foreach(QgsVectorLayer* pLayer, m_pxCustomLayerMap)
				pLayer->startEditing();
		}

		quint32 unDocMark = _pDoc->GetDrawStartID();
		quint64 unStartId = unDocMark;
		{
			QMap<QgsVectorLayer*, QMap<quint32, QList<QgsFeature>>> feasRes;

			// 绘制面状目标
			QList<XFeature*> pAreaFeas = _pDoc->GetAreaFeaturesSeq();
			foreach(XFeature* pFeature, pAreaFeas)
			{

				if (pFeature->unObjType == 119		// 区域显示文本-暂时不显示--文本需要特殊处理
					|| pFeature->unObjType == 306	// 绘制图案线形时 效果不理想暂时不绘制图案线形
					|| pFeature->unObjType == 309	// S52库中绘制指令为空
					|| pFeature->unObjType == 121	// 绘制指令为空
					|| pFeature->unObjType == 73	// 暂时不绘制文本
					)
					continue;

				QStringList strLayerTypes;
				QList<quint32> unSymNos;
				QList<quint32> unRenLevs;
				QList<QgsMapLayer*> pLayers;
				int nShowAttID = -1;
				if (pFeature->unObjType == 112)
					int md = 0;
				quint32 unRes = getS57ObjLayer(pFeature, pLayers, unSymNos, unRenLevs, strLayerTypes, nShowAttID);

				if (unRes != 0 || pLayers.isEmpty())
				{
					//Q_ASSERT_X(false, QString::fromLocal8Bit("添加要素").toLocal8Bit(), QString::fromLocal8Bit("获取要素图层失败！失败码：%1").arg(unRes).toLocal8Bit());
					continue;
				}

				quint32 unIdx = 0;
				foreach(QgsMapLayer* pSuperLayer, pLayers)
				{
					quint32 unSymNo = unSymNos.at(unIdx);
					quint32 unRenLev = unRenLevs.at(unIdx);
					QString strLayerType = strLayerTypes.at(unIdx);
					QgsVectorLayer* pVLayer = nullptr;
					QgsRasterLayer* pRLayer = nullptr;
					if (pSuperLayer == nullptr)
					{
						//Q_ASSERT(false);
						continue;
					}
					if (strLayerTypes.isEmpty())
					{
						//Q_ASSERT(false);
						continue;
					}

					if (pSuperLayer->type() == QgsMapLayer::VectorLayer)
						pVLayer = (QgsVectorLayer*)pSuperLayer;
					else if (pSuperLayer->type() == QgsMapLayer::RasterLayer)
						pRLayer = (QgsRasterLayer*)pSuperLayer;
					else
					{
						//Q_ASSERT_X(false, QString::fromLocal8Bit("添加要素").toLocal8Bit(), QString::fromLocal8Bit("无法识别的图层类型！").toLocal8Bit());
						continue;
					}

					if (pVLayer)
					{
						if (!pVLayer->isEditable())
							pVLayer->startEditing();

						QgsGeometry* pGeom = nullptr;

						if (strLayerType == "area")
						{
							if (pFeature->x2DPointList.size() < 3)
							{
								//Q_ASSERT_X(false, QString::fromLocal8Bit("添加要素").toLocal8Bit(), QString::fromLocal8Bit("区域要素数据点数不足3个，将忽略该要素,要素ID:%1！").arg(pFeature->unIdentifyID).toLocal8Bit());
								continue;
							}
							QgsPolyline xPolyLine;
							foreach(QVector2D* px2D, pFeature->x2DPointList)
								xPolyLine.push_back(px2D->toPointF());

							QgsPolygon polygon;
							polygon.append(xPolyLine);
							pGeom = QgsGeometry::fromPolygon(polygon);
						}
						else if (strLayerType == "line")
						{
							if (pFeature->x2DPointList.size() < 2)
							{
								//Q_ASSERT_X(false, QString::fromLocal8Bit("添加要素").toLocal8Bit(), QString::fromLocal8Bit("线要素数据点数不足2个，将忽略该要素,要素ID:%1！").arg(pFeature->unIdentifyID).toLocal8Bit());
								continue;
							}
							QgsPolyline xPolyLine;
							foreach(QVector2D* px2D, pFeature->x2DPointList)
								xPolyLine.push_back(px2D->toPointF());
							pGeom = QgsGeometry::fromPolyline(xPolyLine);
						}
						else if (strLayerType == "point")
						{
							if (pFeature->x2DPointList.isEmpty() && pFeature->x3DPointList.isEmpty())
							{
								//Q_ASSERT_X(false, QString::fromLocal8Bit("添加要素").toLocal8Bit(), QString::fromLocal8Bit("点要素数据点数不足1个，将忽略该要素,要素ID:%1！").arg(pFeature->unIdentifyID).toLocal8Bit());
								continue;
							}

							QPointF pt = QPointF(2000, 2000);
							if (pFeature->x2DPointList.count() > 1)
							{
								QList<QPointF> ptList;
								foreach(QVector2D* px2D, pFeature->x2DPointList)
									ptList.push_back(px2D->toPointF());
								pt = sfnGetCenterOfGravityPoint(ptList);
							}
							else if (pFeature->x2DPointList.size() == 1)
							{
								pt = pFeature->x2DPointList.first()->toPointF();
							}
							
							if (pt.x() == 2000 || pt.y() == 2000)
								Q_ASSERT(false);
							
							pGeom = QgsGeometry::fromQPointF(pt);
						}

						if (pGeom == nullptr)
						{
							//Q_ASSERT_X(false, QString::fromLocal8Bit("添加要素").toLocal8Bit(), QString::fromLocal8Bit("获取要素几何图形失败，将忽略该要素,要素ID:%1！").arg(pFeature->unIdentifyID).toLocal8Bit());
							continue;
						}

						QString strLabelName = QString();
						QgsFields xFields = pVLayer->fields();
						QgsFeature newFeature(xFields, unStartId++);
						newFeature.setAttribute("id", pFeature->unIdentifyID);
						newFeature.setAttribute("key", QString::number(pFeature->unObjType));
						newFeature.setAttribute("symbolNo", unSymNo);
						newFeature.setAttribute("docMark", QString::number(unDocMark));
						// 组合属性字符串
						QString strKeys;
						QString strVals;
						{
							XMapAtts::Iterator attItr = pFeature->xAtts.begin();
							while (attItr != pFeature->xAtts.end())
							{
								strKeys.append(QString("%1;").arg(attItr.key()));
								strVals.append(attItr.value());
								strVals.append(";");
								attItr++;
							}
						}
						newFeature.setAttribute("attKeys", strKeys);
						newFeature.setAttribute("attVals", strVals);
						newFeature.setGeometry(pGeom);

						// 添加进待添加列表
						{

							if (feasRes.contains(pVLayer) == false)
								feasRes.insert(pVLayer, QMap<quint32, QList<QgsFeature>>());

							if (feasRes[pVLayer].contains(unRenLev) == false)
							{
								feasRes[pVLayer].insert(unRenLev, QList<QgsFeature>());
							}

							feasRes[pVLayer][unRenLev].append(newFeature);
						}
					}
					else if (pRLayer)
					{
						// 栅格图层
						//Q_ASSERT_X(false, QString::fromLocal8Bit("添加区域要素").toLocal8Bit(), QString::fromLocal8Bit("暂时不处理栅格图层！").toLocal8Bit());
						continue;
					}
					unIdx++;
				}
			}
			pAreaFeas.clear();
			
			// 绘制线状目标
			QList<XFeature*> pLineFeas = _pDoc->GetLineFeatures();
			foreach(XFeature* pFeature, pLineFeas)
			{
				//*
				if (pFeature->unObjType != 22
					&& pFeature->unObjType != 30
					&& pFeature->unObjType != 43
					&& pFeature->unObjType != 42
					&& pFeature->unObjType != 94
					&& pFeature->unObjType != 106
					&& pFeature->unObjType != 114
					&& pFeature->unObjType != 116
					&& pFeature->unObjType != 122
					)
				{
					continue;
				}
				//

				QStringList strLayerTypes;
				QList<quint32> unSymNos;
				QList<quint32> unRenLevs;
				QList<QgsMapLayer*> pLayers;
				int nShowAttID = -1;
				quint32 unRes = getS57ObjLayer(pFeature, pLayers, unSymNos, unRenLevs, strLayerTypes,nShowAttID);

				if (unRes != 0 || pLayers.isEmpty())
				{
					//Q_ASSERT_X(false, QString::fromLocal8Bit("添加要素").toLocal8Bit(), QString::fromLocal8Bit("获取要素图层失败！失败码：%1").arg(unRes).toLocal8Bit());
					continue;
				}

				quint32 unIdx = 0;
				foreach(QgsMapLayer* pSuperLayer, pLayers)
				{
					quint32 unSymNo = unSymNos.at(unIdx);
					quint32 unRenLev = unRenLevs.at(unIdx);
					QString strLayerType = strLayerTypes.at(unIdx);
					QgsVectorLayer* pVLayer = nullptr;
					QgsRasterLayer* pRLayer = nullptr;
					if (pSuperLayer == nullptr)
					{
						//Q_ASSERT(false);
						continue;
					}
					if (strLayerTypes.isEmpty())
					{
					//	Q_ASSERT(false);
						continue;
					}

					if (pSuperLayer->type() == QgsMapLayer::VectorLayer)
						pVLayer = (QgsVectorLayer*)pSuperLayer;
					else if (pSuperLayer->type() == QgsMapLayer::RasterLayer)
						pRLayer = (QgsRasterLayer*)pSuperLayer;
					else
					{
						//Q_ASSERT_X(false, QString::fromLocal8Bit("添加要素").toLocal8Bit(), QString::fromLocal8Bit("无法识别的图层类型！").toLocal8Bit());
						continue;
					}

					if (pVLayer)
					{
						if (!pVLayer->isEditable())
							pVLayer->startEditing();

						QgsGeometry* pGeom = nullptr;

						if (strLayerType == "area")
						{
							if (pFeature->x2DPointList.size() < 3)
							{
								//Q_ASSERT_X(false, QString::fromLocal8Bit("添加要素").toLocal8Bit(), QString::fromLocal8Bit("区域要素数据点数不足3个，将忽略该要素,要素ID:%1！").arg(pFeature->unIdentifyID).toLocal8Bit());
								continue;
							}
							QgsPolyline xPolyLine;
							foreach(QVector2D* px2D, pFeature->x2DPointList)
								xPolyLine.push_back(px2D->toPointF());

							QgsPolygon polygon;
							polygon.append(xPolyLine);
							pGeom = QgsGeometry::fromPolygon(polygon);
						}
						else if (strLayerType == "line")
						{
							if (pFeature->x2DPointList.size() < 2)
							{
								//Q_ASSERT_X(false, QString::fromLocal8Bit("添加要素").toLocal8Bit(), QString::fromLocal8Bit("线要素数据点数不足2个，将忽略该要素,要素ID:%1！").arg(pFeature->unIdentifyID).toLocal8Bit());
								continue;
							}
							QgsPolyline xPolyLine;
							foreach(QVector2D* px2D, pFeature->x2DPointList)
								xPolyLine.push_back(px2D->toPointF());
							pGeom = QgsGeometry::fromPolyline(xPolyLine);
						}
						else if (strLayerType == "point")
						{
							if (pFeature->x2DPointList.isEmpty() && pFeature->x3DPointList.isEmpty())
							{
								//Q_ASSERT_X(false, QString::fromLocal8Bit("添加要素").toLocal8Bit(), QString::fromLocal8Bit("点要素数据点数不足1个，将忽略该要素,要素ID:%1！").arg(pFeature->unIdentifyID).toLocal8Bit());
								continue;
							}

							QPointF pt = QPointF(2000, 2000);
							if (pFeature->x2DPointList.count() > 1)
							{
								QList<QPointF> ptList;
								foreach(QVector2D* px2D, pFeature->x2DPointList)
									ptList.push_back(px2D->toPointF());
								pt = sfnGetCenterOfGravityPoint(ptList);
							}
							else if (pFeature->x2DPointList.size() == 1)
							{
								pt = pFeature->x2DPointList.first()->toPointF();
							}
							else
								Q_ASSERT(false);
							if (pt.x() == 2000 || pt.y() == 2000)
								Q_ASSERT(false);

							pGeom = QgsGeometry::fromQPointF(pt);
						}

						if (pGeom == nullptr)
						{
							//Q_ASSERT_X(false, QString::fromLocal8Bit("添加要素").toLocal8Bit(), QString::fromLocal8Bit("获取要素几何图形失败，将忽略该要素,要素ID:%1！").arg(pFeature->unIdentifyID).toLocal8Bit());
							continue;
						}

						QString strLabelName = QString();
						QgsFields xFields = pVLayer->fields();
						QgsFeature newFeature(xFields, unStartId++);
						newFeature.setAttribute("id", pFeature->unIdentifyID);
						newFeature.setAttribute("key", QString::number(pFeature->unObjType));
						newFeature.setAttribute("symbolNo", unSymNo);
						newFeature.setAttribute("docMark", QString::number(unDocMark));
						// 组合属性字符串
						QString strKeys;
						QString strVals;
						{
							XMapAtts::Iterator attItr = pFeature->xAtts.begin();
							while (attItr != pFeature->xAtts.end())
							{
								strKeys.append(QString("%1;").arg(attItr.key()));
								strVals.append(attItr.value());
								strVals.append(";");
								attItr++;
							}
						}
						newFeature.setAttribute("attKeys", strKeys);
						newFeature.setAttribute("attVals", strVals);
						newFeature.setGeometry(pGeom);
						
						// 添加进待添加列表
						{
							if (feasRes.contains(pVLayer) == false)
								feasRes.insert(pVLayer, QMap<quint32, QList<QgsFeature>>());

							if (feasRes[pVLayer].contains(unRenLev) == false)
							{
								feasRes[pVLayer].insert(unRenLev, QList<QgsFeature>());
							}

							feasRes[pVLayer][unRenLev].append(newFeature);
						}
					}
					else if (pRLayer)
					{
						// 栅格图层
						//Q_ASSERT_X(false, QString::fromLocal8Bit("添加区域要素").toLocal8Bit(), QString::fromLocal8Bit("暂时不处理栅格图层！").toLocal8Bit());
						continue;
					}
					unIdx++;
				}
			}
			pLineFeas.clear();

			// 绘制点状目标
			QList<XFeature*> pPtFeas = _pDoc->GetPointFeatures();
			QStringList ulTest;
			foreach(XFeature* pFeature, pPtFeas)
			{
				if (pFeature->unObjType == 9		// 暂时不绘制 -获取绘制Lokup时存在多个  无法确定哪一个
					||pFeature->unObjType == 15		// 暂时不绘制-浮标需要特定绘制方式
					||pFeature->unObjType == 18
					||pFeature->unObjType == 19
					)
				{
					continue;
				}
				
				QStringList strLayerTypes;
				QList<quint32> unSymNos;
				QList<quint32> unRenLevs;
				QList<QgsMapLayer*> pLayers;
				int nShowAttID = -1;
				quint32 unRes = getS57ObjLayer(pFeature, pLayers, unSymNos, unRenLevs, strLayerTypes,nShowAttID);

				if (unRes != 0 || pLayers.isEmpty())
				{
					//Q_ASSERT_X(false, QString::fromLocal8Bit("添加要素").toLocal8Bit(), QString::fromLocal8Bit("获取要素图层失败！失败码：%1").arg(unRes).toLocal8Bit());
					continue;
				}

				quint32 unIdx = 0;
				foreach(QgsMapLayer* pSuperLayer, pLayers)
				{
					quint32 unSymNo = unSymNos.at(unIdx);
					quint32 unRenLev = unRenLevs.at(unIdx);
					QString strLayerType = strLayerTypes.at(unIdx);
					QgsVectorLayer* pVLayer = nullptr;
					QgsRasterLayer* pRLayer = nullptr;
					if (pSuperLayer == nullptr)
					{
						//Q_ASSERT(false);
						continue;
					}
					if (strLayerTypes.isEmpty())
					{
						//Q_ASSERT(false);
						continue;
					}

					if (pSuperLayer->type() == QgsMapLayer::VectorLayer)
						pVLayer = (QgsVectorLayer*)pSuperLayer;
					else if (pSuperLayer->type() == QgsMapLayer::RasterLayer)
						pRLayer = (QgsRasterLayer*)pSuperLayer;
					else
					{
						//Q_ASSERT_X(false, QString::fromLocal8Bit("添加要素").toLocal8Bit(), QString::fromLocal8Bit("无法识别的图层类型！").toLocal8Bit());
						continue;
					}

					if (pVLayer)
					{
						if (!pVLayer->isEditable())
							pVLayer->startEditing();

						QgsGeometry* pGeom = nullptr;

						if (strLayerType == "area")
						{
							if (pFeature->x2DPointList.size() < 3)
							{
								//Q_ASSERT_X(false, QString::fromLocal8Bit("添加要素").toLocal8Bit(), QString::fromLocal8Bit("区域要素数据点数不足3个，将忽略该要素,要素ID:%1！").arg(pFeature->unIdentifyID).toLocal8Bit());
								continue;
							}
							QgsPolyline xPolyLine;
							foreach(QVector2D* px2D, pFeature->x2DPointList)
								xPolyLine.push_back(px2D->toPointF());

							QgsPolygon polygon;
							polygon.append(xPolyLine);
							pGeom = QgsGeometry::fromPolygon(polygon);
						}
						else if (strLayerType == "line")
						{
							if (pFeature->x2DPointList.size() < 2)
							{
								//Q_ASSERT_X(false, QString::fromLocal8Bit("添加要素").toLocal8Bit(), QString::fromLocal8Bit("线要素数据点数不足2个，将忽略该要素,要素ID:%1！").arg(pFeature->unIdentifyID).toLocal8Bit());
								continue;
							}
							QgsPolyline xPolyLine;
							foreach(QVector2D* px2D, pFeature->x2DPointList)
								xPolyLine.push_back(px2D->toPointF());
							pGeom = QgsGeometry::fromPolyline(xPolyLine);
						}
						else if (strLayerType == "point")
						{
							if (pFeature->x2DPointList.isEmpty() && pFeature->x3DPointList.isEmpty() && pFeature->pxSinglePoint->isNull())
							{
								//Q_ASSERT_X(false, QString::fromLocal8Bit("添加要素").toLocal8Bit(), QString::fromLocal8Bit("点要素数据点数不足1个，将忽略该要素,要素ID:%1！").arg(pFeature->unIdentifyID).toLocal8Bit());
								continue;
							}

							QPointF pt = QPointF(2000, 2000);
							if (pFeature->x2DPointList.count() > 1)
							{
								QList<QPointF> ptList;
								foreach(QVector2D* px2D, pFeature->x2DPointList)
									ptList.push_back(px2D->toPointF());
								pt = sfnGetCenterOfGravityPoint(ptList);
							}
							else if (pFeature->x2DPointList.size() == 1)
							{
								pt = pFeature->x2DPointList.first()->toPointF();
							}
							else
								pt = pFeature->pxSinglePoint->toPointF();
							
							if (pt.x() == 2000 || pt.y() == 2000)
								Q_ASSERT(false);

							pGeom = QgsGeometry::fromQPointF(pt);
						}

						if (pGeom == nullptr)
						{
							//Q_ASSERT_X(false, QString::fromLocal8Bit("添加要素").toLocal8Bit(), QString::fromLocal8Bit("获取要素几何图形失败，将忽略该要素,要素ID:%1！").arg(pFeature->unIdentifyID).toLocal8Bit());
							continue;
						}

						QString strLabelName = QString();
						QgsFields xFields = pVLayer->fields();
						QgsFeature newFeature(xFields, unStartId++);
						newFeature.setAttribute("id", pFeature->unIdentifyID);
						newFeature.setAttribute("key", QString::number(pFeature->unObjType));
						newFeature.setAttribute("symbolNo", unSymNo);
						newFeature.setAttribute("docMark", QString::number(unDocMark));
						if (pFeature->xAtts.contains(nShowAttID))
							newFeature.setAttribute("showText", pFeature->xAtts[nShowAttID]);
						
						// 组合属性字符串
						QString strKeys;
						QString strVals;
						{
							XMapAtts::Iterator attItr = pFeature->xAtts.begin();
							while (attItr != pFeature->xAtts.end())
							{
								strKeys.append(QString("%1;").arg(attItr.key()));
								strVals.append(attItr.value());
								strVals.append(";");
								attItr++;
							}
						}
						newFeature.setAttribute("attKeys", strKeys);
						newFeature.setAttribute("attVals", strVals);
						newFeature.setGeometry(pGeom);

						// 添加进待添加列表
						{
							if (feasRes.contains(pVLayer) == false)
								feasRes.insert(pVLayer, QMap<quint32, QList<QgsFeature>>());

							if (feasRes[pVLayer].contains(unRenLev) == false)
							{
								feasRes[pVLayer].insert(unRenLev, QList<QgsFeature>());
							}

							feasRes[pVLayer][unRenLev].append(newFeature);
						}
					}
					else if (pRLayer)
					{
						// 栅格图层
						//Q_ASSERT_X(false, QString::fromLocal8Bit("添加区域要素").toLocal8Bit(), QString::fromLocal8Bit("暂时不处理栅格图层！").toLocal8Bit());
						continue;
					}
					unIdx++;
				}
			}
			pPtFeas.clear();

			// 添加所有矢量图层元素
			QMap<QgsVectorLayer*, QMap<quint32, QList<QgsFeature>>>::iterator itr = feasRes.begin();
			while (itr != feasRes.end())
			{
				for (int nRenLev = 0; nRenLev < itr.value().count(); nRenLev++)
				{
					QList<QgsFeature> feaList = itr.value()[nRenLev];
					foreach(QgsFeature fea, feaList)
					{
						itr.key()->addFeature(fea, false);
					}
				}
				itr++;
			}
		}

		// 提交所有海图图层改变
		{
			foreach(QgsVectorLayer* pLayer, m_pxCustomLayerMap)
				pLayer->commitChanges();
			foreach(QgsVectorLayer* pLayer, m_pxAreaCoverLayerMap)
				pLayer->commitChanges();
			foreach(QgsVectorLayer* pLayer, m_pxAreaNormalLayerMap)
				pLayer->commitChanges();
			foreach(QgsVectorLayer* pLayer, m_pxAreaTransLayerMap)
				pLayer->commitChanges();
			foreach(QgsVectorLayer* pLayer, m_pxLineLayerMap)
				pLayer->commitChanges();
			foreach(QgsVectorLayer* pLayer, m_pxPointLayerMap)
				pLayer->commitChanges();
		}

		m_strLoadS57Files.insert(_pDoc->GetFilePath(),unDocMark);

		// 更新渲染器
		{
			updateHTRenderer(false);
			emit slgLayerUpdate();
/*			touchToCanvas();*/
		}
		return 0;
	}
	// 更新海图渲染器
	void XS57LayerAdapter::updateHTRenderer(bool _bIsRemake/* = false*/)
	{
		// 更新海图渲染器
		if (_bIsRemake)
		{
			m_pxCustomLayerRenderer.clear();
			m_pxAreaCoverLayerRenderer.clear();
			m_pxAreaNormalLayerRenderer.clear();
			m_pxAreaTransLayerRenderer.clear();
			m_pxLineLayerRenderer.clear();
			m_pxPointLayerRenderer.clear();
		}

		// 更新Point层渲染器
		{
			foreach(QgsVectorLayer* pLayer, m_pxPointLayerMap)
			{
				QgsCategorizedSymbolRendererV2* pRender = nullptr;
				if (m_pxPointLayerRenderer.contains(pLayer))
				{
					pRender = m_pxPointLayerRenderer[pLayer];
				}
				else
				{
					pRender = new QgsCategorizedSymbolRendererV2("symbolNo");
					m_pxPointLayerRenderer.insert(pLayer, pRender);
				}

				// 检测是否新填符号
				QMap<QString, quint32>::Iterator itr = m_strPointLayerINSTs.begin();
				while (itr != m_strPointLayerINSTs.end())
				{
					if (isExistSymbol(pRender, itr.value()) == false)
					{
						// 根据绘制命令创建符号
						XSymbol sym;
						QString strClr;
						quint32 unFontSize = 21;
						QgsSymbolV2* pSymV2 = createRendererByCmd(itr.key(), sym, "point",strClr,unFontSize);
						if (!strClr.isEmpty())
						{
							QColor clr;
							QString strLayName = pLayer->name();
							if (m_pxS52Lib->GetColorByKey(strClr, clr) == 0 && strLayName.count() > 5 && strLayName.right(5) == strClr)
							{
								QgsPalLayerSettings setting;
								setting.readFromLayer(pLayer);
								setting.textColor = clr;
								setting.writeToLayer(pLayer);
								//pLayer->enableLabels(true);
							}
						}
						if (pSymV2 != nullptr)
						{
							pRender->addCategory(QgsRendererCategoryV2(itr.value(), pSymV2, QString("point-%1").arg(itr.value())));
							pSymV2 = nullptr;
							pLayer->setRendererV2(pRender);
						}
// 						else
// 							//Q_ASSERT(false);
					}

					itr++;
				}
			}
		}
		// 更新Line层渲染器
		{
			foreach(QgsVectorLayer* pLayer, m_pxLineLayerMap)
			{
				QgsCategorizedSymbolRendererV2* pRender = nullptr;
				if (m_pxLineLayerRenderer.contains(pLayer))
				{
					pRender = m_pxLineLayerRenderer[pLayer];
				}
				else
				{
					pRender = new QgsCategorizedSymbolRendererV2("symbolNo");
					m_pxLineLayerRenderer.insert(pLayer, pRender);
				}

				// 检测是否新填符号
				QMap<QString, quint32>::Iterator itr = m_strLineLayerINSTs.begin();
				while (itr != m_strLineLayerINSTs.end())
				{
					if (isExistSymbol(pRender, itr.value()) == false)
					{
						// 根据绘制命令创建符号
						XSymbol sym;
						QString strClr;
						quint32 unFontSize = 21;
						QgsSymbolV2* pSymV2 = createRendererByCmd(itr.key(), sym, "line",strClr,unFontSize);
						if (pSymV2 != nullptr)
						{
							pRender->addCategory(QgsRendererCategoryV2(itr.value(), pSymV2, QString("point-%1").arg(itr.value())));
							pSymV2 = nullptr;
							pLayer->setRendererV2(pRender);
						}
// 						else
// 							Q_ASSERT(false);
					}

					itr++;
				}
			}
		}
		// 更新AreaTrans层渲染器
		{
			foreach(QgsVectorLayer* pLayer, m_pxAreaTransLayerMap)
			{
				QgsCategorizedSymbolRendererV2* pRender = nullptr;
				if (m_pxAreaTransLayerRenderer.contains(pLayer))
				{
					pRender = m_pxAreaTransLayerRenderer[pLayer];
				}
				else
				{
					pRender = new QgsCategorizedSymbolRendererV2("symbolNo");
					m_pxAreaTransLayerRenderer.insert(pLayer, pRender);
				}

				// 检测是否新填符号
				QMap<QString, quint32>::Iterator itr = m_strAreaTransLayerINSTs.begin();
				while (itr != m_strAreaTransLayerINSTs.end())
				{
					if (isExistSymbol(pRender, itr.value()) == false)
					{
						// 根据绘制命令创建符号
						XSymbol sym;
						QString strClr;
						quint32 unFontSize = 21;
						QgsSymbolV2* pSymV2 = createRendererByCmd(itr.key(), sym, "area",strClr,unFontSize);
						if (pSymV2 != nullptr)
						{
							pRender->addCategory(QgsRendererCategoryV2(itr.value(), pSymV2, QString("TransArea-%1").arg(itr.value())));
							pSymV2 = nullptr;
							pLayer->setRendererV2(pRender);
						}
// 						else
// 							Q_ASSERT(false);
					}

					itr++;
				}

			}
		}
		// 更新AreaNolmal层渲染器
		{
			foreach(QgsVectorLayer* pLayer, m_pxAreaNormalLayerMap)
			{
				QgsCategorizedSymbolRendererV2* pRender = nullptr;
				if (m_pxAreaNormalLayerRenderer.contains(pLayer))
				{
					pRender = m_pxAreaNormalLayerRenderer[pLayer];
				}
				else
				{
					pRender = new QgsCategorizedSymbolRendererV2("symbolNo");
					m_pxAreaNormalLayerRenderer.insert(pLayer, pRender);
				}

				// 检测是否新填符号
				QMap<QString, quint32>::Iterator itr = m_strAreaNormalLayerINSTs.begin();
				while (itr != m_strAreaNormalLayerINSTs.end())
				{
					if (isExistSymbol(pRender, itr.value()) == false)
					{
						// 根据绘制命令创建符号
						XSymbol sym;
						QString strClr;
						quint32 unFontSize = 21;
						QgsSymbolV2* pSymV2 = createRendererByCmd(itr.key(), sym, "area",strClr,unFontSize);
						if (pSymV2 != nullptr)
						{
							pRender->addCategory(QgsRendererCategoryV2(itr.value(), pSymV2, QString("NormalArea-%1").arg(itr.value())));
							pSymV2 = nullptr;
							pLayer->setRendererV2(pRender);
						}
// 						else
// 							Q_ASSERT(false);
					}

					itr++;
				}

			}
		}
		// 更新AreaCover层渲染器
		{
			foreach(QgsVectorLayer* pLayer, m_pxAreaCoverLayerMap)
			{
				QgsCategorizedSymbolRendererV2* pRender = nullptr;
				if (m_pxAreaCoverLayerRenderer.contains(pLayer))
				{
					pRender = m_pxAreaCoverLayerRenderer[pLayer];
				}
				else
				{
					pRender = new QgsCategorizedSymbolRendererV2("symbolNo");
					m_pxAreaCoverLayerRenderer.insert(pLayer, pRender);
				}

				// 检测是否新填符号
				QMap<QString, quint32>::Iterator itr = m_strAreaCoverLayerINSTs.begin();
				while (itr != m_strAreaCoverLayerINSTs.end())
				{
					if (isExistSymbol(pRender, itr.value()) == false)
					{
						// 根据绘制命令创建符号
						XSymbol sym;
						QString strClr;
						quint32 unFontSize = 21;
						QgsSymbolV2* pSymV2 = createRendererByCmd(itr.key(), sym, "area",strClr,unFontSize);
						if (pSymV2 != nullptr)
						{
							pRender->addCategory(QgsRendererCategoryV2(itr.value(), pSymV2, QString("CoverArea-%1").arg(itr.value())));
							pSymV2 = nullptr;
							pLayer->setRendererV2(pRender);
						}
// 						else
// 							Q_ASSERT(false);
					}

					itr++;
				}

			}
		}
		// 更新Custom层渲染器
		{
			foreach(QgsVectorLayer* pLayer, m_pxCustomLayerMap)
			{
				QgsCategorizedSymbolRendererV2* pRender = nullptr;
				if (m_pxCustomLayerRenderer.contains(pLayer))
				{
					pRender = m_pxCustomLayerRenderer[pLayer];
				}
				else
				{
					pRender = new QgsCategorizedSymbolRendererV2("symbolNo");
					m_pxCustomLayerRenderer.insert(pLayer, pRender);
				}

				// 检测是否新填符号
				QMap<QString, quint32>::Iterator itr = m_strCustomLayerINSTs.begin();
				while (itr != m_strCustomLayerINSTs.end())
				{
					if (isExistSymbol(pRender, itr.value()) == false)
					{
						// 根据绘制命令创建符号
						XSymbol sym;
						QString strClr;
						quint32 unFontSize = 21;
						QgsSymbolV2* pSymV2 = createRendererByCmd(itr.key(), sym, "area",strClr,unFontSize);
						if (pSymV2 != nullptr)
						{
							pRender->addCategory(QgsRendererCategoryV2(itr.value(), pSymV2, QString("CustomArea-%1").arg(itr.value())));
							pSymV2 = nullptr;
							pLayer->setRendererV2(pRender);
						}
// 						else
// 							Q_ASSERT(false);
					}

					itr++;
 				}
				
			}
		}
	}
	// 解析原始绘制符号指令
	QString XS57LayerAdapter::parseINSTCmd(XFeature* _pxFeature,const QString& _strOriCmd, qint32& _nShowAttID,QString& _strText, QString& _strColor, quint32& _unFontSize)
	{
		_strText = "";
		_strColor = "";
		_unFontSize = 0;
		_nShowAttID = -1;
		QStringList strINST = _strOriCmd.split(";");
		QString strFinINST = "";
		if (strINST.isEmpty())
			return strFinINST;

	parseLoop1:
		foreach(QString strOne, strINST){
			quint32 unLen = strOne.count();
			if (unLen > 2 && strOne.left(2) == "CS"){
				strINST.removeOne(strOne);
				strOne.remove(0, 2);
				strOne.remove("(");
				strOne.remove(")");
				if (!m_pxCondFNCs.contains(strOne))
					return false;
				XS57LayerAdapter::ConditionFNC pFn = m_pxCondFNCs[strOne];
				QStringList strExtentKeys, strExtentVals;
				QStringList strNewList = (this->*pFn)(_pxFeature, strExtentKeys, strExtentVals);
				if (!strNewList.isEmpty()){
					strINST = strNewList + strINST;
					goto parseLoop1;
				}
			}
			else if (unLen > 2){
				QString strTemp = strOne;
				if (!strFinINST.isEmpty())
					strTemp = QString(";") + strOne;
				strFinINST.append(strTemp);
				if (strOne.left(2) == "TE")
				{
					strOne.remove(0, 3);
					strOne.remove(strOne.count() - 1, 1);
					QStringList strTextPars = strOne.split(",");
					if (strTextPars.count() == 10)
					{
						QString strFormat = strTextPars.at(0);
						QString strAttName = strTextPars.at(1);
						QString strClrName = strTextPars.at(8);
						strFormat.remove("'");
						strAttName.remove("'");
						_unFontSize = strTextPars.at(9).toUInt();
						if (_unFontSize == 0)
							_unFontSize = 21;

						if (!strAttName.isEmpty() && m_xAttsStrToCodes.contains(strAttName))
						{
							quint32 unAttKey = m_xAttsStrToCodes[strAttName];
							QString strAttText = "";
							if (_pxFeature->GetAttr(unAttKey, strAttText))
							{
								int nStrIdx = strFormat.indexOf("%s");
								if (nStrIdx >= 0 && nStrIdx < strFormat.count())
								{
									if (_strText.isEmpty())
										_strText = QString::fromLocal8Bit("%1%2%3").arg(strFormat.left(nStrIdx)).arg(strAttText).arg(strFormat.right(strFormat.count() - 2));
									else
									{
										_strText += ",";
										_strText += QString::fromLocal8Bit("%1%2%3").arg(strFormat.left(nStrIdx)).arg(strAttText).arg(strFormat.right(strFormat.count() - 2));
									}
									}

								_strColor = strClrName;
								_nShowAttID = unAttKey;
							}
						}
					}
				}
				else if (strOne.left(2) == "TX")
				{
					strOne.remove(0, 3);
					strOne.remove(strOne.count() - 1, 1);
					QStringList strTextPars = strOne.split(",");
					if (strTextPars.count() == 9)
					{
						QString strAttName = strTextPars.at(0);
						QString strClrName = strTextPars.at(7);
						strAttName.remove("'");
						_unFontSize = strTextPars.at(8).toUInt();
						if (_unFontSize == 0)
							_unFontSize = 21;

						if (!strAttName.isEmpty() && m_xAttsStrToCodes.contains(strAttName))
						{
							quint32 unAttKey = m_xAttsStrToCodes[strAttName];
							QString strAttText = "";
							if (_pxFeature->GetAttr(unAttKey, strAttText))
							{
								if (_strText.isEmpty())
									_strText = strAttText;
								else
								{
									_strText += ";";
									_strText += strAttText;
								}
								_strColor = strClrName;
								_nShowAttID = unAttKey;
							}
						}
					}
				}
			}
		}

		return strFinINST;
	}
	// 分类渲染器是否包含指定的渲染器
	const bool XS57LayerAdapter::isExistSymbol(QgsCategorizedSymbolRendererV2* _pRender, const quint32& _unSymNo)
	{
		if (!_pRender)
			return false;
		QgsCategoryList::ConstIterator itr = _pRender->categories().begin();
		while (itr != _pRender->categories().end()){
			if (itr->value().toUInt() == _unSymNo)
				return true;
			itr++;
		}
		return false;
	}
	// 根据绘制命令创建渲染器
	QgsSymbolV2* XS57LayerAdapter::createRendererByCmd(const QString& _strCmd, XSymbol& _xSymbol, const QString& _strGeoType, QString& _strColor, quint32& _unFontSize)
	{
		if (_strGeoType.isEmpty())
			return nullptr;
		if (_strGeoType != "point" && _strGeoType != "line" && _strGeoType != "area")
			return nullptr;

		QStringList strINST = _strCmd.split(";");
		if (strINST.isEmpty())
			return nullptr;
		QgsSymbolLayerV2List xAreaSymbolList;
		QgsSymbolLayerV2List xAPSymbolList;
		QgsSymbolLayerV2List xLineSymbolList;
		QgsSymbolLayerV2List xLCLineSymbolList;
		QgsSymbolLayerV2List xPointSymbolList;
		QString strFinalINST = QString();
		quint32 error = -1;

	parseLoop2:
		foreach(QString strOne, strINST){
			quint32 unLen = strOne.count();
			if (unLen > 2 && strOne.left(2) == "AC" && _strGeoType == "area"){
				// 填充颜色 
				strOne.remove(0, 2);
				strOne.remove("(");
				strOne.remove(")");
				QColor xCor = QColor();
				error = m_pxS52Lib->GetColorByKey(strOne, xCor);
				if (error != 0)
					return nullptr;
				QgsSimpleFillSymbolLayerV2* pLayerV2 = new QgsSimpleFillSymbolLayerV2();
				pLayerV2->setFillColor(xCor);
				pLayerV2->setOutlineColor(xCor);
				xAreaSymbolList.push_front(pLayerV2);
				pLayerV2 = NULL;
				strINST.removeFirst();
			}
			else if (unLen > 4 && strOne.left(2) == "LS"){
				// 线型
				strOne.remove(0, 3);
				strOne.remove(strOne.length() - 1, 1);
				strOne.remove(" ");
				QStringList strPrams = strOne.split(",");
				strOne.clear();
				if (strPrams.count() == 3){
					Qt::PenStyle xPenSty = strToQtPenStyle(strPrams.at(0));
					double dWidth = strPrams.at(1).toDouble() * 0.2;
					QColor xColor = QColor();
					error = m_pxS52Lib->GetColorByKey(strPrams.at(2), xColor);
					if (error != 0)
						return nullptr;

					QgsSimpleLineSymbolLayerV2* pLayerV2 = new QgsSimpleLineSymbolLayerV2();
					pLayerV2->setColor(xColor);
					pLayerV2->setPenStyle(xPenSty);
					pLayerV2->setWidth(dWidth);
					xLineSymbolList.push_front(pLayerV2);
					pLayerV2 = NULL;
					strINST.removeFirst();

				}
			}
			else if (unLen > 2 && strOne.left(2) == "CS"){
				strOne.remove(0, 2);
				strOne.remove("(");
				strOne.remove(")");
				strINST.removeFirst();
				if (!m_pxCondFNCs.contains(strOne))
					return nullptr;
				XS57LayerAdapter::ConditionFNC pFn = m_pxCondFNCs[strOne];
				QStringList strExtentKeys, strExtentVals;
				QStringList strNewList;//-DEL = (ms_xInstance.*pFn)(_pxFeature, strExtentKeys, strExtentVals);
// 				if (strExtentKeys.contains("DisplayCat")){
// 					_xeShowLev = xeShowLev(strExtentVals.at(strExtentKeys.indexOf("DisplayCat")).toUInt());
// 				}

				if (!strNewList.isEmpty()){
					strINST = strNewList + strINST;
					//m_pxCondTypes.insert(_pxFeature->unObjType, pFn);
					goto parseLoop2;
				}
				else
					return nullptr;
			}
			else if (unLen > 2 && strOne.left(2) == "AP" && _strGeoType == "area"){
				strOne.remove(0, 2);
				strOne.remove("(");
				strOne.remove(")");
				// 检验文件是否存在
				QString strAPFile = QString("./Data/S52/Symbols/") + colorStyleToStr(m_xeColorStyle) + QString("/") + strOne + QString(".png");
				if (QFile::exists(strAPFile) == false)
					return nullptr;

				QgsRasterFillSymbolLayer* pLayerV2 = new QgsRasterFillSymbolLayer(strAPFile);
				pLayerV2->setWidth(7);
				pLayerV2->setCoordinateMode(QgsRasterFillSymbolLayer::Feature);
				pLayerV2->setWidthUnit(QgsSymbolV2::MM);
				xAPSymbolList.push_front(pLayerV2);
				pLayerV2 = NULL;
				strINST.removeFirst();
			}
			else if (unLen > 2 && strOne.left(2) == "SY"){
				strOne.remove(0, 2);
				strOne.remove("(");
				strOne.remove(")");
				// 检验文件是否存在
				QString strAPFile = QString("./Data/S52/Symbols/") + colorStyleToStr(m_xeColorStyle) + QString("/") + strOne + QString(".png");
				if (QFile::exists(strAPFile) == false)
					return nullptr;

				XImageMarkerSymbolLayerV2* pLayerV2 = new XImageMarkerSymbolLayerV2(strAPFile);
				pLayerV2->setSize(1);

				// 获取图片大小
				QImage img(strAPFile);
				if (img.width() > 45)
					pLayerV2->setSize(1.0 / 2.5);
				/*if (strOne == "CTNARE51")
					pLayerV2->setSize(1.0 / 3.0);
				else if (strOne == "ACHRES61")*/
				//	pLayerV2->setSize(1.0 / 3.0);
// 				if (strOne == "LNDARE01"){
// 					pLayerV2->setSize(1.0 / 3.0);
// 					//unScaleMax = XLNDARE_POINT_SCALE_MAX;
// 				}
// 				if (strOne == "WRECKS01")
// 					pLayerV2->setSize(0.5);
// 				if (strOne == "PRCARE51")
// 					pLayerV2->setSize(1.0 / 3.0);

				xPointSymbolList.append(pLayerV2);
				pLayerV2 = NULL;
				strINST.removeFirst();
			}
			else if (unLen > 2 && strOne.left(2) == "LC"){
				strOne.remove(0, 2);
				strOne.remove("(");
				strOne.remove(")");
				// 检验文件是否存在
				QString strAPFile = QString("./Data/S52/Symbols/") + colorStyleToStr(m_xeColorStyle) + QString("/") + strOne + QString(".png");
				if (QFile::exists(strAPFile) == false)
					return nullptr;
				QgsMarkerLineSymbolLayerV2* pLineMarkerLay = new QgsMarkerLineSymbolLayerV2();

				QgsSymbolLayerV2List xMarkerLayers;
				XImageMarkerSymbolLayerV2* pImageSymber = new XImageMarkerSymbolLayerV2(strAPFile);
				pImageSymber->setSize(2.0);
				pImageSymber->setRenderingPass(10);
				xMarkerLayers.append(pImageSymber);
				QgsMarkerSymbolV2* pMarkerSym = new QgsMarkerSymbolV2(xMarkerLayers);
				;
				pLineMarkerLay->setSubSymbol(pMarkerSym);
				xLCLineSymbolList.push_back(pLineMarkerLay);
			}
			else if (unLen > 2 && strOne.left(2) == "TX")
			{
				strOne.remove(0, 3);
				strOne.remove(strOne.count() - 1, 1);
				QStringList strTextPars = strOne.split(",");
				if (strTextPars.count() == 9)
				{
					_strColor = strTextPars.at(7);
					_unFontSize = strTextPars.at(8).toUInt();
					if (_unFontSize == 0)
						_unFontSize = 21;
				}
			}
			else if (unLen > 2 && strOne.left(2) == "TE")
			{
				strOne.remove(0, 3);
				strOne.remove(strOne.count() - 1, 1);
				QStringList strTextPars = strOne.split(",");
				if (strTextPars.count() == 10)
				{
					_strColor = strTextPars.at(8);
					_unFontSize = strTextPars.at(9).toUInt();
					if (_unFontSize == 0)
						_unFontSize = 21;
				}
			}
		}
		//_xSymbol.unSymbolNo = ++m_unSymbolIndex;
		_xSymbol.strINST = strFinalINST;

		if (_strGeoType == "area")
		{
			xAreaSymbolList << xAPSymbolList << xLineSymbolList << xLCLineSymbolList;

			QgsFillSymbolV2* pFillSym = new QgsFillSymbolV2(xAreaSymbolList);
			return pFillSym;
		}
		else if (_strGeoType == "point")
		{
			if (xPointSymbolList.isEmpty())
				return nullptr;
			
			QgsMarkerSymbolV2* pImgSym = new QgsMarkerSymbolV2(xPointSymbolList);
			return pImgSym;
		}
		else if (_strGeoType == "line")
		{
			if (xLineSymbolList.isEmpty() && xLCLineSymbolList.isEmpty())
				return nullptr;
			if (xLineSymbolList.isEmpty())
				return nullptr;
			//xLCLineSymbolList << xLCLineSymbolList;
			QgsLineSymbolV2* pLineSym = new QgsLineSymbolV2(xLineSymbolList);
			return pLineSym;
		}
		return nullptr;
	}
	// 根据S57 ObjectCode 获取显示图层
	QgsMapLayer* XS57LayerAdapter::getS57ObjLayer(XFeature* _pFe, const QString& _strLayerType, quint32& _unRefSymNo, quint32& _unRenLev)
	{
		QgsMapLayer* pLayer = nullptr;
		_unRefSymNo = -1;
		if (_pFe == nullptr)
			return pLayer;
		if (_strLayerType.isEmpty())
			return pLayer;

		// 获取S57 Class信息
		if (m_xS57ClassVec.contains(_pFe->unObjType) == false)
		{
			return pLayer;
		}

		XS57Class objClass = m_xS57ClassVec[_pFe->unObjType];
		// 判断是否包含图层描述信息
		if (objClass.strLayers.isEmpty() || objClass.strLayers.size() != objClass.strPRIMTypes.size())
		{
			return pLayer;
		}
		int nIdx = objClass.strPRIMTypes.indexOf(_strLayerType);
		if (nIdx < 0)
		{
			return pLayer;
		}
		int nLayerIdx = objClass.strLayers.at(nIdx).toUInt();
		_unRenLev = objClass.strRendererLevs.at(nIdx).toUInt();

		// 获取显示比例尺
		quint32 unMaxS = XDEFAULT_VLAYER_SCALE_MAX;
		quint32 unMinS = XDEFAULT_VLAYER_SCALE_MIN;
		if (!_pFe->GetAttr(132, unMaxS))
			unMaxS = XDEFAULT_VLAYER_SCALE_MAX;
		if (!_pFe->GetAttr(133, unMinS))
			unMinS = XDEFAULT_VLAYER_SCALE_MIN;
		QString strLayerKey = QString("%1-%2").arg(unMinS).arg(unMaxS);

		// 获取S52显示库LookUp
		XLookupTAB* pLup = nullptr;
		{
			if (!m_xS57ClassCodeToStrs.contains(_pFe->unObjType))
				return pLayer;
			QList<XLookupTAB*> xLups = m_pxS52Lib->GetLoopTAB(xePRIM(_pFe->eGeoType), m_xS57ClassCodeToStrs[_pFe->unObjType]);
			bool bIsTrue = true;
			// 排序
			qSort(xLups.begin(), xLups.end(), CompareLoopTab);
			foreach(XLookupTAB* pTempLup, xLups){
				// 条件判断
				if (pTempLup->m_strATTC.count() > 0 && _pFe->xAtts.count() > 0){
					// 条件判断
					QMap<QString, QString>::Iterator itr = pTempLup->m_strATTC.begin();
					while (itr != pTempLup->m_strATTC.end()){
						QString strKey = itr.key();
						QString strValue = itr.value();
						bool bIsDigit = false;
						quint32 unKey = 0;
						strKey.toInt(&bIsDigit);
						if (!bIsDigit){
							if (!m_xAttsStrToCodes.contains(strKey))
								return pLayer;
							unKey = m_xAttsStrToCodes[strKey];
						}
						else
							unKey = strKey.toUInt();

						if (strValue == "?"){
							// 判断该值是否存在
							if (_pFe->xAtts.contains(unKey)){
								bIsTrue = false;
								break;
							}
						}
						else if (_pFe->xAtts.contains(unKey))
						{
							if (_pFe->xAtts[unKey] != strValue){
								bIsTrue = false;
								break;
							}
						}
						else
							bIsTrue = false;

						itr++;
					}
					if (!bIsTrue)
						continue;

					pLup = pTempLup;
					break;
				}
				else if (pTempLup->m_strATTC.count() == 0){
					pLup = pTempLup;
					break;
				}
			}
			xLups.clear();
		}
		if (pLup == nullptr)
		{
			return pLayer;
		}
		// 获取绘制指令
		QString strLabelText = "";
		QColor textClr;
		QString strTxtClr = "";
		quint32 unFontSize = 0;
		int nShowAttID = -1;
		QString strDrawCMD = parseINSTCmd(_pFe, pLup->m_strINST, nShowAttID,strLabelText, strTxtClr, unFontSize);
		if (strDrawCMD.isEmpty())
		{
			return pLayer;
		}

		// 顺序 点 线、   透明的面、           无遮挡的面、  范围面 、    其他 
		// Point=0,Line=1,Area_Transparence=2, Area_Normal=3,Area_Cover=4,Custom=5
		if (nLayerIdx == 0)
		{
			if (m_strPointLayerINSTs.contains(strDrawCMD))
				_unRefSymNo = m_strPointLayerINSTs[strDrawCMD];
			else
			{
				_unRefSymNo = ++m_unSymbolIndex;
				m_strPointLayerINSTs[strDrawCMD] = _unRefSymNo;
			}

			if (!m_pxPointLayerMap.contains(strLayerKey))
			{
				// 创建图层
				QString strName = QString::fromLocal8Bit("PointLayer-%1").arg(strLayerKey);
				QgsPalLayerSettings xLaySettting;
				QStringList strFieldKeys = QStringList();
				QStringList strFieldTypes = QStringList();

				strFieldKeys << "id" << "key" << "symbolNo" << "label" << "attKeys" << "attVals";
				strFieldTypes << "integer" << "string(64)" << "integer" << "string(32)" << "string" << "string";
				xLaySettting.enabled = true;
				QgsVectorLayer* pLayertemp = createVectorLayer(strName, "point", strFieldKeys, strFieldTypes, QPainter::CompositionMode_SourceOver, &xLaySettting
					, nullptr);

				if (pLayertemp == nullptr)
					return pLayer;

				pLayertemp->setScaleBasedVisibility(true);
				pLayertemp->setMaximumScale(unMaxS);
				pLayertemp->setMinimumScale(unMinS);
				//QgsMapLayerRegistry::instance()->addMapLayer(pLayer, true);
				m_pxPointLayerMap.insert(strLayerKey, pLayertemp);
				//m_pxShowLayerSet.push_back(pLayertemp);
				pLayertemp->startEditing();
				pLayer = pLayertemp;
				pLayertemp = nullptr;
			}
			pLayer = m_pxPointLayerMap[strLayerKey];
		}
		else if (nLayerIdx == 1)
		{
			if (m_strLineLayerINSTs.contains(strDrawCMD))
				_unRefSymNo = m_strLineLayerINSTs[strDrawCMD];
			else
			{
				_unRefSymNo = ++m_unSymbolIndex;
				m_strLineLayerINSTs[strDrawCMD] = _unRefSymNo;
			}

			if (!m_pxLineLayerMap.contains(strLayerKey))
			{
				// 创建图层
				QString strName = QString::fromLocal8Bit("LineLayer-%1").arg(strLayerKey);
				QgsPalLayerSettings xLaySettting;
				QStringList strFieldKeys = QStringList();
				QStringList strFieldTypes = QStringList();

				strFieldKeys << "id" << "key" << "symbolNo" << "label" << "attKeys" << "attVals";
				strFieldTypes << "integer" << "string(64)" << "integer" << "string(32)" << "string" << "string";
				xLaySettting.enabled = true;
				QgsVectorLayer* pLayertemp = createVectorLayer(strName, "polygon", strFieldKeys, strFieldTypes, QPainter::CompositionMode_SourceOver, &xLaySettting
					, nullptr);

				if (pLayertemp == nullptr)
					return pLayer;

				pLayertemp->setScaleBasedVisibility(true);
				pLayertemp->setMaximumScale(unMaxS);
				pLayertemp->setMinimumScale(unMinS);
				//QgsMapLayerRegistry::instance()->addMapLayer(pLayer, true);
				m_pxLineLayerMap.insert(strLayerKey, pLayertemp);
				//m_pxShowLayerSet.push_back(pLayertemp);
				pLayertemp->startEditing();
				pLayer = pLayertemp;
				pLayertemp = nullptr;
			}
			pLayer = m_pxLineLayerMap[strLayerKey];
		}
		else if (nLayerIdx == 2)
		{
			if (m_strAreaTransLayerINSTs.contains(strDrawCMD))
				_unRefSymNo = m_strAreaTransLayerINSTs[strDrawCMD];
			else
			{
				_unRefSymNo = ++m_unSymbolIndex;
				m_strAreaTransLayerINSTs[strDrawCMD] = _unRefSymNo;
			}

			if (!m_pxAreaTransLayerMap.contains(strLayerKey))
			{
				// 创建图层
				QString strName = QString::fromLocal8Bit("AreaTransLayer-%1").arg(strLayerKey);
				QgsPalLayerSettings xLaySettting;
				QStringList strFieldKeys = QStringList();
				QStringList strFieldTypes = QStringList();

				strFieldKeys << "id" << "key" << "symbolNo" << "label" << "attKeys" << "attVals";
				strFieldTypes << "integer" << "string(64)" << "integer" << "string(32)" << "string" << "string";
				xLaySettting.enabled = true;
				QgsVectorLayer* pLayertemp = createVectorLayer(strName, "polygon", strFieldKeys, strFieldTypes, QPainter::CompositionMode_SourceOver, &xLaySettting
					, nullptr);

				if (pLayertemp == nullptr)
					return pLayer;

				pLayertemp->setScaleBasedVisibility(true);
				pLayertemp->setMaximumScale(unMaxS);
				pLayertemp->setMinimumScale(unMinS);
				//QgsMapLayerRegistry::instance()->addMapLayer(pLayer, true);
				m_pxAreaTransLayerMap.insert(strLayerKey, pLayertemp);
				//m_pxShowLayerSet.push_back(pLayertemp);
				pLayertemp->startEditing();
				pLayer = pLayertemp;
				pLayertemp = nullptr;
			}
			pLayer = m_pxAreaTransLayerMap[strLayerKey];
		}
		else if (nLayerIdx == 3)
		{
			if (m_strAreaNormalLayerINSTs.contains(strDrawCMD))
				_unRefSymNo = m_strAreaNormalLayerINSTs[strDrawCMD];
			else
			{
				_unRefSymNo = ++m_unSymbolIndex;
				m_strAreaNormalLayerINSTs[strDrawCMD] = _unRefSymNo;
			}

			if (!m_pxAreaNormalLayerMap.contains(strLayerKey))
			{
				// 创建图层
				QString strName = QString::fromLocal8Bit("AreaNormalLayer-%1").arg(strLayerKey);
				QgsPalLayerSettings xLaySettting;
				QStringList strFieldKeys = QStringList();
				QStringList strFieldTypes = QStringList();

				strFieldKeys << "id" << "key" << "symbolNo" << "label" << "attKeys" << "attVals";
				strFieldTypes << "integer" << "string(64)" << "integer" << "string(32)" << "string" << "string";
				xLaySettting.enabled = true;
				QgsVectorLayer* pLayertemp = createVectorLayer(strName, "polygon", strFieldKeys, strFieldTypes, QPainter::CompositionMode_SourceOver, &xLaySettting
					, nullptr);

				if (pLayertemp == nullptr)
					return pLayer;

				pLayertemp->setScaleBasedVisibility(true);
				pLayertemp->setMaximumScale(unMaxS);
				pLayertemp->setMinimumScale(unMinS);
				//QgsMapLayerRegistry::instance()->addMapLayer(pLayer, true);
				m_pxAreaNormalLayerMap.insert(strLayerKey, pLayertemp);
				//m_pxShowLayerSet.push_back(pLayertemp);
				pLayertemp->startEditing();
				pLayer = pLayertemp;
				pLayertemp = nullptr;
			}
			pLayer = m_pxAreaNormalLayerMap[strLayerKey];
		}
		else if (nLayerIdx == 4)
		{
			if (m_strAreaCoverLayerINSTs.contains(strDrawCMD))
				_unRefSymNo = m_strAreaCoverLayerINSTs[strDrawCMD];
			else
			{
				_unRefSymNo = ++m_unSymbolIndex;
				m_strAreaCoverLayerINSTs[strDrawCMD] = _unRefSymNo;
			}

			if (!m_pxAreaCoverLayerMap.contains(strLayerKey))
			{
				// 创建图层
				QString strName = QString::fromLocal8Bit("AreaCoverLayer-%1").arg(strLayerKey);
				QgsPalLayerSettings xLaySettting;
				QStringList strFieldKeys = QStringList();
				QStringList strFieldTypes = QStringList();

				strFieldKeys << "id" << "key" << "symbolNo" << "label" << "attKeys" << "attVals";
				strFieldTypes << "integer" << "string(64)" << "integer" << "string(32)" << "string" << "string";
				xLaySettting.enabled = true;
				QgsVectorLayer* pLayertemp = createVectorLayer(strName, "polygon", strFieldKeys, strFieldTypes, QPainter::CompositionMode_SourceOver, &xLaySettting
					, nullptr);

				if (pLayertemp == nullptr)
					return pLayer;

				pLayertemp->setScaleBasedVisibility(true);
				pLayertemp->setMaximumScale(unMaxS);
				pLayertemp->setMinimumScale(unMinS);
				//QgsMapLayerRegistry::instance()->addMapLayer(pLayer, true);
				m_pxAreaCoverLayerMap.insert(strLayerKey, pLayertemp);
				//m_pxShowLayerSet.push_back(pLayertemp);
				pLayertemp->startEditing();
				pLayer = pLayertemp;
				pLayertemp = nullptr;
			}
			pLayer = m_pxAreaCoverLayerMap[strLayerKey];
		}
		else if (nLayerIdx == 5)
		{
			if (m_strCustomLayerINSTs.contains(strDrawCMD))
				_unRefSymNo = m_strCustomLayerINSTs[strDrawCMD];
			else
			{
				_unRefSymNo = ++m_unSymbolIndex;
				m_strCustomLayerINSTs[strDrawCMD] = _unRefSymNo;
			}

			if (!m_pxCustomLayerMap.contains(strLayerKey))
			{
				// 创建图层
				QString strName = QString::fromLocal8Bit("CustomLayer-%1").arg(strLayerKey);
				QgsPalLayerSettings xLaySettting;
				QStringList strFieldKeys = QStringList();
				QStringList strFieldTypes = QStringList();

				strFieldKeys << "id" << "key" << "symbolNo" << "label"<<"attKeys"<<"attVals";
				strFieldTypes << "integer" << "string(64)" << "integer" << "string(32)" << "string" << "string";
				xLaySettting.enabled = true;
				QgsVectorLayer* pLayertemp = createVectorLayer(strName, "polygon", strFieldKeys, strFieldTypes, QPainter::CompositionMode_SourceOver, &xLaySettting
					, nullptr);

				if (pLayertemp == nullptr)
					return pLayer;

				pLayertemp->setScaleBasedVisibility(true);
				pLayertemp->setMaximumScale(unMaxS);
				pLayertemp->setMinimumScale(unMinS);
				//QgsMapLayerRegistry::instance()->addMapLayer(pLayer, true);
				m_pxCustomLayerMap.insert(strLayerKey, pLayertemp);
				//m_pxShowLayerSet.push_back(pLayertemp);
				pLayertemp->startEditing();
				pLayer = pLayertemp;
				pLayertemp = nullptr;
			}
			pLayer = m_pxCustomLayerMap[strLayerKey];
		}
		else
		{
			int md = 0;
		}

		return pLayer;
	}
	// 根据S57 ObjectCode 获取显示图层
	int XS57LayerAdapter::getS57ObjLayer(XFeature* _pFe, QList<QgsMapLayer*>& _pLayers, QList<quint32>& _unSymNos, QList<quint32>& _unRenLev, QStringList& _strLayerTypes, qint32& _nShowAtt)
	{
		if (_pFe == nullptr)
			return 1;
		_nShowAtt = -1;
		_unRenLev.clear();
		_unSymNos.clear();
		_strLayerTypes.clear();

		// 获取S57 类别信息
		if (m_xS57ClassVec.contains(_pFe->unObjType) == false)
		{
			// 找不到类别信息
			////Q_ASSERT_X(false, QString::fromLocal8Bit("获取要素图层").toLocal8Bit(), QString::fromLocal8Bit("获取要素S57类别信息失败，类别码:%1").arg(_pFe->unObjType).toLocal8Bit());
			return 2;
		}

		XS57Class objClass = m_xS57ClassVec[_pFe->unObjType];

		// 判断是否包含图层描述信息
		if (objClass.strLayers.isEmpty() || objClass.strLayers.size() != objClass.strPRIMTypes.size()
			|| objClass.strLayers.size() != objClass.strRendererLevs.size())
		{
			//Q_ASSERT_X(false, QString::fromLocal8Bit("获取要素图层").toLocal8Bit(), QString::fromLocal8Bit("获取要素图层描述信息失败，类别码:%1").arg(_pFe->unObjType).toLocal8Bit());
			return 3;
		}

		int nIdx = 0;
		QString strType = "";
		if (_pFe->eGeoType == XFeature::xeArea)
			strType = "Area";
		else if (_pFe->eGeoType == XFeature::xeLine)
			strType = "Line";
		else if (_pFe->eGeoType == XFeature::xePoint)
			strType = "Point";
		if (strType.isEmpty())
			return 4;

		nIdx = objClass.strPRIMTypes.indexOf(strType);
		if (nIdx < 0)
			return 5;
		quint32 unLayerIdx = objClass.strLayers.at(nIdx).toUInt();
		quint32 unRenLev = objClass.strRendererLevs.at(nIdx).toUInt();
		quint32 unSymNo = -1;

		//foreach(QString strLayIdx,objClass.strLayers)
		{
			// 获取显示比例尺
			quint32 unMaxS = XDEFAULT_VLAYER_SCALE_MAX;
			quint32 unMinS = XDEFAULT_VLAYER_SCALE_MIN;
			if (!_pFe->GetAttr(132, unMaxS))
				unMaxS = XDEFAULT_VLAYER_SCALE_MAX;
			if (!_pFe->GetAttr(133, unMinS))
				unMinS = XDEFAULT_VLAYER_SCALE_MIN;
			// 查询是否包含自定义比例尺
			{
				if (objClass.strMinScale.size() > nIdx)
				{
					QString strMin = objClass.strMinScale.at(nIdx);
					if (!strMin.isEmpty())
						unMinS = strMin.toUInt();
				}
				if (objClass.strMaxScale.size() > nIdx)
				{
					QString strMax = objClass.strMaxScale.at(nIdx);
					if (!strMax.isEmpty())
						unMaxS = strMax.toUInt();
				}
			}


			// 获取S52显示库LookUp
			XLookupTAB* pLup = nullptr;
			{
				if (!m_xS57ClassCodeToStrs.contains(_pFe->unObjType))
				{
					//Q_ASSERT_X(false, QString::fromLocal8Bit("获取要素图层").toLocal8Bit(), QString::fromLocal8Bit("S57类别码转换字符串失败，类别码:%1").arg(_pFe->unObjType).toLocal8Bit());
					return 4;
				}
				QList<XLookupTAB*> xLups = m_pxS52Lib->GetLoopTAB(xePRIM(_pFe->eGeoType), m_xS57ClassCodeToStrs[_pFe->unObjType]);
				bool bIsTrue = true;
				// 排序
				qSort(xLups.begin(), xLups.end(), CompareLoopTab);
				foreach(XLookupTAB* pTempLup, xLups){
					// 条件判断
					if (pTempLup->m_strATTC.count() > 0 && _pFe->xAtts.count() > 0){
						// 条件判断
						QMap<QString, QString>::Iterator itr = pTempLup->m_strATTC.begin();
						while (itr != pTempLup->m_strATTC.end()){
							QString strKey = itr.key();
							QString strValue = itr.value();
							bool bIsDigit = false;
							quint32 unKey = 0;
							strKey.toInt(&bIsDigit);
							if (!bIsDigit){
								if (!m_xAttsStrToCodes.contains(strKey))
								{
									//Q_ASSERT_X(false, QString::fromLocal8Bit("获取要素图层").toLocal8Bit(), QString::fromLocal8Bit("根据S52提供的属性名称：%1找不到对应的的属性Code").arg(strKey).toLocal8Bit());
									return 5;
								}
								unKey = m_xAttsStrToCodes[strKey];
							}
							else
								unKey = strKey.toUInt();

							if (strValue == "?"){
								// 判断该值是否存在
								if (_pFe->xAtts.contains(unKey)){
									bIsTrue = false;
									break;
								}
							}
							else if (_pFe->xAtts.contains(unKey))
							{
								if (_pFe->xAtts[unKey] != strValue){
									bIsTrue = false;
									break;
								}
							}
							else
								bIsTrue = false;

							itr++;
						}
						if (!bIsTrue)
							continue;

						pLup = pTempLup;
						break;
					}
					else if (pTempLup->m_strATTC.count() == 0){
						pLup = pTempLup;
						break;
					}
				}
				xLups.clear();
			}
			if (pLup == nullptr)
			{
				//Q_ASSERT_X(false, QString::fromLocal8Bit("获取要素图层").toLocal8Bit(), QString::fromLocal8Bit("获取S52符号描述信息失败，类别码:%1").arg(_pFe->unObjType).toLocal8Bit());
				return 6;
			}
// 			if (_pFe->unObjType == 86)
// 				int kd = 0;
// 			if (m_xeShowLev == xeBase && pLup->m_xeDisCat != XLookupTAB::DISPLAYBASE)
// 				return 0;
// 			if (m_xeShowLev == xeStand && pLup->m_xeDisCat != XLookupTAB::STANDARD && pLup->m_xeDisCat != XLookupTAB::DISPLAYBASE 
// 				&& pLup->m_xeDisCat != XLookupTAB::OTHER)
// 				return 0;

			// 获取绘制指令
			QString strLabelText = "";
			QColor textClr;
			QString strTxtClr = "";
			quint32 unFontSize = 0;
			QString strDrawCMD = parseINSTCmd(_pFe, pLup->m_strINST,_nShowAtt, strLabelText, strTxtClr, unFontSize);
			if (strDrawCMD.isEmpty())
			{
				//Q_ASSERT_X(false, QString::fromLocal8Bit("获取要素图层").toLocal8Bit(), QString::fromLocal8Bit("解析S52符号描述指令字符串失败，类别码:%1，原始指令：%2")
				//	.arg(_pFe->unObjType).arg(pLup->m_strINST).toLocal8Bit());
				return 7;
			}

			QString strLayerKey = QString("%1-%2").arg(unMinS).arg(unMaxS);
			if (!strLabelText.isEmpty())
			{
				strLayerKey = QString("%1-%2-%3-%4").arg(unMinS).arg(unMaxS).arg(unFontSize).arg(strTxtClr);
			}

			// 点	   线、   透明的面、           无遮挡的面、  范围面 、    其他 
			// Point=0,Line=1,Area_Transparence=2, Area_Normal=3,Area_Cover=4,Custom=5
			// 如果当前图层不是点图层，但是包含点的绘制信息时，需要提取出点信息 并且绘制在点图层上 否则显示不了
			if (_pFe->eGeoType != XFeature::xePoint && strDrawCMD.contains("SY("))
			{
				int nTemIdx = strDrawCMD.indexOf("SY(");
				QStringList strCMDs = strDrawCMD.split(";");
				QStringList strPots;
				strDrawCMD.clear();
				foreach(QString strCmd, strCMDs)
				{
					if (strCmd.contains("SY("))
						strPots.append(strCmd);
					else
					{
						if (!strDrawCMD.isEmpty())
							strDrawCMD += ";";
						strDrawCMD += strCmd;
					}
				}

				// 循环处理点图层符号信息
				foreach(QString strPot, strPots)
				{
					if (m_strPointLayerINSTs.contains(strPot))
						unSymNo = m_strPointLayerINSTs[strPot];
					else
					{
						unSymNo = ++m_unSymbolIndex;
						m_strPointLayerINSTs[strPot] = unSymNo;
					}

					quint32 unPtMin = unMinS;
					quint32 unPtMax = unMaxS;
					// 查询是否包含自定义点比例尺
					{
						nTemIdx = objClass.strPRIMTypes.indexOf("Point");
						if (nTemIdx >= 0)
						{
							if (objClass.strMinScale.size() > nTemIdx)
							{
								QString strMin = objClass.strMinScale.at(nTemIdx);
								if (!strMin.isEmpty())
									unPtMin = strMin.toUInt();
							}
							if (objClass.strMaxScale.size() > nTemIdx)
							{
								QString strMax = objClass.strMaxScale.at(nTemIdx);
								if (!strMax.isEmpty())
									unPtMax = strMax.toUInt();
							}
						}
						else
						{
							// 多余的参数用来控制点
							if (objClass.strMinScale.size() > objClass.strLayers.size() - 1)
								unPtMin = objClass.strMinScale.last().toUInt();
							if (objClass.strMaxScale.size() > objClass.strLayers.size() - 1)
								unPtMax = objClass.strMaxScale.last().toUInt();
						}
					}
					QString strPtLayKey = QString("%1-%2").arg(unPtMin).arg(unPtMax);

					if (!m_pxPointLayerMap.contains(strPtLayKey))
					{
						// 创建图层
						QString strName = QString::fromLocal8Bit("PointLayer-%1").arg(strPtLayKey);
						QgsPalLayerSettings xLaySettting;
						QStringList strFieldKeys = QStringList();
						QStringList strFieldTypes = QStringList();
						if (!strLabelText.isEmpty())
						{
							xLaySettting.drawLabels = true;
							xLaySettting.textColor = textClr;
							xLaySettting.textFont = QFont(QString::fromLocal8Bit("微软雅黑"),unFontSize);
							xLaySettting.fieldName = "showText";
						}

						strFieldKeys << "id" << "key" << "symbolNo" << "docMark" << "showText" << "attKeys" << "attVals";
						strFieldTypes << "integer" << "string(64)" << "integer" << "string(32)" << "string(64)" << "string(64)" << "string(128)";
						xLaySettting.enabled = true;
						QgsVectorLayer* pLayertemp = createVectorLayer(strName, "point", strFieldKeys, strFieldTypes, QPainter::CompositionMode_SourceOver, &xLaySettting
							, nullptr);

						if (pLayertemp == nullptr)
						{
							return 8;
						}

						pLayertemp->setScaleBasedVisibility(true);
						pLayertemp->setMaximumScale(unPtMax);
						pLayertemp->setMinimumScale(unPtMin);
						m_pxPointLayerMap.insert(strPtLayKey, pLayertemp);
						pLayertemp = nullptr;
					}
					_pLayers.push_back(m_pxPointLayerMap[strPtLayKey]);
					_unSymNos.push_back(unSymNo);
					_unRenLev.push_back(unRenLev);
					_strLayerTypes.push_back("point");
				}
			}

			if (unLayerIdx == 0)
			{
				if (m_strPointLayerINSTs.contains(strDrawCMD))
					unSymNo = m_strPointLayerINSTs[strDrawCMD];
				else
				{
					unSymNo = ++m_unSymbolIndex;
					m_strPointLayerINSTs[strDrawCMD] = unSymNo;
				}

				if (!m_pxPointLayerMap.contains(strLayerKey))
				{
					// 创建图层
					QString strName = QString::fromLocal8Bit("PointLayer-%1").arg(strLayerKey);
					QgsPalLayerSettings xLaySettting;
					
					xLaySettting.labelOffsetInMapUnits = true;
					QStringList strFieldKeys = QStringList();
					QStringList strFieldTypes = QStringList();
					if (!strLabelText.isEmpty())
					{
						xLaySettting.drawLabels = true;
						xLaySettting.textColor = textClr;
						xLaySettting.textFont = QFont(QString::fromLocal8Bit("微软雅黑"), unFontSize);
						xLaySettting.fieldName = "showText";
					}

					strFieldKeys << "id" << "key" << "symbolNo" << "docMark" <<"showText" << "attKeys" << "attVals";
					strFieldTypes << "integer" << "string(64)" << "integer" << "string(32)" << "string(64)" << "string(64)" << "string(128)";
					xLaySettting.enabled = true;
					QgsVectorLayer* pLayertemp = createVectorLayer(strName, "point", strFieldKeys, strFieldTypes, QPainter::CompositionMode_SourceOver, &xLaySettting
						, nullptr);

					if (pLayertemp == nullptr)
					{
						return 8;
					}

					pLayertemp->setScaleBasedVisibility(true);
					pLayertemp->setMaximumScale(unMaxS);
					pLayertemp->setMinimumScale(unMinS);
					m_pxPointLayerMap.insert(strLayerKey, pLayertemp);
					pLayertemp = nullptr;
				}
				_pLayers.push_back(m_pxPointLayerMap[strLayerKey]);
				_unSymNos.push_back(unSymNo);
				_unRenLev.push_back(unRenLev);
				_strLayerTypes.push_back("point");
			}
			else if (unLayerIdx == 1)
			{
				if (m_strLineLayerINSTs.contains(strDrawCMD))
					unSymNo = m_strLineLayerINSTs[strDrawCMD];
				else
				{
					unSymNo = ++m_unSymbolIndex;
					m_strLineLayerINSTs[strDrawCMD] = unSymNo;
				}

				if (!m_pxLineLayerMap.contains(strLayerKey))
				{
					// 创建图层
					QString strName = QString::fromLocal8Bit("LineLayer-%1").arg(strLayerKey);
					QgsPalLayerSettings xLaySettting;
					QStringList strFieldKeys = QStringList();
					QStringList strFieldTypes = QStringList();

					strFieldKeys << "id" << "key" << "symbolNo" << "docMark" << "attKeys" << "attVals";
					strFieldTypes << "integer" << "string(64)" << "integer" << "string(32)" << "string" << "string";
					xLaySettting.enabled = true;
					QgsVectorLayer* pLayertemp = createVectorLayer(strName, "polygon", strFieldKeys, strFieldTypes, QPainter::CompositionMode_SourceOver, &xLaySettting
						, nullptr);

					if (pLayertemp == nullptr)
					{
						return 9;
					}

					pLayertemp->setScaleBasedVisibility(true);
					pLayertemp->setMaximumScale(unMaxS);
					pLayertemp->setMinimumScale(unMinS);
					//QgsMapLayerRegistry::instance()->addMapLayer(pLayer, true);
					m_pxLineLayerMap.insert(strLayerKey, pLayertemp);
					pLayertemp = nullptr;
				}
				_pLayers.push_back(m_pxLineLayerMap[strLayerKey]);
				_unSymNos.push_back(unSymNo);
				_unRenLev.push_back(unRenLev);
				_strLayerTypes.push_back("line");
			}
			else if (unLayerIdx == 2)
			{
				if (m_strAreaTransLayerINSTs.contains(strDrawCMD))
					unSymNo = m_strAreaTransLayerINSTs[strDrawCMD];
				else
				{
					unSymNo = ++m_unSymbolIndex;
					m_strAreaTransLayerINSTs[strDrawCMD] = unSymNo;
				}

				if (!m_pxAreaTransLayerMap.contains(strLayerKey))
				{
					// 创建图层
					QString strName = QString::fromLocal8Bit("AreaTransLayer-%1").arg(strLayerKey);
					QgsPalLayerSettings xLaySettting;
					QStringList strFieldKeys = QStringList();
					QStringList strFieldTypes = QStringList();

					strFieldKeys << "id" << "key" << "symbolNo" << "docMark" << "attKeys" << "attVals";
					strFieldTypes << "integer" << "string(64)" << "integer" << "string(32)" << "string" << "string";
					xLaySettting.enabled = true;
					QgsVectorLayer* pLayertemp = createVectorLayer(strName, "polygon", strFieldKeys, strFieldTypes, QPainter::CompositionMode_SourceOver, &xLaySettting
						, nullptr);

					if (pLayertemp == nullptr)
					{
						return 10;
					}

					pLayertemp->setScaleBasedVisibility(true);
					pLayertemp->setMaximumScale(unMaxS);
					pLayertemp->setMinimumScale(unMinS);
					//QgsMapLayerRegistry::instance()->addMapLayer(pLayer, true);
					m_pxAreaTransLayerMap.insert(strLayerKey, pLayertemp);
					pLayertemp = nullptr;
				}
				_pLayers.push_back(m_pxAreaTransLayerMap[strLayerKey]);
				_unSymNos.push_back(unSymNo);
				_unRenLev.push_back(unRenLev);
				_strLayerTypes.push_back("area");
			}
			else if (unLayerIdx == 3)
			{
				if (m_strAreaNormalLayerINSTs.contains(strDrawCMD))
					unSymNo = m_strAreaNormalLayerINSTs[strDrawCMD];
				else
				{
					unSymNo = ++m_unSymbolIndex;
					m_strAreaNormalLayerINSTs[strDrawCMD] = unSymNo;
				}

				if (!m_pxAreaNormalLayerMap.contains(strLayerKey))
				{
					// 创建图层
					QString strName = QString::fromLocal8Bit("AreaNormalLayer-%1").arg(strLayerKey);
					QgsPalLayerSettings xLaySettting;
					QStringList strFieldKeys = QStringList();
					QStringList strFieldTypes = QStringList();

					strFieldKeys << "id" << "key" << "symbolNo" << "docMark" << "attKeys" << "attVals";
					strFieldTypes << "integer" << "string(64)" << "integer" << "string(32)" << "string" << "string";
					xLaySettting.enabled = true;
					QgsVectorLayer* pLayertemp = createVectorLayer(strName, "polygon", strFieldKeys, strFieldTypes, QPainter::CompositionMode_SourceOver, &xLaySettting
						, nullptr);

					if (pLayertemp == nullptr)
					{
						return 11;
					}

					pLayertemp->setScaleBasedVisibility(true);
					pLayertemp->setMaximumScale(unMaxS);
					pLayertemp->setMinimumScale(unMinS);
					//QgsMapLayerRegistry::instance()->addMapLayer(pLayer, true);
					m_pxAreaNormalLayerMap.insert(strLayerKey, pLayertemp);
					pLayertemp = nullptr;
				}
				_pLayers.push_back(m_pxAreaNormalLayerMap[strLayerKey]);
				_unSymNos.push_back(unSymNo);
				_unRenLev.push_back(unRenLev);
				_strLayerTypes.push_back("area");
			}
			else if (unLayerIdx == 4)
			{
				if (m_strAreaCoverLayerINSTs.contains(strDrawCMD))
					unSymNo = m_strAreaCoverLayerINSTs[strDrawCMD];
				else
				{
					unSymNo = ++m_unSymbolIndex;
					m_strAreaCoverLayerINSTs[strDrawCMD] = unSymNo;
				}

				if (!m_pxAreaCoverLayerMap.contains(strLayerKey))
				{
					// 创建图层
					QString strName = QString::fromLocal8Bit("AreaCoverLayer-%1").arg(strLayerKey);
					QgsPalLayerSettings xLaySettting;
					QStringList strFieldKeys = QStringList();
					QStringList strFieldTypes = QStringList();

					strFieldKeys << "id" << "key" << "symbolNo" << "docMark" << "attKeys" << "attVals";
					strFieldTypes << "integer" << "string(64)" << "integer" << "string(32)" << "string" << "string";
					xLaySettting.enabled = true;
					QgsVectorLayer* pLayertemp = createVectorLayer(strName, "polygon", strFieldKeys, strFieldTypes, QPainter::CompositionMode_SourceOver, &xLaySettting
						, nullptr);

					if (pLayertemp == nullptr)
					{
						return 12;
					}

					pLayertemp->setScaleBasedVisibility(true);
					pLayertemp->setMaximumScale(unMaxS);
					pLayertemp->setMinimumScale(unMinS);
					//QgsMapLayerRegistry::instance()->addMapLayer(pLayer, true);
					m_pxAreaCoverLayerMap.insert(strLayerKey, pLayertemp);
					pLayertemp = nullptr;
				}
				_pLayers.push_back(m_pxAreaCoverLayerMap[strLayerKey]);
				_unSymNos.push_back(unSymNo);
				_unRenLev.push_back(unRenLev);
				_strLayerTypes.push_back("area");
			}
			else if (unLayerIdx == 5)
			{
				if (m_strCustomLayerINSTs.contains(strDrawCMD))
					unSymNo = m_strCustomLayerINSTs[strDrawCMD];
				else
				{
					unSymNo = ++m_unSymbolIndex;
					m_strCustomLayerINSTs[strDrawCMD] = unSymNo;
				}

				if (!m_pxCustomLayerMap.contains(strLayerKey))
				{
					// 创建图层
					QString strName = QString::fromLocal8Bit("CustomLayer-%1").arg(strLayerKey);
					QgsPalLayerSettings xLaySettting;
					QStringList strFieldKeys = QStringList();
					QStringList strFieldTypes = QStringList();

					strFieldKeys << "id" << "key" << "symbolNo" << "docMark" << "attKeys" << "attVals";
					strFieldTypes << "integer" << "string(64)" << "integer" << "string(32)" << "string" << "string";
					xLaySettting.enabled = true;
					QgsVectorLayer* pLayertemp = createVectorLayer(strName, "polygon", strFieldKeys, strFieldTypes, QPainter::CompositionMode_SourceOver, &xLaySettting
						, nullptr);

					if (pLayertemp == nullptr)
					{
						return 13;
					}

					pLayertemp->setScaleBasedVisibility(true);
					pLayertemp->setMaximumScale(unMaxS);
					pLayertemp->setMinimumScale(unMinS);
					//QgsMapLayerRegistry::instance()->addMapLayer(pLayer, true);
					m_pxCustomLayerMap.insert(strLayerKey, pLayertemp);
					pLayertemp = nullptr;
				}
				_pLayers.push_back(m_pxCustomLayerMap[strLayerKey]);
				_unSymNos.push_back(unSymNo);
				_unRenLev.push_back(unRenLev);
				_strLayerTypes.push_back("area");
			}

			nIdx++;
		}

		return 0;
	}

	// 私有槽函数
	// 添加海图数据结果
	void XS57LayerAdapter::OnProcNewDoc(const QString& _strFilePath, XS57DataDoc* _pDoc)
	{
		if (_strFilePath.isEmpty())
			return;
		if (m_pxS57DocMap.contains(_strFilePath) == false)
			return;
		if (_pDoc == nullptr)
			return;

		// 将数据显示到图层
		appendS57Doc(_pDoc);
	}


	qint32 XS57LayerAdapter::createSymbol(XFeature* _pFeature, xeShowLev& _xeShowLev)
	{
		qint32 nSymBol = -1;
		_pFeature->strSymbolInfos.clear();
		_pFeature->strDisplayLayers.clear();

		XSymbol symbolInfo;
		quint32 unScaleMax = XDEFAULT_VLAYER_SCALE_MAX;
		bool bIsCache = false;
		_pFeature->GetAttr(133, unScaleMax);
		if (_pFeature->unObjType == 42)
			unScaleMax = XDEFAULT_VLAYER_SCALE_MAX;
		xeShowLev xeCustomShowLev = xeALL;

		// Find Symbol
		quint32 error = 0;
		XLookupTAB* pLup = NULL;
		QList<XLookupTAB*> xLups;
		quint64 unKey = 0;
		unKey += quint64(_pFeature->unObjType)*quint64(100000000);
		unKey += _pFeature->eGeoType * 10000000 + m_pxS52Lib->m_xeSymbolStyle * 1000 + m_pxS52Lib->m_xeBoundaryStyle * 10 + m_xeColorStyle;
		if (m_xLookupTABs.contains(unKey) == false){
			if (m_xS57ClassCodeToStrs.contains(_pFeature->unObjType))
				xLups = m_pxS52Lib->GetLoopTAB(xePRIM(_pFeature->eGeoType), m_xS57ClassCodeToStrs[_pFeature->unObjType]);
			if (xLups.isEmpty())
				return -1;
			m_xLookupTABs.insert(unKey, xLups);
		}
		else
			xLups = m_xLookupTABs[unKey];

		if (xLups.isEmpty())
			return -1;

		XSymbolInfo xSymInfo;
		unKey = m_pxS52Lib->m_xeSymbolStyle * 1000 + m_pxS52Lib->m_xeBoundaryStyle * 10 + m_xeColorStyle;	// 符号样式*1000+边界样式*10+颜色类别
		if (m_xSymbolInfos.contains(unKey)){
			xSymInfo = m_xSymbolInfos[unKey];
		}

		bool bIsTrue = true;
		// 排序
		qSort(xLups.begin(), xLups.end(), CompareLoopTab);
		foreach(XLookupTAB* pTempLup, xLups){
			// 条件判断
			if (pTempLup->m_strATTC.count() > 0 && _pFeature->xAtts.count() > 0){
				// 条件判断
				QMap<QString, QString>::Iterator itr = pTempLup->m_strATTC.begin();
				while (itr != pTempLup->m_strATTC.end()){
					QString strKey = itr.key();
					QString strValue = itr.value();
					bool bIsDigit = false;
					quint32 unKey = 0;
					strKey.toInt(&bIsDigit);
					if (!bIsDigit){
						if (!m_xAttsStrToCodes.contains(strKey))
							return -1;
						unKey = m_xAttsStrToCodes[strKey];
					}
					else
						unKey = strKey.toUInt();

					if (strValue == "?"){
						// 判断该值是否存在
						if (_pFeature->xAtts.contains(unKey)){
							bIsTrue = false;
							break;
						}
					}
					else if (_pFeature->xAtts.contains(unKey))
					{
						if (_pFeature->xAtts[unKey] != strValue){
							bIsTrue = false;
							break;
						}
					}
					else
						bIsTrue = false;

					itr++;
				}
				if (!bIsTrue)
					continue;

				pLup = pTempLup;
				break;
			}
			else if (pTempLup->m_strATTC.count() == 0){
				pLup = pTempLup;
				break;
			}
		}
		if (!pLup)
			return nSymBol;

		{
			// Create Symbol
			if (!parseSymbolInfo(_pFeature, unScaleMax, pLup, symbolInfo, _xeShowLev, bIsCache))
				return nSymBol = -1;
			nSymBol = symbolInfo.unSymbolNo;
			if (_pFeature->strSymbolInfos.contains("SY(LNDARE01)"))
				unScaleMax = XLNDARE_POINT_SCALE_MAX;
		}

		if (pLup->m_xeDisCat == XLookupTAB::DISPLAYBASE || _xeShowLev == xeBase)
		{
			_xeShowLev = xeBase;
			if (m_xS57ClassCodeToStrs.contains(_pFeature->unObjType)){
				if (symbolInfo.pxACFillSymbol){
					if (_pFeature->unObjType == 302){
						if (!(isExistSymbol(xSymInfo.pxNoDataLayerRender, nSymBol)))
							xSymInfo.pxNoDataLayerRender->addCategory(QgsRendererCategoryV2(nSymBol, symbolInfo.pxACFillSymbol, m_xS57ClassCodeToStrs[_pFeature->unObjType]));
						if (!_pFeature->strDisplayLayers.contains("AC-NODATA"))
							_pFeature->strDisplayLayers.append("AC-NODATA");

						//m_pxNoDataLayer->setRendererV2(xSymInfo.pxNoDataLayerRender);
					}
					else{
						if (_pFeature->unObjType == 42 && !_pFeature->strSymbolInfos.contains("AC(DEPIT)")){
							if (!(isExistSymbol(xSymInfo.pxBaseDepAreRender, nSymBol)))
								xSymInfo.pxBaseDepAreRender->addCategory(QgsRendererCategoryV2(nSymBol, symbolInfo.pxACFillSymbol, m_xS57ClassCodeToStrs[_pFeature->unObjType]));
							if (!_pFeature->strDisplayLayers.contains("AC-DEPTH"))
								_pFeature->strDisplayLayers.append("AC-DEPTH");

							m_pxBaseAreDepLay->setRendererV2(xSymInfo.pxBaseDepAreRender);
						}
						else if (_pFeature->unObjType == 42 && _pFeature->strSymbolInfos.contains("AC(DEPIT)") || _pFeature->unObjType == 71){
							if (!(isExistSymbol(xSymInfo.pxBaseHAreRender, nSymBol)))
								xSymInfo.pxBaseHAreRender->addCategory(QgsRendererCategoryV2(nSymBol, symbolInfo.pxACFillSymbol, m_xS57ClassCodeToStrs[_pFeature->unObjType]));
							if (!_pFeature->strDisplayLayers.contains("AC-HEIGHT"))
								_pFeature->strDisplayLayers.append("AC-HEIGHT");
							m_pxBaseHFillAreLay->setRendererV2(xSymInfo.pxBaseHAreRender);
						}
						else if (symbolInfo.pxAPFillSymbol){
							if (!m_pxBaseACFillAreLays.contains(unScaleMax)){
								QString strName = QString::fromLocal8Bit("区域颜色填充图层(") + QString::number(unScaleMax) + QString(")");;
								QgsPalLayerSettings xLaySettting;
								QStringList strFieldKeys = QStringList();
								QStringList strFieldTypes = QStringList();

								strFieldKeys << "id" << "key" << "symbolNo" << "label";
								strFieldTypes << "integer" << "string(64)" << "integer" << "string(32)";
								xLaySettting.enabled = true;
								QgsVectorLayer* pLayer = createVectorLayer(strName, "polygon", strFieldKeys, strFieldTypes, QPainter::CompositionMode_SourceOver, &xLaySettting
									, NULL);

								if (pLayer == NULL)
									return -1;

								pLayer->setScaleBasedVisibility(true);
								pLayer->setMaximumScale(unScaleMax);
								pLayer->setMinimumScale(XDEFAULT_VLAYER_SCALE_MIN);
								m_pxBaseACFillAreLays.insert(unScaleMax, pLayer);
								pLayer = NULL;
							}
							if (!xSymInfo.pxBaseACFillRenders.contains(unScaleMax)){
								QgsCategorizedSymbolRendererV2* pCatV2 = new QgsCategorizedSymbolRendererV2("symbolNo");
								xSymInfo.pxBaseACFillRenders.insert(unScaleMax, pCatV2);
								pCatV2 = NULL;
							}
							if (!isExistSymbol(xSymInfo.pxBaseACFillRenders[unScaleMax], (quint32)nSymBol))
								xSymInfo.pxBaseACFillRenders[unScaleMax]->addCategory(QgsRendererCategoryV2(nSymBol, symbolInfo.pxACFillSymbol, m_xS57ClassCodeToStrs[_pFeature->unObjType]));
							if (!_pFeature->strDisplayLayers.contains("AC-FILL"))
								_pFeature->strDisplayLayers.append("AC-FILL");
							m_pxBaseACFillAreLays[unScaleMax]->setRendererV2(xSymInfo.pxBaseACFillRenders[unScaleMax]);
						}
					}
				}

				if (symbolInfo.pxAPFillSymbol){
					if (!m_pxBaseAPFillAreLays.contains(unScaleMax)){
						QString strName = QString::fromLocal8Bit("区域图形填充图层(") + QString::number(unScaleMax) + QString(")");;
						QgsPalLayerSettings xLaySettting;
						QStringList strFieldKeys = QStringList();
						QStringList strFieldTypes = QStringList();

						strFieldKeys << "id" << "key" << "symbolNo" << "label";
						strFieldTypes << "integer" << "string(64)" << "integer" << "string(32)";
						xLaySettting.enabled = true;
						QgsVectorLayer* pLayer = createVectorLayer(strName, "polygon", strFieldKeys, strFieldTypes, QPainter::CompositionMode_SourceOver, &xLaySettting
							, NULL);

						if (pLayer == NULL)
							return -1;

						pLayer->setScaleBasedVisibility(true);
						pLayer->setMaximumScale(unScaleMax);
						pLayer->setMinimumScale(XDEFAULT_VLAYER_SCALE_MIN);
						m_pxBaseAPFillAreLays.insert(unScaleMax, pLayer);
						pLayer = NULL;
					}
					if (!xSymInfo.pxBaseAPFillRenders.contains(unScaleMax)){
						QgsCategorizedSymbolRendererV2* pCatV2 = new QgsCategorizedSymbolRendererV2("symbolNo");
						xSymInfo.pxBaseAPFillRenders.insert(unScaleMax, pCatV2);
						pCatV2 = NULL;
					}
					if (!(isExistSymbol(xSymInfo.pxBaseAPFillRenders[unScaleMax], nSymBol)))
						xSymInfo.pxBaseAPFillRenders[unScaleMax]->addCategory(QgsRendererCategoryV2(nSymBol, symbolInfo.pxAPFillSymbol, m_xS57ClassCodeToStrs[_pFeature->unObjType]));
					if (!_pFeature->strDisplayLayers.contains("AP-FILL"))
						_pFeature->strDisplayLayers.append("AP-FILL");

					m_pxBaseAPFillAreLays[unScaleMax]->setRendererV2(xSymInfo.pxBaseAPFillRenders[unScaleMax]);
				}

				if (symbolInfo.pxLSLineSymbol && symbolInfo.pxAPFillSymbol){
					if (!m_pxBaseACLineAreLays.contains(unScaleMax)){
						QString strName = QString::fromLocal8Bit("区域颜色线形图层(") + QString::number(unScaleMax) + QString(")");;
						QgsPalLayerSettings xLaySettting;
						QStringList strFieldKeys = QStringList();
						QStringList strFieldTypes = QStringList();

						strFieldKeys << "id" << "key" << "symbolNo" << "label";
						strFieldTypes << "integer" << "string(64)" << "integer" << "string(32)";
						xLaySettting.enabled = true;
						QgsVectorLayer* pLayer = createVectorLayer(strName, "linestring", strFieldKeys, strFieldTypes, QPainter::CompositionMode_SourceOver, &xLaySettting
							, NULL);

						if (pLayer == NULL)
							return -1;

						pLayer->setScaleBasedVisibility(true);
						pLayer->setMaximumScale(unScaleMax);
						pLayer->setMinimumScale(XDEFAULT_VLAYER_SCALE_MIN);
						m_pxBaseACLineAreLays.insert(unScaleMax, pLayer);
						pLayer = NULL;
					}
					if (!xSymInfo.pxBaseACLineRenders.contains(unScaleMax)){
						QgsCategorizedSymbolRendererV2* pCatV2 = new QgsCategorizedSymbolRendererV2("symbolNo");
						xSymInfo.pxBaseACLineRenders.insert(unScaleMax, pCatV2);
						pCatV2 = NULL;
					}
					if (!(isExistSymbol(xSymInfo.pxBaseACLineRenders[unScaleMax], nSymBol)))
						xSymInfo.pxBaseACLineRenders[unScaleMax]->addCategory(QgsRendererCategoryV2(nSymBol, symbolInfo.pxLSLineSymbol, m_xS57ClassCodeToStrs[_pFeature->unObjType]));
					if (!_pFeature->strDisplayLayers.contains("AC-LINE"))
						_pFeature->strDisplayLayers.append("AC-LINE");

					m_pxBaseACLineAreLays[unScaleMax]->setRendererV2(xSymInfo.pxBaseACLineRenders[unScaleMax]);
				}
				else if (symbolInfo.pxLSLineSymbol && !symbolInfo.pxAPFillSymbol){
					if (_pFeature->unObjType == 302){
						if (!(isExistSymbol(xSymInfo.pxTopLinesRender, nSymBol)))
							xSymInfo.pxTopLinesRender->addCategory(QgsRendererCategoryV2(nSymBol, symbolInfo.pxLSLineSymbol, m_xS57ClassCodeToStrs[_pFeature->unObjType]));
						if (!_pFeature->strDisplayLayers.contains("AC-TOP_LINE"))
							_pFeature->strDisplayLayers.append("AC-TOP_LINE");
						m_pxTopLineLayer->setRendererV2(xSymInfo.pxTopLinesRender);
					}
					else{
						if (!m_pxBaseACHLineLays.contains(unScaleMax)){
							QString strName = QString::fromLocal8Bit("高层颜色线形图层(") + QString::number(unScaleMax) + QString(")");;
							QgsPalLayerSettings xLaySettting;
							QStringList strFieldKeys = QStringList();
							QStringList strFieldTypes = QStringList();

							strFieldKeys << "id" << "key" << "symbolNo" << "label";
							strFieldTypes << "integer" << "string(64)" << "integer" << "string(32)";
							xLaySettting.enabled = true;
							QgsVectorLayer* pLayer = createVectorLayer(strName, "linestring", strFieldKeys, strFieldTypes, QPainter::CompositionMode_SourceOver, &xLaySettting
								, NULL);

							if (pLayer == NULL)
								return -1;

							pLayer->setScaleBasedVisibility(true);
							pLayer->setMaximumScale(unScaleMax);
							pLayer->setMinimumScale(XDEFAULT_VLAYER_SCALE_MIN);
							m_pxBaseACHLineLays.insert(unScaleMax, pLayer);
							pLayer = NULL;
						}
						if (!xSymInfo.pxBaseACHLineRenders.contains(unScaleMax)){
							QgsCategorizedSymbolRendererV2* pCatV2 = new QgsCategorizedSymbolRendererV2("symbolNo");
							xSymInfo.pxBaseACHLineRenders.insert(unScaleMax, pCatV2);
							pCatV2 = NULL;
						}
						if (!(isExistSymbol(xSymInfo.pxBaseACHLineRenders[unScaleMax], nSymBol)))
							xSymInfo.pxBaseACHLineRenders[unScaleMax]->addCategory(QgsRendererCategoryV2(nSymBol, symbolInfo.pxLSLineSymbol, m_xS57ClassCodeToStrs[_pFeature->unObjType]));
						if (!_pFeature->strDisplayLayers.contains("ACH-LINE"))
							_pFeature->strDisplayLayers.append("ACH-LINE");

						m_pxBaseACHLineLays[unScaleMax]->setRendererV2(xSymInfo.pxBaseACHLineRenders[unScaleMax]);
					}
				}

				if (symbolInfo.pxLCLineSymbol && symbolInfo.pxAPFillSymbol){
					if (!m_pxBaseACLineAreLays.contains(unScaleMax)){
						QString strName = QString::fromLocal8Bit("区域颜色线形图层(") + QString::number(unScaleMax) + QString(")");;
						QgsPalLayerSettings xLaySettting;
						QStringList strFieldKeys = QStringList();
						QStringList strFieldTypes = QStringList();

						strFieldKeys << "id" << "key" << "symbolNo" << "label";
						strFieldTypes << "integer" << "string(64)" << "integer" << "string(32)";
						xLaySettting.enabled = true;
						QgsVectorLayer* pLayer = createVectorLayer(strName, "linestring", strFieldKeys, strFieldTypes, QPainter::CompositionMode_SourceOver, &xLaySettting
							, NULL);

						if (pLayer == NULL)
							return -1;

						pLayer->setScaleBasedVisibility(true);
						pLayer->setMaximumScale(unScaleMax);
						pLayer->setMinimumScale(XDEFAULT_VLAYER_SCALE_MIN);
						m_pxBaseACLineAreLays.insert(unScaleMax, pLayer);
						pLayer = NULL;
					}
					if (!xSymInfo.pxBaseACLineRenders.contains(unScaleMax)){
						QgsCategorizedSymbolRendererV2* pCatV2 = new QgsCategorizedSymbolRendererV2("symbolNo");
						xSymInfo.pxBaseACLineRenders.insert(unScaleMax, pCatV2);
						pCatV2 = NULL;
					}
					if (!(isExistSymbol(xSymInfo.pxBaseACLineRenders[unScaleMax], nSymBol)))
						xSymInfo.pxBaseACLineRenders[unScaleMax]->addCategory(QgsRendererCategoryV2(nSymBol, symbolInfo.pxLCLineSymbol, m_xS57ClassCodeToStrs[_pFeature->unObjType]));
					if (!_pFeature->strDisplayLayers.contains("AC-LINE"))
						_pFeature->strDisplayLayers.append("AC-LINE");

					m_pxBaseACLineAreLays[unScaleMax]->setRendererV2(xSymInfo.pxBaseACLineRenders[unScaleMax]);
				}
				else if (symbolInfo.pxLCLineSymbol && !symbolInfo.pxAPFillSymbol){
					if (!m_pxBaseACHLineLays.contains(unScaleMax)){
						QString strName = QString::fromLocal8Bit("高层颜色线形图层(") + QString::number(unScaleMax) + QString(")");;
						QgsPalLayerSettings xLaySettting;
						QStringList strFieldKeys = QStringList();
						QStringList strFieldTypes = QStringList();

						strFieldKeys << "id" << "key" << "symbolNo" << "label";
						strFieldTypes << "integer" << "string(64)" << "integer" << "string(32)";
						xLaySettting.enabled = true;
						QgsVectorLayer* pLayer = createVectorLayer(strName, "linestring", strFieldKeys, strFieldTypes, QPainter::CompositionMode_SourceOver, &xLaySettting
							, NULL);

						if (pLayer == NULL)
							return -1;

						pLayer->setScaleBasedVisibility(true);
						pLayer->setMaximumScale(unScaleMax);
						pLayer->setMinimumScale(XDEFAULT_VLAYER_SCALE_MIN);
						m_pxBaseACHLineLays.insert(unScaleMax, pLayer);
						pLayer = NULL;
					}
					if (!xSymInfo.pxBaseACHLineRenders.contains(unScaleMax)){
						QgsCategorizedSymbolRendererV2* pCatV2 = new QgsCategorizedSymbolRendererV2("symbolNo");
						xSymInfo.pxBaseACHLineRenders.insert(unScaleMax, pCatV2);
						pCatV2 = NULL;
					}
					if (!(isExistSymbol(xSymInfo.pxBaseACHLineRenders[unScaleMax], nSymBol)))
						xSymInfo.pxBaseACHLineRenders[unScaleMax]->addCategory(QgsRendererCategoryV2(nSymBol, symbolInfo.pxLCLineSymbol, m_xS57ClassCodeToStrs[_pFeature->unObjType]));
					if (!_pFeature->strDisplayLayers.contains("ACH-LINE"))
						_pFeature->strDisplayLayers.append("ACH-LINE");

					m_pxBaseACHLineLays[unScaleMax]->setRendererV2(xSymInfo.pxBaseACHLineRenders[unScaleMax]);
				}

				if (symbolInfo.pxSYPointSymbol){
					if (!m_pxBasePointSymbolLays.contains(unScaleMax)){
						QString strName = QString::fromLocal8Bit("基础点符号图层(") + QString::number(unScaleMax) + QString(")");;
						QgsPalLayerSettings xLaySettting;
						QStringList strFieldKeys = QStringList();
						QStringList strFieldTypes = QStringList();

						strFieldKeys << "id" << "key" << "symbolNo" << "label";
						strFieldTypes << "integer" << "string(64)" << "integer" << "string(32)";
						xLaySettting.enabled = true;
						QgsVectorLayer* pLayer = createVectorLayer(strName, "point", strFieldKeys, strFieldTypes, QPainter::CompositionMode_SourceOver, &xLaySettting
							, NULL);

						if (pLayer == NULL)
							return -1;

						pLayer->setScaleBasedVisibility(true);
						pLayer->setMaximumScale(unScaleMax);
						pLayer->setMinimumScale(XDEFAULT_VLAYER_SCALE_MIN);
						m_pxBasePointSymbolLays.insert(unScaleMax, pLayer);
						pLayer = NULL;
					}
					if (!xSymInfo.pxBaseSymbolRenders.contains(unScaleMax)){
						QgsCategorizedSymbolRendererV2* pCatV2 = new QgsCategorizedSymbolRendererV2("symbolNo");
						xSymInfo.pxBaseSymbolRenders.insert(unScaleMax, pCatV2);
						pCatV2 = NULL;
					}
					if (!(isExistSymbol(xSymInfo.pxBaseSymbolRenders[unScaleMax], nSymBol)))
						xSymInfo.pxBaseSymbolRenders[unScaleMax]->addCategory(QgsRendererCategoryV2(nSymBol, symbolInfo.pxSYPointSymbol, m_xS57ClassCodeToStrs[_pFeature->unObjType]));
					if (!_pFeature->strDisplayLayers.contains("SY-POINT"))
						_pFeature->strDisplayLayers.append("SY-POINT");

					m_pxBasePointSymbolLays[unScaleMax]->setRendererV2(xSymInfo.pxBaseSymbolRenders[unScaleMax]);
				}
			}
		}

		m_xSymbolInfos[unKey] = xSymInfo;
		return symbolInfo.unSymbolNo;
	}
	quint32 XS57LayerAdapter::InitAdapter()
	{
		if (m_bIsInit)
			return 1;	// 重复初始化

		// 控制参数
		{
			m_bUseTwoShades = false;
			m_dSafetyContour = 8.0;
			m_dShallowContour = 3.0;
			m_dDeepContour = 10.0;
			m_xeShowLev = xeBase;
			m_dSafetyDepth = 5.0;
			m_xeDepthUnitDisplay = xeMeter;
			m_strCurLoadPaths.clear();
			m_unSymbolIndex = 0;
		}

		// 初始化S57 Class 
		{
			// 获取像素DPI
			{
				bool bRes = GetWindowsMonitorSize(m_xRealScreenSize);
				if (bRes == false)
					return -1;
				m_xScreenSize = QApplication::desktop()->screen()->size();
				m_dXPixPerMM = m_xScreenSize.width() / m_xRealScreenSize.width();
				m_dYPixPerMM = m_xScreenSize.height() / m_xRealScreenSize.height();
				m_dCanvasScaleFactor = m_xScreenSize.width() / (m_xRealScreenSize.width() / 1000.0);
				m_dTideRectangleScale = 100;
				m_dCurrentArrowScale = 100;
				m_dTideDrawScaler = m_dXPixPerMM*0.254*m_dTideRectangleScale / 100.0;
				m_dCurrentDrawScaler = m_dXPixPerMM * 0.254 * m_dCurrentArrowScale / 100.0;
			}

			// S57 Class
			{
				QFile xFile("./Data/S57/s57objectclasses.csv");
				if (!xFile.open(QIODevice::ReadOnly))
					return -1;

				QString strLine = QString();
				QTextStream stream(&xFile);
				strLine = stream.readLine();
				if (!strLine.contains("20160120")){
					// 识别码错误 用以简单校验文件内容
					xFile.close();
					return -1;
				}
				strLine = stream.readLine();

				QString strTemp = QString();
				while (!stream.atEnd())
				{
					QStringList strOneRec = stream.readLine().split(",");
					if (strOneRec.count() == 8){
						XS57Class xNewClass;
						xNewClass.unCode = strOneRec.at(0).toUInt();
						xNewClass.strName = strOneRec.at(1);
						xNewClass.strAcronym = strOneRec.at(2);
						xNewClass.strAttsA = strOneRec.at(3).split(";");
						xNewClass.strAttsB = strOneRec.at(4).split(";");
						xNewClass.strAttsC = strOneRec.at(5).split(";");
						xNewClass.strPRIMTypes = strOneRec.at(7).split(";");
						if (strOneRec.at(6).count() == 1)
							xNewClass.ucClassTypes = strOneRec.at(6).at(0).cell();
						m_xS57ClassVec.insert(xNewClass.unCode, xNewClass);
						m_xS57ClassCodeToStrs.insert(xNewClass.unCode, xNewClass.strAcronym);
						m_xS57ClassStrToCodes.insert(xNewClass.strAcronym, xNewClass.unCode);
					}
					else
						int md = 0;
				}
				xFile.close();
			}

			// S57 Atts
			{
				QFile xFile("./Data/S57/s57attributes.csv");
				if (!xFile.open(QIODevice::ReadOnly))
					return -1;

				QString strLine = QString();
				QTextStream stream(&xFile);
				strLine = stream.readLine();
				if (!strLine.contains("20160120")){
					// 识别码错误 用以简单校验文件内容
					xFile.close();
					return -1;
				}
				strLine = stream.readLine();

				QString strTemp = QString();
				while (!stream.atEnd())
				{
					QStringList strOneRec = stream.readLine().split(",");
					if (strOneRec.count() == 5){
						XS57Atts xNewAtts;
						xNewAtts.unCode = strOneRec.at(0).toUInt();
						xNewAtts.strName = strOneRec.at(1);
						xNewAtts.strAcronym = strOneRec.at(2);
						if (strOneRec.at(3).count() == 1)
							xNewAtts.ucAttsType = strOneRec.at(3).at(0).cell();
						if (strOneRec.at(4).count() == 1)
							xNewAtts.ucClassType = strOneRec.at(4).at(0).cell();

						m_xS57AttsVec.insert(xNewAtts.unCode, xNewAtts);
						m_xAttsCodeToStrs.insert(xNewAtts.unCode, xNewAtts.strAcronym);
						m_xAttsStrToCodes.insert(xNewAtts.strAcronym, xNewAtts.unCode);
					}
					else
						int md = 0;
				}
				xFile.close();
			}

			// Atts期望值
			{
				QFile xFile("./Data/S57/s57expectedinput.csv");
				if (!xFile.open(QIODevice::ReadOnly))
					return -1;

				QString strLine = QString();
				QTextStream stream(&xFile);
				strLine = stream.readLine();
				if (!strLine.contains("20160120")){
					// 识别码错误 用以简单校验文件内容
					xFile.close();
					return -1;
				}
				strLine = stream.readLine();

				QString strTemp = QString();
				while (!stream.atEnd())
				{
					QStringList strOneRec = stream.readLine().split(",");
					if (strOneRec.count() == 3){
						quint32 unCode = strOneRec.at(0).toUInt();
						qint32  nID = strOneRec.at(1).toInt();
						QString strName = strOneRec.at(2);
						if (m_xS57AttsVec.contains(unCode))
							m_xS57AttsVec[unCode].strExpectedVals.insert(nID, strName);
					}
					else
						int md = 0;
				}
				xFile.close();
			}
		}

		// 初始化色表
		quint32 error = 0;
		{
			m_strColorStyleNames.insert(xeDayBright, "DAY_BRIGHT");
			m_strColorStyleNames.insert(xeDayBlack, "DAY_BLACKBACK");
			m_strColorStyleNames.insert(xeDayWhite, "DAY_WHITEBACK");
			m_strColorStyleNames.insert(xeDusk, "DUSK");
			m_strColorStyleNames.insert(xeNight, "NIGHT");

			m_pxS52Lib = XS52Lib::fromFile("./Data/S52/S52RAZDS.RLE");
			if (!m_pxS52Lib)
				return -1;
		}

		// 002-背景图层-SHP文件图层
		{
			QFileInfo xFileInfo("./Data/Shp/Countries.shp");
			QString strVectorDir = xFileInfo.filePath();
			QString vectorbasename = xFileInfo.completeBaseName();
			QString strProviderName = "ogr";

			QgsVectorLayer* pNewLayer = new QgsVectorLayer(strVectorDir, QString::fromLocal8Bit("世界基础图层"), strProviderName, false);
			pNewLayer->setProviderEncoding("System");
			if (pNewLayer == NULL)
				return 2;
			if (!pNewLayer->isValid())
				return 3;

			QgsMapLayerRegistry::instance()->addMapLayer(pNewLayer, true);
			m_pxShowLayerSet.push_back(pNewLayer);
			//m_pxWorldBaseLay = pNewLayer;
		}

		// 001-无数据填充图层
		{
			QString strName = QString::fromLocal8Bit("无数据填充图层");
			QgsPalLayerSettings xLaySettting;
			QStringList strFieldKeys = QStringList();
			QStringList strFieldTypes = QStringList();

			strFieldKeys << "id" << "key" << "symbolNo" << "label";
			strFieldTypes << "integer" << "string(64)" << "integer" << "string(32)";
			xLaySettting.enabled = true;
			QgsVectorLayer* pLayer = createVectorLayer(strName, "polygon", strFieldKeys, strFieldTypes, QPainter::CompositionMode_SourceOver, &xLaySettting
				, NULL);

			if (pLayer == NULL)
				return -1;

			pLayer->setScaleBasedVisibility(true);
			pLayer->setMaximumScale(XDEFAULT_VLAYER_SCALE_MAX);
			pLayer->setMinimumScale(XDEFAULT_VLAYER_SCALE_MIN);
			//QgsMapLayerRegistry::instance()->addMapLayer(pLayer, true);
			//m_pxNoDataLayer = pLayer;
			pLayer = NULL;
		}

		// 003-基础深度区域图层
		{
			QString strName = QString::fromLocal8Bit("深度区域图层");
			QgsPalLayerSettings xLaySettting;
			QStringList strFieldKeys = QStringList();
			QStringList strFieldTypes = QStringList();

			strFieldKeys << "id" << "key" << "symbolNo" << "label";
			strFieldTypes << "integer" << "string(64)" << "integer" << "string(32)";
			xLaySettting.enabled = true;
			QgsVectorLayer* pLayer = createVectorLayer(strName, "polygon", strFieldKeys, strFieldTypes, QPainter::CompositionMode_SourceOver, &xLaySettting
				, NULL);

			if (pLayer == NULL)
				return -1;

			pLayer->setScaleBasedVisibility(true);
			pLayer->setMaximumScale(XDEFAULT_VLAYER_SCALE_MAX);
			pLayer->setMinimumScale(XDEFAULT_VLAYER_SCALE_MIN);
			//QgsMapLayerRegistry::instance()->addMapLayer(pLayer, true);
			m_pxBaseAreDepLay = pLayer;
			pLayer = NULL;
		}
		// 005-高层区域图层
		{
			QString strName = QString::fromLocal8Bit("高层区域图层");
			QgsPalLayerSettings xLaySettting;
			QStringList strFieldKeys = QStringList();
			QStringList strFieldTypes = QStringList();

			strFieldKeys << "id" << "key" << "symbolNo" << "label";
			strFieldTypes << "integer" << "string(64)" << "integer" << "string(32)";
			xLaySettting.enabled = true;
// 			xLaySettting.fieldName = "label";
// 			xLaySettting.textFont = QFont(QString::fromLocal8Bit("黑体"), 11);
			QgsVectorLayer* pLayer = createVectorLayer(strName, "polygon", strFieldKeys, strFieldTypes, QPainter::CompositionMode_SourceOver, &xLaySettting
				, NULL);

			if (pLayer == NULL)
				return -1;

			pLayer->setScaleBasedVisibility(true);
			pLayer->setMaximumScale(XDEFAULT_VLAYER_SCALE_MAX);
			pLayer->setMinimumScale(XDEFAULT_VLAYER_SCALE_MIN);
			//QgsMapLayerRegistry::instance()->addMapLayer(pLayer, true);
			m_pxBaseHFillAreLay = pLayer;
			pLayer = NULL;
		}
		// 009-顶层线图层
		{
			QString strName = QString::fromLocal8Bit("顶层颜色线形图层");
			QgsPalLayerSettings xLaySettting;
			QStringList strFieldKeys = QStringList();
			QStringList strFieldTypes = QStringList();

			strFieldKeys << "id" << "key" << "symbolNo" << "label";
			strFieldTypes << "integer" << "string(64)" << "integer" << "string(32)";
			xLaySettting.enabled = true;
			QgsVectorLayer* pLayer = createVectorLayer(strName, "linestring", strFieldKeys, strFieldTypes, QPainter::CompositionMode_SourceOver, &xLaySettting
				, NULL);

			if (pLayer == NULL)
				return -1;

			pLayer->setScaleBasedVisibility(true);
			pLayer->setMaximumScale(XDEFAULT_VLAYER_SCALE_MAX);
			pLayer->setMinimumScale(XDEFAULT_VLAYER_SCALE_MIN);
			//QgsMapLayerRegistry::instance()->addMapLayer(pLayer, true);
			m_pxTopLineLayer = pLayer;
			pLayer = NULL;
		}

		SetColorStyle(XS57LayerAdapter::xeDayBright);

		{
			XSymbolInfo xSymInfo;
			quint32 unKey = m_pxS52Lib->m_xeSymbolStyle * 1000 + m_pxS52Lib->m_xeBoundaryStyle * 10 + m_xeColorStyle;	// 符号样式*1000+边界样式*10+颜色类别
			if (m_xSymbolInfos.contains(unKey)){
				xSymInfo = m_xSymbolInfos[unKey];
			}
			else{
				// 图层渲染器
				{
					xSymInfo = XSymbolInfo();
					QColor xColor = QColor();
					quint32 error = m_pxS52Lib->GetColorByKey("BASEF", xColor);
					if (error != 0)
						return error;
					QgsSimpleFillSymbolLayerV2* pxSymLay = new QgsSimpleFillSymbolLayerV2();
					pxSymLay->setFillColor(xColor);
					pxSymLay->setOutlineColor(xColor);
					QgsSymbolLayerV2List symbolList;
					symbolList.push_front(pxSymLay);
					xSymInfo.pxWorldLayerRender = new QgsSingleSymbolRendererV2(new QgsFillSymbolV2(symbolList));
					//m_pxWorldBaseLay->setRendererV2(xSymInfo.pxWorldLayerRender);
					pxSymLay = NULL;
					symbolList.clear();
				}
				m_xSymbolInfos.insert(unKey, xSymInfo);
			}
		}
		

		// Test 生成图像文件
		{
			updateSymbolsFile();
		}

		//m_pxMapCanvas = _pTouchCanvas;
		//SetShowLev(xeShowLev::xeBase);
		//m_pxMapCanvas->zoomToFullExtent();

		// 链接信号槽
// 		{
// 			QObject::connect(XS57Document::GetInstance(), SIGNAL(slgLoadNewFile(XS57DataDoc*)), &ms_xInstance, SLOT(OnLoadNewFile(XS57DataDoc*)));
// 		}
		
		m_bIsInit = true;
		return 0;
	}

	quint32 XS57LayerAdapter::CreateS57LayerByFiles(const QStringList& _strS57FilePaths, QgsMapCanvas* _pTouchCanvas)
	{
		if (_pTouchCanvas == NULL)
			return -1;

		m_bIsBusy = true;
		m_pxMapCanvas = _pTouchCanvas;
		SetShowLev(m_xeShowLev);
		m_pxMapCanvas->zoomToFullExtent();

//		XS57Document::GetInstance()->LoadFiles(_strS57FilePaths);

		return 0;
	}

	quint32 XS57LayerAdapter::addAreaFeatures(const XS57DataDoc* _pDoc, const QList<XFeature*>& _pAreaFeatures, const bool _bIsDrawInside/* = true*/)
	{
		if (_pAreaFeatures.count() <= 0)
			return 0;

		QList<QgsFeature> xBaseDepAreaFeatures;
		QList<QgsFeature> xBaseHAreaFeatures;
		QMap<quint32,QList<QgsFeature>> xBaseACFillFeatures;
		QMap<quint32, QList<QgsFeature>> xBaseAPFillFeatures;
		QMap<quint32, QList<QgsFeature>> xBaseACLineFeatures;
		QMap<quint32, QList<QgsFeature>> xBaseACHLineFeatures;
		QMap<quint32, QList<QgsFeature>> xBasePointSymFeatures;
		QList<QgsFeature> xNodataFeatures;
		QList<QgsFeature> xTopLineFeatures;

		QList<QgsFeature> xStandAreaFeatures;
		QList<QgsFeature> xStandLinesFeatures;
		QList<QgsFeature> xStandPointFeatures;
		quint32 error = 0;
		foreach(XFeature* pAF, _pAreaFeatures)
		{
			if (!m_bAddDataMark)
				break;

			quint32 unScaleMax = XDEFAULT_VLAYER_SCALE_MAX;
			pAF->GetAttr(133, unScaleMax);
			if (pAF->unObjType != 86 && pAF->unObjType != 42 && pAF->unObjType != 71 && pAF->unObjType != 96
				&& pAF->unObjType != 302)
				continue;

			if (pAF->unObjType == 302 && pAF->xAtts.contains(18) && pAF->xAtts[18].toUInt() != 1)
				continue;

			if (pAF->eGeoType != XFeature::xeArea)
				return -1;

			if (pAF->x2DPointList.count() < 2)
				return -1;

			quint32 unObjType = pAF->unObjType;
			xeShowLev xeLev = xeBase;
			quint32 unSymNo = 0;

			qint32 nTempNo = createSymbol(pAF,xeLev);
			if (nTempNo < 0)
				return -1;
			unSymNo = nTempNo;

			QgsPolyline xPolyLine;
			foreach(QVector2D* px2D, pAF->x2DPointList)
				xPolyLine.push_back(px2D->toPointF());
			QgsPolygon polygon;
			polygon.append(xPolyLine);
			QgsGeometry* pGeom = QgsGeometry::fromPolygon(polygon);

			// 挖洞-暂时只有陆地执行挖洞-用来显示被陆地包围的深度区域-全部执行挖洞将会很卡（待以后优化）
			if (pAF->unObjType == 71)
			{
				QList<QgsGeometry*> xTempGems;

				QList<QgsPolygon> xPs;
				foreach(XFeature* pInsideFA, pAF->xInsideAreas){
					QgsPolyline xPolyLineTemp;
					foreach(QVector2D* px2D, pInsideFA->x2DPointList)
						xPolyLineTemp.push_back(px2D->toPointF());
					QgsPolygon polygonTemp;
					polygonTemp.append(xPolyLineTemp);
					QgsGeometry* pTemp = QgsGeometry::fromPolygon(polygonTemp);
					pGeom = pGeom->difference(pTemp);
				}
			}
			
			if (xeLev == xeBase){
				if (pAF->strDisplayLayers.contains("AC-NODATA")){
					QString strLabelName = QString();
					QgsFields xFields;// = m_pxNoDataLayer->fields();
					QgsFeature newPolygon(xFields, pAF->unIdentifyID);
					newPolygon.setAttribute("id", pAF->unIdentifyID);
					newPolygon.setAttribute("key", pAF->strKey);
					newPolygon.setAttribute("symbolNo", unSymNo);
					newPolygon.setAttribute("label", QString::number(pAF->unIdentifyID));
					newPolygon.setGeometry(pGeom);
					xNodataFeatures.push_back(newPolygon);
				}
				if (pAF->strDisplayLayers.contains("AC-TOP_LINE")){
					QString strLabelName = QString();
					QgsFields xFields = m_pxTopLineLayer->fields();
					QgsFeature newPolygon(xFields, pAF->unIdentifyID);
					newPolygon.setAttribute("id", pAF->unIdentifyID);
					newPolygon.setAttribute("key", pAF->strKey);
					newPolygon.setAttribute("symbolNo", unSymNo);
					newPolygon.setAttribute("label", QString::number(pAF->unIdentifyID));
					newPolygon.setGeometry(QgsGeometry::fromPolyline(xPolyLine));
					xTopLineFeatures.append(newPolygon);
				}
				if (pAF->strDisplayLayers.contains("AC-DEPTH")){
					QString strLabelName = QString();
					QgsFields xFields = m_pxBaseAreDepLay->fields();
					QgsFeature newPolygon(xFields, pAF->unIdentifyID);
					newPolygon.setAttribute("id", pAF->unIdentifyID);
					newPolygon.setAttribute("key", pAF->strKey);
					newPolygon.setAttribute("symbolNo", unSymNo);
					newPolygon.setAttribute("label", QString::number(pAF->unIdentifyID));
					newPolygon.setGeometry(pGeom);
					xBaseDepAreaFeatures.push_back(newPolygon);
				}
				if (pAF->strDisplayLayers.contains("AC-HEIGHT")){
					QString strLabelName = QString();
					pAF->GetAttr(116, strLabelName);
					QgsFields xFields = m_pxBaseHFillAreLay->fields();
					QgsFeature newPolygon(xFields, pAF->unIdentifyID);
					newPolygon.setAttribute("id", pAF->unIdentifyID);
					newPolygon.setAttribute("key", pAF->strKey);
					newPolygon.setAttribute("symbolNo", unSymNo);
					newPolygon.setAttribute("label", strLabelName);
					newPolygon.setGeometry(pGeom);
					xBaseHAreaFeatures.push_back(newPolygon);

					addText(pAF->unIdentifyID, strLabelName, XTEXT_LAND_NAME_SCALE_MAX, pGeom->boundingBox().center().toQPointF());
				}
				if (pAF->strDisplayLayers.contains("AC-FILL")){
					QgsPolygon polygon;
					polygon.append(xPolyLine);
					QString strLabelName = QString();
					QgsFields xFields = m_pxBaseACFillAreLays[unScaleMax]->fields();
					QgsFeature newPolygon(xFields, pAF->unIdentifyID);
					newPolygon.setAttribute("id", pAF->unIdentifyID);
					newPolygon.setAttribute("key", pAF->strKey);
					newPolygon.setAttribute("symbolNo", unSymNo);
					newPolygon.setAttribute("label", QString::number(pAF->unIdentifyID));
					newPolygon.setGeometry(QgsGeometry::fromPolygon(polygon));
					xBaseACFillFeatures[unScaleMax].append(newPolygon);
				}
				if (pAF->strDisplayLayers.contains("AP-FILL")){
					QgsPolygon polygon;
					polygon.append(xPolyLine);
					QString strLabelName = QString();
					QgsFields xFields = m_pxBaseAPFillAreLays[unScaleMax]->fields();
					QgsFeature newPolygon(xFields, pAF->unIdentifyID);
					newPolygon.setAttribute("id", pAF->unIdentifyID);
					newPolygon.setAttribute("key", pAF->strKey);
					newPolygon.setAttribute("symbolNo", unSymNo);
					newPolygon.setAttribute("label", QString::number(pAF->unIdentifyID));
					newPolygon.setGeometry(QgsGeometry::fromPolygon(polygon));
					xBaseAPFillFeatures[unScaleMax].append(newPolygon);
				}
				if (pAF->strDisplayLayers.contains("AC-LINE")){
					QString strLabelName = QString();
					QgsFields xFields = m_pxBaseACFillAreLays[unScaleMax]->fields();
					QgsFeature newPolygon(xFields, pAF->unIdentifyID);
					newPolygon.setAttribute("id", pAF->unIdentifyID);
					newPolygon.setAttribute("key", pAF->strKey);
					newPolygon.setAttribute("symbolNo", unSymNo);
					newPolygon.setAttribute("label", QString::number(pAF->unIdentifyID));
					newPolygon.setGeometry(QgsGeometry::fromPolyline(xPolyLine));
					xBaseACLineFeatures[unScaleMax].append(newPolygon);
				}
				if (pAF->strDisplayLayers.contains("ACH-LINE")){
					QString strLabelName = QString();
					QgsFields xFields = m_pxBaseACHLineLays[unScaleMax]->fields();
					QgsFeature newLine(xFields, pAF->unIdentifyID);
					newLine.setAttribute("id", pAF->unIdentifyID);
					newLine.setAttribute("key", pAF->strKey);
					newLine.setAttribute("symbolNo", unSymNo);
					newLine.setAttribute("label", QString::number(pAF->unIdentifyID));
					newLine.setGeometry(QgsGeometry::fromPolyline(xPolyLine));
					xBaseACHLineFeatures[unScaleMax].append(newLine);
				}
 				if (pAF->strDisplayLayers.contains("SY-POINT")){
 					QgsGeometry* xGeom = QgsGeometry::fromPolyline(xPolyLine);
 					QPointF xCenPoint = xGeom->boundingBox().center().toQPointF();
 					QString strLabelName = QString();
 					QgsFields xFields = m_pxBasePointSymbolLays[unScaleMax]->fields();
 					QgsFeature newPoint(xFields, pAF->unIdentifyID);
 					newPoint.setAttribute("id", pAF->unIdentifyID);
 					newPoint.setAttribute("key", pAF->strKey);
 					newPoint.setAttribute("symbolNo", unSymNo);
 					newPoint.setGeometry(QgsGeometry::fromPoint(xCenPoint));
					xBasePointSymFeatures[unScaleMax].append(newPoint);
 				}
			}
		}

		if (m_xeShowLev >= xeBase){
			if (!xNodataFeatures.isEmpty()){
// 				m_pxNoDataLayer->startEditing();
// 				foreach(QgsFeature pFA, xNodataFeatures){
// 					m_pxNoDataLayer->addFeature(pFA);
// 				}
// 				m_pxNoDataLayer->commitChanges();
			}
			if (!xTopLineFeatures.isEmpty()){
				m_pxTopLineLayer->startEditing();
				foreach(QgsFeature pFA, xTopLineFeatures){
					m_pxTopLineLayer->addFeature(pFA);
				}
				m_pxTopLineLayer->commitChanges();
			}
			if (!xBaseDepAreaFeatures.isEmpty()){
				m_pxBaseAreDepLay->startEditing();
				foreach(QgsFeature pFA, xBaseDepAreaFeatures){
					m_pxBaseAreDepLay->addFeature(pFA);
				}
				m_pxBaseAreDepLay->commitChanges();
			}

			if (!xBaseHAreaFeatures.isEmpty()){
				m_pxBaseHFillAreLay->startEditing();
				foreach(QgsFeature pFA, xBaseHAreaFeatures){
					m_pxBaseHFillAreLay->addFeature(pFA);
				}
				m_pxBaseHFillAreLay->commitChanges();
			}

			if (!xBaseACFillFeatures.isEmpty()){
				foreach(QgsVectorLayer* pLayer, m_pxBaseACFillAreLays)
					pLayer->startEditing();

				QMap<quint32, QList<QgsFeature>>::Iterator itr = xBaseACFillFeatures.begin();
				while (itr != xBaseACFillFeatures.end()){
					quint32 unScaleMax = itr.key();
					foreach(QgsFeature pFA, *itr){
						m_pxBaseACFillAreLays[unScaleMax]->addFeature(pFA);
					}
					itr++;
				}
				
				foreach(QgsVectorLayer* pLayer, m_pxBaseACFillAreLays)
					pLayer->commitChanges();
			}

			if (!xBaseAPFillFeatures.isEmpty()){
				foreach(QgsVectorLayer* pLayer, m_pxBaseAPFillAreLays)
					pLayer->startEditing();

				QMap<quint32, QList<QgsFeature>>::Iterator itr = xBaseAPFillFeatures.begin();
				while (itr != xBaseAPFillFeatures.end()){
					quint32 unScaleMax = itr.key();
					foreach(QgsFeature pFA, *itr){
						m_pxBaseAPFillAreLays[unScaleMax]->addFeature(pFA);
					}
					itr++;
				}

				foreach(QgsVectorLayer* pLayer, m_pxBaseAPFillAreLays)
					pLayer->commitChanges();
			}

			if (!xBaseACLineFeatures.isEmpty()){
				foreach(QgsVectorLayer* pLayer, m_pxBaseACLineAreLays)
					pLayer->startEditing();

				QMap<quint32, QList<QgsFeature>>::Iterator itr = xBaseACLineFeatures.begin();
				while (itr != xBaseACLineFeatures.end()){
					quint32 unScaleMax = itr.key();
					foreach(QgsFeature pFA, *itr){
						m_pxBaseACLineAreLays[unScaleMax]->addFeature(pFA);
					}
					itr++;
				}

				foreach(QgsVectorLayer* pLayer, m_pxBaseACLineAreLays)
					pLayer->commitChanges();
			}

			if (!xBasePointSymFeatures.isEmpty()){
				foreach(QgsVectorLayer* pLayer, m_pxBasePointSymbolLays)
					pLayer->startEditing();

				QMap<quint32, QList<QgsFeature>>::Iterator itr = xBasePointSymFeatures.begin();
				while (itr != xBasePointSymFeatures.end()){
					quint32 unScaleMax = itr.key();
					foreach(QgsFeature pFA, *itr){
						m_pxBasePointSymbolLays[unScaleMax]->addFeature(pFA);
					}
					itr++;
				}

				foreach(QgsVectorLayer* pLayer, m_pxBasePointSymbolLays)
					pLayer->commitChanges();
			}

			if (!xBaseACHLineFeatures.isEmpty()){
				foreach(QgsVectorLayer* pLayer, m_pxBaseACHLineLays)
					pLayer->startEditing();

				QMap<quint32, QList<QgsFeature>>::Iterator itr = xBaseACHLineFeatures.begin();
				while (itr != xBaseACHLineFeatures.end()){
					quint32 unScaleMax = itr.key();
					foreach(QgsFeature pFA, *itr){
						m_pxBaseACHLineLays[unScaleMax]->addFeature(pFA);
					}
					itr++;
				}

				foreach(QgsVectorLayer* pLayer, m_pxBaseACHLineLays)
					pLayer->commitChanges();
			}
		}
		m_strSymbols;
// 		if (!xBaseDepAreaFeatures.isEmpty()){
// 			m_pxBaseAreaVLayerSet.at(0)->startEditing();
// 			foreach(QgsFeature pFA, xBaseDepAreaFeatures){
// 				m_pxBaseAreaVLayerSet.at(0)->addFeature(pFA)
// 			}
// 			m_pxBaseAreaVLayerSet.at(0)->commitChanges();
// 		}
// 
// 		if (!xBaseAPFeatures.isEmpty()){
// 			m_pxBaseAreaVLayerSet.at(1)->startEditing();
// 			foreach(QgsFeature pFA, xBaseAPFeatures)
// 				m_pxBaseAreaVLayerSet.at(1)->addFeature(pFA);
// 			m_pxBaseAreaVLayerSet.at(1)->commitChanges();
// 		}
// 
// 		if (!xBaseLandAreaFeatures.isEmpty()){
// 			m_pxBaseAreaVLayerSet.at(2)->startEditing();
// 			foreach(QgsFeature pFA, xBaseLandAreaFeatures)
// 				m_pxBaseAreaVLayerSet.at(2)->addFeature(pFA);
// 			m_pxBaseAreaVLayerSet.at(2)->commitChanges();
// 		}
// 		
// 		if (!xBaseLinesFeatures.isEmpty()){
// 			m_pxBaseLinesLay->startEditing();
// 			foreach(QgsFeature pFA, xBaseLinesFeatures){
// 				m_pxBaseLinesLay->addFeature(pFA);
// 			}
// 			m_pxBaseLinesLay->commitChanges();
// 		}
// 		if (!xBasePointFeatures.isEmpty()){
// 			m_pxBasePointLay->startEditing();
// 			foreach(QgsFeature pFA, xBasePointFeatures){
// 				m_pxBasePointLay->addFeature(pFA);
// 			}
// 			m_pxBasePointLay->commitChanges();
// 		}

		return 0;
	}

	Qt::PenStyle XS57LayerAdapter::strToQtPenStyle(const QString& _strStyle)
	{
		Qt::PenStyle xPenStyle = Qt::SolidLine;
		if (_strStyle == "DASH")
			xPenStyle = Qt::DashLine;
		else if (_strStyle == "DOTT")
			xPenStyle = Qt::DotLine;

		return xPenStyle;
	}

	quint32 XS57LayerAdapter::createImage(const QString& _strType, const QString& _strName, QString& _strOutPath)
	{
		QString strCurStyle = colorStyleToStr(m_xeColorStyle);
		QString strCurSymbolPath = QString("./Data/S52/Symbols/") + strCurStyle + QString("/");

		if (_strType == "SY"){
			XSymbolStyle* pSymStl = m_pxS52Lib->GetSymbolStyle(_strName);
			if (pSymStl){
				// 矢量图形
				if (pSymStl->m_ucDefinition == uchar('V')){
					QString strOutTemp = strCurSymbolPath + pSymStl->m_strName + QString(".png");
					double dScale = 100.0 / m_dXPixPerMM;
					QImage image(QSize(pSymStl->m_xBoundRect.width() + 80, pSymStl->m_xBoundRect.height() + 80), QImage::Format_ARGB32);
					image.fill(qRgba(0, 0, 0, 0));
					QPainter xPainter(&image);
					xPainter.setRenderHint(QPainter::HighQualityAntialiasing);
					xPainter.setRenderHint(QPainter::SmoothPixmapTransform);
					xPainter.begin(&image);
					QColor xCurColor = QColor(0, 0, 0, 0);
					quint32 unWidth = 1*200/m_dXPixPerMM;
					QPointF xCurPoint = QPointF(0, 0);
					qint32 nIdx = 0;
					QPointF xStartPos = pSymStl->m_xBoundRect.topLeft();
					qint32 nST = 0;
					qint32 nPM = -1;	// 多边形模式
					QPainterPath xPMPath;
					QPolygonF xCurPoly = QPolygonF();
					xStartPos -= QPointF(40, 40);
					foreach(QString strLine, pSymStl->m_strVector){
						QStringList strParts = strLine.split(";");
						foreach(QString strP, strParts){
							quint32 unLen = strP.length();
							if (unLen > 2 && strP.left(2) == "SP"){
								// 选择颜色
								strP.remove(0, 2);
								if (pSymStl->m_strColRef.contains(strP))
									m_pxS52Lib->GetColorByKey(pSymStl->m_strColRef[strP], xCurColor);
							}
							else if (unLen > 2 && strP.left(2) == "SW"){
								// 设置宽度
								strP.remove(0, 2);
								unWidth = strP.toUInt()*200/m_dXPixPerMM;
							}
							else if (unLen > 2 && strP.left(2) == "PU"){
								// 提笔
								strP.remove(0, 2);
								nIdx = strP.indexOf(",");
								xCurPoint.setX(strP.left(nIdx).toDouble());
								strP.remove(0, nIdx + 1);
								xCurPoint.setY(strP.toDouble());
								xCurPoint = xCurPoint - xStartPos;
								if (nPM == 0 || nPM == 1){
									xCurPoly.push_back(xCurPoint);
								}
							}
							else if (unLen >= 2 && strP.left(2) == "PD"){
								strP.remove(0, 2);
								nIdx = strP.indexOf(",");
								if (nIdx < 0){
									QPen xPen(xCurColor);
									xPen.setWidth(unWidth);
									xPainter.setPen(xPen);
									xPainter.drawPoint(xCurPoint);
								}
								else
								{
									while (nIdx > 0 && strP.count() > 0){
										QPointF toPoint = QPointF(0, 0);
										if (unLen > 2){
											nIdx = strP.indexOf(",");
											toPoint.setX(strP.left(nIdx).toDouble());
											strP.remove(0, nIdx + 1);
											nIdx = strP.indexOf(",");
											toPoint.setY(strP.left(nIdx).toDouble());
											toPoint = toPoint - xStartPos;
											strP.remove(0, nIdx + 1);
											nIdx = strP.indexOf(",");
										}
										if (nPM == 0 || nPM == 1){
											xCurPoly.push_back(toPoint);
										}
										else{
											QPen xPen(xCurColor);
											xPen.setWidth(unWidth);
											xPainter.setPen(xPen);
											xPainter.drawLine(xCurPoint, toPoint);
										}

										xCurPoint = toPoint;
									}
								}
							}
							else if (unLen > 2 && strP.left(2) == "CI"){
								strP.remove(0, 2);
								double dRadio = strP.toDouble();
								if (nPM == 0 || nPM == 1){
									QPointF xCenPos = xCurPoint;
									xPMPath.addEllipse(xCenPos, dRadio,dRadio);
								}
								else{
									QPen xPen(xCurColor);
									xPen.setWidth(unWidth);
									xPainter.setPen(xPen);
									QPainterPath xPath;
									QPointF xCenPos = xCurPoint;
									xPath.addEllipse(xCenPos, dRadio, dRadio);
									xPainter.drawPath(xPath);
								}
							}
							else if (unLen > 2 && strP.left(2) == "ST"){
								strP.remove(0, 2);
								// 分类
								nST = strP.toInt();
								//-1 打开所有分类(或最优组合)
								// 0 关闭所有分类; 以接收到的次序绘图
								// 1 画笔分类: 向量用画笔颜色分类, 而不以其被接收到的次序分类
								// 2 终点交换(双向绘图) : 交换相继向量的终点, 以减少画笔移动
								// 4 地理分类 : 在移到另一个地理区域前, 用其地理区域分类所有向量
							}
							else if (unLen > 2 && strP.left(2) == "PM"){
								// 
								strP.remove(0, 2);
								nPM = strP.toInt();
								if (nPM == 0){
									// 清除多边形缓冲区, 并进入多边形模式。
									xPMPath = QPainterPath();
									xCurPoly.push_back(xCurPoint);
								}
								else if (nPM == 1){
									// 关闭当前多边形或子多边形
									xCurPoly.push_back(xCurPoly.first());
									xPMPath.addPolygon(xCurPoly);
									xCurPoly.clear();
								}
								else if (nPM == 2){
									// 关闭当前多边形或子多边形，并退出多边形模式
									if (xCurPoly.count() >= 1)
										xCurPoly.push_back(xCurPoly.first());
									xPMPath.addPolygon(xCurPoly);
									nPM = -1;
									xCurPoly.clear();
								}
								else
									nPM = 0;
							}
							else if (unLen == 2 && strP == "FP"){
								// 填充多边形
								xPainter.fillPath(xPMPath,xCurColor);
								xPMPath = QPainterPath();
							}
							else if (unLen == 2 && strP == "EP"){
								QPen xPen(xCurColor);
								xPen.setWidth(unWidth);
								xPainter.setPen(xPen);
								xPainter.drawPath(xPMPath);
							}
							else if (!strP.isEmpty())
								int md = 0;
						}
					}
					xPainter.end();

					//image = image.copy(xStartPos.x()-1, xStartPos.y()-1, pSymStl->m_xBoundRect.width()+2, pSymStl->m_xBoundRect.height()+2);
					image = image.scaled(image.width() / dScale, image.height() / dScale, Qt::IgnoreAspectRatio, Qt::SmoothTransformation);
					QFile::remove(strOutTemp);
					image.save(strOutTemp);
					_strOutPath = strOutTemp;
					int md = 0;
				}
				// 光栅图形
				else if (pSymStl->m_ucDefinition == uchar('R')){
					QString strOutTemp = strCurSymbolPath + pSymStl->m_strName + QString(".png");
					QImage image(QSize(pSymStl->m_xBoundRect.width(), pSymStl->m_xBoundRect.height()), QImage::Format_ARGB32);
					image.fill(qRgba(0, 0, 0, 0));
					QMap<QString, QColor> xColors;
					if (pSymStl->m_strBitMap.count() > 0 && pSymStl->m_strBitMap.count() == pSymStl->m_xBoundRect.height() && pSymStl->m_strBitMap.at(0).count() == image.width()){
						quint32 unHIdx = 0;
						foreach(QString strLine, pSymStl->m_strBitMap){
							quint32 unWidx = 0;
							foreach(QChar xChar, strLine){
								uchar ucOne = xChar.cell();
								if (ucOne != '@' && pSymStl->m_strColRef.contains(xChar)){
									QColor xCol = QColor();
									QString strKey = pSymStl->m_strColRef[xChar];
									if (xColors.contains(strKey))
										xCol = xColors[strKey];
									else{
										quint32 error = m_pxS52Lib->GetColorByKey(strKey, xCol);
										if (error != 0)
											return -1;
										xColors.insert(strKey, xCol);
									}

									image.setPixel(QPoint(unWidx, unHIdx), xCol.rgba());
								}
								unWidx++;
							}
							unHIdx++;
						}
						QFile::remove(strOutTemp);
						image.save(strOutTemp);
						_strOutPath = strOutTemp;
					}
				}
				else
					return -1;
			}
		}else if(_strType == "PA"){
			XPatternsStyle* pPAT = m_pxS52Lib->GetPatternsStyle(_strName);
			// 光栏图像
			if (pPAT && pPAT->m_ucDefinition == uchar('R')){
				QString strOutTemp = strCurSymbolPath + pPAT->m_strName + QString(".png");
				QImage image(QSize(pPAT->m_xBoundRect.width(), pPAT->m_xBoundRect.height()), QImage::Format_ARGB32);
				image.fill(qRgba(0, 0, 0, 0));
				QMap<QString, QColor> xColors;
				if (pPAT->m_strBitMap.count() > 0 && pPAT->m_strBitMap.count() == pPAT->m_xBoundRect.height() && pPAT->m_strBitMap.at(0).count() == image.width()){
					quint32 unHIdx = 0;
					foreach(QString strLine, pPAT->m_strBitMap){
						quint32 unWidx = 0;
						foreach(QChar xChar, strLine){
							uchar ucOne = xChar.cell();
							if (ucOne != '@' && pPAT->m_strColRef.contains(xChar)){
								QColor xCol = QColor();
								QString strKey = pPAT->m_strColRef[xChar];
								if (xColors.contains(strKey))
									xCol = xColors[strKey];
								else{
									quint32 error = m_pxS52Lib->GetColorByKey(strKey, xCol);
									if (error != 0)
										return -1;
									xColors.insert(strKey, xCol);
								}

								image.setPixel(QPoint(unWidx, unHIdx), xCol.rgba());
							}
							unWidx++;
						}
						unHIdx++;
					}
					QFile::remove(strOutTemp);
					image.save(strOutTemp);
					_strOutPath = strOutTemp;
				}
				else
					return -1;
			}
			// 矢量图形
			if (pPAT && pPAT->m_ucDefinition == uchar('V')){
				QString strOutTemp = strCurSymbolPath + pPAT->m_strName + QString(".png");
				double dScale = 100.0 / m_dXPixPerMM;
				QImage image(QSize(pPAT->m_xBoundRect.width() + 60, pPAT->m_xBoundRect.height() + 60), QImage::Format_ARGB32);
				image.fill(qRgba(0, 0, 0, 0));
				QPainter xPainter(&image);
				xPainter.setRenderHint(QPainter::HighQualityAntialiasing);
				xPainter.setRenderHint(QPainter::SmoothPixmapTransform);
				xPainter.begin(&image);
				QColor xCurColor = QColor(0, 0, 0, 0);
				quint32 unWidth = 1 * 200/m_dXPixPerMM;
				QPointF xCurPoint = QPointF(0, 0);
				qint32 nIdx = 0;
				QPointF xStartPos = pPAT->m_xBoundRect.topLeft();
				qint32 nST = 0;
				qint32 nPM = -1;	// 多边形模式
				QPainterPath xPMPath;
				QPolygonF xCurPoly = QPolygonF();
				xStartPos -= QPointF(30, 30);
				foreach(QString strLine, pPAT->m_strVector){
					QStringList strParts = strLine.split(";");
					foreach(QString strP, strParts){
						quint32 unLen = strP.length();
						if (unLen > 2 && strP.left(2) == "SP"){
							// 选择颜色
							strP.remove(0, 2);
							if (pPAT->m_strColRef.contains(strP))
								m_pxS52Lib->GetColorByKey(pPAT->m_strColRef[strP], xCurColor);
						}
						else if (unLen > 2 && strP.left(2) == "SW"){
							// 设置宽度
							strP.remove(0, 2);
							unWidth = strP.toUInt()*200/m_dXPixPerMM;
						}
						else if (unLen > 2 && strP.left(2) == "PU"){
							// 提笔
							strP.remove(0, 2);
							nIdx = strP.indexOf(",");
							xCurPoint.setX(strP.left(nIdx).toDouble());
							strP.remove(0, nIdx + 1);
							xCurPoint.setY(strP.toDouble());
							xCurPoint = xCurPoint - xStartPos;
							if (nPM == 0 || nPM == 1){
								xCurPoly.push_back(xCurPoint);
							}
						}
						else if (unLen >= 2 && strP.left(2) == "PD"){
							strP.remove(0, 2);
							nIdx = strP.indexOf(",");
							if (nIdx < 0){
								QPen xPen(xCurColor);
								xPen.setWidth(unWidth);
								xPainter.setPen(xPen);
								xPainter.drawPoint(xCurPoint);
							}
							else
							{
								while (nIdx > 0 && strP.count() > 0){
									QPointF toPoint = QPointF(0, 0);
									if (unLen > 2){
										nIdx = strP.indexOf(",");
										toPoint.setX(strP.left(nIdx).toDouble());
										strP.remove(0, nIdx + 1);
										nIdx = strP.indexOf(",");
										toPoint.setY(strP.left(nIdx).toDouble());
										toPoint = toPoint - xStartPos;
										strP.remove(0, nIdx + 1);
										nIdx = strP.indexOf(",");
									}
									if (nPM == 0 || nPM == 1){
										xCurPoly.push_back(toPoint);
									}
									else{
										QPen xPen(xCurColor);
										xPen.setWidth(unWidth);
										xPainter.setPen(xPen);
										xPainter.drawLine(xCurPoint, toPoint);
									}

									xCurPoint = toPoint;
								}
							}
						}
						else if (unLen > 2 && strP.left(2) == "CI"){
							strP.remove(0, 2);
							double dRadio = strP.toDouble();
							if (nPM == 0 || nPM == 1){
								QPointF xCenPos = xCurPoint;
								xPMPath.addEllipse(xCenPos, dRadio, dRadio);
							}
							else{
								QPen xPen(xCurColor);
								xPen.setWidth(unWidth);
								xPainter.setPen(xPen);
								QPainterPath xPath;
								QPointF xCenPos = xCurPoint;
								xPath.addEllipse(xCenPos, dRadio, dRadio);
								xPainter.drawPath(xPath);
							}
						}
						else if (unLen > 2 && strP.left(2) == "ST"){
							strP.remove(0, 2);
							// 分类
							nST = strP.toInt();
							//-1 打开所有分类(或最优组合)
							// 0 关闭所有分类; 以接收到的次序绘图
							// 1 画笔分类: 向量用画笔颜色分类, 而不以其被接收到的次序分类
							// 2 终点交换(双向绘图) : 交换相继向量的终点, 以减少画笔移动
							// 4 地理分类 : 在移到另一个地理区域前, 用其地理区域分类所有向量
						}
						else if (unLen > 2 && strP.left(2) == "PM"){
							// 
							strP.remove(0, 2);
							nPM = strP.toInt();
							if (nPM == 0){
								// 清除多边形缓冲区, 并进入多边形模式。
								xPMPath = QPainterPath();
								xCurPoly.push_back(xCurPoint);
							}
							else if (nPM == 1){
								// 关闭当前多边形或子多边形
								xCurPoly.push_back(xCurPoly.first());
								xPMPath.addPolygon(xCurPoly);
								xCurPoly.clear();
							}
							else if (nPM == 2){
								// 关闭当前多边形或子多边形，并退出多边形模式
								if (xCurPoly.count() >= 1)
									xCurPoly.push_back(xCurPoly.first());
								xPMPath.addPolygon(xCurPoly);
								nPM = -1;
								xCurPoly.clear();
							}
							else
								nPM = 0;
						}
						else if (unLen == 2 && strP == "FP"){
							// 填充多边形
							xPainter.fillPath(xPMPath, xCurColor);
							xPMPath = QPainterPath();
						}
						else if (unLen == 2 && strP == "EP"){
							QPen xPen(xCurColor);
							xPen.setWidth(unWidth);
							xPainter.setPen(xPen);
							xPainter.drawPath(xPMPath);
						}
						else if (!strP.isEmpty())
							int md = 0;
					}
				}
				xPainter.end();

				//image = image.copy(xStartPos.x()-1, xStartPos.y()-1, pSymStl->m_xBoundRect.width()+2, pSymStl->m_xBoundRect.height()+2);
				image = image.scaled(image.width() / dScale, image.height() / dScale, Qt::IgnoreAspectRatio, Qt::SmoothTransformation);
				QFile::remove(strOutTemp);
				image.save(strOutTemp);
				_strOutPath = strOutTemp;
				int md = 0;
			}
		}
		else if (_strType == "LS"){
			XLineStyle* pLS = m_pxS52Lib->GetLineStyle(_strName);
			if (pLS){
				QString strOutTemp = strCurSymbolPath + pLS->m_strName + QString(".png");
				double dScale = 100.0 / m_dXPixPerMM;
				QImage image(QSize(pLS->m_xBoundRect.width() + 60, pLS->m_xBoundRect.height() + 60), QImage::Format_ARGB32);
				image.fill(qRgba(0, 0, 0, 0));
				QPainter xPainter(&image);
				xPainter.setRenderHint(QPainter::HighQualityAntialiasing);
				xPainter.setRenderHint(QPainter::SmoothPixmapTransform);
				xPainter.begin(&image);
				QColor xCurColor = QColor(0, 0, 0, 0);
				quint32 unWidth = 1 * 200/m_dXPixPerMM;
				QPointF xCurPoint = QPointF(0, 0);
				qint32 nIdx = 0;
				QPointF xStartPos = pLS->m_xBoundRect.topLeft();
				qint32 nST = 0;
				qint32 nPM = -1;	// 多边形模式
				QPainterPath xPMPath;
				QPolygonF xCurPoly = QPolygonF();
				xStartPos -= QPointF(30, 30);
				foreach(QString strLine, pLS->m_strVector){
					QStringList strParts = strLine.split(";");
					foreach(QString strP, strParts){
						quint32 unLen = strP.length();
						if (unLen > 2 && strP.left(2) == "SP"){
							// 选择颜色
							strP.remove(0, 2);
							if (pLS->m_strColRef.contains(strP))
								m_pxS52Lib->GetColorByKey(pLS->m_strColRef[strP], xCurColor);
						}
						else if (unLen > 2 && strP.left(2) == "SW"){
							// 设置宽度
							strP.remove(0, 2);
							unWidth = strP.toUInt()*200/m_dXPixPerMM;
						}
						else if (unLen > 2 && strP.left(2) == "PU"){
							// 提笔
							strP.remove(0, 2);
							nIdx = strP.indexOf(",");
							xCurPoint.setX(strP.left(nIdx).toDouble());
							strP.remove(0, nIdx + 1);
							xCurPoint.setY(strP.toDouble());
							xCurPoint = xCurPoint - xStartPos;
							if (nPM == 0 || nPM == 1){
								xCurPoly.push_back(xCurPoint);
							}
						}
						else if (unLen >= 2 && strP.left(2) == "PD"){
							strP.remove(0, 2);
							nIdx = strP.indexOf(",");
							if (nIdx < 0){
								QPen xPen(xCurColor);
								xPen.setWidth(unWidth);
								xPainter.setPen(xPen);
								xPainter.drawPoint(xCurPoint);
							}
							else
							{
								while (nIdx > 0 && strP.count() > 0){
									QPointF toPoint = QPointF(0, 0);
									if (unLen > 2){
										nIdx = strP.indexOf(",");
										toPoint.setX(strP.left(nIdx).toDouble());
										strP.remove(0, nIdx + 1);
										nIdx = strP.indexOf(",");
										toPoint.setY(strP.left(nIdx).toDouble());
										toPoint = toPoint - xStartPos;
										strP.remove(0, nIdx + 1);
										nIdx = strP.indexOf(",");
									}
									if (nPM == 0 || nPM == 1){
										xCurPoly.push_back(toPoint);
									}
									else{
										QPen xPen(xCurColor);
										xPen.setWidth(unWidth);
										xPainter.setPen(xPen);
										xPainter.drawLine(xCurPoint, toPoint);
									}

									xCurPoint = toPoint;
								}
							}
						}
						else if (unLen > 2 && strP.left(2) == "CI"){
							strP.remove(0, 2);
							double dRadio = strP.toDouble();
							if (nPM == 0 || nPM == 1){
								QPointF xCenPos = xCurPoint;
								xPMPath.addEllipse(xCenPos, dRadio, dRadio);
							}
							else{
								QPen xPen(xCurColor);
								xPen.setWidth(unWidth);
								xPainter.setPen(xPen);
								QPainterPath xPath;
								QPointF xCenPos = xCurPoint;
								xPath.addEllipse(xCenPos, dRadio, dRadio);
								xPainter.drawPath(xPath);
							}
						}
						else if (unLen > 2 && strP.left(2) == "ST"){
							strP.remove(0, 2);
							// 分类
							nST = strP.toInt();
							//-1 打开所有分类(或最优组合)
							// 0 关闭所有分类; 以接收到的次序绘图
							// 1 画笔分类: 向量用画笔颜色分类, 而不以其被接收到的次序分类
							// 2 终点交换(双向绘图) : 交换相继向量的终点, 以减少画笔移动
							// 4 地理分类 : 在移到另一个地理区域前, 用其地理区域分类所有向量
						}
						else if (unLen > 2 && strP.left(2) == "PM"){
							// 
							strP.remove(0, 2);
							nPM = strP.toInt();
							if (nPM == 0){
								// 清除多边形缓冲区, 并进入多边形模式。
								xPMPath = QPainterPath();
								xCurPoly.push_back(xCurPoint);
							}
							else if (nPM == 1){
								// 关闭当前多边形或子多边形
								xCurPoly.push_back(xCurPoly.first());
								xPMPath.addPolygon(xCurPoly);
								xCurPoly.clear();
							}
							else if (nPM == 2){
								// 关闭当前多边形或子多边形，并退出多边形模式
								if (xCurPoly.count() >= 1)
									xCurPoly.push_back(xCurPoly.first());
								xPMPath.addPolygon(xCurPoly);
								nPM = -1;
								xCurPoly.clear();
							}
							else
								nPM = 0;
						}
						else if (unLen == 2 && strP == "FP"){
							// 填充多边形
							xPainter.fillPath(xPMPath, xCurColor);
							xPMPath = QPainterPath();
						}
						else if (unLen == 2 && strP == "EP"){
							QPen xPen(xCurColor);
							xPen.setWidth(unWidth);
							xPainter.setPen(xPen);
							xPainter.drawPath(xPMPath);
						}
						else if (!strP.isEmpty())
							int md = 0;
					}
				}
				xPainter.end();

				//image = image.copy(xStartPos.x()-1, xStartPos.y()-1, pSymStl->m_xBoundRect.width()+2, pSymStl->m_xBoundRect.height()+2);
				QImage scaleImg = image.scaled(image.width() / dScale, image.height() / dScale, Qt::IgnoreAspectRatio, Qt::SmoothTransformation);
				QFile::remove(strOutTemp);
				scaleImg.save(strOutTemp);
				_strOutPath = strOutTemp;
				int md = 0;
			}
		}

		return 0;
	}

	bool XS57LayerAdapter::GetWindowsMonitorSize(QSizeF& _xMontorSize)
	{
		int nScreenWidth, nScreenHeight;    
		HDC hdcScreen = GetDC(NULL);   //获取屏幕的HDC    
		nScreenWidth = GetDeviceCaps(hdcScreen, HORZSIZE);    
		nScreenHeight = GetDeviceCaps(hdcScreen, VERTSIZE);    
		const double MILLIMETRE_TO_INCH = 0.03937;    
		double fDiagonalLen = sqrt(double(nScreenHeight * nScreenHeight + nScreenWidth * nScreenWidth))*MILLIMETRE_TO_INCH;

		DISPLAY_DEVICE dd;
		dd.cb = sizeof(dd);
		DWORD dev = 0; // device index
		int id = 1; // monitor number, as used by Display Properties > Settings

		QString DeviceID;
		bool bFoundDevice = false;
		while (EnumDisplayDevices(0, dev, &dd, 0) && !bFoundDevice)
		{
			DISPLAY_DEVICE ddMon;
			memset(&ddMon, 0, sizeof(ddMon));
			ddMon.cb = sizeof(ddMon);
			DWORD devMon = 0;

			while (EnumDisplayDevices(dd.DeviceName, devMon, &ddMon, 0) && !bFoundDevice)
			{
				if (ddMon.StateFlags & DISPLAY_DEVICE_ACTIVE &&
					!(ddMon.StateFlags & DISPLAY_DEVICE_MIRRORING_DRIVER))
				{
					DeviceID = QString::fromWCharArray(ddMon.DeviceID);
					DeviceID = DeviceID.remove(0, 8);
					DeviceID = DeviceID.left(DeviceID.indexOf('\\'));

					bFoundDevice = GetSizeForDevID(DeviceID, _xMontorSize);
				}
				devMon++;

				ZeroMemory(&ddMon, sizeof(ddMon));
				ddMon.cb = sizeof(ddMon);
			}

			ZeroMemory(&dd, sizeof(dd));
			dd.cb = sizeof(dd);
			dev++;
		}

		return bFoundDevice;
	}

	bool GetMonitorSizeFromEDID(const HKEY hDevRegKey, QSizeF& _xSize)
	{
		DWORD dwType, AcutalValueNameLength = 128;
		WCHAR valueName[128];

		BYTE EDIDdata[1024];
		DWORD edidsize = sizeof(EDIDdata);

		for (LONG i = 0, retValue = ERROR_SUCCESS; retValue != ERROR_NO_MORE_ITEMS; ++i)
		{
			retValue = RegEnumValue(hDevRegKey, i, &valueName[0],
				&AcutalValueNameLength, NULL, &dwType,
				EDIDdata, // buffer
				&edidsize); // buffer size
			
			if (retValue != ERROR_SUCCESS || QString::fromWCharArray(valueName) != "EDID")
				continue;

			_xSize.setWidth(((EDIDdata[68] & 0xF0) << 4) + EDIDdata[66]);
			_xSize.setHeight(((EDIDdata[68] & 0x0F) << 8) + EDIDdata[67]);

			return true; // valid EDID found
		}

		return false; // EDID not found
	}

	bool XS57LayerAdapter::GetSizeForDevID(const QString& TargetDevID, QSizeF& _xSize)
	{
		HDEVINFO devInfo = SetupDiGetClassDevsEx(
			&GUID_CLASS_MONITOR, //class GUID
			NULL, //enumerator
			NULL, //HWND
			DIGCF_PRESENT, // Flags //DIGCF_ALLCLASSES|
			NULL, // device info, create a new one.
			NULL, // machine name, local machine
			NULL);// reserved

		if (NULL == devInfo)
			return false;

		bool bRes = false;

		for (ULONG i = 0; ERROR_NO_MORE_ITEMS != GetLastError(); ++i)
		{
			SP_DEVINFO_DATA devInfoData;
			memset(&devInfoData, 0, sizeof(devInfoData));
			devInfoData.cbSize = sizeof(devInfoData);

			if (SetupDiEnumDeviceInfo(devInfo, i, &devInfoData))
			{
				WCHAR    Instance[80];
				SetupDiGetDeviceInstanceId(devInfo, &devInfoData, (PWSTR)Instance, MAX_PATH, NULL);
				QString strInstance = QString::fromWCharArray(Instance);
				if (strInstance.toUpper().contains(TargetDevID.toUpper()) == false)
					continue;

				HKEY hDevRegKey = SetupDiOpenDevRegKey(devInfo, &devInfoData,
					DICS_FLAG_GLOBAL, 0, DIREG_DEV, KEY_READ);

				if (!hDevRegKey || (hDevRegKey == INVALID_HANDLE_VALUE))
					continue;

				bRes = GetMonitorSizeFromEDID(hDevRegKey, _xSize);

				RegCloseKey(hDevRegKey);
			}
		}
		SetupDiDestroyDeviceInfoList(devInfo);
		return bRes;
	}


	void XS57LayerAdapter::OnLoadNewFile(XS57DataDoc* _pxS57Doc)
	{
// 		SetShowLev(m_xeShowLev);
// 		return;
		m_pxLoadThread->AddData(_pxS57Doc);
		return;
		if (_pxS57Doc && _pxS57Doc->isValid()){
			quint32 error = 0;
			m_xUpdateLock.lock();

			// 面状特征
			{
				// 基础面状特征	陆地(71)&深度区域(42)
				error = addAreaFeatures(_pxS57Doc,_pxS57Doc->GetAreaFeaturesSeq(), false);

				if (error != 0){
//					m_pxBaseAreLay->deleteFeatures(m_pxBaseAreLay->allFeatureIds());
					return;
				}
			}

			// 线状特征
			{
				error = addLineFeature(_pxS57Doc,_pxS57Doc->GetLineFeatures());
				if (error != 0){
					//m_pxBaseLinesLay->deleteFeatures(m_pxBaseLinesLay->allFeatureIds());
				}
				
			}

			// 点特征
			{
				error = addPointFeatures(_pxS57Doc, _pxS57Doc->GetPointFeatures());
				if (error != 0){
					int md = 0;
				}
			}

			SetShowLev(m_xeShowLev);

			m_bIsBusy = false;
			m_strCurLoadPaths.push_back(_pxS57Doc->GetFilePath());
			m_xUpdateLock.unlock();
		}
	
		int md = 0;
	}

	quint32 XS57LayerAdapter::addLineFeature(const XS57DataDoc* _pDoc, const QList<XFeature*>& _pxLineFeatures)
	{
		if (_pxLineFeatures.count() <= 0)
			return 0;
		
		QMap<quint32, QList<QgsFeature>> xBaseACHLineFeatures;

		QList<QgsFeature> xStandAreaFeatures;
		QList<QgsFeature> xStandLinesFeatures;
		QList<QgsFeature> xStandPointFeatures;
		quint32 error = 0;
		foreach(XFeature* pLF, _pxLineFeatures)
		{
			if (!m_bAddDataMark)
				break;

			quint32 unScaleMax = XDEFAULT_VLAYER_SCALE_MAX;
			pLF->GetAttr(133, unScaleMax);

			if (pLF->eGeoType != XFeature::xeLine)
				return -1;
			if (pLF->unObjType != 30 && pLF->unObjType != 43 && pLF->unObjType != 302 && pLF->unObjType != 122)
				continue;
			if (pLF->x2DPointList.count() < 2)
				return -1;

			if (pLF->unObjType == 122)
				int md = 0;

			xeShowLev xeLev = xeBase;
			quint32 unSymNo = 0;
			qint32 nTempNo = createSymbol(pLF, xeLev);
			if (nTempNo < 0)
				return -1;
			unSymNo = nTempNo;

			QgsPolyline xPolyLine;
			foreach(QVector2D* px2D, pLF->x2DPointList)
				xPolyLine.push_back(px2D->toPointF());

			if (xeLev == xeBase){
				if (pLF->strDisplayLayers.contains("ACH-LINE")){
					QString strLabelName = QString();
					QgsFields xFields = m_pxBaseACHLineLays[unScaleMax]->fields();
					QgsFeature newPolygon(xFields, pLF->unIdentifyID);
					newPolygon.setAttribute("id", pLF->unIdentifyID);
					newPolygon.setAttribute("key", pLF->strKey);
					newPolygon.setAttribute("symbolNo", unSymNo);
					newPolygon.setAttribute("label", QString::number(pLF->unIdentifyID));
					newPolygon.setGeometry(QgsGeometry::fromPolyline(xPolyLine));
					xBaseACHLineFeatures[unScaleMax].append(newPolygon);
				}
			}
		}

		if (m_xeShowLev == xeBase){
			if (!xBaseACHLineFeatures.isEmpty()){
				foreach(QgsVectorLayer* pLayer, m_pxBaseACHLineLays)
					pLayer->startEditing();

				QMap<quint32, QList<QgsFeature>>::Iterator itr = xBaseACHLineFeatures.begin();
				while (itr != xBaseACHLineFeatures.end()){
					quint32 unScaleMax = itr.key();
					foreach(QgsFeature pFA, *itr){
						m_pxBaseACHLineLays[unScaleMax]->addFeature(pFA);
					}
					itr++;
				}

				foreach(QgsVectorLayer* pLayer, m_pxBaseACHLineLays)
					pLayer->commitChanges();
			}
		}
		return 0;
	}

	QStringList XS57LayerAdapter::DEPARE01(const XFeature* _pxFeature, QStringList& _strExtentKeys, QStringList& _strExtentVals)
	{
		QStringList strLines = QStringList();

		double dMinDep = -1.0;
		double dMaxDep = 0.0;
		bool bMinIsFound = false;
		bool bShallow = true;

		if (_pxFeature->xAtts.contains(87)){
			bMinIsFound = true;
			dMinDep = _pxFeature->xAtts[87].toDouble();
		}

		dMaxDep = dMinDep + 0.1;
		if (_pxFeature->xAtts.contains(88))
			dMaxDep = _pxFeature->xAtts[88].toDouble();

//		quint32 unStartNo = _unSymNo;

		if (_pxFeature->unObjType == 46){
			if (!bMinIsFound){
				strLines.append("AC(DEPMD)");
				bShallow = false;
				//_unSymNo = unStartNo + 1;
			}
			strLines.append(";AP(DRGARE01)");
			strLines.append(";LS(DASH,1,CHGRF)");
			//_unSymNo = unStartNo + 2;
			return strLines;
		}

		QString strColor = QString();
		strColor = "AC(DEPIT)";
		if (dMinDep >= 0.0 && dMaxDep >= 0.0){
			strColor = "AC(DEPVS)";
			//_unSymNo = unStartNo + 1;
		}

		if (m_bUseTwoShades == true){
			if (dMinDep >= m_dSafetyContour && dMaxDep > m_dSafetyContour){
				strColor = "AC(DEPDW)";
				bShallow = false;
				//_unSymNo = unStartNo + 2;
			}
		}
		else{
			if (dMinDep >= m_dShallowContour && dMaxDep > m_dShallowContour){
				strColor = "AC(DEPMS)";
				//_unSymNo = unStartNo + 3;
			}
			if (dMinDep >= m_dSafetyContour && dMaxDep > m_dSafetyContour){
				strColor = "AC(DEPMD)";
				bShallow = false;
				//_unSymNo = unStartNo + 4;
			}
			if (dMinDep > m_dDeepContour && dMaxDep > m_dDeepContour){
				strColor = "AC(DEPDW)";
				bShallow = false;
				//_unSymNo = unStartNo + 2;
			}
		}
		strLines.push_front(strColor);

		return strLines;
	}

	QStringList XS57LayerAdapter::QUAPOS01(const XFeature* _pxFeature, QStringList& _strExtentKeys, QStringList& _strExtentVals)
	{
		if (_pxFeature->eGeoType == XFeature::xeLine)
			return CSQUALIN01(_pxFeature);
		else
			return CSQUAPNT01(_pxFeature);

		return QStringList();
	}

	QStringList XS57LayerAdapter::CSQUALIN01(const XFeature* _pxFeature)
	{
		QStringList strINST = QStringList();
		quint32 unAttKey = 0;
		quint32 unAttVal = 0;

		if (!m_xAttsStrToCodes.contains("QUAPOS"))
			return strINST;
		unAttKey = m_xAttsStrToCodes["QUAPOS"];
		bool bIsExistQupos = _pxFeature->xAtts.contains(unAttKey);

		if (bIsExistQupos){
			unAttVal = _pxFeature->xAtts[unAttKey].toUInt();
			if (unAttVal >= 2 && unAttVal < 10){
				strINST.append("LC(LOWACC21)");
				//_unSymNo = unStartNo;
			}
		}
		else{
			if (_pxFeature->unObjType == 30/*COALNE*/){
				if (!m_xAttsStrToCodes.contains("CONRAD"))
					return strINST;
				unAttKey = m_xAttsStrToCodes["CONRAD"];
				bool bIsExistConRad = _pxFeature->xAtts.contains(unAttKey);
				if (bIsExistConRad){
					unAttVal = _pxFeature->xAtts[unAttKey].toUInt();
					if (unAttVal == 1){
						strINST << "LS(SOLD,3,CHMGF)" << "LS(SOLD,1,CSTLN)";
						//_unSymNo = unStartNo + 1;
					}
					else{
						strINST.append("LS(SOLD,1,CSTLN)");
						//_unSymNo = unStartNo + 2;
					}
				}
				else{
					strINST.append("LS(SOLD,1,CSTLN)");
					//_unSymNo = unStartNo + 2;
				}
			}
			else {// LNDARE
				strINST.append("LS(SOLD,1,CSTLN)");
				//_unSymNo = unStartNo + 2;
			}
		}
		return strINST;
	}

	QStringList XS57LayerAdapter::CSQUAPNT01(const XFeature* _pxFeature)
	{
		QStringList strINST = QStringList();
		bool	bIsAccurate = true;
		quint32 unQualty = 10;
		quint32 unAttKey = 0;
		//quint32 unStartNo = _unSymNo;
		if (!m_xAttsStrToCodes.contains("QUAPOS"))
			return strINST;
		unAttKey = m_xAttsStrToCodes["QUAPOS"];
		bool bIsExistQualty = _pxFeature->xAtts.contains(unAttKey);
		
		if (bIsExistQualty){
			unQualty = _pxFeature->xAtts[unAttKey].toUInt();
			if (unQualty >= 2 && unQualty < 10)
				bIsAccurate = false;
		}

		if (!bIsAccurate){
			switch (unQualty)
			{
			case 4:
				strINST.append("SY(QUAPOS01)"); 
				/*_unSymNo = unStartNo;*/ break;	// "PA"
			case 5:
				strINST.append("SY(QUAPOS02)"); 
				/*_unSymNo = unStartNo + 1;*/ break;	// "PD"
			case 7:
			case 8:
				strINST.append("SY(QUAPOS03)"); 
				/*_unSymNo = unStartNo + 2;*/ break;	// "REP"
			default:
				strINST.append("SY(LOWACC01)"); 
				/*_unSymNo = unStartNo + 3;*/ break;	// "?"
			}
		}

		return strINST;
	}

	QStringList XS57LayerAdapter::DEPCNT02(const XFeature* _pxFeature, QStringList& _strExtentKeys, QStringList& _strExtentVals)
	{
		QStringList strINST = QStringList();

		double dDepthVal = 0.0;
		double dMinDep = 0.0;
		quint32 unAttKey = 0;
		double dMaxDep = 0.0;
		double dSafeContour = m_dSafetyContour;
		bool bSafe = false;
//		quint32 unStartNo = _unSymNo;

		if (_pxFeature->unObjType == 42 && _pxFeature->eGeoType == XFeature::xeLine){
			dMinDep = 0.0;
			if (_pxFeature->xAtts.contains(87))
				dMinDep = _pxFeature->xAtts[87].toDouble();
			dMaxDep = dMinDep;
			if (_pxFeature->xAtts.contains(88))
				dMaxDep = _pxFeature->xAtts[88].toDouble();

			if (dMinDep <= dSafeContour && dMinDep > 0){
				if (dMaxDep > dSafeContour){
					bSafe = true;
				}	
			}
			else{
				if (dMinDep == m_dDeepContour)
					bSafe = true;
				else if (qAbs(dMinDep - dSafeContour) < 1e-4)
					bSafe = true;
			}
			dDepthVal = dMinDep;
		}
		else{
			double dValdco = 0.0;
			if (!m_xAttsStrToCodes.contains("VALDCO"))
				return strINST;
			unAttKey = m_xAttsStrToCodes["VALDCO"];

			if (_pxFeature->xAtts.contains(unAttKey))
				dValdco = _pxFeature->xAtts[unAttKey].toDouble();
			if (dValdco == m_dDeepContour)
				bSafe = true;
			else{
				if (qAbs(dValdco - m_dDeepContour) < 1e-4)
					bSafe = true;
			}
		}

		if (!m_xAttsStrToCodes.contains("QUAPOS"))
			return strINST;
		unAttKey = m_xAttsStrToCodes["QUAPOS"];
		bool bIsExistQupos = _pxFeature->xAtts.contains(unAttKey);

		//quint32 unQuapos = 0;
		if (bIsExistQupos){
			quint32 unQuapos = _pxFeature->xAtts[unAttKey].toUInt();
			if (unQuapos >= 2 && unQuapos < 10){
				if (bSafe){
					XLookupTAB* pLookup = m_pxS52Lib->GetSingleLoopTAB(XLookupTAB::PLAIN_BOUNDARIES, "SAFECD");
					if (pLookup){
						strINST = pLookup->m_strINST.split(",");
						pLookup = NULL;
						//_unSymNo = unStartNo;
					}
					else{
						strINST.append("LS(DASH,2,DEPSC)");
						//_unSymNo = unStartNo + 1;
					}	
				}
				else{
					strINST.append("LS(DASH,1,DEPCN)");
					//_unSymNo = unStartNo + 2;
				}
			}
		}
		else{
			if (bSafe){
				XLookupTAB* pLookup = m_pxS52Lib->GetSingleLoopTAB(XLookupTAB::PLAIN_BOUNDARIES, "SAFECN");
				if (pLookup){
					strINST = pLookup->m_strINST.split(",");
					pLookup = NULL;
					//_unSymNo = unStartNo + 3;
				}
				else{
					strINST.append("LS(SOLD,2,DEPSC)");
					//_unSymNo = unStartNo + 4;
				}
			}
			else{
				strINST.append("LS(SOLD,1,DEPCN)");
				//_unSymNo = unStartNo + 5;
			}
		}
		
		if (bSafe){
			_strExtentKeys.push_back("DisplayCat");
			_strExtentVals.push_back(QString::number(quint32(xeBase)));
			_strExtentKeys.push_back("Scamin");
			_strExtentVals.push_back(QString::number(1e8));
		}

		return strINST;
	}

	QStringList XS57LayerAdapter::OBSTRN04(const XFeature* _pxFeature, QStringList& _strExtentKeys, QStringList& _strExtentVals)
	{
		QStringList strINST = QStringList();

		bool bPromote = false;
		double dValsou = 0xFFFFFFFF;
		double dDepthVal = 0xFFFFFFFF;
		double dLEastDepth = 0xFFFFFFFF;
		quint32 unAttKey = 0;
		if (m_xAttsStrToCodes.contains("VALSOU"))
			unAttKey = m_xAttsStrToCodes["VALSOU"];
		else
			return strINST;
		
		QStringList str02 = QStringList();
		if (_pxFeature->xAtts.contains(unAttKey) && !_pxFeature->xAtts[unAttKey].isEmpty()){
			// 水深值-单独处理这里不再处理
			dValsou = _pxFeature->xAtts[unAttKey].toDouble();
			dDepthVal = dValsou;
			str02 = SNDFRM02(_pxFeature, dValsou);
			str02.clear();
			int md = 0;
		}
		else{
			if (_pxFeature->eGeoType == XFeature::xeArea){
				if (_pxFeature->xAtts.contains(87))
					dLEastDepth = _pxFeature->xAtts[87].toDouble();
			}
			if (dLEastDepth == 0xFFFFFFFF){
				quint32 unCatobs = 0;
				quint32 unWatlev = 0;
				if (_pxFeature->xAtts.contains(42))
					unCatobs = _pxFeature->xAtts[42].toUInt();
				if (_pxFeature->xAtts.contains(187))
					unWatlev = _pxFeature->xAtts[187].toUInt();
				if (unCatobs == 6)
					dDepthVal = 0.01;
				else if (unWatlev == 0)
					dDepthVal = -15.0;
				else{
					switch (unWatlev)
					{
					case 5:dDepthVal = 0.0; break;
					case 3:dDepthVal = 0.01; break;
					case 4:
					case 1:
					case 2:
					default:dDepthVal = -15.0; break;
					}
				}
			}
			else
				dDepthVal = dLEastDepth;
		}

		QStringList str03 = QStringList();
		bool bIsDanger = false;
		str03 = _UDWHAZ03(_pxFeature, dDepthVal,bIsDanger);
		
		if (bIsDanger){
			_strExtentKeys.push_back("DisplayCat");
			_strExtentVals.push_back(QString::number(quint32(xeBase)));
		}

		QStringList str04 = QStringList();
		if (_pxFeature->eGeoType == XFeature::xePoint){
			bool bSounding = false;
			QStringList strQuapnt01 = CSQUAPNT01(_pxFeature);
			if (!str03.isEmpty()){
				str04.append(str03);
				//str04.append(strQuapnt01);
				goto end;
			}

			if (dValsou != 0xFFFFFFFF){
				if (dValsou <= 20.0){
					quint32 unWatLev = 0;
					_pxFeature->GetAttr(187, unWatLev);

					if (_pxFeature->unObjType == 153){	// 暗礁/适淹礁
						if (unWatLev == 0){ // default
							str04.append("SY(DANGER51)");
							bSounding = true;
						}
						else{
							switch (unWatLev)
							{
							case 3:str04.append("SY(DANGER51)"); bSounding = true; break;
							case 4:
							case 5:str04.append("SY(UWTROC04)"); bSounding = false; break;
							default:str04.append("SY(DANGER51)"); bSounding = true; break;
							}
						}
					}
					else{
						// OBSTRN
						if (unWatLev == 0){
							str04.append("SY(DANGER01)");
							bSounding = true;
						}
						else{
							switch (unWatLev)
							{
							case 1:
							case 2:str04.append("SY(LNDARE01)"); bSounding = false; break;
							case 3:str04.append("SY(DANGER52)"); bSounding = true; break;
							case 4:
							case 5:str04.append("SY(DANGER53)"); bSounding = true; break;
							default:str04.append("SY(DANGER51)"); bSounding = true; break;
							}
						}
					}
				}
				else {
					// dValsou > 20.0
					str04.append("SY(DANGER52)");
					bSounding = true;
				}
			}
			else{
				quint32 unWatLev = 0;
				_pxFeature->GetAttr(187, unWatLev);
				if (_pxFeature->unObjType == 153){
					if (unWatLev == 0)
						str04.append("SY(UWTROC04)");
					else{
						switch (unWatLev)
						{
						case 2:str04.append("SY(LNDARE01)"); break;
						case 3:str04.append("SY(UWTROC03)"); break;
						default:str04.append("SY(UWTROC04)"); break;
						}
					}
				}
				else{
					// OSBTRN
					if (unWatLev == 0)
						str04.append("SY(OBSTRN01)");
					else{
						switch (unWatLev)
						{
						case 1:
						case 2:str04.append("SY(OBSTRN11)"); break;
						case 3:str04.append("SY(OBSTRN01)"); break;
						case 4:
						case 5:str04.append("SY(OBSTRN03)"); break;
						default:str04.append("SY(OBSTRN01)"); break;
						}
					}
				}
			}

			if (bSounding)
				str04.append(str02);
			//str04.append(strQuapnt01);

			goto end;
		}
		else{
			if (_pxFeature->eGeoType == XFeature::xeLine){
				QStringList strQuapnt01 = CSQUAPNT01(_pxFeature);
				if (!strQuapnt01.isEmpty() && strQuapnt01.count() == 1){
					quint32 unQuapos = strQuapnt01.first().right(2).toUInt();
					if (unQuapos >= 2 && unQuapos < 10){
						if (!str03.isEmpty())
							str04.append("LC(LOWACC41)");
						else
							str04.append("LC(LOWACC31)");
					}
					goto end;
				}

				if (!str03.isEmpty()){
					str04.append("LS(DOTT,2,CHBLK)");
					goto end;
				}

				if (dValsou != 0xFFFFFFFF){
					if (dValsou <= 20.0)
						str04.append("LS(DOTT,2,CHBLK)");
					else
						str04.append("LS(DASH,2,CHBLK)");
				}
				else
					str04.append("LS(DOTT,2,CHBLK)");
				if (str03.isEmpty()){
					if (dValsou != 0xFFFFFFFF){
						if (dValsou <= 20.0)
							str04.append(str02);
					}
				}
			}
			else{
				QStringList strQuapnt01 = CSQUAPNT01(_pxFeature);
				if (!str03.isEmpty()){
					str04.append("AC(DEPVS)");
					str04.append("AP(FOULAR11)");
					str04.append("LS(DOTT,2,CHBLK)");
					str04.append(str03);
					//str04.append(strQuapnt01);
					goto end;
				}

				if (dValsou != 0xFFFFFFFF){
					if (dValsou <= 20.0)
						str04.append("LS(DOTT,2,CHBLK)");
					else
						str04.append("LS(DASH, 2, CHBLK)");
					str04.append(str02);
				}
				else{
					quint32 unWatLev = 0;
					_pxFeature->GetAttr(187, unWatLev);
					if (unWatLev == 0){
						str04.append("AC(DEPVS)");
						str04.append("LS(DOTT,2,CHBLK)");
					}
					else{
						bool bIsProcess = false;
						if (unWatLev == 3){
							quint32 unCatobs = 0;
							_pxFeature->GetAttr(42, unCatobs);
							if (unCatobs == 6){
								str04.append("AC(DEPVS)");
								str04.append("AP(FOULAR11)");
								str04.append("LS(DOTT,2,CHBLK)");
								bIsProcess = true;
							}
						}
						if(!bIsProcess){
							switch (unWatLev)
							{
							case 1:
							case 2:{
								str04.append("AC(CHBRN)");
								str04.append("LS(SOLD,2,CSTLN)");
								}break;
							case 4:{
								str04.append("AC(DEPIT)");
								str04.append("LS(DASH,2,CSTLN)");
								}break;
							case 5:
							case 3:
							default:{
								str04.append("AC(DEPVS)");
								str04.append("LS(DOTT,2,CHBLK)");
								}break;
							}
						}
					}
				}

				//str04.append(strQuapnt01);
				goto end;
			}
		}
	end:

		strINST = str04;
		return strINST;
	}

	QStringList XS57LayerAdapter::_UDWHAZ03(const XFeature* _pxFeature, const double& _depthVal, bool& _bIsDanger)
	{
		QStringList strINST = QStringList();

		bool bIsDanger = false;
		double dSafeContour = m_dSafetyContour;
		bool bPromote = false;
		if (_depthVal == 0xFFFFFFFF)
			bIsDanger = true;

		else if ( _depthVal <= dSafeContour){
			QList<XFeature*> xAssoFeatures = _pxFeature->pxParentDoc->GetAssociateObjects(_pxFeature);

			if (xAssoFeatures.isEmpty())
				bIsDanger = false;
			else
				int md = 0;

			foreach(XFeature* pFeature, xAssoFeatures){
				if (pFeature->eGeoType == XFeature::xeLine){
					double dRVal2 = 0.0;
					pFeature->GetAttr(88, dRVal2);
					if (dRVal2 > dSafeContour){
						bIsDanger = true;
						break;
					}
					else
						bIsDanger = false;
				}
				else{
					double dRVal1 = 0.0;
					pFeature->GetAttr(87, dRVal1);
					if (dRVal1 >= dSafeContour){
						bIsDanger = true;
						break;
					}
					else
						bIsDanger = false;
				}
			}
			xAssoFeatures.clear();
		}

		if (bIsDanger){
			quint32 unWatLev = 0;
			_pxFeature->GetAttr(187, unWatLev);
			if (unWatLev == 1 || unWatLev == 2){
			}
			else
				strINST.append("SY(ISODGR51)");
			_bIsDanger = bIsDanger;
		}

		return strINST;
	}

	QStringList XS57LayerAdapter::SNDFRM02(const XFeature* _pxFeature, const double& _dDepthVal)
	{
		QStringList strINST;

		QString strSymbolPrefixA;
		QString strSymbolPrefix = QString();

		QString strTecsou = QString();
		QString strQuasou = QString();
		QString strStatus = QString();
		double dLeadingDigit = 0.0;
		double dSafeDepth = m_dSafetyDepth;
		double dDepVal = _dDepthVal;

		if (_dDepthVal > 40000.0)
			dDepVal = 99999.0;
		if (_dDepthVal < -1000.0)
			dDepVal = 0.0;

		switch (m_xeDepthUnitDisplay){
		case xeFeet:
			dDepVal = dDepVal * 3 * 39.37 / 36;
			dSafeDepth = dSafeDepth * 3 * 39.37 / 36;
			break;
		case xeFathoms:
			dDepVal = dDepVal * 3 * 39.37 / (36 * 6);
			dSafeDepth = dSafeDepth * 3 * 39.37 / (36 * 6);
			break;
		default:
			break;
		}
	//	100000
		dDepVal += (dDepVal > 0.0) ? 0.01 : -0.01;
		dLeadingDigit = (int)dDepVal;
		//quint32 unStartNo = _unSymNo;

		quint32 unOff = 0;
		if (dDepVal <= dSafeDepth)
			strSymbolPrefix = "SOUNDS";	// 100000
		else{
			strSymbolPrefix = "SOUNDG";
		}
			
		strSymbolPrefixA = strSymbolPrefix;

		QStringList strTecsouList = QStringList();
		_pxFeature->GetAttr(156, strTecsouList);
		if (!strTecsouList.isEmpty()){
			if (strTecsouList.contains("\006")){
				QString strRes = QString("SY(") + strSymbolPrefix + QString("B1)");
				strINST.append(strRes);
			}
		}

		QStringList strQuasouList = QStringList();
		_pxFeature->GetAttr(125, strQuasouList);
		QStringList strStatusList = QStringList();
		_pxFeature->GetAttr(149, strStatusList);

		if (strQuasouList.contains("\003") || strQuasouList.contains("\004") || strQuasouList.contains("\005") || strQuasouList.contains("\010") || strQuasouList.contains("\011")
			 || strStatusList.contains("\022")){
			QString strRes = QString("SY(") + strSymbolPrefix + QString("C2)");
			strINST.append(strRes);
		}
		else{
			quint32 unQuapos = 0;
			if (_pxFeature->GetAttr(402,unQuapos)){
				if (unQuapos >= 2 && unQuapos < 10){
					QString strRes = QString("SY(") + strSymbolPrefix + QString("C2)");
					strINST.append(strRes);
				}
			}
		}

		// A
		if (dDepVal < 10.0){
			if ((m_xeDepthUnitDisplay == xeMeter) && (dDepVal > 0)){
				dDepVal = int(dDepVal < 0?dDepVal-0.5:dDepVal+0.5);
				dLeadingDigit = (int)dDepVal;
			}
			if (dDepVal < 10.0){
				int nFraction = qAbs((dDepVal - dLeadingDigit) * 10);
				QString strRes = QString("SY(") + strSymbolPrefix + QString("1") + QString::number((int)qAbs(dLeadingDigit)) + QString(")");
				strINST.append(strRes);
				if (nFraction > 0){
					QString strRes = QString("SY(") + strSymbolPrefix + QString("5") + QString::number((int)qAbs(nFraction)) + QString(")");
					strINST.append(strRes);
				}
				if (dDepVal < 0.0){
					QString strRes = QString("SY(") + strSymbolPrefix + QString("A1)");
					strINST.append(strRes);
				}
				goto xReturnPoint;
			}
		}

		if (dDepVal < 31.0){
			if ((m_xeDepthUnitDisplay == xeMeter) && (dDepVal > 0)){
				dDepVal = int(dDepVal < 0 ? dDepVal - 0.5 : dDepVal + 0.5);
				dLeadingDigit = (int)dDepVal;
			}

			double dFraction = dDepVal - qFloor(dLeadingDigit);

			if (dFraction != 0.0) {
				dFraction = dFraction * 10;
				if (dLeadingDigit >= 10.0){
					QString strRes = QString("SY(") + strSymbolPrefix + QString("2") + QString::number((int)dLeadingDigit / 10) + QString(")");
					strINST.append(strRes);
				}

				double dFirstDigit = qFloor(dLeadingDigit / 10);
				int nSecndDigit = (int)(floor(dLeadingDigit - (dFirstDigit * 10)));
				QString strRes = QString("SY(") + strSymbolPrefix + QString("1") + QString::number(nSecndDigit) + QString(")");
				strINST.append(strRes);

				if ((int)dFirstDigit > 0) {
					QString strRes = QString("SY(") + strSymbolPrefix + QString("5") + QString::number((int)dFirstDigit) + QString(")");
					strINST.append(strRes);
				}

				goto xReturnPoint;
			}
		}

		// B
		dDepVal = dLeadingDigit;
		if (dDepVal < 100.0){
			double dFirstDigit = qFloor(dLeadingDigit / 10.0);
			double dSecondDigit = qFloor(dLeadingDigit - (dFirstDigit*10));
			QString strRes = QString("SY(") + strSymbolPrefix + QString("1") + QString::number((int)dFirstDigit) + QString(")");
			strINST.append(strRes);
			strRes = QString("SY(") + strSymbolPrefix + QString("0") + QString::number((int)dSecondDigit) + QString(")");
			strINST.append(strRes);

			goto xReturnPoint;
		}

		if (dDepVal < 1000.0){
			double dFirstDigit = qFloor(dLeadingDigit / 100.0);
			double dSecondDigit = qFloor((dLeadingDigit - (dFirstDigit * 100))/10);
			double dThirdDigit = qFloor(dLeadingDigit - (dFirstDigit * 100) - (dSecondDigit * 10));
			QString strRes = QString("SY(") + strSymbolPrefix + QString("2") + QString::number((int)dFirstDigit) + QString(")");
			strINST.append(strRes);
			strRes = QString("SY(") + strSymbolPrefix + QString("1") + QString::number((int)dSecondDigit) + QString(")");
			strINST.append(strRes);
			strRes = QString("SY(") + strSymbolPrefix + QString("0") + QString::number((int)dThirdDigit) + QString(")");
			strINST.append(strRes);

			goto xReturnPoint;
		}

		if (dDepVal < 10000.0){
			double dFirstDigit = qFloor(dLeadingDigit / 1000.0);
			double dSecondDigit = qFloor((dLeadingDigit - (dFirstDigit * 1000)) / 100);
			double dThirdDigit = qFloor((dLeadingDigit - (dFirstDigit * 1000) - (dSecondDigit * 100)) / 10);
			double dLastDigit = qFloor(dLeadingDigit - (dFirstDigit * 1000) - (dSecondDigit * 100) - (dThirdDigit * 10));
			QString strRes = QString("SY(") + strSymbolPrefix + QString("2") + QString::number((int)dFirstDigit) + QString(")");
			strINST.append(strRes);
			strRes = QString("SY(") + strSymbolPrefix + QString("1") + QString::number((int)dSecondDigit) + QString(")");
			strINST.append(strRes);
			strRes = QString("SY(") + strSymbolPrefix + QString("0") + QString::number((int)dThirdDigit) + QString(")");
			strINST.append(strRes);
			strRes = QString("SY(") + strSymbolPrefix + QString("4") + QString::number((int)dLastDigit) + QString(")");
			strINST.append(strRes);

			goto xReturnPoint;
		}
		
		{
			double dFirstDigit = qFloor(dLeadingDigit / 10000);
			double dSecondDigit = qFloor((dLeadingDigit - (dFirstDigit * 10000)) / 1000);
			double dThirdDigit = qFloor((dLeadingDigit - (dFirstDigit * 10000) - (dSecondDigit * 1000)) / 100);
			double dFourthDigit = qFloor((dLeadingDigit - (dFirstDigit * 10000) - (dSecondDigit * 1000) - (dThirdDigit * 100))/10);
			double dLastDigit = qFloor(dLeadingDigit - (dFirstDigit * 10000) - (dSecondDigit * 1000) - (dThirdDigit * 100) - (dFourthDigit*10));

			QString strRes = QString("SY(") + strSymbolPrefix + QString("3") + QString::number((int)dFirstDigit) + QString(")");
			strINST.append(strRes);
			strRes = QString("SY(") + strSymbolPrefix + QString("2") + QString::number((int)dSecondDigit) + QString(")");
			strINST.append(strRes);
			strRes = QString("SY(") + strSymbolPrefix + QString("1") + QString::number((int)dThirdDigit) + QString(")");
			strINST.append(strRes);
			strRes = QString("SY(") + strSymbolPrefix + QString("0") + QString::number((int)dFourthDigit) + QString(")");
			strINST.append(strRes);
			strRes = QString("SY(") + strSymbolPrefix + QString("4") + QString::number((int)dLastDigit) + QString(")");
			strINST.append(strRes);

			goto xReturnPoint;
		}

	xReturnPoint:
		return strINST;
	}

	QString XS57LayerAdapter::colorStyleToStr(const xEColorStyle& _xeCS)
	{
		if (_xeCS == xeDayBright)
			return QString("DayBright");
		else if (_xeCS == xeDayWhite)
			return QString("DayWhite");
		else if (_xeCS == xeDayBlack)
			return QString("DayBlack");
		else if (_xeCS == xeDusk)
			return QString("Dusk");
		else if (_xeCS == xeNight)
			return QString("Night");
		return QString("DayBright");
	}

	bool XS57LayerAdapter::parseSymbolInfo(XFeature* _pxFeature, const quint32& _unScaleMax, const XLookupTAB* _pxLup, XSymbol& _xSymbol, xeShowLev& _xeShowLev, bool& _bIsCache)
	{
		QStringList strINST = _pxLup->m_strINST.split(";");
		QgsSymbolLayerV2List xAreaSymbolList;
		QgsSymbolLayerV2List xAPSymbolList;
		QgsSymbolLayerV2List xLineSymbolList;
		QgsSymbolLayerV2List xLCLineSymbolList;
		QgsSymbolLayerV2List xPointSymbolList;
		QString strFinalINST = QString();
		_xeShowLev = xeALL;
		quint32 error = -1;
		quint32 unScaleMax = _unScaleMax;

	parseLoop1:
		foreach(QString strOne, strINST){
			quint32 unLen = strOne.count();
			if (unLen > 2 && strOne.left(2) == "CS"){
				strOne.remove(0, 2);
				strOne.remove("(");
				strOne.remove(")");
				strINST.removeFirst();
				if (!m_pxCondFNCs.contains(strOne))
					return false;
				XS57LayerAdapter::ConditionFNC pFn = m_pxCondFNCs[strOne];
				QStringList strExtentKeys, strExtentVals;
				QStringList strNewList = (this->*pFn)(_pxFeature, strExtentKeys, strExtentVals);
				if (strExtentKeys.contains("DisplayCat")){
					_xeShowLev = xeShowLev(strExtentVals.at(strExtentKeys.indexOf("DisplayCat")).toUInt());
				}

				if (!strNewList.isEmpty()){
					strINST = strNewList + strINST;
					m_pxCondTypes.insert(_pxFeature->unObjType, pFn);
					goto parseLoop1;
				}
			}
			else if(unLen > 2){
				QString strTemp = strOne;
				if (!strFinalINST.isEmpty())
					strTemp = QString(";") + strOne;
				strFinalINST.append(strTemp);
			}
		}
		
		_pxFeature->strSymbolInfos = strFinalINST;
		if (m_strSymbols.contains(_unScaleMax) && m_strSymbols.value(_unScaleMax).contains(_pxFeature->strSymbolInfos)){
			_xSymbol = m_strSymbols.value(_unScaleMax).value(strFinalINST);
			_bIsCache = true;
			goto end;
		}
		strINST = strFinalINST.split(";");
		
	parseLoop2:
		foreach(QString strOne, strINST){
			quint32 unLen = strOne.count();
			if (unLen > 2 && strOne.left(2) == "AC"){

				// 填充颜色 
				strOne.remove(0, 2);
				strOne.remove("(");
				strOne.remove(")");
				QColor xCor = QColor();
				error = m_pxS52Lib->GetColorByKey(strOne, xCor);
				if (error != 0)
					return false;
				QgsSimpleFillSymbolLayerV2* pLayerV2 = new QgsSimpleFillSymbolLayerV2();
				pLayerV2->setFillColor(xCor);
				pLayerV2->setOutlineColor(xCor);
				xAreaSymbolList.push_front(pLayerV2);
				pLayerV2 = NULL;
				strINST.removeFirst();
			}
			else if (unLen > 4 && strOne.left(2) == "LS"){
				// 线型
				strOne.remove(0, 3);
				strOne.remove(strOne.length() - 1, 1);
				strOne.remove(" ");
				QStringList strPrams = strOne.split(",");
				strOne.clear();
				if (strPrams.count() == 3){
					Qt::PenStyle xPenSty = strToQtPenStyle(strPrams.at(0));
					double dWidth = strPrams.at(1).toDouble() * 0.25;
					QColor xColor = QColor();
					error = m_pxS52Lib->GetColorByKey(strPrams.at(2), xColor);
					if (error != 0)
						return error;

					QgsSimpleLineSymbolLayerV2* pLayerV2 = new QgsSimpleLineSymbolLayerV2();
					pLayerV2->setColor(xColor);
					pLayerV2->setPenStyle(xPenSty);
					pLayerV2->setWidth(dWidth); 
					xLineSymbolList.push_front(pLayerV2);
					pLayerV2 = NULL;
					strINST.removeFirst();

				}
			}
			else if (unLen > 2 && strOne.left(2) == "CS"){
				strOne.remove(0, 2);
				strOne.remove("(");
				strOne.remove(")");
				strINST.removeFirst();
				if (!m_pxCondFNCs.contains(strOne))
					return false;
				XS57LayerAdapter::ConditionFNC pFn = m_pxCondFNCs[strOne];
				QStringList strExtentKeys, strExtentVals;
				QStringList strNewList;//-DEL = (ms_xInstance.*pFn)(_pxFeature, strExtentKeys, strExtentVals);
				if (strExtentKeys.contains("DisplayCat")){
					_xeShowLev = xeShowLev(strExtentVals.at(strExtentKeys.indexOf("DisplayCat")).toUInt());
				}

				if (!strNewList.isEmpty()){
					strINST = strNewList + strINST;
					m_pxCondTypes.insert(_pxFeature->unObjType, pFn);
					goto parseLoop2;
				}
				else
					return false;
			}
			else if (unLen > 2 && strOne.left(2) == "AP"){
				strOne.remove(0, 2);
				strOne.remove("(");
				strOne.remove(")");
				// 检验文件是否存在
				QString strAPFile = QString("./Data/S52/Symbols/") + colorStyleToStr(m_xeColorStyle) + QString("/") + strOne + QString(".png");
				if (QFile::exists(strAPFile) == false)
					return false;

				QgsRasterFillSymbolLayer* pLayerV2 = new QgsRasterFillSymbolLayer(strAPFile);
				pLayerV2->setWidth(7);
				pLayerV2->setCoordinateMode(QgsRasterFillSymbolLayer::Feature);
				pLayerV2->setWidthUnit(QgsSymbolV2::MM);
				xAPSymbolList.push_front(pLayerV2);
				pLayerV2 = NULL;
				strINST.removeFirst();
			}
			else if (unLen > 2 && strOne.left(2) == "SY"){
				strOne.remove(0, 2);
				strOne.remove("(");
				strOne.remove(")");
				// 检验文件是否存在
				QString strAPFile = QString("./Data/S52/Symbols/") + colorStyleToStr(m_xeColorStyle) + QString("/") + strOne + QString(".png");
				if (QFile::exists(strAPFile) == false)
					return false;

				XImageMarkerSymbolLayerV2* pLayerV2 = new XImageMarkerSymbolLayerV2(strAPFile);
				pLayerV2->setSize(1);
				if (strOne == "LNDARE01"){
					pLayerV2->setSize(1.0 / 3.0);
					unScaleMax = XLNDARE_POINT_SCALE_MAX;
				}
				if (strOne == "WRECKS01")
					pLayerV2->setSize(0.5);
				if (strOne == "PRCARE51")
					pLayerV2->setSize(1.0 / 3.0);

				xPointSymbolList.append(pLayerV2);
				pLayerV2 = NULL;
				strINST.removeFirst();
			}
			else if (unLen > 2 && strOne.left(2) == "LC"){
				strOne.remove(0, 2);
				strOne.remove("(");
				strOne.remove(")");
				// 检验文件是否存在
				QString strAPFile = QString("./Data/S52/Symbols/") + colorStyleToStr(m_xeColorStyle) + QString("/") + strOne + QString(".png");
				if (QFile::exists(strAPFile) == false)
					return false;
				QgsMarkerLineSymbolLayerV2* pLineMarkerLay = new QgsMarkerLineSymbolLayerV2();
				
				QgsSymbolLayerV2List xMarkerLayers;
				XImageMarkerSymbolLayerV2* pImageSymber = new XImageMarkerSymbolLayerV2(strAPFile);
				pImageSymber->setSize(1);
				xMarkerLayers.append(pImageSymber);
				QgsMarkerSymbolV2* pMarkerSym = new QgsMarkerSymbolV2(xMarkerLayers);
				pLineMarkerLay->setSubSymbol(pMarkerSym);
				xLCLineSymbolList.push_back(pLineMarkerLay);
			}
		}
		_xSymbol.unSymbolNo = ++m_unSymbolIndex;
		_xSymbol.strINST = strFinalINST;

		end:
		if (_xeShowLev == xeALL){
			switch (_pxLup->m_xeDisCat)
			{
			case XLookupTAB::DISPLAYBASE:_xeShowLev = xeBase; break;
			case XLookupTAB::STANDARD:_xeShowLev = xeStand; break;
			case XLookupTAB::OTHER:_xeShowLev = xeALL; break;
			default:_xeShowLev = xeBase; break;
			}
		}
		if (_pxFeature->unObjType == 302)
			_xeShowLev = xeBase;
		if (_pxFeature->unObjType == 96)
			_xeShowLev = xeBase;

		if (!_bIsCache){
			if (!xAreaSymbolList.isEmpty())
				_xSymbol.pxACFillSymbol = new QgsFillSymbolV2(xAreaSymbolList);
			if (!xAPSymbolList.isEmpty())
				_xSymbol.pxAPFillSymbol = new QgsFillSymbolV2(xAPSymbolList);
			if (!xLineSymbolList.isEmpty())
				_xSymbol.pxLSLineSymbol = new QgsLineSymbolV2(xLineSymbolList);
			if (!xPointSymbolList.isEmpty())
				_xSymbol.pxSYPointSymbol = new QgsMarkerSymbolV2(xPointSymbolList);
			if (!xLCLineSymbolList.isEmpty())
				_xSymbol.pxLCLineSymbol = new QgsLineSymbolV2(xLCLineSymbolList);
			m_strSymbols[unScaleMax].insert(strFinalINST, _xSymbol);
		}
		
		return true;
	}

	QStringList XS57LayerAdapter::DATCVR01(const XFeature* _pFeature, QStringList& _strExtentKeys, QStringList& _strExtentVals)
	{
		return QStringList() << "LS(SOLD,1,CHGRN)" << "AC(NODTA)";/*"LC(HODATA01)"*/;
	}

	quint32 XS57LayerAdapter::addPointFeatures(const XS57DataDoc* _pDoc, const QList<XFeature*>& _pxPotFeatures)
	{
		if (_pxPotFeatures.count() <= 0)
			return 0;

		QMap<quint32, QList<QgsFeature>> xBasePointSymFeatures;

		QList<QgsFeature> xStandAreaFeatures;
		QList<QgsFeature> xStandLinesFeatures;
		QList<QgsFeature> xStandPointFeatures;
		quint32 error = 0;
		// 129 为水深点数据
		foreach(XFeature* pPF, _pxPotFeatures)
		{
			if (!m_bAddDataMark)
				break;

			quint32 unScaleMax = XDEFAULT_VLAYER_SCALE_MAX;
			pPF->GetAttr(133, unScaleMax);

			if (pPF->eGeoType != XFeature::xePoint)
				return -1;
			if (pPF->unObjType != 86 && pPF->unObjType != 159 && pPF->unObjType != 153 && pPF->unObjType != 71)
				continue;

			if (pPF->unObjType == 71)
				unScaleMax = XLNDARE_POINT_SCALE_MAX;
				

			xeShowLev xeLev = xeBase;
			quint32 unSymNo = 0;
			qint32 nTempNo = createSymbol(pPF, xeLev);
			if (nTempNo < 0)
				return -1;
			unSymNo = nTempNo;

			if (pPF->pxSinglePoint == NULL)
				return -1;

			if (xeLev == xeBase){
				if (pPF->strDisplayLayers.contains("SY-POINT")){
					QString strLabelName = QString();
					QgsFields xFields = m_pxBasePointSymbolLays[unScaleMax]->fields();
					QgsFeature newPolygon(xFields, pPF->unIdentifyID);
					newPolygon.setAttribute("id", pPF->unIdentifyID);
					newPolygon.setAttribute("key", pPF->strKey);
					newPolygon.setAttribute("symbolNo", unSymNo);
					newPolygon.setAttribute("label", QString::number(pPF->unIdentifyID));
					newPolygon.setGeometry(QgsGeometry::fromQPointF(pPF->pxSinglePoint->toPointF()));
					xBasePointSymFeatures[unScaleMax].append(newPolygon);
				}
			}
		}

		if (m_xeShowLev == xeBase){
			if (!xBasePointSymFeatures.isEmpty()){
				foreach(QgsVectorLayer* pLayer, m_pxBasePointSymbolLays)
					pLayer->startEditing();

				QMap<quint32, QList<QgsFeature>>::Iterator itr = xBasePointSymFeatures.begin();
				while (itr != xBasePointSymFeatures.end()){
					quint32 unScaleMax = itr.key();
					foreach(QgsFeature pFA, *itr){
						m_pxBasePointSymbolLays[unScaleMax]->addFeature(pFA);
					}
					itr++;
				}

				foreach(QgsVectorLayer* pLayer, m_pxBasePointSymbolLays)
					pLayer->commitChanges();
			}
		}
		return 0;
	}

	QStringList XS57LayerAdapter::WRECKS02(const XFeature* _pFeature, QStringList& _strExtentKeys, QStringList& _strExtentVals)
	{
		QStringList strWreck;
		QStringList strSndfrm;
		QStringList strUdwhaz03;
		QStringList strQuapnt01;
		//QString strQuasou;
		double dLeastDepth = XDOUBLE_UNKNOWN;
		double dDepthVal = XDOUBLE_UNKNOWN;
		double dValsou = XDOUBLE_UNKNOWN;
		bool bPromote = false;
		quint32 unWatlev = 0;
		quint32 unCatWrk = 0;
		quint32 unQuasou = XDOUBLE_UNKNOWN;
		double dSafetyContour = m_dSafetyContour;
		bool bIsDanger = false;

		_pFeature->GetAttr(179, dValsou);
		_pFeature->GetAttr(187, unWatlev);
		_pFeature->GetAttr(71, unCatWrk);
		_pFeature->GetAttr(125, unQuasou);

		if (dValsou != XDOUBLE_UNKNOWN){
			dDepthVal = dValsou;
			strSndfrm = SNDFRM02(_pFeature, dDepthVal);
			strSndfrm.clear();
		}
		else{
			if (_pFeature->eGeoType == XFeature::xeArea)
				dLeastDepth = XDOUBLE_UNKNOWN;
			if (dLeastDepth == XDOUBLE_UNKNOWN){
				if (unCatWrk != 0){
					switch (unCatWrk)
					{
					case 1:dDepthVal = 20.0; break;//safe
					case 2:dDepthVal = 0.0; break;//dangerous
					case 4:
					case 5:dDepthVal = -15.0; break;
					}
				}
				else{
					if (unWatlev == 0)
						dDepthVal = -15.0;
					else{
						switch (unWatlev)
						{
						case 1:
						case 2:dDepthVal = -15.0; break;
						case 3:dDepthVal = 0.01; break;
						case 4:dDepthVal = -15.0; break;
						case 5:dDepthVal = 0.0; break;
						case 6:dDepthVal = -15.0; break;
						}
					}
				}
			}
			else
				dDepthVal = dLeastDepth;
		}

		QStringList strQuasous = QStringList();
		_pFeature->GetAttr(125, strQuasous);

		if (strQuasous.isEmpty() || !strQuasous.contains("\07"))
			strUdwhaz03 = _UDWHAZ03(_pFeature, dDepthVal, bIsDanger);
		else{
			unQuasou = 7;
			strUdwhaz03.clear();
		}

		if (bIsDanger){
			_strExtentKeys.push_back("DisplayCat");
			_strExtentVals.push_back(QString::number(quint32(xeBase)));
		}

		strQuapnt01 = CSQUAPNT01(_pFeature);

		if (_pFeature->eGeoType == XFeature::xePoint){
			if (!strUdwhaz03.isEmpty()){
				strWreck = strUdwhaz03;
				//strWreck.append(strQuapnt01);
			}
			else{
				if (dValsou != XDOUBLE_UNKNOWN){
					if (dValsou < dSafetyContour)
						strWreck = QStringList() << "SY(DANGER51)";
					else
						strWreck = QStringList() << "SY(DANGER52)";
					strWreck.append("TX('Wk',2,1,2,'15110',1,0,CHBLK,21)");
					if (unQuasou == 7)
						strWreck.append("SY(WRECKS07)");
					strWreck.append(strSndfrm);
					strWreck.append(strUdwhaz03);
					//strWreck.append(strQuapnt01);
				}
				else{
					QString strSym = QString();
					if (unCatWrk != 0 && unWatlev != 0){
						if (unCatWrk == 1 && unWatlev == 3)
							strSym = "SY(WRECKS04)";
						else{
							if (unCatWrk == 2 && unWatlev == 3)
								strSym = "SY(WRECKS05)";
							else{
								if (unCatWrk == 4 || unCatWrk == 5)
									strSym = "SY(WRECKS01)";
								else{
									if (unWatlev == 1 || unWatlev == 2 || unWatlev == 5 || unWatlev == 4)
										strSym = "SY(WRECKS01)";
									else
										strSym = "SY(WRECKS05)";
								}
							}
						}
					}
					strWreck = QStringList() << strSym;
// 					if (!strQuapnt01.isEmpty())
// 						strWreck.append(strQuapnt01);
				}
			}
		}
		else{
			quint32 unQuapos = 0;
			_pFeature->GetAttr(402, unQuapos);

			QString strLine;
			if (unQuapos >= 2 && unQuapos < 10)
				strLine = "LC(LOWACC41)";
			else{
				if (!strUdwhaz03.isEmpty())
					strLine = "LS(DOTT,2,CHBLK)";
				else{
					if (XDOUBLE_UNKNOWN != dValsou){
						if (dValsou <= 20)
							strLine = "LS(DOTT,2,CHBLK)";
						else
							strLine = "LS(DASH,2,CHBLK)";
					}
					else{
						if (unWatlev == 0)
							strLine = "LS(DOTT,2,CSTLN)";
						else{
							switch (unWatlev)
							{
							case 1:
							case 2:strLine = "LS(SOLD,2,CSTLN)"; break;
							case 4:strLine = "LS(DASH,2,CSTLN)"; break;
							case 3:
							case 5:
							default:strLine = "LS(DOTT,2,CSTLN)"; break;
							}
						}
					}
				}
			}

			strWreck = QStringList()<<strLine;

			if (XDOUBLE_UNKNOWN != dValsou){
				if (dValsou <= 20){
					strWreck.append(strUdwhaz03);
					//strWreck.append(strQuapnt01);
					strWreck.append(strSndfrm);
				}
				else{
					strWreck.append(strUdwhaz03);
					//strWreck.append(strQuapnt01);
				}
			}
			else{
				QString strAC = QString();
				if (unWatlev == 0)
					strAC = "AC(DEPVS)";
				else{
					switch (unWatlev)
					{
					case 1:
					case 2:strAC = "AC(CHBRN)"; break;
					case 4:strAC = "AC(DEPIT)"; break;
					case 5:
					case 3:
					default:strAC = "AC(DEPVS)"; break;
					}
				}

				strWreck.append(strAC);
				strWreck.append(strUdwhaz03);
				//strWreck.append(strQuapnt01);
			}
		}

		return strWreck;
	}

	QStringList XS57LayerAdapter::SLCONS03(const XFeature* _pFeature, QStringList& _strExtentKeys, QStringList& _strExtentVals)
	{
		QStringList strINST = QStringList();

		bool bValStr = true;
		quint32 unVal = 0;
		quint32 unQuapos = 0;
		bool bQuapos = _pFeature->GetAttr(402, unQuapos);
		QString strSym = QString();

		if (_pFeature->eGeoType == XFeature::xePoint){
			if (bQuapos){
				if (unQuapos >= 2 && unQuapos < 10)
					strSym = "SY(LOWACC01)";
			}
		}else{
			if (_pFeature->eGeoType == XFeature::xeArea)
				strINST.append("AP(CROSSX01)");
			if (bQuapos){
				if (unQuapos >= 2 && unQuapos < 10)
					strSym = "SY(LOWACC01)";
			}else{
				bValStr = _pFeature->GetAttr(81, unVal);
				if (bValStr && (unVal == 1 || unVal == 2))
					strSym = "LS(DASH,1,CSTLN)";
				else{
					unVal = 0;
					bValStr = _pFeature->GetAttr(60, unVal);

					if (bValStr && (unVal == 4 || unVal == 6 || unVal == 8 || unVal == 15 || unVal == 16))
						strSym = "LS(SOLD,4,CSTLN)";
					else{
						bValStr = _pFeature->GetAttr(187, unVal);
						if (bValStr && unVal == 2)
							strSym = "LS(SOLD,2,CSTLN)";
						else{
							if (bValStr && (unVal == 3 || unVal == 4))
								strSym = "LS(DASH,2,CSTLN)";
							else
								strSym = "LS(SOLD,2,CSTLN)";
						}
					}
				}
			}
		}

		if (!strSym.isEmpty())
			strINST.append(strSym);

		return strINST;
	}

	QStringList XS57LayerAdapter::RESTRN01(const XFeature* _pFeature, QStringList& _strExtentKeys, QStringList& _strExtentVals)
	{
		QStringList strINST = QStringList();

		QStringList strRestrn = QStringList();
		_pFeature->GetAttr(131, strRestrn);

		QString strSym = QString();
		if (!strRestrn.isEmpty()){
			if (strRestrn.contains("\007") || strRestrn.contains("\010") || strRestrn.contains("\016"))
			{
				if (strRestrn.contains("\001") || strRestrn.contains("\002") || strRestrn.contains("\003") || strRestrn.contains("\004") || strRestrn.contains("\005")
					|| strRestrn.contains("\006"))
					strSym = "SY(ENTRES61)";
				else{
					if (strRestrn.contains("\011") || strRestrn.contains("\012") || strRestrn.contains("\013") || strRestrn.contains("\014") || strRestrn.contains("\015"))
						strSym = "SY(ENTRES71)";
					else
						strSym = "SY(ENTRES51)";
				}
			}
			else{
				if (strRestrn.contains("\001") || strRestrn.contains("\002")){
					if (strRestrn.contains("\003") || strRestrn.contains("\004") || strRestrn.contains("\005") || strRestrn.contains("\006"))
						strSym = "SY(ACHRES61)";
					else{
						if (strRestrn.contains("\011") || strRestrn.contains("\012") || strRestrn.contains("\013") || strRestrn.contains("\014") || strRestrn.contains("\015"))
							strSym = "SY(ACHRES71)";
						else
							strSym = "SY(ACHRES51)";
					}
				}
				else{
					if (strRestrn.contains("\003") || strRestrn.contains("\004") || strRestrn.contains("\005") || strRestrn.contains("\006")){
						if (strRestrn.contains("\011") || strRestrn.contains("\012") || strRestrn.contains("\013") || strRestrn.contains("\014") || strRestrn.contains("\015"))
							strSym = "SY(FSHRES71)";
						else
							strSym = "SY(FSHRES51)";
					}
					else{
						if (strRestrn.contains("\011") || strRestrn.contains("\012") || strRestrn.contains("\013") || strRestrn.contains("\014") || strRestrn.contains("\015"))
							strSym = "SY(INFARE51)";
						else
							strSym = "SY(RSRDEF51)";
					}
				}
			}
		}

		if (!strSym.isEmpty())
			strINST << strSym;
		return strINST;
	}

	quint32 XS57LayerAdapter::addText(const quint32& _unNo, const QString& _strText, const quint32& _unScaleMax, const QPointF& _xPoint)
	{
		if (_strText.isEmpty())
			return 0;
		// 寻找指定文字图层
		QgsVectorLayer* pLayer = NULL;
		if (!m_pxBaseTextSymbolLays.contains(_unScaleMax))
		{
			// 创建文字图层
			QString strName = QString::fromLocal8Bit("基础文字图层(") + QString::number(_unScaleMax) + QString(")");
			QgsPalLayerSettings xLaySettting;
			QStringList strFieldKeys = QStringList();
			QStringList strFieldTypes = QStringList();

			strFieldKeys << "id" << "key" << "symbolNo" << "label";
			strFieldTypes << "integer" << "string(64)" << "integer" << "string(32)";
			xLaySettting.enabled = true;
			xLaySettting.fieldName = "label";
			xLaySettting.textFont = QFont(QString::fromLocal8Bit("方正准圆_GBK"), 11);
			xLaySettting.textColor = Qt::darkBlue;
			pLayer = createVectorLayer(strName, "point", strFieldKeys, strFieldTypes, QPainter::CompositionMode_SourceOver, &xLaySettting
				, NULL);

			if (pLayer == NULL)
				return -1;

			pLayer->setScaleBasedVisibility(true);
			pLayer->setMaximumScale(_unScaleMax);
			pLayer->setMinimumScale(XDEFAULT_VLAYER_SCALE_MIN);
			m_pxBaseTextSymbolLays.insert(_unScaleMax, pLayer);

			QgsSimpleMarkerSymbolLayerV2* pSym = new QgsSimpleMarkerSymbolLayerV2();
			pSym->setSize(11);
			QgsMarkerSymbolV2* pMarker = new QgsMarkerSymbolV2(QgsSymbolLayerV2List() << pSym);
			pMarker->setAlpha(0);
			pLayer->setRendererV2(new QgsSingleSymbolRendererV2(pMarker));
		}
		else
			pLayer = m_pxBaseTextSymbolLays[_unScaleMax];
		
		QgsFields xFields = pLayer->fields();
		QgsFeature xTextFeature(xFields, _unNo);
		xTextFeature.setAttribute("id", _unNo);
		xTextFeature.setAttribute("label", _strText);
		xTextFeature.setGeometry(QgsGeometry::fromQPointF(_xPoint));
		
		pLayer->startEditing();
		pLayer->addFeature(xTextFeature);
		pLayer->commitChanges();
		return 0;
	}

	bool XS57LayerAdapter::addDataDoc(XS57DataDoc* _pxDoc)
	{
		m_bAddDataMark = true;
		if (_pxDoc && _pxDoc->isValid()){
			quint32 error = 0;
			//m_xUpdateLock.lock();

			// 面状特征
			{
				// 基础面状特征	陆地(71)&深度区域(42)
				error = addAreaFeatures(_pxDoc, _pxDoc->GetAreaFeaturesSeq(), false);

				if (error != 0){
					//m_pxBaseAreLay->deleteFeatures(m_pxBaseAreLay->allFeatureIds());
					return false;
				}
			}

			// 线状特征
			if (0)
			{
				error = addLineFeature(_pxDoc, _pxDoc->GetLineFeatures());
				if (error != 0){
					//m_pxBaseLinesLay->deleteFeatures(m_pxBaseLinesLay->allFeatureIds());
					return false;
				}

			}

			// 点特征
			if (0)
			{
				error = addPointFeatures(_pxDoc, _pxDoc->GetPointFeatures());
				if (error != 0){
					return false;
				}
			}

			//SetShowLev(m_xeShowLev);

			//m_bIsBusy = false;
			m_strCurLoadPaths.push_back(_pxDoc->GetFilePath());
			//m_xUpdateLock.unlock();
		}
		m_bAddDataMark = false;
		return true;
	}

	void XS57LayerAdapter::OnLoadFinish()
	{
		SetShowLev(m_xeShowLev);

//		XS57Document::GetInstance()->CreateCache();
	}

	void XS57LayerAdapter::StopAddData()
	{
		m_bAddDataMark = false;
	}

	QStringList XS57LayerAdapter::RESARE02(const XFeature* _pFeature, QStringList& _strExtentKeys, QStringList& _strExtentVals)
	{
		QStringList strRestrn;
		QStringList strCatrea;
		bool bIsOk = _pFeature->GetAttr(131, strRestrn);
		bIsOk = _pFeature->GetAttr(56, strCatrea);

		QString strSym;
		QString strLine;
		QString strPrio;

		QList<quint32> ulNums;
		if (!strRestrn.isEmpty())
		{
			ulNums << 7 << 10 << 16;
			if (sfnFindNumberByStrList(strRestrn, ulNums))
			{
				// 条件A
				ulNums.clear();
				ulNums << 1 << 2 << 3 << 4 << 5 << 6;
				if (sfnFindNumberByStrList(strRestrn, ulNums))
					strSym = "SY(ENTRES61)";
				else
				{
					ulNums.clear();
					ulNums << 1 << 10 << 11 << 14 << 16 << 23 << 25 << 31;
					if (sfnFindNumberByStrList(strCatrea, ulNums))
					{
						strSym = "SY(ENTRES61)";
					}
					else
					{
						ulNums.clear();
						ulNums << 11 << 12 << 13 << 14 << 15;
						if (sfnFindNumberByStrList(strRestrn, ulNums))
							strSym = "SY(ENTRES71)";
						else
						{
							ulNums.clear();
							ulNums << 4 << 5 << 6 << 7 << 12 << 22 << 24 << 26 << 27 << 30;
							if (sfnFindNumberByStrList(strCatrea, ulNums))
							{
								strSym = "SY(ENTRES71)";
							}
							else
								strSym = "SY(ENTRES51)";
						}
					}
				}

				// 是否启用符号化边界
				if (m_pxS52Lib->GetBoundStyle() == XLookupTAB::SYMBOLIZED_BOUNDARIES)
				{
					strLine = "LC(RESARE51)";
				}
				else
					strLine = "LS(DASH,2,CHMGD)";

			}
			else
			{
				ulNums.clear();
				ulNums << 1 << 2;
				if (sfnFindNumberByStrList(strRestrn, ulNums))
				{
					// 条件B
					ulNums.clear();
					ulNums << 3 << 4 << 5 << 6;
					if (sfnFindNumberByStrList(strRestrn, ulNums))
					{
						strSym = "SY(ACHRES61)";
					}
					else
					{
						ulNums.clear();
						ulNums << 1 << 10 << 11 << 14 << 16 << 23 << 25 << 31;
						if (sfnFindNumberByStrList(strCatrea, ulNums))
						{
							strSym = "SY(ACHRES61)";
						}
						else
						{
							ulNums.clear();
							ulNums << 11 << 12 << 13 << 14 << 15;
							if (sfnFindNumberByStrList(strRestrn, ulNums))
							{
								strSym = "SY(ACHRES71)";
							}
							else
							{
								ulNums.clear();
								ulNums << 4 << 5 << 6 << 7 << 12 << 22 << 24 << 26 << 27 << 30;
								if (sfnFindNumberByStrList(strCatrea, ulNums))
								{
									strSym = "SY(ACHRES71)";
								}
								else
									strSym = "SY(RESTRN51)";
							}
						}
					}

					// 是否启用符号化边界
					if (m_pxS52Lib->GetBoundStyle() == XLookupTAB::SYMBOLIZED_BOUNDARIES)
					{
						strLine = "LC(RESARE51)";
					}
					else
						strLine = "LS(DASH,2,CHMGD)";
				}
				else
				{
					// 条件C
					ulNums.clear();
					ulNums << 3 << 4 << 5 << 6;
					if (sfnFindNumberByStrList(strRestrn, ulNums))
					{
						ulNums.clear();
						ulNums << 1 << 10 << 11 << 14 << 16 << 23 << 25 << 31;
						if (sfnFindNumberByStrList(strCatrea, ulNums))
							strSym = "SY(FSHRES51)";
						else
						{
							ulNums.clear();
							ulNums << 11 << 12 << 13 << 14 << 15;
							if (sfnFindNumberByStrList(strRestrn, ulNums))
							{
								strSym = "SY(FSHRES71)";
							}
							else
							{
								ulNums.clear();
								ulNums << 4 << 5 << 6 << 7 << 12 << 22 << 24 << 26 << 27 << 30;
								if (sfnFindNumberByStrList(strCatrea, ulNums))
									strSym = "SY(FSHRES71)";
								else
									strSym = "SY(FSHRES51)";
							}
						}
						// 是否启用符号化边界
						if (m_pxS52Lib->GetBoundStyle() == XLookupTAB::SYMBOLIZED_BOUNDARIES)
						{
							strLine = "LC(FSHRES51)";
						}
						else
							strLine = "LS(DASH,2,CHMGD)";
					}
					else
					{
						ulNums.clear();
						ulNums << 11 << 12 << 13 << 14 << 15;
						if (sfnFindNumberByStrList(strRestrn, ulNums))
						{
							strSym = "SY(INFARE51)";
						}
						else
							strSym = "SY(RSRDEF51)";

						// 是否启用符号化边界
						if (m_pxS52Lib->GetBoundStyle() == XLookupTAB::SYMBOLIZED_BOUNDARIES)
						{
							strLine = "LC(CTYARE51)";
						}
						else
							strLine = "LS(DASH,2,CHMGD)";
					}
				}
			}
		}
		else
		{
			// 条件D
			if (!strCatrea.isEmpty())
			{
				ulNums.clear();
				ulNums << 1 << 10 << 11 << 14 << 16 << 23 << 25 << 31;
				if (sfnFindNumberByStrList(strCatrea, ulNums))
				{
					ulNums.clear();
					ulNums << 4 << 5 << 6 << 7 << 12 << 22 << 24 << 26 << 27 << 30;
					if (sfnFindNumberByStrList(strCatrea, ulNums))
						strSym = "SY(CTYARE71)";
					else
						strSym = "SY(CTYARE51)";
				}
				else
				{
					ulNums.clear();
					ulNums << 4 << 5 << 6 << 7 << 12 << 22 << 24 << 26 << 27 << 30;
					if (sfnFindNumberByStrList(strCatrea, ulNums))
						strSym = "SY(INFARE51)";
					else
						strSym = "SY(RSRDEF51)";
				}
			}
			else
				strSym = "SY(RSRDEF51)";

			// 是否启用符号化边界
			if (m_pxS52Lib->GetBoundStyle() == XLookupTAB::SYMBOLIZED_BOUNDARIES)
			{
				strLine = "LC(CTYARE51)";
			}
			else
				strLine = "LS(DASH,2,CHMGD)";
		}
		
		QStringList strRes;
		if (!strLine.isEmpty())
			strRes << strLine;
		if (!strSym.isEmpty())
			strRes << strSym;
		
		return strRes;
	}

	// 获取当前加载的海图数据列表
	const QStringList XS57LayerAdapter::GetS57Files() const
	{
		return m_pxS57DocMap.keys();
	}

	// 卸载海图数据
	void XS57LayerAdapter::UnloadS57File(const QString& _strFilePath)
	{
		QString strAbsFilePath = sfnGetAbsPath(_strFilePath);
		if (m_strLoadS57Files.contains(strAbsFilePath))
		{
			XS57DataDoc* pDoc = m_pxS57DocMap[strAbsFilePath];
			if (pDoc)
			{
				quint64 unMark = m_strLoadS57Files[strAbsFilePath];

				foreach(QgsVectorLayer* pLayer, m_pxCustomLayerMap)
				{
					pLayer->startEditing();
					QgsFeatureIterator feqItr = pLayer->getFeatures();
					QgsFeature feq;
					while (feqItr.nextFeature(feq))
					{
						if (feq.attribute(3).toUInt() == pDoc->GetDrawStartID())
							pLayer->deleteFeature(feq.id());
					}
					pLayer->commitChanges();
				}
				foreach(QgsVectorLayer* pLayer, m_pxAreaCoverLayerMap)
				{
					pLayer->startEditing();
					QgsFeatureIterator feqItr = pLayer->getFeatures();
					QgsFeature feq;
					while (feqItr.nextFeature(feq))
					{
						if (feq.attribute(3).toUInt() == pDoc->GetDrawStartID())
							pLayer->deleteFeature(feq.id());
					}
					pLayer->commitChanges();
				}
				foreach(QgsVectorLayer* pLayer, m_pxAreaNormalLayerMap)
				{
					pLayer->startEditing();
					QgsFeatureIterator feqItr = pLayer->getFeatures();
					QgsFeature feq;
					while (feqItr.nextFeature(feq))
					{
						if (feq.attribute(3).toUInt() == pDoc->GetDrawStartID())
							pLayer->deleteFeature(feq.id());
					}
					pLayer->commitChanges();
				}
				foreach(QgsVectorLayer* pLayer, m_pxAreaTransLayerMap)
				{
					pLayer->startEditing();
					QgsFeatureIterator feqItr = pLayer->getFeatures();
					QgsFeature feq;
					while (feqItr.nextFeature(feq))
					{
						if (feq.attribute(3).toUInt() == pDoc->GetDrawStartID())
							pLayer->deleteFeature(feq.id());
					}
					pLayer->commitChanges();
				}
				foreach(QgsVectorLayer* pLayer, m_pxLineLayerMap)
				{
					pLayer->startEditing();
					QgsFeatureIterator feqItr = pLayer->getFeatures();
					QgsFeature feq;
					while (feqItr.nextFeature(feq))
					{
						if (feq.attribute(3).toUInt() == pDoc->GetDrawStartID())
							pLayer->deleteFeature(feq.id());
					}
					pLayer->commitChanges();
				}
				foreach(QgsVectorLayer* pLayer, m_pxPointLayerMap)
				{
					pLayer->startEditing();
					QgsFeatureIterator feqItr = pLayer->getFeatures();
					QgsFeature feq;
					while (feqItr.nextFeature(feq))
					{
						if (feq.attribute(3).toUInt() == pDoc->GetDrawStartID())
							pLayer->deleteFeature(feq.id());
					}
					pLayer->commitChanges();
				}

				delete pDoc;
				pDoc = nullptr;

				m_pxMapCanvas->refreshAllLayers();

			}
			m_pxS57DocMap.remove(strAbsFilePath);
		}
	}

	// 根据坐标点获取海图元素信息
	void XS57LayerAdapter::GetHTInfo(QPointF _pt, HTFeatureInfoList& _xRefRes)
	{
		// 获取当前显示比例尺
		QgsRectangle curRang = m_pxMapCanvas->extent();
		quint32 unWidth = m_pxMapCanvas->width();
		double dPixU = double(curRang.width()) / double(unWidth);
		double dExtend = dPixU * 15;

		// 获取点图层信息
		foreach(QgsVectorLayer* pLayer, m_pxPointLayerMap)
		{
			if (pLayer)
			{
				QgsRectangle selRect(_pt.x() - dExtend, _pt.y() + dExtend, _pt.x() + dExtend, _pt.y() - dExtend);
				QScopedPointer<QgsGeometry> selectGeomTrans(QgsGeometry::fromRect(selRect));
				QgsFeatureRequest request;
				request.setFilterRect(selRect);
				request.setFlags(QgsFeatureRequest::ExactIntersect);
				QgsFeatureIterator fit = pLayer->getFeatures(request);
				QgsFeature f;
				while (fit.nextFeature(f))
				{
					quint32 unObjType = f.attribute("key").toUInt();
					QStringList strAttKey = f.attribute("attKeys").toString().split(";");
					QStringList strAttVal = f.attribute("attVals").toString().split(";");

					HTFeatureInfo fInfo;
					if (m_xS57ClassCodeToStrs.contains(unObjType))
						fInfo.strType = m_xS57ClassCodeToStrs[unObjType];

					if (strAttKey.count() != strAttVal.count())
					{
						continue;
					}
					int nIdx = 0;
					foreach(QString strAtt, strAttKey)
					{
						quint32 unAttCode = strAtt.toUInt();
						if (m_xS57AttsVec.contains(unAttCode))
						{
							XS57Atts atts = m_xS57AttsVec[unAttCode];
							fInfo.strAttNames.append(atts.strName);

							QString strVal = strAttVal.at(nIdx);
							if (atts.ucAttsType == 'E'
								|| atts.ucAttsType == 'L'
								|| atts.ucAttsType == 'I')
							{
								bool bIsNum = false;
								qint32 unCode = strVal.toInt(&bIsNum);
								if (bIsNum && atts.strExpectedVals.contains(unCode))
								{
									strVal = atts.strExpectedVals[unCode];
								}
							}

							fInfo.strAttVals.append(strVal);
						}
						nIdx++;
					}

					_xRefRes.append(fInfo);
				}
			}
		}

		// 获取线图层信息
		foreach(QgsVectorLayer* pLayer, m_pxLineLayerMap)
		{
			if (pLayer)
			{
				QgsRectangle selRect(_pt.x() - dExtend, _pt.y() + dExtend, _pt.x() + dExtend, _pt.y() - dExtend);
				QScopedPointer<QgsGeometry> selectGeomTrans(QgsGeometry::fromRect(selRect));
				QgsFeatureRequest request;
				request.setFilterRect(selRect);
				request.setFlags(QgsFeatureRequest::ExactIntersect);
				QgsFeatureIterator fit = pLayer->getFeatures(request);
				QgsFeature f;
				while (fit.nextFeature(f))
				{
					quint32 unObjType = f.attribute("key").toUInt();
					QStringList strAttKey = f.attribute("attKeys").toString().split(";");
					QStringList strAttVal = f.attribute("attVals").toString().split(";");

					HTFeatureInfo fInfo;
					if (m_xS57ClassCodeToStrs.contains(unObjType))
						fInfo.strType = m_xS57ClassCodeToStrs[unObjType];

					if (strAttKey.count() != strAttVal.count())
					{
						continue;
					}
					int nIdx = 0;
					foreach(QString strAtt, strAttKey)
					{
						quint32 unAttCode = strAtt.toUInt();
						if (m_xS57AttsVec.contains(unAttCode))
						{
							XS57Atts atts = m_xS57AttsVec[unAttCode];
							fInfo.strAttNames.append(atts.strName);

							QString strVal = strAttVal.at(nIdx);
							if (atts.ucAttsType == 'E'
								|| atts.ucAttsType == 'L'
								|| atts.ucAttsType == 'I')
							{
								bool bIsNum = false;
								qint32 unCode = strVal.toInt(&bIsNum);
								if (bIsNum && atts.strExpectedVals.contains(unCode))
								{
									strVal = atts.strExpectedVals[unCode];
								}
							}

							fInfo.strAttVals.append(strVal);
						}
						nIdx++;
					}

					_xRefRes.append(fInfo);
				}
			}
		}

		// 获取透明面图层信息
		foreach(QgsVectorLayer* pLayer, m_pxAreaTransLayerMap)
		{
			if (pLayer)
			{
				QgsRectangle selRect(_pt.x() - dExtend, _pt.y() + dExtend, _pt.x() + dExtend, _pt.y() - dExtend);
				QScopedPointer<QgsGeometry> selectGeomTrans(QgsGeometry::fromRect(selRect));
				QgsFeatureRequest request;
				request.setFilterRect(selRect);
				request.setFlags(QgsFeatureRequest::ExactIntersect);
				QgsFeatureIterator fit = pLayer->getFeatures(request);
				QgsFeature f;
				while (fit.nextFeature(f))
				{
					quint32 unObjType = f.attribute("key").toUInt();
					QStringList strAttKey = f.attribute("attKeys").toString().split(";");
					QStringList strAttVal = f.attribute("attVals").toString().split(";");

					HTFeatureInfo fInfo;
					if (m_xS57ClassCodeToStrs.contains(unObjType))
						fInfo.strType = m_xS57ClassCodeToStrs[unObjType];

					if (strAttKey.count() != strAttVal.count())
					{
						continue;
					}
					int nIdx = 0;
					foreach(QString strAtt, strAttKey)
					{
						quint32 unAttCode = strAtt.toUInt();
						if (m_xS57AttsVec.contains(unAttCode))
						{
							XS57Atts atts = m_xS57AttsVec[unAttCode];
							fInfo.strAttNames.append(atts.strName);

							QString strVal = strAttVal.at(nIdx);
							if (atts.ucAttsType == 'E'
								|| atts.ucAttsType == 'L'
								|| atts.ucAttsType == 'I')
							{
								bool bIsNum = false;
								qint32 unCode = strVal.toInt(&bIsNum);
								if (bIsNum && atts.strExpectedVals.contains(unCode))
								{
									strVal = atts.strExpectedVals[unCode];
								}
							}

							fInfo.strAttVals.append(strVal);
						}
						nIdx++;
					}

					_xRefRes.append(fInfo);
				}
			}
		}
		// 获取无遮挡面图层信息
		foreach(QgsVectorLayer* pLayer, m_pxAreaNormalLayerMap)
		{
			if (pLayer)
			{
				QgsRectangle selRect(_pt.x() - dExtend, _pt.y() + dExtend, _pt.x() + dExtend, _pt.y() - dExtend);
				QScopedPointer<QgsGeometry> selectGeomTrans(QgsGeometry::fromRect(selRect));
				QgsFeatureRequest request;
				request.setFilterRect(selRect);
				request.setFlags(QgsFeatureRequest::ExactIntersect);
				QgsFeatureIterator fit = pLayer->getFeatures(request);
				QgsFeature f;
				while (fit.nextFeature(f))
				{
					quint32 unObjType = f.attribute("key").toUInt();
					QStringList strAttKey = f.attribute("attKeys").toString().split(";");
					QStringList strAttVal = f.attribute("attVals").toString().split(";");

					HTFeatureInfo fInfo;
					if (m_xS57ClassCodeToStrs.contains(unObjType))
						fInfo.strType = m_xS57ClassCodeToStrs[unObjType];

					if (strAttKey.count() != strAttVal.count())
					{
						continue;
					}
					int nIdx = 0;
					foreach(QString strAtt, strAttKey)
					{
						quint32 unAttCode = strAtt.toUInt();
						if (m_xS57AttsVec.contains(unAttCode))
						{
							XS57Atts atts = m_xS57AttsVec[unAttCode];
							fInfo.strAttNames.append(atts.strName);

							QString strVal = strAttVal.at(nIdx);
							if (atts.ucAttsType == 'E'
								|| atts.ucAttsType == 'L'
								|| atts.ucAttsType == 'I')
							{
								bool bIsNum = false;
								qint32 unCode = strVal.toInt(&bIsNum);
								if (bIsNum && atts.strExpectedVals.contains(unCode))
								{
									strVal = atts.strExpectedVals[unCode];
								}
							}

							fInfo.strAttVals.append(strVal);
						}
						nIdx++;
					}

					_xRefRes.append(fInfo);
				}
			}
		}
		// 获取范围面图层信息
		foreach(QgsVectorLayer* pLayer, m_pxAreaCoverLayerMap)
		{
			if (pLayer)
			{
				QgsRectangle selRect(_pt.x() - dExtend, _pt.y() + dExtend, _pt.x() + dExtend, _pt.y() - dExtend);
				QScopedPointer<QgsGeometry> selectGeomTrans(QgsGeometry::fromRect(selRect));
				QgsFeatureRequest request;
				request.setFilterRect(selRect);
				request.setFlags(QgsFeatureRequest::ExactIntersect);
				QgsFeatureIterator fit = pLayer->getFeatures(request);
				QgsFeature f;
				while (fit.nextFeature(f))
				{
					quint32 unObjType = f.attribute("key").toUInt();
					QStringList strAttKey = f.attribute("attKeys").toString().split(";");
					QStringList strAttVal = f.attribute("attVals").toString().split(";");

					HTFeatureInfo fInfo;
					if (m_xS57ClassCodeToStrs.contains(unObjType))
						fInfo.strType = m_xS57ClassCodeToStrs[unObjType];

					if (strAttKey.count() != strAttVal.count())
					{
						continue;
					}
					int nIdx = 0;
					foreach(QString strAtt, strAttKey)
					{
						quint32 unAttCode = strAtt.toUInt();
						if (m_xS57AttsVec.contains(unAttCode))
						{
							XS57Atts atts = m_xS57AttsVec[unAttCode];
							fInfo.strAttNames.append(atts.strName);

							QString strVal = strAttVal.at(nIdx);
							if (atts.ucAttsType == 'E'
								|| atts.ucAttsType == 'L'
								|| atts.ucAttsType == 'I')
							{
								bool bIsNum = false;
								qint32 unCode = strVal.toInt(&bIsNum);
								if (bIsNum && atts.strExpectedVals.contains(unCode))
								{
									strVal = atts.strExpectedVals[unCode];
								}
							}

							fInfo.strAttVals.append(strVal);
						}
						nIdx++;
					}

					_xRefRes.append(fInfo);
				}
			}
		}
		// 获取自定义图层信息
		foreach(QgsVectorLayer* pLayer, m_pxCustomLayerMap)
		{
			if (pLayer)
			{
				QgsRectangle selRect(_pt.x() - dExtend, _pt.y() + dExtend, _pt.x() + dExtend, _pt.y() - dExtend);
				QScopedPointer<QgsGeometry> selectGeomTrans(QgsGeometry::fromRect(selRect));
				QgsFeatureRequest request;
				request.setFilterRect(selRect);
				request.setFlags(QgsFeatureRequest::ExactIntersect);
				QgsFeatureIterator fit = pLayer->getFeatures(request);
				QgsFeature f;
				while (fit.nextFeature(f))
				{
					quint32 unObjType = f.attribute("key").toUInt();
					QStringList strAttKey = f.attribute("attKeys").toString().split(";");
					QStringList strAttVal = f.attribute("attVals").toString().split(";");

					HTFeatureInfo fInfo;
					if (m_xS57ClassCodeToStrs.contains(unObjType))
						fInfo.strType = m_xS57ClassCodeToStrs[unObjType];

					if (strAttKey.count() != strAttVal.count())
					{
						continue;
					}
					int nIdx = 0;
					foreach(QString strAtt, strAttKey)
					{
						quint32 unAttCode = strAtt.toUInt();
						if (m_xS57AttsVec.contains(unAttCode))
						{
							XS57Atts atts = m_xS57AttsVec[unAttCode];
							fInfo.strAttNames.append(atts.strName);

							QString strVal = strAttVal.at(nIdx);
							if (atts.ucAttsType == 'E'
								|| atts.ucAttsType == 'L'
								|| atts.ucAttsType == 'I')
							{
								bool bIsNum = false;
								qint32 unCode = strVal.toInt(&bIsNum);
								if (bIsNum && atts.strExpectedVals.contains(unCode))
								{
									strVal = atts.strExpectedVals[unCode];
								}
							}

							fInfo.strAttVals.append(strVal);
						}
						nIdx++;
					}

					_xRefRes.append(fInfo);
				}
			}
		}
	}

	QList<QgsMapLayer*> XS57LayerAdapter::GetLayers()
	{
		QList<QgsMapLayer*> pResultCanvasLays;

		foreach(QgsVectorLayer* pLayer, m_pxCustomLayerMap)
			pResultCanvasLays.push_back(pLayer);
		foreach(QgsVectorLayer* pLayer, m_pxAreaCoverLayerMap)
			pResultCanvasLays.push_back(pLayer);
		foreach(QgsVectorLayer* pLayer, m_pxAreaNormalLayerMap)
			pResultCanvasLays.push_back(pLayer);
		foreach(QgsVectorLayer* pLayer, m_pxAreaTransLayerMap)
			pResultCanvasLays.push_back(pLayer);
		foreach(QgsVectorLayer* pLayer, m_pxLineLayerMap)
			pResultCanvasLays.push_back(pLayer);
		foreach(QgsVectorLayer* pLayer, m_pxPointLayerMap)
			pResultCanvasLays.push_back(pLayer);

		return pResultCanvasLays;
	}


	XSymbolInfo::XSymbolInfo()
	{
		pxBaseDepAreRender = new QgsCategorizedSymbolRendererV2("symbolNo");
		pxBaseHAreRender = new QgsCategorizedSymbolRendererV2("symbolNo");
		pxTopLinesRender = new QgsCategorizedSymbolRendererV2("symbolNo");
		pxNoDataLayerRender = new QgsCategorizedSymbolRendererV2("symbolNo");
	}

}