#include "XS57Document.h"
#include <QtCore/QFileInfo>
#include "XS57Reader.h"
#ifdef WIN64
#include <windows.h>
#endif

namespace XSpace_Plugins
{
	XS57Document ms_xInstance;
	xIMPLEMENT_SERIAL(XS57Document,XDocument,xPLUGINS_S57_VERSION_1_0)
	XS57Document::XS57Document(QObject* _pxObj/* = NULL*/)
	: QObject(_pxObj)
	{
		m_bIsInit = false;
		m_pxDatas.clear();
		m_pxLoadingDatas.clear();
	}

	XS57Document::~XS57Document()
	{
		// 当前载入文件缓存
		{
			CreateCache();
		}

		ClearMem();
	}

	bool XS57Document::LoadFiles(const QStringList& _strFilePaths)
	{
		foreach(QString strPath, _strFilePaths){
			if (sfnFileIsExist(strPath))
				return false;
			if (sfnGetFileSuffix(strPath) != "000")
				return false;
		}

		m_xDataLock.lock();
		foreach(QString strPath, _strFilePaths){
			XS57DataDoc* pxDoc = new XS57DataDoc();
			pxDoc->LoadFile(strPath);
			m_pxLoadingDatas.push_back(pxDoc);
			pxDoc = NULL;
		}
		m_xDataLock.unlock();

		return true;
	}

	void XS57Document::Serialize(XArchive& _xArchive)
	{
		if (!IsSerialize())
			return;

		int nClassVersion = 0xFFFF;
		if (_xArchive.IsStoring())
		{
			nClassVersion = GetRuntimeClass()->GetClassVersion();
			if (nClassVersion >= xPLUGINS_S57_VERSION_1_0)
			{
				// << 
			}
		}
		else if (_xArchive.IsLoading())
		{
			nClassVersion = _xArchive.GetObjectVersion();
			if (nClassVersion >= xPLUGINS_S57_VERSION_1_0)
			{
				// >> 
			}
		}
	}

	void XS57Document::ClearMem()
	{
		m_xDataLock.lock();
		foreach(XS57DataDoc* pDoc, m_pxDatas)
			delete pDoc;
		m_pxDatas.clear();

		foreach(XS57DataDoc* pDoc, m_pxLoadingDatas)
			delete pDoc;
		m_pxLoadingDatas.clear();
		m_xDataLock.unlock();

		m_bIsInit = false;
	}

	void XS57Document::procAreaCover(QList<XFeature*> _xSrcList, QList<XFeature*>& _xDescList)
	{
		{
			QList<XFeature*>::Iterator itr = _xSrcList.begin();
			while (itr != _xSrcList.end())
			{
				quint32 unTem = _xDescList.count();
				quint32 unIdx = 0;
				XFeature* pNewA = *itr;
				if (pNewA->eGeoType != XFeature::xeArea){
					itr++;
					continue;
				}

				for (unIdx; unIdx < unTem; unIdx++)
				{
					QPolygonF xLastPoly;
					QPolygonF xNewPoly;
					XFeature* pLastA = _xDescList.at(unIdx);
					foreach(QVector2D* p2D, pLastA->x2DPointList)
						xLastPoly.append(QPointF(p2D->x(),p2D->y()));
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

				//if (pNewA->xInsideAreas.count() > 0)
				//	int md = 0;
			//	procAreaCover(pNewA->xInsideAreas, _xDescList);

				itr++;
			}

		}
	}

	void XS57Document::processS57Doc(XS57DataDoc* _pxDoc)
	{
		if (_pxDoc == NULL)
			return;

		if (!_pxDoc->m_bIsValid)
			return;
		
		m_xDataLock.lock();

		if (m_pxDatas.contains(_pxDoc->m_strFilePath)){
			m_xDataLock.unlock();
			return;
		}

		m_pxLoadingDatas.removeOne(_pxDoc);
		m_pxDatas.insert(_pxDoc->m_strFilePath, _pxDoc);

		m_xDataLock.unlock();
		emit slgLoadNewFile(_pxDoc);
		_pxDoc = NULL;
	}

	bool XS57Document::CreateCache()
	{
		m_xDataLock.lock();
		foreach(XS57DataDoc* pDoc, m_pxDatas)
			pDoc->saveCache();
		m_xDataLock.unlock();
		return true;
	}

}