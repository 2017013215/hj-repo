// Desc：	S57 Document
// Date:	2017/04/03
// Author:	Li XiaoQiang

#ifndef XS57DOCUMENT_HEADER
#define XS57DOCUMENT_HEADER

#include "XS57Types.h"
#include <QtCore/QMap>
#include <Common/XDocument.h>
#include "XS57FileInfo.h"
#include <QtCore/QThread>
#include <Common/XMemPool.h>
#include "XS57DataDoc.h"
#include <QtCore/QMutex>

using namespace XSpace_Common;
namespace XSpace_Plugins
{
	class XPLUGINS_S57_EXPORT XS57Document : public QObject,XDocument 
	{
		Q_OBJECT
		xDECLARE_SERIAL_DLL(XS57Document)
		friend class XS57LoadThread;
		friend struct XFeature;
		friend class XS57DataDoc;
		friend class XLoadThread;
	public:
		XS57Document(QObject* _pxObj = NULL);
		~XS57Document();

		virtual void Serialize(XArchive& _xArchive);
		void ClearMem();
		bool LoadFiles(const QStringList& _strFilePaths );

		bool CreateCache();

	private:
		static void procAreaCover(QList<XFeature*> _xSrcList, QList<XFeature*>& _xDescList);
		void processS57Doc(XS57DataDoc* _pxDoc);

	signals:
		void slgLoadNewFile(XS57DataDoc* _pxDoc);

	private:
		bool m_bIsInit;
		QMutex m_xDataLock;
		QMap<QString, XS57DataDoc*> m_pxDatas;	// 加载完成的数据
		QList<XS57DataDoc*> m_pxLoadingDatas;	// 正在加载中的数据
	};
}
#endif
