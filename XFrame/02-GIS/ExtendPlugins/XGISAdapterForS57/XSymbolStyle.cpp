#include "XSymbolStyle.h"

#define xUS 0x1F
#define xRS 0x1E

namespace XSpace_Plugins
{
	XSymbolStyle::XSymbolStyle()
	{
	}


	XSymbolStyle::~XSymbolStyle()
	{
	}

	XSymbolStyle* XSpace_Plugins::XSymbolStyle::formString(const QStringList& _strRecordStr)
	{
		XSymbolStyle* pSymbol = NULL;

		foreach(QString strLine, _strRecordStr)
		{
			if (strLine == "0001" || strLine == "****")
				continue;

			qint32 nIdx = strLine.indexOf(" ");
			if (nIdx < 0){
				if (pSymbol)
					delete pSymbol;
				return NULL;
			}
			QString strTypeLine = strLine.left(nIdx);
			strLine.remove(0, nIdx);
			while (!strLine.isEmpty() && strLine.at(0) == ' ')
				strLine.remove(0, 1);

			QString strLenLine = QString();
			quint32 unLenLine = 0;
			while (!strLine.isEmpty() && strLine.at(0).isDigit())
			{
				strLenLine.push_back(strLine.at(0));
				strLine.remove(0, 1);
				if (strLenLine.toUInt() == strLine.length()){
					unLenLine = strLenLine.toUInt();
					break;
				}
			}
			if (unLenLine == 0){
				if (pSymbol)
					delete pSymbol;
				return NULL;
			}

			if (strTypeLine == "SYMB" && unLenLine >= 10){
				pSymbol = new XSymbolStyle();
				pSymbol->m_strType = strLine.left(2);
				pSymbol->m_unRCID = strLine.mid(2, unLenLine - 5).toUInt();
			}
			else if (strTypeLine == "SYMD" && pSymbol && unLenLine >= 39){
				if (unLenLine > 39)
					int md = 0;
				pSymbol->m_strName = strLine.left(8);
				pSymbol->m_ucDefinition = strLine.at(8).cell();
				pSymbol->m_xPivot = QPointF(strLine.mid(9, 5).toDouble(), strLine.mid(14, 5).toDouble());
				pSymbol->m_xBoundRect = QRectF(strLine.mid(29, 5).toDouble(), strLine.mid(34, 5).toDouble(), strLine.mid(19, 5).toDouble(), strLine.mid(24, 5).toDouble());
			}
			else if (strTypeLine == "SXPO" && pSymbol && unLenLine > 0){
				nIdx = strLine.indexOf(xUS);
				if (nIdx > 0)
					pSymbol->m_strExposition = strLine.left(nIdx);
			}
			else if (strTypeLine == "SCRF" && pSymbol && unLenLine > 0){
				if (unLenLine % 6 == 0){
					while (!strLine.isEmpty()){
						pSymbol->m_strColRef.insert(strLine.at(0), strLine.mid(1, 5));
						strLine.remove(0, 6);
					}
				}
			}
			else if (strTypeLine == "SVCT" && pSymbol && unLenLine > 0)
				pSymbol->m_strVector.append(strLine);
			else if (strTypeLine == "SBTM" && pSymbol && unLenLine > pSymbol->m_xBoundRect.width()){
				nIdx = strLine.indexOf(xUS);
				if (nIdx != pSymbol->m_xBoundRect.width()){
					delete pSymbol;
					pSymbol = NULL;
					return NULL;
				}
				pSymbol->m_strBitMap.append(strLine.left(nIdx));
			}
			else
				int md = 0;
		}

		if (pSymbol && pSymbol->m_strName.isEmpty()){
			delete pSymbol;
			pSymbol = NULL;
		}
		if (pSymbol&&pSymbol->m_ucDefinition == 'R'){
			quint32 unBitMapRows = pSymbol->m_strBitMap.count();
			if (unBitMapRows != pSymbol->m_xBoundRect.height()){
				delete pSymbol;
				pSymbol = NULL;
			}

			// Éú³ÉBMPÍ¼Ïñ
		}

		return pSymbol;
	}
}