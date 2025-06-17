#pragma once
#include <QPoint>
#include <QString>
#include <QVector>
#include <QSize>

#define COOR_FAILD_VAL -1000000

namespace XGIS
{
	class Q_JBItem;
	struct ExceptiveInfo;
	class XJBItem
	{
		friend class XJBDrawHelper;
		friend class XJBRubber;
	public:
		XJBItem(quint32 _unLibID, quint32 _unJBID);
		~XJBItem(void);

	public:
		void SetSize(int _nW,int _nH);
		void SetAngle(double _dAngle);

		void SetCenterPnt(QPointF _cPnt);

		// 添加坐标点
		void AddPoint(QPointF _cGeoPos);
		const quint32 GetPtCount() const;
		ExceptiveInfo* GetExceptiveInfo() const;

	private:
		QPointF m_cCenterPnt;

		QString m_strJBName;
		QSize m_cSize;					// JB的大小
		double m_dAngle;				// 角度 正北为0
		ExceptiveInfo* m_pExceptiveInfo;			// 特殊绘制信息

		QVector<QPointF> m_cUserPnts;		// 用户设定的坐标点 -经纬度

		// JB绘制信息
		Q_JBItem* m_pJBDrawInfo;
	};
}