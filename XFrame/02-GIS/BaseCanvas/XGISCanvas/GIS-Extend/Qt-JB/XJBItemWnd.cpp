#include "XJBItemWnd.h"
#include <QPaintEvent>
#include <QPainter>
#include "xjbdrawhelper.h"
#include "GIS-Extend\XShapeDef.h"

namespace XGIS
{
	XJBItemWnd::XJBItemWnd(QWidget* _pParentWnd)
		: QDialog(_pParentWnd,nullptr)
	{
		setFixedSize(700,700);
	}

	XJBItemWnd::~XJBItemWnd(void)
	{
	}

	void XJBItemWnd::ShowJB(quint32 _unLibID, quint32 _unItemID, bool _bIsShowDetail /*= true*/)
	{
		m_unLibID = _unLibID;
		m_unItemID = _unItemID;
		this->exec();
	}

	void XJBItemWnd::paintEvent(QPaintEvent* _paintEvt)
	{
		QPainter pntThis(this);
		XShapePointJB shapePntJB(XCoordinateGeo(0,0,0),m_unLibID,m_unItemID);
		shapePntJB.SetSize(QSize(700,700));
		XJBDrawHelper::DrawPointJB(&pntThis,&shapePntJB,QPointF(350,350),true,false);
	}
}