#pragma once
#include <XDocument.h>

using namespace XSpace_Base;
#define xeVersion_V1	1

namespace XSpace_Plugins
{
	struct XS57Class
	{
		quint32 unCode;		// 编码
		QString strName;	// 名称
		QString strAcronym;	// 编码缩写
		QStringList strAttsA;// 属性A
		QStringList strAttsB;// 属性B
		QStringList strAttsC;// 属性C
		uchar	ucClassTypes; // 类型
		QStringList strPRIMTypes;	// 原始数据类型
		QStringList strLayers;		// 渲染图层		// 1-Custom图层
		QStringList	strRendererLevs;// 渲染级别		数值越小越早渲染
	};

	class XS57ClassDef : public XDocument
	{
		xDECLARE_SERIAL(XS57ClassDef)
	public:
		XS57ClassDef();
		~XS57ClassDef();

		virtual void Serialize(XArchive& _xArchive);

		// 从原始数据初始化
		bool InitFromPrim(const QString& _strFilePath);
	
	public:
		QMap<quint32, XS57Class> m_xS57ClassVec;
		QMap<quint32, QString> m_xS57CodeToStrs;
		QMap<QString, quint32> m_xS57StrToCodes;
	};
}