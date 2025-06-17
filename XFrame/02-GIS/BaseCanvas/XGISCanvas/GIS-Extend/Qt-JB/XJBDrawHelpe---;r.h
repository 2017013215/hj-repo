#ifndef XJBDRAWHELPER_H
#define XJBDRAWHELPER_H

#include "XGISCanvasDef.h"
#include <QPainter>

namespace XGIS
{
	class LINE_ATTR;
	class Q_JBItemUnitInfo;
	class XShapePointJB;
	class XShapeLineJB;
	class XJBDrawHelper
	{
	private:
		XJBDrawHelper();
		~XJBDrawHelper();

	public:
		// 绘制点JB
		static const bool DrawPointJB(QPainter* _p, const XShapePointJB* _pShapePtr , QPointF& _qScreenPnt, bool _bIsDetail = false);

		// 绘制线JB
		static const bool DrawLineJB(QPainter* _p, const XShapeLineJB* _pLinePtr, QList<QPointF>& _qScrPnts);

	private:
		static void dwBLine(Q_JBItemUnitInfo& _cRefUnit, bool _bIsDetail = false);
		static void dwBBLine(Q_JBItemUnitInfo& _cRefUnit, bool _bIsDetail = false);
		static void dwPLine(Q_JBItemUnitInfo& _cRefUnit, bool _bIsDetail = false);
		static void dwRect(Q_JBItemUnitInfo& _cRefUnit, bool _bIsDetail = false);
		static void dwEllipse(Q_JBItemUnitInfo& _cRefUnit, bool _bIsDetail = false);
		static void dwPie(Q_JBItemUnitInfo& _cRefUnit, bool _bIsDetail = false);
		static void dwArc(Q_JBItemUnitInfo& _cRefUnit, bool _bIsDetail = false);
		static void dwPolygon(Q_JBItemUnitInfo& _cRefUnit, bool _bIsDetail = false);
		static void dwText(Q_JBItemUnitInfo& _cRefUnit, bool _bIsDetail = false);
		static void dwTextBorder(Q_JBItemUnitInfo& _cRefUnit, bool _bIsDetail = false);
		static void setLineAttr(const LINE_ATTR& _pLineAttr);
		static int calcLinePts(Q_JBItemUnitInfo& _cRefUnit, int flag);
		static void calcBezier(QVector<QPointF>& _cPnts, int _nPntCount, QVector<QPointF>& _cRefResultPnts);
		static int quadrant(int x, int y);

	private:
		static QPainter* ms_pPainter;
		static float ms_fJBFactor;
		static float ms_fTextFactor;
		static QRect ms_cBoxRct;
		static double ms_dScale;
		static double ms_dScaleX;
		static double ms_dScaleY;
		static bool ms_bIsUseUserClr;
		static QColor ms_cUserClr;
		static quint16 ms_unBezierPrecision;
		static const int ms_nConstCurveFlag;
	};
}


#endif // XJBDRAWHELPER_H
