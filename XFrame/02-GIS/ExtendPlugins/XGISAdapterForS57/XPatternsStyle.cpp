#include "XPatternsStyle.h"

#define xUS 0x1F
#define xRS 0x1E

namespace XSpace_Plugins
{
	XPatternsStyle::XPatternsStyle()
	{
	}


	XPatternsStyle::~XPatternsStyle()
	{
	}

	XPatternsStyle* XPatternsStyle::fromString(const QStringList& _strRecordStr)
	{
		XPatternsStyle* pPPAT = NULL;

		foreach(QString strLine, _strRecordStr)
		{
			if (strLine == "0001" || strLine == "****")
				continue;

			qint32 nIdx = strLine.indexOf(" ");
			if (nIdx < 0){
				if (pPPAT)
					delete pPPAT;
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
				if (pPPAT)
					delete pPPAT;
				return NULL;
			}

			if (strTypeLine == "PATT" && unLenLine >= 10){
				pPPAT = new XPatternsStyle();
				pPPAT->m_strType = strLine.left(2);
				pPPAT->m_unRCID = strLine.mid(2, unLenLine - 5).toUInt();
			}
			else if (strTypeLine == "PATD" && pPPAT && unLenLine >= 55){
				if (unLenLine > 55)
					int md = 0;
				pPPAT->m_strName = strLine.left(8);
				pPPAT->m_ucDefinition = strLine.at(8).cell();
				pPPAT->m_ucFillType = strLine.at(9).cell();
				pPPAT->m_ucSpacing = strLine.at(12).cell();
				pPPAT->m_unMinDist = strLine.mid(15, 5).toUInt();
				pPPAT->m_unMaxDist = strLine.mid(20, 5).toUInt();
				pPPAT->m_xPivot = QPointF(strLine.mid(25, 5).toDouble(), strLine.mid(30, 5).toDouble());
				pPPAT->m_xBoundRect = QRectF(strLine.mid(45, 5).toDouble(), strLine.mid(50, 5).toDouble(), strLine.mid(35, 5).toDouble(), strLine.mid(40, 5).toDouble());
			}
			else if (strTypeLine == "PXPO" && pPPAT && unLenLine > 1){
				nIdx = strLine.indexOf(xUS);
				if (nIdx > 0)
					pPPAT->m_strExposition = strLine.left(nIdx);
			}
			else if (strTypeLine == "PCRF" && pPPAT && unLenLine > 0){
				if (unLenLine % 6 == 0){
					while (!strLine.isEmpty()){
						pPPAT->m_strColRef.insert(strLine.at(0), strLine.mid(1, 5));
						strLine.remove(0, 6);
					}
				}
			}
			else if (strTypeLine == "PVCT" && pPPAT && unLenLine > 0)
				pPPAT->m_strVector.append(strLine);
			else if (strTypeLine == "PBTM" && pPPAT && unLenLine > pPPAT->m_xBoundRect.width()){
				nIdx = strLine.indexOf(xUS);
				if (nIdx != pPPAT->m_xBoundRect.width()){
					delete pPPAT;
					pPPAT = NULL;
					return NULL;
				}
				pPPAT->m_strBitMap.append(strLine.left(nIdx));
			}
			else
				int md = 0;
		}

		if (pPPAT && pPPAT->m_strName.isEmpty()){
			delete pPPAT;
			pPPAT = NULL;
		}

		if (pPPAT&&pPPAT->m_strType == "SY"){
			quint32 unBitMapRows = pPPAT->m_strBitMap.count();
			if (unBitMapRows != pPPAT->m_xBoundRect.height()){
				delete pPPAT;
				pPPAT = NULL;
			}
		}

		return pPPAT;
	}

}