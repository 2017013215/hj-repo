#include "XRuntimeClass.h"
#include "XArchive.h"
#include "XObject.h"
namespace XSpace_Common
{
	XRuntimeClass* XRuntimeClass::ms_pFirstClass = NULL;
	XRuntimeClass::XRuntimeClass(void)
	{
		// 成员初始化
		m_strClassName		= "";
		m_nObjectSize		= 0;
		m_nClassVersion		= 0xFFFF;
		m_pfnCreateObject	= NULL;
		m_pBaseClass		= NULL;

		ms_pFirstClass		= NULL;
		m_pNextClass		= NULL;
	}

	XRuntimeClass::XRuntimeClass( QString _strClassName, int _nObjectSize, int _nVersion, XObject* (xPASCAL* _pfnCreateObject)(), XRuntimeClass* _pBaseClass )
	{
		m_strClassName		= _strClassName;
		m_nObjectSize		= _nObjectSize;
		m_nClassVersion		= _nVersion;
		m_pfnCreateObject	= _pfnCreateObject;
		m_pBaseClass		= _pBaseClass;
	}


	XRuntimeClass::~XRuntimeClass(void)
	{
	}

	void XRuntimeClass::SetFirstClass( XRuntimeClass* _pFirstClass )
	{
		ms_pFirstClass = _pFirstClass;
	}

	void XRuntimeClass::SetNextClass( XRuntimeClass* _pNextClass )
	{
		m_pNextClass = _pNextClass;
	}

	XRuntimeClass* XRuntimeClass::GetFirstClass()
	{
		return XRuntimeClass::ms_pFirstClass;
	}

	XRuntimeClass* XRuntimeClass::GetNextClass()
	{
		return m_pNextClass;
	}

	XRuntimeClass* XRuntimeClass::GetBaseRuntimeClass()
	{
		return m_pBaseClass;
	}

	XObject* XRuntimeClass::CreateObject()
	{
		XObject* pRetObj = NULL;

		if( m_pfnCreateObject == NULL )
		{	
			// Error:Trying to create object which is not "xDECLARE_DYNCREATE \n or xDECLARE_SERIAL: %hs. \n",
			// m_strClassName
			return pRetObj;
		}

		pRetObj = (*m_pfnCreateObject)();
		return pRetObj;

		return pRetObj;
	}

	QString XRuntimeClass::GetClassName()
	{
		return m_strClassName;
	}

	int XRuntimeClass::GetClassVersion()
	{
		return m_nClassVersion;
	}

	void XRuntimeClass::SetClassVersion( int _nClassVersion )
	{
		m_nClassVersion = _nClassVersion;
	}

	void XRuntimeClass::Store( XArchive& _xArchive ) const
	{
		_xArchive << m_nClassVersion;
		_xArchive << m_strClassName;
	}

	XRuntimeClass* xPASCAL XRuntimeClass::Load( XArchive& _xArchive, int& _nClassVersion )
	{
		XRuntimeClass* pRetClass = NULL;
		QString strClassName	= QString();
		int		nVersion		= 0xFFFF;

		_xArchive>>nVersion;
		_xArchive>>strClassName;
		_nClassVersion = nVersion;

		// 寻找该类型的对象
		pRetClass = XObject::GetRuntimeClassByName(strClassName);

		return pRetClass;
	}

	xSYS_CLASSINIT::xSYS_CLASSINIT(XRuntimeClass* _pNewClass)
	{
		_pNewClass->SetNextClass(XRuntimeClass::GetFirstClass());
		XRuntimeClass::SetFirstClass(_pNewClass);
	}
}
