#include "XLoadLayerFeatureThread.h"
#include "XS57LayerAdapter.h"

namespace XSpace_Plugins
{
	XLoadLayerFeatureThread::XLoadLayerFeatureThread(QObject* _pxObj)
		:QThread(_pxObj)
	{
		m_bRunMark = false;
	}


	XLoadLayerFeatureThread::~XLoadLayerFeatureThread()
	{
	}

	void XLoadLayerFeatureThread::run()
	{
		XS57DataDoc* pCurDoc = NULL;
	loadLoop:
		m_xLock.lock();
		if (m_pxWaitLoads.isEmpty()){
			emit slgLoadFinish();
			m_xLock.unlock();
			return;
		}
		
		pCurDoc = m_pxWaitLoads.first();
		m_pxWaitLoads.removeFirst();
		m_xLock.unlock();

		// ¿ªÊ¼ÔØÈë
		//XS57LayerAdapter::GetInstance()->addDataDoc(pCurDoc);

		if (m_bRunMark)
			goto loadLoop;
	}

	void XLoadLayerFeatureThread::AddData(XS57DataDoc* _pxDataDoc)
	{
		if (_pxDataDoc == NULL)
			return;

		m_xLock.lock();

		m_pxWaitLoads.append(_pxDataDoc);

		m_xLock.unlock();

		if (!isRunning())
		{
			m_bRunMark = true;
			start();
		}
	}

	void XLoadLayerFeatureThread::Stop()
	{
		//XS57LayerAdapter::GetInstance()->StopAddData();
	}

}
