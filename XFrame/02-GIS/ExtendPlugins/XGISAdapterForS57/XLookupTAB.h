
#ifndef XLOOKUPTAB_HEADER
#define XLOOKUPTAB_HEADER

#include <QtCore/QtGlobal>
#include <QtCore/QStringList>
#include <QtCore/QMap>

namespace XSpace_Plugins
{
	class XLookupTAB
	{
	public:
		// 对象类型
		enum xeObjType{
			POINT_T = 'P',
			LINES_T = 'L',
			AREAS_T = 'A',
		};
		// 显示优先级
		enum xeDisPrio{
			PRIO_NODATA = '0',                  // no data fill area pattern
			PRIO_GROUP1 = '1',                  // S57 group 1 filled areas
			PRIO_AREA_1 = '2',                  // superimposed areas
			PRIO_AREA_2 = '3',                  // superimposed areas also water features
			PRIO_SYMB_POINT = '4',              // point symbol also land features
			PRIO_SYMB_LINE = '5',               // line symbol also restricted areas
			PRIO_SYMB_AREA = '6',               // area symbol also traffic areas
			PRIO_ROUTEING = '7',                // routeing lines
			PRIO_HAZARDS = '8',					// hazards
			PRIO_MARINERS = '9',                // VRM, EBL, own ship
		};

		// RADAR Priority
		enum xeRadPrio{
			RAD_OVER = 'O',            // presentation on top of RADAR
			RAD_SUPP = 'S',            // presentation suppressed by RADAR
		};

		// name of the addressed look up table set (fifth letter)
		enum xeLUPname{
			SIMPLIFIED = 'L',				// points
			PAPER_CHART = 'R',				// points
			LINES = 'S',					// lines
			PLAIN_BOUNDARIES = 'N',			// areas
			SYMBOLIZED_BOUNDARIES = 'O',	// areas
		};
		// 显示类别
		enum xeDisCat{
			DISPLAYBASE = 'D',          //
			STANDARD = 'S',				//
			OTHER = 'O',				// O for OTHER
			MARINERS_STANDARD = 'M',            // Mariner specified
			MARINERS_OTHER,                     // value not defined
			DISP_CAT_NUM,                       // value not defined
		};

	private:
		XLookupTAB();
	public:
		~XLookupTAB();

	public:
		quint32 m_unSequenceNo;	// 序号
		quint32 m_unRCID;		// RCID
		QString m_strType;		// LU
		QString m_strObjName;	// Obj名称
		xeObjType m_xeObjType;	// 类型 点/线/面
		xeDisPrio m_xeDisPrio;	// 显示优先级
		xeRadPrio m_xeRadPrio;	// RADAR 优先级
		xeLUPname m_xeLUPName;	// LUP 名称

		QMap<QString,QString>	m_strATTC;	// 属性列表
		QString	m_strINST;		// 指令字段 逗号分隔
		xeDisCat m_xeDisCat;	// 显示类型
		quint32 m_unComment;	// 描述

	private:
		static quint32 m_unNo;

	public:
		static XLookupTAB* fromString(const QStringList& _strLUPTStr);
		
	};
}
#endif