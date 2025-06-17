#include "XLineStyle.h"

#define xUS 0x1F
#define xRS 0x1E

namespace XSpace_Plugins
{
	XLineStyle::XLineStyle()
	{
	}


	XLineStyle::~XLineStyle()
	{
	}

	XLineStyle* XLineStyle::fromString(const QStringList& _strRecordStr)
	{
		XLineStyle* pLNST = NULL;

		foreach(QString strLine, _strRecordStr)
		{
			if (strLine == "0001" || strLine == "****")
				continue;

			qint32 nIdx = strLine.indexOf(" ");
			if (nIdx < 0){
				if (pLNST)
					delete pLNST;
				return NULL;
			}
			QString strTypeLine = strLine.left(nIdx);
			strLine.remove(0, nIdx);
			while (!strLine.isEmpty() && strLine.at(0).cell() == ' ')
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
				if (pLNST)
					delete pLNST;
				return NULL;
			}
			
			if (strTypeLine == "LNST" && unLenLine >= 10){
				pLNST = new XLineStyle();
				pLNST->m_strType = strLine.left(2);
				pLNST->m_unRCID = strLine.mid(2, unLenLine - 5).toUInt();
			}
			else if (strTypeLine == "LIND" && pLNST && unLenLine >= 38 ){
				if (unLenLine > 38)
					int md = 0;
				pLNST->m_strName = strLine.left(8);
				pLNST->m_xPivot = QPointF(strLine.mid(8,5).toDouble(),strLine.mid(13,5).toDouble());
				pLNST->m_xBoundRect = QRectF(strLine.mid(28, 5).toDouble(), strLine.mid(33, 5).toDouble(), strLine.mid(18, 5).toDouble(), strLine.mid(23, 5).toDouble());
			}
			else if (strTypeLine == "LXPO" && pLNST && unLenLine > 1){
				nIdx = strLine.indexOf(xUS);
				if (nIdx > 0)
					pLNST->m_strExposition = strLine.left(nIdx);
			}
			else if (strTypeLine == "LCRF" && pLNST && unLenLine > 0){
				if (unLenLine % 6 == 0){
					while (!strLine.isEmpty()){
						pLNST->m_strColRef.insert(strLine.at(0), strLine.mid(1, 5));
						strLine.remove(0, 6);
					}
				}
			}
			else if (strTypeLine == "LVCT" && pLNST && unLenLine > 0)
				pLNST->m_strVector.append(strLine);
			else
				int md = 0;
		}

		if (pLNST && pLNST->m_strName.isEmpty()){
			delete pLNST;
			pLNST = NULL;
		}

		return pLNST;
	}
}