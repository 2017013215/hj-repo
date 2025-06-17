#include "XS57DataDoc.h"
#include "XS57Types.h"

#include <QtCore/QFileInfo>
#include "XS57FileInfo.h"
#include "XS57Reader.h"
#include "XS57Document.h"

namespace XSpace_Plugins
{
	xIMPLEMENT_SERIAL(XS57DataDoc, XDocument, xPLUGINS_S57_VERSION_1_0)
	XS57DataDoc::XS57DataDoc()
	{
		m_pxLoadThread = new XLoadThread(this);
		m_bIsValid = false;
		m_strFilePath = QString();
		m_bIsChanged = false;
		m_pxOrignFileInfo = NULL;

		// 初始化内存池
		XMemPoolCfg xPoolCfg;
		xPoolCfg.blockCfgList.push_back(XMemBlockCfg(sizeof(QVector3D), 65536));
		xPoolCfg.blockCfgList.push_back(XMemBlockCfg(sizeof(QVector2D), 65536));
		m_xMemPool.InitMemPool(xPoolCfg);
		m_pxAreaFeatures.clear();
		m_pxLineFeatures.clear();
		m_pxPointFeatures.clear();
	}

	XS57DataDoc::~XS57DataDoc()
	{
		saveCache();
		clear();
	}

	void XS57DataDoc::Serialize(XArchive& _xArchive)
	{
		if (!IsSerialize())
			return;

		int nClassVersion = 0xFFFF;
		if (_xArchive.IsStoring())
		{
			nClassVersion = GetRuntimeClass()->GetClassVersion();
			if (nClassVersion >= xPLUGINS_S57_VERSION_1_0)
			{
				XObject::Serialize(_xArchive);

				_xArchive << m_bIsValid;
				_xArchive << m_strFilePath;

				_xArchive << m_xDSID.unIdentifyID << m_xDSID.strAddress << m_xDSID.strName << m_xDSID.unID;
				_xArchive << m_xDSID.eEXPP << m_xDSID.eINTU << m_xDSID.strDSName << m_xDSID.strEDTN << m_xDSID.strUPDN << m_xDSID.xUPDT
					<< m_xDSID.xISDT << m_xDSID.dS57Number << m_xDSID.ePRSP << m_xDSID.strPSDN << m_xDSID.strPRED << m_xDSID.ePROF
					<< m_xDSID.strAGEN << m_xDSID.strCOMT;

				_xArchive << m_xDSPM.unIdentifyID << m_xDSPM.strRCNM << m_xDSPM.eHDAT << m_xDSPM.eVDAT << m_xDSPM.eSDAT << m_xDSPM.unCSCL << m_xDSPM.eDUNI << m_xDSPM.eCOUN
					<< m_xDSPM.unCOMF << m_xDSPM.unSOMF << m_xDSPM.strCOMT << m_xDSPM.xDSPR.eType << m_xDSPM.xDSPR.dProjParam[0] << m_xDSPM.xDSPR.dProjParam[1]
					<< m_xDSPM.xDSPR.dProjParam[2] << m_xDSPM.xDSPR.dProjParam[3] << m_xDSPM.xDSPR.dEAS << m_xDSPM.xDSPR.dNOR << m_xDSPM.xDSPR.unFPMF << m_xDSPM.xDSPR.strCOMT
					<< m_xDSPM.xDSRC.unRPID << m_xDSPM.xDSRC.dRYCO << m_xDSPM.xDSRC.dRXCO << m_xDSPM.xDSRC.eCURP << m_xDSPM.xDSRC.unFPMF << m_xDSPM.xDSRC.dRXVL << m_xDSPM.xDSRC.dRYVL
					<< m_xDSPM.xDSRC.strCOMT;

				_xArchive << m_pxResultAreaSeq.count();
				foreach(XFeature* pAreaF, m_pxResultAreaSeq)
					featureSerialize(_xArchive, pAreaF);

				_xArchive << m_pxResultLineSeq.count();
				foreach(XFeature* pLineF, m_pxResultLineSeq)
					featureSerialize(_xArchive, pLineF);

				_xArchive << m_pxResultPointSeq.count();
				foreach(XFeature* pPotF, m_pxResultPointSeq)
					featureSerialize(_xArchive, pPotF);
			}
		}
		else if (_xArchive.IsLoading())
		{
			nClassVersion = _xArchive.GetObjectVersion();
			if (nClassVersion >= xPLUGINS_S57_VERSION_1_0)
			{
				XObject::Serialize(_xArchive);
				_xArchive >> m_bIsValid;
				_xArchive >> m_strFilePath;

				quint32 unEnum = 0;
				_xArchive >> m_xDSID.unIdentifyID >> m_xDSID.strAddress >> m_xDSID.strName >> m_xDSID.unID;
				_xArchive >> unEnum;
				m_xDSID.eEXPP = XS57_Struct_DSID::xeEXPP(unEnum);
				_xArchive >> unEnum;
				m_xDSID.eINTU = XS57_Struct_DSID::xeINTU(unEnum);
				_xArchive >> m_xDSID.strDSName >> m_xDSID.strEDTN >> m_xDSID.strUPDN >> m_xDSID.xUPDT
					>> m_xDSID.xISDT >> m_xDSID.dS57Number >> unEnum;
				m_xDSID.ePRSP = XS57_Struct_DSID::xePRSP(unEnum);
				_xArchive >> m_xDSID.strPSDN >> m_xDSID.strPRED >> unEnum;
				m_xDSID.ePROF = XS57_Struct_DSID::xePROF(unEnum);
				_xArchive >> m_xDSID.strAGEN >> m_xDSID.strCOMT;

				_xArchive >> m_xDSPM.unIdentifyID >> m_xDSPM.strRCNM >> unEnum;
				m_xDSPM.eHDAT = xeHORDAT(unEnum);
				_xArchive >> unEnum;
				m_xDSPM.eVDAT = xeVERDAT(unEnum);
				_xArchive >> unEnum;
				m_xDSPM.eSDAT = xeVERDAT(unEnum);
				_xArchive >> m_xDSPM.unCSCL >> unEnum;
				m_xDSPM.eDUNI = xeDUNITS(unEnum);
				_xArchive >> unEnum;
				m_xDSPM.eCOUN = xeCUNITS(unEnum);
				_xArchive >> m_xDSPM.unCOMF >> m_xDSPM.unSOMF >> m_xDSPM.strCOMT >> unEnum;
				m_xDSPM.xDSPR.eType = XS57_Struct_DSPR::xeProjType(unEnum);
				_xArchive >> m_xDSPM.xDSPR.dProjParam[0] >> m_xDSPM.xDSPR.dProjParam[1]
					>> m_xDSPM.xDSPR.dProjParam[2] >> m_xDSPM.xDSPR.dProjParam[3] >> m_xDSPM.xDSPR.dEAS >> m_xDSPM.xDSPR.dNOR >> m_xDSPM.xDSPR.unFPMF >> m_xDSPM.xDSPR.strCOMT
					>> m_xDSPM.xDSRC.unRPID >> m_xDSPM.xDSRC.dRYCO >> m_xDSPM.xDSRC.dRXCO >> unEnum;
				m_xDSPM.xDSRC.eCURP = XS57_Struct_DSRC::xeCURP(unEnum);
				_xArchive >> m_xDSPM.xDSRC.unFPMF >> m_xDSPM.xDSRC.dRXVL >> m_xDSPM.xDSRC.dRYVL >> m_xDSPM.xDSRC.strCOMT;

				quint32 unCount = 0;
				_xArchive >> unCount;
				for (quint32 unIdx = 0; unIdx < unCount; unIdx++){
					XFeature* pAreaF = new XFeature();
					pAreaF->pxParentDoc = this;
					pAreaF->eGeoType = XFeature::xeArea;
					featureSerialize(_xArchive, pAreaF);
					m_pxResultAreaSeq.append(pAreaF);

					QMultiMap<quint32, QList<XFeature*>>::Iterator itr = m_pxAreaFeatures.find(pAreaF->unObjType);
					if (m_pxAreaFeatures.contains(pAreaF->unObjType) == false)
						itr = m_pxAreaFeatures.insert(pAreaF->unObjType, QList<XFeature*>());
					itr->push_back(pAreaF);
				}

				unCount = 0;
				_xArchive >> unCount;
				for (quint32 unIdx = 0; unIdx < unCount; unIdx++){
					XFeature* pLineF = new XFeature();
					pLineF->pxParentDoc = this;
					pLineF->eGeoType = XFeature::xeLine;
					featureSerialize(_xArchive, pLineF);
					m_pxResultLineSeq.append(pLineF);

					QMultiMap<quint32, QList<XFeature*>>::Iterator itr = m_pxLineFeatures.find(pLineF->unObjType);
					if (m_pxLineFeatures.contains(pLineF->unObjType) == false)
						itr = m_pxLineFeatures.insert(pLineF->unObjType, QList<XFeature*>());
					itr->push_back(pLineF);
				}

				unCount = 0;
				_xArchive >> unCount;
				for (quint32 unIdx = 0; unIdx < unCount; unIdx++){
					XFeature* pPotF = new XFeature();
					pPotF->pxParentDoc = this;
					pPotF->eGeoType = XFeature::xePoint;
					featureSerialize(_xArchive, pPotF);
					m_pxResultPointSeq.append(pPotF);

					QMultiMap<quint32, QList<XFeature*>>::Iterator itr = m_pxPointFeatures.find(pPotF->unObjType);
					if (m_pxPointFeatures.contains(pPotF->unObjType) == false)
						itr = m_pxPointFeatures.insert(pPotF->unObjType, QList<XFeature*>());
					itr->push_back(pPotF);
				}
				int md = 0;
			}
		}
	}

	bool XS57DataDoc::LoadFile(const QString& _strFilePath)
	{
		// 判断是否存在缓存文件
		m_strFilePath = _strFilePath;
		m_pxLoadThread->StartLoad(this, _strFilePath);
		
		return true;
	}

	void XS57DataDoc::StopLoading()
	{
		m_pxLoadThread->quit();
		// 清理内存
		clear();
	}

	void XS57DataDoc::clear()
	{
		if (m_pxLoadThread->isRunning())
			m_pxLoadThread->quit();

		// 清理内存
		m_pxLoadThread = NULL;
		m_bIsValid = false;
		m_bIsChanged = false;
		m_pxResultLineSeq.clear();
		m_pxResultPointSeq.clear();
		m_strFilePath = QString();
		m_pxResultAreaSeq.clear();
		m_pxBaseDepAreFeatures.clear();

		if (m_pxOrignFileInfo)
			delete m_pxOrignFileInfo;
		m_pxOrignFileInfo = NULL;

		QList<XFeature*> xAreaList;
		foreach(xAreaList, m_pxAreaFeatures){
			foreach(XFeature* pFA, xAreaList){
				if (pFA)
					delete pFA;
				pFA = NULL;
			}
		}
		m_pxAreaFeatures.clear();

		QList<XFeature*> xLineList;
		foreach(xLineList, m_pxLineFeatures){
			foreach(XFeature* pLA, xLineList){
				if (pLA)
					delete pLA;
				pLA = NULL;
			}
		}
		m_pxLineFeatures.clear();

		QList<XFeature*> xPointList;
		foreach(xPointList, m_pxPointFeatures){
			foreach(XFeature* pPA, xPointList){
				if (pPA)
					delete pPA;
				pPA = NULL;
			}
		}
		m_pxPointFeatures.clear();

		m_xMemPool.ClearAllMem();
	}

	void XS57DataDoc::saveCache()
	{
		QFileInfo fileInfo(m_strFilePath);
		QString strCachePath = QString("./Data/S57Chart/Cache/") + fileInfo.fileName() + QString(".Cache");
		strCachePath = sfnGetAbsPath(strCachePath);
		if (!m_bIsChanged && QFile::exists(strCachePath))
			return;

		SaveDocument(strCachePath, true);
	}

	void XS57DataDoc::featureSerialize(XArchive& _xArchive, XFeature* _pFeature)
	{
		if (!IsSerialize())
			return;

		int nClassVersion = 0xFFFF;
		if (_xArchive.IsStoring())
		{
			nClassVersion = GetRuntimeClass()->GetClassVersion();
			if (nClassVersion >= xPLUGINS_S57_VERSION_1_0)
			{
				_xArchive << _pFeature->eGeoType << _pFeature->strName << _pFeature->unObjType << _pFeature->strKey << _pFeature->unIdentifyID 
					<< _pFeature->strSymbolInfos << _pFeature->strDisplayLayers << _pFeature->xAtts.count();
				QMap<quint32, QString>::Iterator itrAtts = _pFeature->xAtts.begin();
				while (itrAtts != _pFeature->xAtts.end()){
					_xArchive << itrAtts.key() << itrAtts.value();
					itrAtts++;
				}
				if (_pFeature->eGeoType == XFeature::xeArea || _pFeature->eGeoType == XFeature::xeLine){
					_xArchive << _pFeature->x2DPointList.count();
					foreach(QVector2D* pxPot, _pFeature->x2DPointList)
						_xArchive << *pxPot;
					if (_pFeature->eGeoType == XFeature::xeArea){
						_xArchive << _pFeature->xInsideAreas.count();
						foreach(XFeature* _pInsideAF, _pFeature->xInsideAreas)
							featureSerialize(_xArchive, _pInsideAF);
					}
				}
				else if (_pFeature->eGeoType == XFeature::xePoint){
					bool bIsSingleP = false;
					if (_pFeature->pxSinglePoint)
						bIsSingleP = true;
					_xArchive << bIsSingleP;
					if (bIsSingleP)
						_xArchive << *(_pFeature->pxSinglePoint);
					_xArchive << _pFeature->x3DPointList.count();
					foreach(QVector3D* p3D, _pFeature->x3DPointList){
						_xArchive << *p3D;
					}
				}
			}
		}
		else if (_xArchive.IsLoading())
		{
			nClassVersion = _xArchive.GetObjectVersion();
			if (nClassVersion >= xPLUGINS_S57_VERSION_1_0)
			{
				quint32 unCount = 0;
				quint32 unGeoType = 0;
				_xArchive >> unGeoType >> _pFeature->strName >> _pFeature->unObjType >> _pFeature->strKey >> _pFeature->unIdentifyID
					>> _pFeature->strSymbolInfos >> _pFeature->strDisplayLayers;
				_pFeature->eGeoType = XFeature::xeGeomType(unGeoType);
				_xArchive >> unCount;
				for (quint32 unIdx = 0; unIdx < unCount; unIdx++){
					quint32 unKey = 0;
					QString strValue = QString();
					_xArchive >> unKey >> strValue;
					_pFeature->xAtts.insert(unKey, strValue);
				}

				if (_pFeature->eGeoType == XFeature::xeArea || _pFeature->eGeoType == XFeature::xeLine){
					_xArchive >> unCount;
					for (quint32 unIdx = 0; unIdx < unCount; unIdx++){
						QVector2D* pVec2D = (QVector2D*)m_xMemPool.AllocMem(sizeof(QVector2D));
						_xArchive >> *pVec2D;
						_pFeature->x2DPointList.push_back(pVec2D);
					}

					if (_pFeature->eGeoType == XFeature::xeArea){
						_xArchive >> unCount;
						for (quint32 unIdx = 0; unIdx < unCount; unIdx++){
							XFeature* pxInsideAF = new XFeature();
							pxInsideAF->pxParentDoc = this;
							pxInsideAF->eGeoType = XFeature::xeArea;
							featureSerialize(_xArchive, pxInsideAF);
							_pFeature->xInsideAreas.append(pxInsideAF);
						}

						if (_pFeature->unObjType == 42 || _pFeature->unObjType == 46)
							m_pxBaseDepAreFeatures.append(_pFeature);
					}
				}
				else if (_pFeature->eGeoType == XFeature::xePoint){
					bool bIsSingleP = false;
					_xArchive >> bIsSingleP;
					if (bIsSingleP){
						_pFeature->pxSinglePoint = (QVector2D*)m_xMemPool.AllocMem(sizeof(QVector2D));
						_xArchive >> *(_pFeature->pxSinglePoint);
					}
					
					_xArchive >> unCount;
					for (quint32 unIdx = 0; unIdx < unCount; unIdx ++){
						QVector3D* p3D = (QVector3D*)m_xMemPool.AllocMem(sizeof(QVector3D));
						_xArchive >> *p3D;
						_pFeature->x3DPointList.append(p3D);
					}
				}
			}
			int md = 0;
		}
	}

	QList<XFeature*> XS57DataDoc::GetLineFeatures()
	{
		return m_pxResultLineSeq;
	}

	QList<XFeature*> XS57DataDoc::GetAssociateObjects(const XFeature* _xFeatureObj)
	{
		QList<XFeature*> xRetList;
		//return xRetList;

		// DEPARE & DRGARE

		double dLon = 0;
		double dLat = 0;

		QPainterPath xTargetPath;
		if (_xFeatureObj->eGeoType == XFeature::xeArea || _xFeatureObj->eGeoType == XFeature::xeLine){
			QPolygonF xPoly;
			foreach(QVector2D* p2D, _xFeatureObj->x2DPointList)
				xPoly.push_back(p2D->toPointF());
			dLon = xPoly.boundingRect().center().x();
			dLat = xPoly.boundingRect().center().y();
		}
		else if (_xFeatureObj->eGeoType == XFeature::xePoint){
			QPolygonF xPoly;
			if (_xFeatureObj->pxSinglePoint)
				xPoly.push_back(_xFeatureObj->pxSinglePoint->toPointF());
			dLon = _xFeatureObj->pxSinglePoint->x();
			dLat = _xFeatureObj->pxSinglePoint->y();
		}

		QList<XFeature*>::Iterator itr = m_pxBaseDepAreFeatures.end() - 1;
		while(itr != m_pxBaseDepAreFeatures.begin()-1){
			QPainterPath xPath;
			XFeature* pFA = *itr;
			if (pFA->eGeoType == XFeature::xeArea || pFA->eGeoType == XFeature::xeLine){
				QPolygonF xPoly;
				foreach(QVector2D* p2D, pFA->x2DPointList)
					xPoly.push_back(p2D->toPointF());
				if (xPoly.boundingRect().contains(dLon, dLat)){
					if (xPoly.containsPoint(QPointF(dLon, dLat), Qt::OddEvenFill)){
						xRetList.append(pFA);

					if (_xFeatureObj->eGeoType == XFeature::xePoint || _xFeatureObj->eGeoType == XFeature::xeLine)
						break;
					} 
				}
			}
			else if (pFA->eGeoType == XFeature::xePoint){
				QPolygonF xPoly;
				if (pFA->pxSinglePoint)
					xPoly.push_back(pFA->pxSinglePoint->toPointF());
				if (pFA->pxSinglePoint->x() == dLon && pFA->pxSinglePoint->y() == dLat){
					xRetList.append(pFA);
					break;
				}
			}
			itr--;
		}

		return xRetList;
	}

	QList<XFeature*> XS57DataDoc::GetAreaFeaturesSeq()
	{
		return m_pxResultAreaSeq;
	}

	QList<XFeature*> XS57DataDoc::GetPointFeatures()
	{
		return m_pxResultPointSeq;
	}


	XSpace_Plugins::xeError_S57Plugins XLoadThread::StartLoad(XS57DataDoc* _pxS57Doc, const QString& _strFilePath)
	{
		if (isRunning() || _pxS57Doc == NULL || !QFile::exists(_strFilePath))
			return xeError_S57Plugins_FAILD;

		m_pxCurS57Doc = _pxS57Doc;
		m_strFilePath = _strFilePath;
		start();

		return xeError_S57Plugins_OK;
	}

	XLoadThread::XLoadThread(QObject* _pxObj /*= NULL*/)
	{
		m_pxCurS57Doc = NULL;
		m_strFilePath = QString();
		m_pxS57FileInfo = NULL;
	}

	void XLoadThread::run()
	{
		QFileInfo fileInfo(m_strFilePath);
		QString strFileName = fileInfo.fileName();
		bool bIsCache = false;
		QString strCacheFile(QString("./Data/S57Chart/Cache/") + strFileName + QString(".Cache"));
		if (QFile::exists(strCacheFile)){
			if (m_pxCurS57Doc->OpenDocument(strCacheFile) != 0){
				QFile::remove(strCacheFile);
				m_strFilePath = QString();
			}
			else
				bIsCache = true;
		}
		if (bIsCache){
			//XS57Document::GetInstance()->processS57Doc(m_pxCurS57Doc);
			m_pxCurS57Doc->m_strFilePath = m_strFilePath;
			emit m_pxCurS57Doc->slgLoadFinish(m_strFilePath, m_pxCurS57Doc);
			return;
		}

		// 读取文件内容
		m_pxS57FileInfo = new XS57FileInfo();
		XS57Reader s57Reader;
		xeError_S57Plugins xError = s57Reader.ReadFile(m_strFilePath, m_pxS57FileInfo);
		if (xError != xeError_S57Plugins_OK){
			// 读取失败
			delete m_pxS57FileInfo;
			m_pxS57FileInfo = NULL;
		}
		
		// 提取显示数据
		if (!takeShowData()){
			delete m_pxS57FileInfo;
			m_pxS57FileInfo = NULL;
		}

		m_pxCurS57Doc->m_bIsValid = true;
		m_pxCurS57Doc->m_bIsChanged = true;
		m_pxCurS57Doc->m_pxOrignFileInfo = m_pxS57FileInfo;
		m_pxS57FileInfo = NULL;

		emit m_pxCurS57Doc->slgLoadFinish(m_strFilePath,m_pxCurS57Doc);
		//XS57Document::GetInstance()->processS57Doc(m_pxCurS57Doc);
	}

	const bool XLoadThread::takeShowData()
	{
		if (m_pxCurS57Doc == NULL || m_pxS57FileInfo == NULL)
			return false;

		m_pxCurS57Doc->m_xDSID = m_pxS57FileInfo->m_xDSCommonInfo;
		m_pxCurS57Doc->m_xDSPM = m_pxS57FileInfo->m_xDSGeoInfo;

		QString strList;
		quint32 unId = 0;
		foreach(XS57_Struct_FRID* pFRID, m_pxS57FileInfo->m_xFRIDs)
		{
			if (pFRID->ePRIM == xePRIM_A)
			{
				XFeature* pBaseFeature = new XFeature();
				pBaseFeature->pxParentDoc = m_pxCurS57Doc;
				pBaseFeature->eGeoType = XFeature::xeArea;
				pBaseFeature->unObjType = pFRID->unOBJL;
				pBaseFeature->strKey = pFRID->strKey;
				pBaseFeature->unIdentifyID = pFRID->unIdentifyID;

				foreach(XS57_Struct_ATTVF xAtt, pFRID->xATTFs)
					pBaseFeature->xAtts.insert(xAtt.unATTL, xAtt.strATVL);

				XFeature* pCurFeature = pBaseFeature;
				QList<QVector2D*> xFeatureFinshPoints;
				foreach(XS57_Struct_FSPT xFSPT, pFRID->xFSPTs)
				{
					const XS57_Struct_VRID* pVRID = m_pxS57FileInfo->GetVRIDByName(xFSPT.strNAME);
					QList<QVector2D*> xPointList;
					if (pVRID->eRCNM == XS57_Struct_VRID::xeVE)
					{
						foreach(QVector2D var2D, pVRID->xSG2Ds)
						{
							QVector2D* pPoint = (QVector2D*)m_pxCurS57Doc->m_xMemPool.AllocMem(sizeof(QVector2D));
							*pPoint = var2D;

							if (xFSPT.eORNT == xeORNT_R)
								xPointList.push_front(pPoint);
							else
								xPointList.append(pPoint);
						}

						foreach(XS57_Struct_VRPT xVRPT, pVRID->xVRPTs)
						{
							const XS57_Struct_VRID* pPointVRID = m_pxS57FileInfo->GetVRIDByName(xVRPT.strNAME);
							if (!pPointVRID)
								return false;

							QVector2D* pPoint = (QVector2D*)m_pxCurS57Doc->m_xMemPool.AllocMem(sizeof(QVector2D));
							*pPoint = pPointVRID->xSG2Ds.at(0);
							if (xFSPT.eORNT == xeORNT_R)
							{

								if (xVRPT.eTOPI == xeTOPI_B){
									xPointList.push_back(pPoint);
								}
								else if (xVRPT.eTOPI == xeTOPI_E)
								{
									xPointList.push_front(pPoint);
								}
							}
							else
							{
								if (xVRPT.eTOPI == xeTOPI_B){
									xPointList.push_front(pPoint);
								}

								else if (xVRPT.eTOPI == xeTOPI_E){
									xPointList.push_back(pPoint);
								}
							}
						}
						// 内部区域
						QVector2D vFirst = QVector2D(0, 0);
						QVector2D vSecond = QVector2D(0, 0);
						double dDistance = 0.0;
						xFeatureFinshPoints.append(xPointList);
						if (xFeatureFinshPoints.count() >= 3)
						{
							vFirst = *(xFeatureFinshPoints.first());
							vSecond = *(xFeatureFinshPoints.last());
							dDistance = sqrt( (vSecond.x()-vFirst.x())*(vSecond.x()-vFirst.x()) + (vSecond.y()-vFirst.y())*(vSecond.y()-vFirst.y()) );
						}
						if (dDistance < 0.0000001 && xFeatureFinshPoints.count() > 3)
						{
							if (xFSPT.eUSAG == xeUSAG_T)
							{
								pCurFeature = new XFeature;
								pCurFeature->pxParentDoc = m_pxCurS57Doc;
								pCurFeature->eGeoType = XFeature::xeArea;
								pCurFeature->strKey = pFRID->strKey;
								pCurFeature->unObjType = pBaseFeature->unObjType;
								pCurFeature->xAtts = pBaseFeature->xAtts;
								pCurFeature->unIdentifyID = pBaseFeature->unIdentifyID;
								pBaseFeature->xInsideAreas.push_back(pCurFeature);
							}
							pCurFeature->x2DPointList.append(xFeatureFinshPoints);
							xFeatureFinshPoints.clear();
						}
					}
				}

				if (pBaseFeature)
				{
					QMultiMap<quint32, QList<XFeature*>>::Iterator itr = m_pxCurS57Doc->m_pxAreaFeatures.find(pFRID->unOBJL);
					if (m_pxCurS57Doc->m_pxAreaFeatures.contains(pFRID->unOBJL) == false)
						itr = m_pxCurS57Doc->m_pxAreaFeatures.insert(pFRID->unOBJL, QList<XFeature*>());
					itr->push_back(pBaseFeature);
				}
				pBaseFeature = NULL;
			}
			else if (pFRID->ePRIM == xePRIM_P)
			{
				// 水深数据
				if (pFRID->unOBJL == 129){
					XFeature* pBaseFeature = new XFeature();
					pBaseFeature->pxParentDoc = m_pxCurS57Doc;
					pBaseFeature->eGeoType = XFeature::xePoint;
					pBaseFeature->unObjType = pFRID->unOBJL;
					pBaseFeature->strKey = pFRID->strKey;
					pBaseFeature->unIdentifyID = pFRID->unIdentifyID;

					foreach(XS57_Struct_ATTVF xAtt, pFRID->xATTFs)
						pBaseFeature->xAtts.insert(xAtt.unATTL, xAtt.strATVL);

					QList<QVector3D*> xFeatureFinshPoints;
					foreach(XS57_Struct_FSPT xFSPT, pFRID->xFSPTs)
					{
						const XS57_Struct_VRID* pVRID = m_pxS57FileInfo->GetVRIDByName(xFSPT.strNAME);
						if (!pVRID)
							continue;
						foreach(QVector3D ver3D, pVRID->xSG3Ds)
						{
							QVector3D* p3D = (QVector3D*)(m_pxCurS57Doc->m_xMemPool.AllocMem(sizeof(QVector3D)));
							*p3D = ver3D;
							xFeatureFinshPoints.push_back(p3D);
						}
					}
					pBaseFeature->x3DPointList = xFeatureFinshPoints;
					xFeatureFinshPoints.clear();
					if (pBaseFeature)
					{
						QMultiMap<quint32, QList<XFeature*>>::Iterator itr = m_pxCurS57Doc->m_pxPointFeatures.find(pFRID->unOBJL);
						if (m_pxCurS57Doc->m_pxPointFeatures.contains(pFRID->unOBJL) == false)
							itr = m_pxCurS57Doc->m_pxPointFeatures.insert(pFRID->unOBJL, QList<XFeature*>());
						itr->push_back(pBaseFeature);
					}
					pBaseFeature = NULL;
				}else {
					XFeature* pBaseFeature = new XFeature();
					pBaseFeature->pxParentDoc = m_pxCurS57Doc;
					pBaseFeature->eGeoType = XFeature::xePoint;
					pBaseFeature->unObjType = pFRID->unOBJL;
					pBaseFeature->strKey = pFRID->strKey;
					pBaseFeature->unIdentifyID = pFRID->unIdentifyID;

					foreach(XS57_Struct_ATTVF xAtt, pFRID->xATTFs)
						pBaseFeature->xAtts.insert(xAtt.unATTL, xAtt.strATVL);

					if (pFRID->xFSPTs.count() == 1)
					{
						XS57_Struct_FSPT xFSPT = pFRID->xFSPTs.first();
						QList<QVector2D*> xFeatureFinshPoints;
						const XS57_Struct_VRID* pVRID = m_pxS57FileInfo->GetVRIDByName(xFSPT.strNAME);
						if (!pVRID)
							continue;
						foreach(QVector2D ver2D, pVRID->xSG2Ds)
						{
							QVector2D* p2D = (QVector2D*)(m_pxCurS57Doc->m_xMemPool.AllocMem(sizeof(QVector2D)));
							*p2D = ver2D;
							xFeatureFinshPoints.push_back(p2D);
						}
						if (xFeatureFinshPoints.count() == 1)
							pBaseFeature->pxSinglePoint = xFeatureFinshPoints.first();
						else
							int nB = 0;

						xFeatureFinshPoints.clear();
						if (pBaseFeature)
						{
							QMultiMap<quint32, QList<XFeature*>>::Iterator itr = m_pxCurS57Doc->m_pxPointFeatures.find(pFRID->unOBJL);
							if (m_pxCurS57Doc->m_pxPointFeatures.contains(pFRID->unOBJL) == false)
								itr = m_pxCurS57Doc->m_pxPointFeatures.insert(pFRID->unOBJL, QList<XFeature*>());
							itr->push_front(pBaseFeature);
						}
						pBaseFeature = NULL;
					}
					else
						int md = 0;
				}
			}
			else if (pFRID->ePRIM == xePRIM_L){
				XFeature* pBaseFeature = new XFeature();
				pBaseFeature->pxParentDoc = m_pxCurS57Doc;
				pBaseFeature->eGeoType = XFeature::xeLine;
				pBaseFeature->unObjType = pFRID->unOBJL;
				pBaseFeature->strKey = pFRID->strKey;
				pBaseFeature->unIdentifyID = pFRID->unIdentifyID;

				foreach(XS57_Struct_ATTVF xAtt, pFRID->xATTFs)
					pBaseFeature->xAtts.insert(xAtt.unATTL, xAtt.strATVL);

				XFeature* pCurFeature = pBaseFeature;
				QList<QVector2D*> xFeatureFinshPoints;
				foreach(XS57_Struct_FSPT xFSPT, pFRID->xFSPTs)
				{
					const XS57_Struct_VRID* pVRID = m_pxS57FileInfo->GetVRIDByName(xFSPT.strNAME);
					QList<QVector2D*> xPointList;
					if (pVRID->eRCNM == XS57_Struct_VRID::xeVE){
						foreach(QVector2D var2D, pVRID->xSG2Ds)
						{
							QVector2D* pPoint = (QVector2D*)m_pxCurS57Doc->m_xMemPool.AllocMem(sizeof(QVector2D));
							*pPoint = var2D;

							if (xFSPT.eORNT == xeORNT_R)
								xPointList.push_front(pPoint);
							else
								xPointList.append(pPoint);
						}

						foreach(XS57_Struct_VRPT xVRPT, pVRID->xVRPTs)
						{
							const XS57_Struct_VRID* pPointVRID = m_pxS57FileInfo->GetVRIDByName(xVRPT.strNAME);
							if (!pPointVRID)
								return false;

							QVector2D* pPoint = (QVector2D*)m_pxCurS57Doc->m_xMemPool.AllocMem(sizeof(QVector2D));
							*pPoint = pPointVRID->xSG2Ds.at(0);
							if (xFSPT.eORNT == xeORNT_R)
							{

								if (xVRPT.eTOPI == xeTOPI_B){
									xPointList.push_back(pPoint);
								}
								else if (xVRPT.eTOPI == xeTOPI_E)
								{
									xPointList.push_front(pPoint);
								}
							}
							else
							{
								if (xVRPT.eTOPI == xeTOPI_B){
									xPointList.push_front(pPoint);
								}

								else if (xVRPT.eTOPI == xeTOPI_E){
									xPointList.push_back(pPoint);
								}
							}
						}
						xFeatureFinshPoints.append(xPointList);
						{
							pCurFeature->x2DPointList.append(xFeatureFinshPoints);
							xFeatureFinshPoints.clear();
						}
					}
				}

				if (pBaseFeature)
				{
					QMultiMap<quint32, QList<XFeature*>>::Iterator itr = m_pxCurS57Doc->m_pxLineFeatures.find(pFRID->unOBJL);
					if (m_pxCurS57Doc->m_pxLineFeatures.contains(pFRID->unOBJL) == false)
						itr = m_pxCurS57Doc->m_pxLineFeatures.insert(pFRID->unOBJL, QList<XFeature*>());
					itr->push_back(pBaseFeature);
				}
				pBaseFeature = NULL;
			}
			else
				int kd = 0;
		}

		// 处理面状图层覆盖关系
		{
			// 处理覆盖范围边界区域
 			QList<XFeature*> xAreaResult;
			QList<XFeature*> xBOUNDAreas;
			if (m_pxCurS57Doc->m_pxAreaFeatures.contains(302)){
				xBOUNDAreas = m_pxCurS57Doc->m_pxAreaFeatures.value(302);
				m_pxCurS57Doc->m_pxAreaFeatures.remove(302);
			}
 				
// 
// 			if (!xAreaResult.isEmpty()){
// 				QPainterPath resPath;
// 				XFeature* pResF = NULL;
// 				double dArea = 0xFFFFFFFF;
// 				foreach(XFeature* pF, xAreaResult){
// 					QPainterPath FPath;
// 					QPolygonF poly;
// 					foreach(QVector2D* p2D, pF->x2DPointList)
// 						poly.append(p2D->toPointF());
// 					FPath.addPolygon(poly);
// 					if (resPath.isEmpty()){
// 						resPath = FPath;
// 						pResF = pF;
// 						continue;
// 					}
// 
// 					if (resPath.contains(FPath)){
// 						resPath = FPath;
// 						delete pResF;
// 						pResF = pF;
// 					}
// 				}
// 				xAreaResult.clear();
// 				xAreaResult.append(pResF);
// 				if (pResF)
// 					m_pxCurS57Doc->m_pxAreaFeatures.replace(302,xAreaResult);
// 				xAreaResult.clear();
// 			}

			if (m_pxCurS57Doc->m_pxAreaFeatures.contains(42))
				procAreaCover(m_pxCurS57Doc->m_pxAreaFeatures.value(42), m_pxCurS57Doc->m_pxBaseDepAreFeatures);
			if (m_pxCurS57Doc->m_pxAreaFeatures.contains(46))
				procAreaCover(m_pxCurS57Doc->m_pxAreaFeatures.value(46), m_pxCurS57Doc->m_pxBaseDepAreFeatures);

			// New
			xAreaResult.clear();
			QMultiMap<quint32, QList<XFeature*>>::Iterator itr = m_pxCurS57Doc->m_pxAreaFeatures.begin();
			while (itr != m_pxCurS57Doc->m_pxAreaFeatures.end()){
				QList<XFeature*> xTempList = *itr;
				procAreaCover(xTempList, xAreaResult);
				itr++;
			}
			if (!xAreaResult.isEmpty())
				m_pxCurS57Doc->m_pxResultAreaSeq = xAreaResult;
			xAreaResult.clear();
			foreach(XFeature* pF, xBOUNDAreas)
				m_pxCurS57Doc->m_pxResultAreaSeq.push_front(pF);

			QList<XFeature*> xLineResult;
			itr = m_pxCurS57Doc->m_pxLineFeatures.begin();
			while (itr != m_pxCurS57Doc->m_pxLineFeatures.end()){
				QList<XFeature*> xTempList = *itr;
				xLineResult.append(xTempList);
				itr++;
			}
			if (!xLineResult.isEmpty())
				m_pxCurS57Doc->m_pxResultLineSeq = xLineResult;
			xLineResult.clear();

			QList<XFeature*> xPotResult;
			itr = m_pxCurS57Doc->m_pxPointFeatures.begin();
			while (itr != m_pxCurS57Doc->m_pxPointFeatures.end()){
				QList<XFeature*> xTempList = *itr;
				xPotResult.append(xTempList);
				itr++;
			}
			if (!xPotResult.isEmpty())
				m_pxCurS57Doc->m_pxResultPointSeq = xPotResult;
			xPotResult.clear();
		}


		return true;
	}

	void XLoadThread::procAreaCover(QList<XFeature*> _xSrcList, QList<XFeature*>& _xDescList)
	{
		QList<XFeature*>::Iterator itr = _xSrcList.begin();
		while (itr != _xSrcList.end())
		{
			quint32 unTem = _xDescList.count();
			quint32 unIdx = 0;
			XFeature* pNewA = *itr;
			if (pNewA&&pNewA->eGeoType != XFeature::xeArea){
				itr++;
				continue;
			}
			for (unIdx; unIdx < unTem; unIdx++)
			{
				QPolygonF xLastPoly;
				QPolygonF xNewPoly;
				XFeature* pLastA = _xDescList.at(unIdx);
				foreach(QVector2D* p2D, pLastA->x2DPointList)
					xLastPoly.append(QPointF(p2D->x(), p2D->y()));
				foreach(QVector2D* p2D, pNewA->x2DPointList)
					xNewPoly.append(QPointF(p2D->x(), p2D->y()));

				QPainterPath xLPath;
				QPainterPath xNPath;
				xLPath.addPolygon(xLastPoly);
				xNPath.addPolygon(xNewPoly);
				if (xNPath.contains(xLPath)){
					_xDescList.insert(unIdx, *itr);
					break;
				}
			}

			if (unIdx == unTem)
				_xDescList.push_back(pNewA);

			itr++;
		}
	}

}