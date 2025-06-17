#include "XArchive.h"
#include "XObject.h"

#define nNewClassTag ((int)0xFFFF)
#define nClassTag ((int)0x8000)

namespace XSpace_Common
{
	// 构造析构函数--------------------------------------------------------------------------------
	// 构造函数
	XArchive::XArchive(QFile* _pFile, EMode _eMode)
	{
		if( _pFile != NULL && _pFile->isOpen() )
		{
			m_qArchiveStream.setDevice(_pFile);
		}
		m_eMode = _eMode;
		m_nStoreObjectCount = 0;
		m_nStoreClassCount	= 0;
		m_nLoadClassCount	= 0;
		m_nObjectVersion	= 0xFFFF;
	}
	// 构造函数
	XArchive::XArchive(QByteArray& _qBytes,EMode _eMode)
		:m_qArchiveStream(&_qBytes,QIODevice::ReadWrite)
	{
		m_eMode = _eMode;
		m_nStoreObjectCount = 0;
		m_nStoreClassCount = 0;
		m_nLoadClassCount = 0;
		m_nObjectVersion = 0xFFFF;
	}
	// 析构函数
	XArchive::~XArchive(void)
	{
	}
	// 获取当前序列化模式是否为持久化（存储）模式
	const bool XArchive::IsStoring() const
	{
		if( m_eMode == EMode_Store )
			return true;
		else
			return false;
	}
	// 获取当前序列化模式是否为载入数据模式
	const bool XArchive::IsLoading() const
	{
		if( m_eMode == EMode_Load )
			return true;
		else
			return false;
	}
	// 设置序列化对象版本号
	void XArchive::SetObjectVersion(const int& _nClassVersion)
	{
		m_nObjectVersion = _nClassVersion;
	}
	// 获取序列化的对象版本号
	const int XArchive::GetObjectVersion() const
	{
		return m_nObjectVersion;
	}
	// 从数据流中读取并构建一个基对象（返回指针）
	XObject* XArchive::ReadObject( )
	{
		int nClassVersion = 0xFFFF;
		XObject* pRetObj = NULL;
		int nObjectTag = -1;
		XRuntimeClass* pClassRef = ReadClass(nClassVersion,nObjectTag);
		if( pClassRef != NULL )
		{
			pRetObj = pClassRef->CreateObject();
			int nOldVersion		= m_nObjectVersion;
			m_nObjectVersion	= nClassVersion;
			pRetObj->Serialize(*this);
			m_nObjectVersion	= nOldVersion;
		}


		return pRetObj;
	}
	// 从数据流中读取并初始化一个基对象
	void XArchive::ReadObject( XObject& _pRefObject )
	{
		int nClassVersion = 0xFFFF;
		XObject* pRetObj = NULL;
		int nObjectTag = -1;
		XRuntimeClass* pClassRef = ReadClass(nClassVersion,nObjectTag);
		if( pClassRef != NULL )
		{
			int nOldVersion		= m_nObjectVersion;
			m_nObjectVersion	= nClassVersion;
			_pRefObject.Serialize(*this);
			m_nObjectVersion	= nOldVersion;
		}
	}
	// 将一个基对象指针写入数据流中
	void XArchive::WriteObject( XObject* _pObject )
	{
		if( _pObject == NULL )
			return ;
		XRuntimeClass* pClassInfo = _pObject->GetRuntimeClass();
		if( pClassInfo == NULL )
			return ;
		WriteClass(pClassInfo);
		
		m_nStoreObjectCount += 1;
		m_xStoreObjectMap.insert(_pObject,m_nStoreObjectCount);

		_pObject->Serialize(*this);
	}
	// 将一个基对象引用写入数据流中
	void XArchive::WriteObject( XObject& _pRefObject )
	{
		XRuntimeClass* pClassInfo = _pRefObject.GetRuntimeClass();
		if( pClassInfo == NULL )
			return ;
		WriteClass(pClassInfo);

		m_nStoreObjectCount += 1;
		m_xStoreObjectMap.insert(&_pRefObject,m_nStoreObjectCount);

		_pRefObject.Serialize(*this);
	}
	// 将一个运行时类信息写入数据流中
	void XArchive::WriteClass( XRuntimeClass* _pClassRef )
	{
		if ( _pClassRef == NULL )
			return;

		if( _pClassRef->GetClassVersion() == 0xFFFF )
		{
			qDebug("Cannot call WriteClass/WriteObject for %s.",_pClassRef->GetClassName().toStdString().c_str());
			return;
		}

		int nClassIndex = -1;
		if( !m_xStoreClassMap.contains(_pClassRef) )
		{
			// Add New Class Info
			*this << nNewClassTag;
			_pClassRef->Store(*this);
			m_nStoreClassCount += 1;
			nClassIndex = nClassTag | m_nStoreClassCount;
			m_xStoreClassMap.insert(_pClassRef,nClassIndex);
		}else
		{
			nClassIndex = nClassTag | m_xStoreClassMap.value(_pClassRef);
			
			*this << nClassIndex;
		}
	}
	// 从数据流中读取一个运行时类信息
	XRuntimeClass* XArchive::ReadClass(int& _nClassVersion, int& _nObjectTag)
	{
		XRuntimeClass* pRetClass = NULL;

		int nTag		= 0;
		int nVersion	= 0xFFFF;
		*this >> nTag;

		int nClassIndex = -1;
		if( nTag == nNewClassTag )
		{
			if( pRetClass = XRuntimeClass::Load(*this,nVersion) )
			{
				_nClassVersion	= nVersion;
				_nObjectTag		= nTag;
				// Add
				m_nLoadClassCount += 1;
				nClassIndex = nClassTag | m_nLoadClassCount;
				m_xLoadClassMap.insert(nClassIndex,pRetClass);
				m_xLoadClassVersionMap.insert(nClassIndex,nVersion);
			}
		}else
		{
			nClassIndex		= nTag;
			_nObjectTag		= nTag;
			_nClassVersion	= m_xLoadClassVersionMap[nClassIndex];
			pRetClass		= m_xLoadClassMap[nClassIndex];
		}

		return pRetClass;
	}

	// 基对象运算符重载
	XArchive& xPASCAL operator<<(XArchive& _xArchive,XObject* _pObject)
	{
		_xArchive.WriteObject(_pObject);
		return _xArchive;
	}
	XArchive& xPASCAL operator<< (XArchive& _xArchive,XObject& _pRefObject)
	{
		_xArchive.WriteObject(_pRefObject);
		return _xArchive;
	}
	XArchive& xPASCAL operator>>(XArchive& _xArchive,XObject* &_pObject)
	{
		_pObject = _xArchive.ReadObject();
		return _xArchive;
	}
	XArchive& xPASCAL operator>>(XArchive& _xArchive,XObject& _pRefObject)
	{
		_xArchive.ReadObject(_pRefObject);
		return _xArchive;
	}

	// 载入操作重载----------------------------------------------------------------------------
	XArchive& XArchive::operator>>( int& _nRefVal )
	{
		m_qArchiveStream>>_nRefVal;
		return *this;
	}

	XArchive& XArchive::operator>>( QString& _qstrRefVal )
	{
		m_qArchiveStream >> _qstrRefVal;
		return *this;
	}

	XArchive& XArchive::operator>>( double& _dRefVal )
	{
		m_qArchiveStream >> _dRefVal;
		return *this;
	}

	XArchive& XArchive::operator>>(unsigned int& _unRefVal)
	{
		m_qArchiveStream >> _unRefVal;
		return *this;
	}

	XArchive& XArchive::operator>>(qint64& _nInt64Val)
	{
		m_qArchiveStream >> _nInt64Val;
		return *this;
	}

	XArchive& XArchive::operator>>(quint64& _unInt64Val)
	{
		m_qArchiveStream >> _unInt64Val;
		return *this;
	}

	XArchive& XArchive::operator>>(QBitArray& _pRefBitsVal)
	{
		m_qArchiveStream >> _pRefBitsVal;
		return *this;
	}

	XArchive& XArchive::operator>>(QByteArray& _pRefBytesVal)
	{
		m_qArchiveStream >> _pRefBytesVal;
		return *this;
	}

	XArchive& XArchive::operator>>(char& _chRefVal)
	{
		qint8 nTemp = 0;
		m_qArchiveStream >> nTemp;
		_chRefVal = nTemp;
		return *this;
	}

	XArchive& XArchive::operator>>(QDate& _pRefDateVal)
	{
		m_qArchiveStream >> _pRefDateVal;
		return *this;
	}

	XArchive& XArchive::operator>>(QTime& _pRefTimeVal)
	{
		m_qArchiveStream >> _pRefTimeVal;
		return *this;
	}

	XArchive& XArchive::operator>>(QDateTime& _pRefDateTimeVal)
	{
		m_qArchiveStream >> _pRefDateTimeVal;
		_pRefDateTimeVal = _pRefDateTimeVal.toLocalTime();
		return *this;
	}

	XArchive& XArchive::operator>>(QLineF& _pRefLineFVal)
	{
		m_qArchiveStream >> _pRefLineFVal;
		return *this;
	}

	XArchive& XArchive::operator>>(QPointF& _pRefPointVal)
	{
		m_qArchiveStream >> _pRefPointVal;
		return *this;
	}

	XArchive& XArchive::operator>>(QRectF& _pRefRectVal)
	{
		m_qArchiveStream >> _pRefRectVal;
		return *this;
	}

	XArchive& XArchive::operator>>(QSizeF& _pRefSizeVal)
	{
		m_qArchiveStream >> _pRefSizeVal;
		return *this;
	}

	XArchive& XArchive::operator>>(QSize& _pRefSizeVal)
	{
		m_qArchiveStream >> _pRefSizeVal;
		return *this;
	}

	XArchive& XArchive::operator>>(QStringList& _pRefStringListVal)
	{
		m_qArchiveStream >> _pRefStringListVal;
		return *this;
	}

	XArchive& XArchive::operator>>(QUrl& _pRefUrl)
	{
		m_qArchiveStream >> _pRefUrl;
		return *this;
	}

	XArchive& XArchive::operator>>(QColor& _pRefCor)
	{
		m_qArchiveStream >> _pRefCor;
		return *this;
	}

	XArchive& XArchive::operator>>(QFont& _pRefFont)
	{
		m_qArchiveStream >> _pRefFont;
		return *this;
	}

	XArchive& XArchive::operator>>(QImage& _pRefImg)
	{
		m_qArchiveStream >> _pRefImg;
		return *this;
	}

	XArchive& XArchive::operator>>(qint8& _nRefInt8Val)
	{
		m_qArchiveStream >> _nRefInt8Val;
		return *this;
	}

	XArchive& XArchive::operator>>(quint8& _nRefUInt8Val)
	{
		m_qArchiveStream >> _nRefUInt8Val;
		return *this;
	}

	XArchive& XArchive::operator>>(qint16& _nInt16Val)
	{
		m_qArchiveStream >> _nInt16Val;
		return *this;
	}

	XArchive& XArchive::operator>>(quint16& _nUInt16Val)
	{
		m_qArchiveStream >> _nUInt16Val;
		return *this;
	}

	XArchive& XArchive::operator>>(bool& _bRefBoolVal)
	{
		m_qArchiveStream >> _bRefBoolVal;
		return *this;
	}

	XArchive& XArchive::operator>>(float& _fVal)
	{
		m_qArchiveStream >> _fVal;
		return *this;
	}

	XArchive& XArchive::operator>>(QVector2D& _pRefVector2D)
	{
		m_qArchiveStream >> _pRefVector2D;
		return *this;
	}

	XArchive& XArchive::operator>>(QVector3D& _pRefVector3D)
	{
		m_qArchiveStream >> _pRefVector3D;
		return *this;
	}


	// 写入操作符重载------------------------------------------------------------------------------
	XArchive& XArchive::operator<<( int _nVal )
	{
		m_qArchiveStream<<_nVal;
		return *this;
	}

	XArchive& XArchive::operator<<( QString _qstrVal )
	{
		m_qArchiveStream <<_qstrVal;
		return *this;
	}

	XArchive& XArchive::operator<<( double _dVal )
	{
		m_qArchiveStream << _dVal;
		return *this;
	}

	XArchive& XArchive::operator<<(unsigned int _unVal)
	{
		m_qArchiveStream << _unVal;
		return *this;
	}

	XArchive& XArchive::operator<<(qint64 _nInt64Val)
	{
		m_qArchiveStream << _nInt64Val;
		return *this;
	}

	XArchive& XArchive::operator<<(quint64 _unInt64Val)
	{
		m_qArchiveStream << _unInt64Val;
		return *this;
	}

	XArchive& XArchive::operator<<(const QBitArray& _pRefBitsVal)
	{
		m_qArchiveStream << _pRefBitsVal;
		return *this;
	}

	XArchive& XArchive::operator<<(const QByteArray& _pRefBytesVal)
	{
		m_qArchiveStream << _pRefBytesVal;
		
		return *this;
	}

	XArchive& XArchive::operator<<(char _chVal)
	{
		m_qArchiveStream << qint8(_chVal);
		return *this;
	}

	XArchive& XArchive::operator<<(const QDate& _pRefDateVal)
	{
		m_qArchiveStream << _pRefDateVal;
		return *this;
	}

	XArchive& XArchive::operator<<(const QDateTime& _pRefDateTimeVal)
	{
		m_qArchiveStream << _pRefDateTimeVal.toUTC();
		return *this;
	}

	XArchive& XArchive::operator<<(const QTime& _pRefTimeVal)
	{
		m_qArchiveStream << _pRefTimeVal;
		return *this;
	}

	XArchive& XArchive::operator<<(const QStringList& _pRefStringListVal)
	{
		m_qArchiveStream << _pRefStringListVal;
		return *this;
	}

	XArchive& XArchive::operator<<(const QLineF& _pRefLineFVal)
	{
		m_qArchiveStream << _pRefLineFVal;
		return *this;
	}

	XArchive& XArchive::operator<<(const QPointF& _pRefPointVal)
	{
		m_qArchiveStream << _pRefPointVal;
		return *this;
	}

	XArchive& XArchive::operator<<(const QRectF& _pRefRectVal)
	{
		m_qArchiveStream << _pRefRectVal;
		return *this;
	}

	XArchive& XArchive::operator<<(const QSizeF& _pRefSizeVal)
	{
		m_qArchiveStream << _pRefSizeVal;
		return *this;
	}

	XArchive& XArchive::operator<<(const QSize& _pRefSizeVal)
	{
		m_qArchiveStream << _pRefSizeVal;
		return *this;
	}

	XArchive& XArchive::operator<<(const QUrl& _pRefUrl)
	{
		m_qArchiveStream << _pRefUrl;
		return *this;
	}

	XArchive& XArchive::operator<<(const QColor& _pRefCor)
	{
		m_qArchiveStream << _pRefCor;
		return *this;
	}

	XArchive& XArchive::operator<<(const QFont& _pRefFont)
	{
		m_qArchiveStream << _pRefFont;
		return *this;
	}

	XArchive& XArchive::operator<<(const QImage& _pRefImgVal)
	{
		m_qArchiveStream << _pRefImgVal;
		return *this;
	}

	XArchive& XArchive::operator<<(qint8 _nInt8Val)
	{
		m_qArchiveStream << _nInt8Val;
		return *this;
	}

	XArchive& XArchive::operator<<(quint8 _nUInt8Val)
	{
		m_qArchiveStream << _nUInt8Val;
		return *this;
	}

	XArchive& XArchive::operator<<(qint16 _nInt16Val)
	{
		m_qArchiveStream << _nInt16Val;
		return *this;
	}

	XArchive& XArchive::operator<<(quint16 _nUInt16Val)
	{
		m_qArchiveStream << _nUInt16Val;
		return *this;
	}

	XArchive& XArchive::operator<<(const bool& _bRefBoolVal)
	{
		m_qArchiveStream << _bRefBoolVal;
		return *this;
	}

	XArchive& XArchive::operator<<(const float& _fVal)
	{
		m_qArchiveStream << _fVal;
		return *this;
	}

	XArchive& XArchive::operator<<(const QVector2D& _pRefVector2D)
	{
		m_qArchiveStream << _pRefVector2D;
		return *this;
	}

	XArchive& XArchive::operator<<(const QVector3D& _pRefVector3D)
	{
		m_qArchiveStream << _pRefVector3D;
		return *this;
	}

}
