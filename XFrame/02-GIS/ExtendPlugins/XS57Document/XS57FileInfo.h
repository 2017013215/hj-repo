// Desc：	S57 Parse FileInfo
// Date:	2017/04/05
// Author:	Li XiaoQiang

#ifndef XS57FILEINFO_HEADER
#define XS57FILEINFO_HEADER
#include "XS57Types.h"

namespace XSpace_Plugins
{
	class XPLUGINS_S57_EXPORT XS57FileInfo
	{
		friend class XS57Reader;
		friend class XS57Document;
	public:
		XS57FileInfo();
		~XS57FileInfo();

		void Clear();

		const XS57_Struct_VRID* GetVRIDByName(const QString& _strName);

	public:
		QString m_strFilePath;				// 文件路径
		XS57DataStructInfo m_xStructInfo;	// 数据结构信息

		XS57_Struct_DSID m_xDSCommonInfo;	// 数据集通用数据信息 Key DSID
		XS57_Struct_DSPM m_xDSGeoInfo;		// 数据集地理参照记录 Key DSPM

		QList<XS57_Struct_VRID*> m_xVRIDs;	// 矢量记录列表
		QList<XS57_Struct_FRID*> m_xFRIDs;	// 特征记录列表

		QMap<QString, XS57_Struct_VRID*> m_xVRIDSet;

		// 显示用数据


		QList<XFieldRecord> m_xErrorRecords;
	};
}

#endif // !XS57FILEINFO_HEADER