#include "XLookupTAB.h"


#define xUS 0x1F
#define xRS 0x1E

namespace XSpace_Plugins
{
	quint32 XLookupTAB::m_unNo = 0;
	XLookupTAB::XLookupTAB()
	{
	}

	XLookupTAB::~XLookupTAB()
	{
	}

	XLookupTAB* XLookupTAB::fromString(const QStringList& _strLUPTStr)
	{
		XLookupTAB* pLUPT = NULL;

		foreach(QString strLine, _strLUPTStr)
		{
			if (strLine == "0001" || strLine == "****")
				continue;

			qint32 nIdx = strLine.indexOf(" ");
			if (nIdx < 0){
				if (pLUPT)
					delete pLUPT;
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
				if (pLUPT)
					delete pLUPT;
				return NULL;
			}

			if (strTypeLine == "LUPT"/* && unLenLine >= 36*/)
			{
				pLUPT = new XLookupTAB();

				pLUPT->m_strType = strLine.left(2);
				pLUPT->m_unSequenceNo = XLookupTAB::m_unNo;
				XLookupTAB::m_unNo += 1;
				strLine.remove(0, 2);
				pLUPT->m_unRCID = strLine.left(5).toUInt();
				strLine.remove(0, 8);
				pLUPT->m_strObjName = strLine.left(6);
				strLine.remove(0, 6);

				pLUPT->m_xeObjType = (xeObjType)strLine.at(0).cell();
				pLUPT->m_xeDisPrio = (xeDisPrio)strLine.at(5).cell();
				pLUPT->m_xeRadPrio = (xeRadPrio)strLine.at(6).cell();
				pLUPT->m_xeLUPName = (xeLUPname)strLine.at(11).cell();
			}
			else if (strTypeLine == "ATTC" && pLUPT && unLenLine > 1)
			{
				nIdx = strLine.indexOf(xUS);
				while (nIdx > 6)
				{
					QString strOne = strLine.left(nIdx);
					strLine.remove(0, nIdx + 1);
					QString strKey = strOne.left(6);
					strOne.remove(0, 6);
					pLUPT->m_strATTC.insert(strKey, strOne);
					nIdx = strLine.indexOf(xUS);
				}
			}
			else if (strTypeLine == "INST" && pLUPT && unLenLine > 1)
			{
				nIdx = strLine.indexOf(xUS);
				while (nIdx > 0)
				{
					pLUPT->m_strINST.push_back(strLine.left(nIdx));
					strLine.remove(0, nIdx + 1);
					nIdx = strLine.indexOf(xUS);
				}
			}
			else if (strTypeLine == "DISC" && pLUPT && unLenLine > 1)
			{
				pLUPT->m_xeDisCat = (xeDisCat)strLine.at(0).cell();
			}
			else if (strTypeLine == "LUCM" && pLUPT && unLenLine > 1)
			{
				nIdx = strLine.indexOf(xUS);
				if (nIdx > 0)
					pLUPT->m_unComment = strLine.left(nIdx).toUInt();
			} else if (strTypeLine != "ATTC" && strTypeLine != "INST" && strTypeLine != "DISC" && strTypeLine != "LUCM")
				int md = 0;
		}

		return pLUPT;
	}

}