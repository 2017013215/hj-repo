#ifndef XS52LIB_HEADER
#define XS52LIB_HEADER

#include "XTypeDef.h"
#include <QtCore/QString>
#include <QtCore/QTextStream>
#include <QtCore/QFile>

#include "XColorTable.h"
#include "XLookupTAB.h"
#include "XLineStyle.h"
#include "XPatternsStyle.h"
#include "XSymbolStyle.h"
#include "XS57Types.h"

namespace XSpace_Plugins
{
	class XS52Lib
	{
	private:
		XS52Lib();
	public:
		~XS52Lib();

		static XS52Lib* fromFile(const QString& _strFilePath);

		quint32 GetColorByKey(const QString& _strKey, QColor& _xRefRes);

		QList<XLookupTAB*> GetLoopTAB(const XSpace_Plugins::xePRIM& _xePRIM, const QString& _strName);

		XLookupTAB* GetSingleLoopTAB(const XLookupTAB::xeLUPname& _xeLPType, const QString& _strName);

		XSymbolStyle* GetSymbolStyle(const QString& _strName);

		XPatternsStyle* GetPatternsStyle(const QString& _strName);

		XLineStyle* GetLineStyle(const QString& _strName);

		XLookupTAB::xeLUPname GetBoundStyle();

	private:
		quint32 parseSymbolRecord(const QStringList& _strRecord);

	public:
		QMap<QString, XColorTable*> m_xColorTABS;	// 
		QMap<XLookupTAB::xeLUPname, QMultiMap<QString, XLookupTAB*>> m_xLUPTSet;
		QMap<QString, XLineStyle*> m_xLineStyles;
		QMap<QString, XPatternsStyle*> m_xPatternsStyles;
		QMap<QString, XSymbolStyle*> m_xSymbolStyles;

		QString m_strCurColorStyle;
		XLookupTAB::xeLUPname m_xeSymbolStyle;		// 图形符号样式	(纸海图.默认)	(简化.可选) 适用于点图层
		XLookupTAB::xeLUPname m_xeBoundaryStyle;	// 边界样式		(符号化.可选)	(平面.默认) 适用于面图层
	};
}
#endif