#include "XS52Lib.h"

namespace XSpace_Plugins
{
	XS52Lib::XS52Lib()
	{
		m_strCurColorStyle = QString();
		m_xeSymbolStyle = XLookupTAB::PAPER_CHART;
		m_xeBoundaryStyle = XLookupTAB::PLAIN_BOUNDARIES;
	}


	XS52Lib::~XS52Lib()
	{
		foreach(XColorTable* pColorTAB, m_xColorTABS)
		{
			if (pColorTAB)
				delete pColorTAB;
		}
		m_xColorTABS.clear();

		QMap<QString, XLookupTAB*> xLoops;
		foreach(xLoops, m_xLUPTSet)
		{
			foreach(XLookupTAB* pLUPT, xLoops)
			{
				if (pLUPT)
					delete pLUPT;
			}
			xLoops.clear();
		}
		m_xLUPTSet.clear();

		foreach(XLineStyle* pLNST, m_xLineStyles)
		{
			if (pLNST)
				delete pLNST;
			pLNST = NULL;
		}
		m_xLineStyles.clear();

		foreach(XPatternsStyle* pPATT, m_xPatternsStyles)
		{
			if (pPATT)
				delete pPATT;
			pPATT = NULL;
		}
		m_xPatternsStyles.clear();

		foreach(XSymbolStyle* pSymbol, m_xSymbolStyles)
		{
			if (pSymbol)
				delete pSymbol;
			pSymbol = NULL;
		}
		m_xSymbolStyles.clear();
	}

	XS52Lib* XS52Lib::fromFile(const QString& _strFilePath)
	{
		XS52Lib* pLib = NULL;

		QFile file(_strFilePath);
		quint32 error = 0;
		if (file.open(QIODevice::ReadOnly))
		{
			pLib = new XS52Lib();
			QTextStream stream(&file);
			QString strLine = QString();
			QStringList strRecord = QStringList();
			bool bIsNewRecord = false;
			while (!stream.atEnd())
			{
				strLine = stream.readLine();
				if (strLine == "0001")
					bIsNewRecord = true;

				if (bIsNewRecord)
					strRecord.append(strLine);

				if (strLine == "****")
				{
					error = pLib->parseSymbolRecord(strRecord);
					if (error != 0){
						delete pLib;
						pLib = NULL;
						return pLib;
					}
					strRecord.clear();
					bIsNewRecord = false;
				}
			}

			file.close();
		}
		else
			return pLib;

		return pLib;
	}

	quint32 XS52Lib::parseSymbolRecord(const QStringList& _strRecord)
	{
		if (_strRecord.count() <= 3)
			return -1;
		if (_strRecord.first() != "0001" || _strRecord.last() != "****")
			return -1;

		QString strType = _strRecord.at(1).left(4);
		quint32 error = 0;
		// 色表文件
		if (strType == "COLS"){
			XColorTable* pColorTAB = XColorTable::fromString(_strRecord);
			if (pColorTAB == NULL)
				return -1;
			m_xColorTABS.insert(pColorTAB->m_strName, pColorTAB);
			if (m_strCurColorStyle.isEmpty())
				m_strCurColorStyle = pColorTAB->m_strName;
		}
		else if (strType == "LUPT"){
			//if (_strRecord.at(1).contains("PLAIN_BOUNDARIES")) // 普通的边界 
			{
				XLookupTAB* pLUPT = XLookupTAB::fromString(_strRecord);
				if (pLUPT == NULL)
					return -1;
				m_xLUPTSet[pLUPT->m_xeLUPName].insert(pLUPT->m_strObjName, pLUPT);
			}
		}
		else if (strType == "LNST"){
			XLineStyle* pLNST = XLineStyle::fromString(_strRecord);
			if (pLNST == NULL)
				return -1;
			m_xLineStyles[pLNST->m_strName] = pLNST;
			pLNST = NULL;
		}
		else if (strType == "PATT"){
			XPatternsStyle* pPATT = XPatternsStyle::fromString(_strRecord);
			if (pPATT == NULL)
				return -1;
			m_xPatternsStyles[pPATT->m_strName] = pPATT;
			pPATT = NULL;
		}
		else if (strType == "SYMB"){
			XSymbolStyle* pSYMB = XSymbolStyle::formString(_strRecord);
			if (pSYMB == NULL)
				return -1;
			m_xSymbolStyles[pSYMB->m_strName] = pSYMB;
			pSYMB = NULL;
		}
		else
			int md = 0;
		return 0;
	}

	quint32 XS52Lib::GetColorByKey(const QString& _strKey, QColor& _xRefRes)
	{
		bool bRes = false;
		if (m_xColorTABS[m_strCurColorStyle] != NULL)
			bRes = m_xColorTABS[m_strCurColorStyle]->GetColor(_strKey,_xRefRes);
		if (bRes == false)
			return -1;

		return 0;
	}

	QList<XLookupTAB*> XS52Lib::GetLoopTAB(const XSpace_Plugins::xePRIM& _xePRIM, const QString& _strName)
	{

		QList<XLookupTAB*> xLups;

		XLookupTAB::xeLUPname xeLuName = XLookupTAB::PAPER_CHART;
		if (_xePRIM == XSpace_Plugins::xePRIM_P || _xePRIM == XSpace_Plugins::xePRIM_N){
			if (m_xeSymbolStyle != XLookupTAB::PAPER_CHART)
				xeLuName = XLookupTAB::SIMPLIFIED;
		}
		else if (_xePRIM == XSpace_Plugins::xePRIM_A){
			if (m_xeBoundaryStyle == XLookupTAB::PLAIN_BOUNDARIES)
				xeLuName = XLookupTAB::PLAIN_BOUNDARIES;
			else
				xeLuName = XLookupTAB::SYMBOLIZED_BOUNDARIES;
		}
		else
			xeLuName = XLookupTAB::LINES;

		if (m_xLUPTSet.contains(xeLuName)){
			if (m_xLUPTSet[xeLuName].contains(_strName))
				xLups = m_xLUPTSet[xeLuName].values(_strName);
		}

		return xLups;
	}

	XSymbolStyle* XS52Lib::GetSymbolStyle(const QString& _strName)
	{
		if (m_xSymbolStyles.contains(_strName))
			return m_xSymbolStyles[_strName];
		return NULL;
	}

	XPatternsStyle* XS52Lib::GetPatternsStyle(const QString& _strName)
	{
		if (m_xPatternsStyles.contains(_strName))
			return m_xPatternsStyles[_strName];
		return NULL;
	}

	XLineStyle* XS52Lib::GetLineStyle(const QString& _strName)
	{
		if (m_xLineStyles.contains(_strName))
			return m_xLineStyles[_strName];
		return NULL;
	}

	XLookupTAB* XS52Lib::GetSingleLoopTAB(const XLookupTAB::xeLUPname& _xeLPType, const QString& _strName)
	{
		QList<XLookupTAB*> pResList;
		if (m_xLUPTSet.contains(_xeLPType)){
			if (m_xLUPTSet[_xeLPType].contains(_strName))
				pResList = m_xLUPTSet[_xeLPType].values(_strName);
		}
		if (pResList.count() == 1)
			return pResList.first();
		return NULL;
	}

	XLookupTAB::xeLUPname XS52Lib::GetBoundStyle()
	{
		return m_xeBoundaryStyle;
	}

}