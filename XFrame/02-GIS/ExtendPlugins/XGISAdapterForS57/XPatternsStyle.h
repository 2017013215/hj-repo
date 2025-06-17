#ifndef XPATTERNSSTYLE_HEADER
#define XPATTERNSSTYLE_HEADER
// PATT
#include <QtCore/QStringList>
#include <QtCore/QPointF>
#include <QtCore/QRectF>
#include <QtCore/QMap>

namespace XSpace_Plugins
{
	class XPatternsStyle
	{
	private:
		XPatternsStyle();
	public:
		~XPatternsStyle();

		static XPatternsStyle* fromString(const QStringList& _strRecordStr);

	public:
		quint32 m_unRCID;
		QString m_strType;		// LS简单线
		QString m_strName;		// 名称

		uchar	m_ucDefinition;
		uchar	m_ucFillType;	// 填充类型-// first character 'S' or 'L', for staggered or linear
		uchar	m_ucSpacing;	// 间隔符

		quint32 m_unMinDist;
		quint32 m_unMaxDist;
		QPointF	m_xPivot;		// 中心点x-y
		QRectF	m_xBoundRect;	// 包围盒范围
		QString m_strExposition;// 阐述
		QMap<QString,QString> m_strColRef;// 颜色引用
		QStringList m_strVector;// 矢量绘制命令列表 单行以分号分隔
		QStringList m_strBitMap;// 位图绘制命令列表 
	};
}
#endif