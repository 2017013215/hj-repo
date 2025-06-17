#include "XDynamicTestThread.h"
#include <XGISCanvas.h>
#include "GIS-Extend\XShapeDef.h"

XDynamicTestThread::XDynamicTestThread(QObject *parent,XGIS::XGISCanvas* _pGISCanvas)
	: QThread(parent)
	, m_pGISCanvas(_pGISCanvas)
	, m_nRumMark(0)
{
	assert(m_pGISCanvas);
}

XDynamicTestThread::~XDynamicTestThread()
{
	// 如果目标没删除-删除目标
	if(1 == m_nRumMark || 2 == m_nRumMark)
	{
		Stop();
	}

	m_pGISCanvas = nullptr;
}

void XDynamicTestThread::run()
{
	if(0 != m_nRumMark )
	{
		// 正在运行
		return;
	}
	if(0 == m_nRumMark)
	{
		// 执行初始化操作

		// 初始化动作点集合
		// 飞机
		XCoordinateGeo xAirPlaneStart(120.0,18.0,8000);
		//XCoordinateGeo xAirPlaneStop(130.0,20.0,8000);
		XCoordinateGeo xShipStart(115.0,16.0,0);
		//XCoordinateGeo xShipStop(115.0,10.0,0);

		// 分隔成100秒来更新位置
		for (int i=1;i<100;i++)
		{
			m_xAirPlanePoints.append(XCoordinateGeo((125.0-120.0)/100.0*i+120.0, (23.0-18.0)/100.0*i+18.0, 8000));
			m_xShipPoints.append(XCoordinateGeo((110.0-115.0)/100.0*i+115.0, (11.0-16.0)/100.0*i+16.0, 0));
		}

		double dAirPlaneAng = sfnGetPntAngle(QPointF(120.0,18.0),QPointF(125.0,23.0));
		double dShipAng = sfnGetPntAngle(QPointF(115.0,16.0),QPointF(110.0,11.0));

		// 创建1架轰炸机 LibID=32 ItemID=12000
		m_pAirPlaneMark = new XShapePointJB(xAirPlaneStart,32,12000,QString::fromLocal8Bit("轰-1"));
		m_pAirPlaneMark->SetUserClr(Qt::blue);
		m_pAirPlaneMark->SetHeading(dAirPlaneAng);

		// 创建1艘驱逐舰 LibID=32 ItemID=8600
		m_pShipMark = new XShapePointJB(xShipStart,32,8600,QString::fromLocal8Bit("驱-504"));
		m_pShipMark->SetHeading(dShipAng);

		// 添加到画布
		m_pGISCanvas->AddShape(m_pAirPlaneMark);
		m_pGISCanvas->AddShape(m_pShipMark);
	}

	m_nRumMark = 1; // 设置运行状态
	while(0 != m_nRumMark)
	{
		if(2 == m_nRumMark)
		{
			// 暂停
			msleep(1000);
			continue;
		}
		
		// 计算角度
		double dAirAng = sfnGetPntAngle(m_pAirPlaneMark->GetPos(),QPointF(m_xAirPlanePoints.first().m_dLontitude,m_xAirPlanePoints.first().m_dLatitude));
		double dShipAng = sfnGetPntAngle(m_pShipMark->GetPos(),QPointF(m_xShipPoints.first().m_dLontitude,m_xShipPoints.first().m_dLatitude));
		// 更新最新位置
		m_pAirPlaneMark->SetPosGeo(m_xAirPlanePoints.first());
		m_pShipMark->SetPosGeo(m_xShipPoints.first());
// 		m_pAirPlaneMark->SetHeading(dAirAng);
// 		m_pShipMark->SetHeading(dShipAng);

		m_xAirPlanePoints.removeFirst();
		m_xShipPoints.removeFirst();

		if(m_xAirPlanePoints.isEmpty() && m_xShipPoints.isEmpty())
		{
			Stop();
			break;
		}
		msleep(1000);
	}
}

void XDynamicTestThread::Stop()
{
	if(0 == m_nRumMark)
		return ;

	m_nRumMark = 0;

	// 删除目标
	assert(m_pGISCanvas);
	m_pGISCanvas->RemoveShapeByID(m_pAirPlaneMark->GetID());
	m_pAirPlaneMark = nullptr;
	m_pGISCanvas->RemoveShapeByID(m_pShipMark->GetID());
	m_pShipMark = nullptr;
	m_xAirPlanePoints.clear();
	m_xShipPoints.clear();
}

void XDynamicTestThread::Pause()
{
	m_nRumMark = 2;
}

void XDynamicTestThread::Continue()
{
	if(2 != m_nRumMark)
		return ;

	m_nRumMark = 1;
}
