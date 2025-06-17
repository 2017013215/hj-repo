#ifndef XSYMBOLSTYLE_HEADER
#define XSYMBOLSTYLE_HEADER

// SYMB
#include <QtCore/QStringList>
#include <QtCore/QPointF>
#include <QtCore/QRectF>
#include <QtCore/QMap>

namespace XSpace_Plugins
{
	class XSymbolStyle
	{
	private:
		XSymbolStyle();
	public:
		~XSymbolStyle();

		static XSymbolStyle* formString(const QStringList& _strRecordStr);

	public:
		quint32 m_unRCID;
		QString m_strType;		// LS简单线
		QString m_strName;		// 名称

		uchar	m_ucDefinition;
		QPointF	m_xPivot;		// 中心点x-y
		QRectF	m_xBoundRect;	// 包围盒范围
		QString m_strExposition;// 阐述
		QMap<QString,QString> m_strColRef;// 颜色引用
		QStringList m_strVector;// 矢量绘制命令列表 单行以分号分隔
		QStringList m_strBitMap;// 位图绘制命令列表 
	};
}
#endif