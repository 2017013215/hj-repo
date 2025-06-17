// Desc£º	S57 Format Reader
// Date:	2017/04/03
// Author:	Li XiaoQiang

#ifndef XS57READER_HEADER
#define XS57READER_HEADER

#include "XS57FileInfo.h"

namespace XSpace_Plugins
{
	class XPLUGINS_S57_EXPORT XS57Reader
	{
	public:
		XS57Reader();
		~XS57Reader();

		xeError_S57Plugins ReadFile(const QString& _strPath, XS57FileInfo*& _pRefFileInfo);

	private:

		xeError_S57Plugins takeDDRInfo(QByteArray& _xData, XS57FileInfo* _pRefFileInfo);

		xeError_S57Plugins takeDRInfo(QByteArray& _xData, XS57FileInfo* _pRefFileInfo);

		xeError_S57Plugins parseDataStruct(QByteArray& _xData, XS57DataStructInfo& _xDataStructInfo);

		xeError_S57Plugins parseDRData(QByteArray& _xData, XS57FileInfo* _pRefFileInfo);

		xeError_S57Plugins takeFieldData(QByteArray& _xBytes, const XField& _xFieldef, XFieldRecord& _xRefRecord);

		xeError_S57Plugins parseLeader( QByteArray& _xContent, XLeader& _xLeader);

		xeError_S57Plugins parseField( QByteArray& _xBytes, XField& _xField );

		xeError_S57Plugins dataConverVarByType(QByteArray& _xBytes, 
			quint32& _unFByteWidth, const QVariant::Type& _eType, const quint32& _unStructCode,
			const quint32& _unDataType, const bool& _bIsBinary, const QString& _strEscapeCharSet, QVariant& _xVarRef);

		xeError_S57Plugins procNewRecords(const XFieldRecord& _xNewRecord, XS57FileInfo* _pRefFileInfo);

		xeError_S57Plugins procDSSI(const XFieldRecord& _xDSSIRec, XS57_Struct_DSSI& _xRefDSSI);

		xeError_S57Plugins procDSID(const XFieldRecord& _xDSIDRec, XS57_Struct_DSID& _xRefDSID);

		xeError_S57Plugins procDSPM(const XFieldRecord& _xDSPMRec, XS57_Struct_DSPM& _xRefDSPM);

		xeError_S57Plugins procVRID(const XFieldRecord& _xVRIDRec, const quint32& _unCOUN, const quint32& _unSOMF, XS57_Struct_VRID& _xRefVRID);

		xeError_S57Plugins procSG3D(const XFieldRecord& _xSG3DRec, const quint32& _unCOUN, const quint32& _unSOMF,QList<QVector3D>& _xSG3Ds);

		xeError_S57Plugins procSG2D(const XFieldRecord& _xSG2DRec, const quint32& _unCOUN, const quint32& _unSOMF, QList<QVector2D>& _xSG2Ds);

		xeError_S57Plugins procATTV(const XFieldRecord& _xATTVRec, QList<XS57_Struct_ATTVF>& _xRefATTVs);

		xeError_S57Plugins procVRPT(const XFieldRecord& _xVRPTRec, QList<XS57_Struct_VRPT>& _xRefVRPTs);

		xeError_S57Plugins procFRID(const XFieldRecord& _xFRIDRec, XS57_Struct_FRID& _xRefFRID);

		xeError_S57Plugins procFOID(const XFieldRecord& _xFOIDRec, XS57_Struct_FOID& _xRefFOID);

		xeError_S57Plugins procFSPT(const XFieldRecord& _xFSPTRec,QList< XS57_Struct_FSPT>& _xRefFSPTs);

		xeError_S57Plugins procFFPT(const XFieldRecord& _xFFPTRec, QList<XS57_Struct_FFPT>& _xRefFFPTs);
	};
}

#endif // !XS57READER_HEADER