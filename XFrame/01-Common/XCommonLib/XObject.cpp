#include "XObject.h"

namespace XSpace_Common
{
	// 运行时类的初始化
	static char szXObject[] = "XObject";

	XRuntimeClass XObject::ms_xClassXObject = 
		XRuntimeClass(QString::fromLocal8Bit(szXObject),sizeof(XObject),0xFFFF, NULL ,NULL);
	 xSYS_CLASSINIT XObject::ms_init_XObject(&XObject::ms_xClassXObject);

	// 构造函数
	XObject::XObject(void)
	{
	}
	// 析构函数
	XObject::~XObject(void)
	{
	}
	// 获取当前类的运行时对象信息
	XRuntimeClass* XObject::GetRuntimeClass() const
	{
		return &XObject::ms_xClassXObject;
	}
	// 判断一个对象是否是另一个对象的子类
	bool XObject::IsKindOf( const XRuntimeClass* _pClass ) const
	{
		XRuntimeClass* pClassThis = GetRuntimeClass(); 
		while ( pClassThis != NULL )
		{
			if( pClassThis == _pClass )
				return true;
			pClassThis = (XRuntimeClass*)pClassThis->GetBaseRuntimeClass();
		}
		return false;
	}
	// 判断当前类是否支持序列化
	bool XObject::IsSerialize() const
	{
		bool bRes = false;

		if (GetRuntimeClass()->GetClassVersion() != 0xFFFF)
			bRes = true;

		return bRes;
	}
	// 序列化函数
	void XObject::Serialize(XArchive& _xArchive)
	{
		// 空函数什么都不做 如此函数为纯虚函数
		// 会导致所有未实现此函数的子类无法被实例化
	}
	// 根据类名称创建一个类对象实例
	XObject* XObject::CreateObjectByName( const QString& _strObjClassName )
	{
		XObject* pRetObj = NULL;

		// 寻找指定名称对象的RuntimeClass
		XRuntimeClass* pThisClass	= XObject::GetRuntimeClassByName(_strObjClassName);

		if( pThisClass )
			pRetObj = pThisClass->CreateObject();

		return pRetObj;
	}
	// 根据类名称获取类的运行时类指针
	XRuntimeClass* XObject::GetRuntimeClassByName( const QString& _strClassName )
	{
		XRuntimeClass* pRetClass = NULL;
		// 寻找指定名称对象的RuntimeClass
		XRuntimeClass* pThisClass	= XObject::ms_xClassXObject.GetFirstClass();

		while ( pThisClass )
		{
			if( pThisClass->GetClassName() == _strClassName )
			{
				pRetClass = pThisClass;
				break;
			}
			pThisClass = pThisClass->GetNextClass();
		}
		
		return pRetClass;
	}
}
