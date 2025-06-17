#ifndef XLOAD_LAYER_FEATURE_THREAD_HEADER
#define XLOAD_LAYER_FEATURE_THREAD_HEADER

#include <QtCore/QThread>
#include <QtCore/QList>
#include "XS57DataDoc.h"

namespace XSpace_Plugins
{
	class XLoadLayerFeatureThread :
		public QThread
	{
		Q_OBJECT
	public:
		XLoadLayerFeatureThread(QObject* _pxObj);
		~XLoadLayerFeatureThread();

		void run() override;

		void AddData(XS57DataDoc* _pxDataDoc);

		void Stop();

	signals:
		void slgLoadFinish();

	private:
		QList<XS57DataDoc*> m_pxWaitLoads;
		QMutex m_xLock;

		bool m_bRunMark;
	};
}

#endif
