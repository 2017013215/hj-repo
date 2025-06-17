#include "XS57FileInfo.h"

namespace XSpace_Plugins
{
	XS57FileInfo::XS57FileInfo()
	{
		m_strFilePath = QString();
		m_xStructInfo = XS57DataStructInfo();
		m_xDSCommonInfo = XS57_Struct_DSID();
		m_xDSGeoInfo = XS57_Struct_DSPM();
	}


	XS57FileInfo::~XS57FileInfo()
	{
		Clear();
	}

	void XS57FileInfo::Clear()
	{
		m_strFilePath = QString();
		m_xStructInfo = XS57DataStructInfo();
		m_xDSCommonInfo = XS57_Struct_DSID();
		m_xDSGeoInfo = XS57_Struct_DSPM();

		m_xVRIDSet.clear();
		for each (XS57_Struct_VRID* pVRID in m_xVRIDs)
		{
			if (pVRID != NULL)
				delete pVRID;
			pVRID = NULL;
		}
		m_xVRIDs.clear();

		for each (XS57_Struct_FRID* pFRID in m_xFRIDs)
		{
			if (pFRID != NULL)
				delete pFRID;
			pFRID = NULL;
		}
		m_xFRIDs.clear();

	}

	const XS57_Struct_VRID* XS57FileInfo::GetVRIDByName(const QString& _strName)
	{
		if (m_xVRIDSet.contains(_strName))
			return m_xVRIDSet[_strName];

		foreach(XS57_Struct_VRID* pVRID, m_xVRIDs)
		{
			if (pVRID&& pVRID->strKey == _strName)
				return pVRID;
		}
		return NULL;
	}

}