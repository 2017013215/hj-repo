#include "XS57Reader.h"
#include <QtCore/QFileInfo>


#define xUS 0x1F
#define xRS 0x1E

namespace XSpace_Plugins
{
	XS57Reader::XS57Reader() 
	{
		
	}

	XS57Reader::~XS57Reader()
	{
	}

	XSpace_Plugins::xeError_S57Plugins XS57Reader::parseLeader( QByteArray& _xContent, XLeader& _xLeader)
	{
		quint32 unCount = _xContent.count();
		if (unCount < 24)
			return xeError_S57Plugins_FileParseFaild;

		_xLeader.unLen = _xContent.left(5).toUInt();
		_xLeader.ucSwapLv = (uchar)_xContent.at(5);
		_xLeader.ucMark = (uchar)_xContent.at(6);
		_xLeader.ucExpand = (uchar)_xContent.at(7);
		_xLeader.ucVersion = (uchar)_xContent.at(8);
		_xLeader.ucAppMark = (uchar)_xContent.at(9);
		_xLeader.unFieldLen = _xContent.mid(10, 2).toUInt();
		_xLeader.unFieldAddress = _xContent.mid(12, 5).toUInt();
		_xLeader.strExpandCharSet = QString::fromLocal8Bit(_xContent.mid(17, 3));
		_xLeader.unFieldLenSize = _xContent.at(20) - '0';
		_xLeader.unFieldPosSize = _xContent.at(21) - '0';
		_xLeader.ucRersvedWord = (uchar)_xContent.at(22);
		_xLeader.unFieldMarkSize = _xContent.at(23) - '0';

		_xContent.remove(0, 24);

		return xeError_S57Plugins_OK;
	}

	XSpace_Plugins::xeError_S57Plugins XS57Reader::ReadFile(const QString& _strPath, XS57FileInfo*& _pRefFileInfo)
	{
		// 判断文件是否存在
		QFileInfo s57FileInfo(_strPath);
		if (!s57FileInfo.exists())
			return xeError_S57Plugins_FileNotFound;

		// 判断文件后缀--暂时仅以此校验文件正确性
		if (s57FileInfo.suffix() != "000")
			return xeError_S57Plugins_FileFormatFaild;

		QFile s57File(_strPath);
		if (!s57File.open(QIODevice::ReadOnly))
			return xeError_S57Plugins_FileOpenFaild;

		_pRefFileInfo = new XS57FileInfo();
		_pRefFileInfo->m_strFilePath = _strPath;
		_pRefFileInfo->m_xStructInfo = XS57DataStructInfo();

		// 读取DDR头标区-该区主要存储了数据结构描述信息
		QByteArray bytes = s57File.readAll();
		xeError_S57Plugins xError = takeDDRInfo(bytes,_pRefFileInfo);
		if (xError != xeError_S57Plugins_OK)
		{
			s57File.close();
			return xError;
		}

		// 读取数据
		xError = takeDRInfo(bytes,_pRefFileInfo);
		if (xError != xeError_S57Plugins_OK)
		{
			delete _pRefFileInfo;
			_pRefFileInfo = NULL;
			s57File.close();
			return xError;
		}

		s57File.close();
		return xeError_S57Plugins_OK;
	}

	XSpace_Plugins::xeError_S57Plugins XS57Reader::takeDDRInfo(QByteArray& _xData, XS57FileInfo* _pRefFileInfo)
	{
		// 获取长度信息
		quint32 unDataLen = _xData.count();
		if (unDataLen < 5)
			return xeError_S57Plugins_FileParseFaild;

		quint32 unDDRLen = _xData.left(5).toUInt();
		if (unDDRLen <= 0 || unDataLen < unDDRLen)
			return xeError_S57Plugins_FileParseFaild;

		QByteArray ddrBytes = _xData.left(unDDRLen);
		_xData.remove(0, unDDRLen);
		unDataLen = _xData.count();

		xeError_S57Plugins xError = parseDataStruct(ddrBytes, _pRefFileInfo->m_xStructInfo);
		if (xError != xeError_S57Plugins_OK)
			return xError;

		return xeError_S57Plugins_OK;
	}

	XSpace_Plugins::xeError_S57Plugins XS57Reader::parseDataStruct(QByteArray& _xData, XS57DataStructInfo& _xDataStructInfo)
	{
		xeError_S57Plugins xError = parseLeader(_xData, _xDataStructInfo.xLeader);
		if (xError != xeError_S57Plugins_OK)
			return xError;

		// 解析数据结构信息
		if (_xData.count() < 4)
			return xeError_S57Plugins_FileParseFaild;
		if (_xData.left(4) != "0000")
			return xeError_S57Plugins_FileParseFaild;	// 格式错误
		quint32 unEndIdx = _xData.indexOf(xRS);		// Find ‘RS’记录分离符
		if (unEndIdx < 0 || unEndIdx < quint32(+_xDataStructInfo.xLeader.unFieldLenSize+_xDataStructInfo.xLeader.unFieldPosSize))
			return xeError_S57Plugins_FileParseFaild;
		QByteArray xStructBytes = _xData.left(unEndIdx+1);
		_xData.remove(0, unEndIdx+1);

		quint32 unSizeLen = _xDataStructInfo.xLeader.unFieldLenSize;
		quint32 unSizePos = _xDataStructInfo.xLeader.unFieldPosSize;

		// 循环获取数据结构信息
		QStringList strKeySeq;
		while ((quint32)xStructBytes.count() >= unSizeLen + unSizePos +4 && xStructBytes.at(0) != xRS)
		{
			XField xField;
			xField.strKey = xStructBytes.left(4);
			xField.unLen = xStructBytes.mid(4, unSizeLen).toUInt();
			xField.unPos = xStructBytes.mid(4 + unSizeLen, unSizePos).toUInt();
			strKeySeq.push_back(xField.strKey);
			_xDataStructInfo.xFieldMaps.insert(xField.strKey, xField);
			xStructBytes.remove(0, 4 + unSizeLen + unSizePos);
		}
		
		// 读取各字段数据内容
		quint32 unReadLen = 0;
		for each (QString strKey in strKeySeq)
		{
			XField xTempField = _xDataStructInfo.xFieldMaps[strKey];
			QByteArray valBytes = _xData.mid(xTempField.unPos, xTempField.unLen);
			unReadLen += xTempField.unLen;
			// 解析文件控制字段
			if (valBytes.left(9) == "0000;&   ")
			{
				xTempField.eValType = QVariant::Invalid;
				xTempField.strName = QString::fromLocal8Bit(valBytes.mid(9, valBytes.indexOf(xUS) - 9));
				_xDataStructInfo.xFieldMaps[strKey] = xTempField;
				valBytes.remove(0, valBytes.indexOf(xUS) + 1);
				// 解析字段嵌套关系
				QString strTempKey = QString();
				while (valBytes.count()>=4 && valBytes.at(0)!= xRS)
				{
					if (valBytes.left(4) == "0001" && valBytes.count()>=8)
					{
						strTempKey = valBytes.mid(4, 4);
						valBytes.remove(0, 8);
					}
					
					_xDataStructInfo.xFieldMaps[strTempKey].strSubFieldKeyList.push_back(valBytes.left(4));
					valBytes.remove(0, 4);
				}
			}
			else
			{
				// 根据不同的类型解析不同的数据
				if (parseField(valBytes, xTempField) != xeError_S57Plugins_OK)
					return xeError_S57Plugins_FileParseFaild;
					_xDataStructInfo.xFieldMaps[xTempField.strKey] = xTempField;
			}
		}

		if (unReadLen != _xData.length())
			return xeError_S57Plugins_FileParseFaild;
		_xData.clear();
		return xeError_S57Plugins_OK;
	}

	XSpace_Plugins::xeError_S57Plugins XS57Reader::parseField(QByteArray& _xBytes, XField& _xField)
	{
		QString strTemp = _xBytes.left(9);
		quint32 unIdx = -1;

		// 解析字段控制类型
		_xField.unStructType = strTemp.at(0).cell() - '0';
		_xField.unDataType = strTemp.at(1).cell() - '0';
		_xField.strExtendControl = strTemp.mid(2, 2);
		_xField.strEscapeCharSet = strTemp.right(3);

		_xBytes.remove(0, 9);
		unIdx = _xBytes.indexOf(xUS);
		_xField.strName = _xBytes.left(unIdx);
		_xBytes.remove(0, unIdx+1);
		if (_xBytes.at(0) == xUS)
			_xBytes.remove(0, 1);
		if (_xBytes.at(_xBytes.count() - 1) == xRS)
			_xBytes.remove(_xBytes.count() - 1, 1);

		QStringList strSubFields = QStringList();
		if (_xField.unStructType != 0)
		{
			unIdx = _xBytes.indexOf(xUS);
			strSubFields = QString(_xBytes.left(unIdx)).split('!');
			_xBytes.remove(0, unIdx + 1);
		}
		// 数据格式字段校验
		if (_xBytes.at(0) != '(' && _xBytes.at(_xBytes.count() - 1) != ')')
			return xeError_S57Plugins_FileFormatFaild;
		_xBytes.remove(0, 1);
		_xBytes.remove(_xBytes.count() - 1, 1);
		QStringList strSubFieldTypes = QString(_xBytes).split(',');
		_xBytes.clear();
		if (_xField.unStructType == 0)
		{	// 单数据项
			xeError_S57Plugins xError = _xField.SetFormat(strSubFieldTypes.at(0));
			if (xError != xeError_S57Plugins_OK)
				return xError;
		}
		else
		{
			if (strSubFieldTypes.count() < strSubFields.count())
			{	// 数据类型中使用了缩写 重新解析数据类型
				QStringList strTemps;
				for each (strTemp in strSubFieldTypes)
				{
					QString strNumber = QString();
					while (true)
					{
						uchar ucTemp = strTemp.at(0).cell();
						if (ucTemp <= '0' || ucTemp > '9')
							break;
						strNumber.append(ucTemp);
						strTemp.remove(0, 1);
					}
					if (!strNumber.isEmpty())
					{
						for (quint32 unIndex = 0; unIndex < strNumber.toUInt(); unIndex++)
							strTemps.push_back(strTemp);
					}
					else
						strTemps.push_back(strTemp);
				}

				if (strTemps.count() != strSubFields.count())
					return xeError_S57Plugins_FileParseFaild;
				strSubFieldTypes = strTemps;
			}
			for (quint32 unIndex = 0; unIndex < (quint32)strSubFields.count(); unIndex++)
			{
				XField xSubValField;
				xSubValField.strKey = strSubFields.at(unIndex);
				xSubValField.strName = _xField.strKey;
				xSubValField.unLen = 0;
				xSubValField.unPos = 0;
				xSubValField.unFormatWidth = 0;
				xSubValField.eBinaryFormat = NotBinary;
				xSubValField.unStructType = _xField.unStructType;
				xSubValField.unDataType = _xField.unDataType;
				xSubValField.strExtendControl = _xField.strExtendControl;
				xSubValField.strEscapeCharSet = _xField.strEscapeCharSet;
				xSubValField.eValType = QVariant::Invalid;
				xeError_S57Plugins xError = xSubValField.SetFormat(strSubFieldTypes.at(unIndex));
				if (xError != xeError_S57Plugins_OK)
					return xError;
				_xField.xValFields.push_back(xSubValField);
				_xField.unFormatWidth = 0;
				_xField.eBinaryFormat = NotBinary;
				_xField.eValType = QVariant::Invalid;
			}
		}

		if (_xBytes.isEmpty() == false)
			return xeError_S57Plugins_FileParseFaild;

		return xeError_S57Plugins_OK;
	}

	XSpace_Plugins::xeError_S57Plugins XS57Reader::takeDRInfo(QByteArray& _xData,XS57FileInfo* _pRefFileInfo)
	{
		while (_xData.count() > 5)
		{
			quint32 unLen = _xData.left(5).toUInt();
			if ((quint32)_xData.count() < unLen)
				break;

			QByteArray xLineBytes = _xData.left(unLen);
			_xData.remove(0, unLen);

			xeError_S57Plugins xError = parseDRData(xLineBytes,_pRefFileInfo);
			if (xError != xeError_S57Plugins_OK)
				return xError;
		}
		if (_xData.count() > 0)
			return xeError_S57Plugins_FileParseFaild;
		return xeError_S57Plugins_OK;
	}

	XSpace_Plugins::xeError_S57Plugins XS57Reader::parseDRData(QByteArray& _xData,XS57FileInfo* _pRefFileInfo)
	{
		XLeader xLeader;
		xeError_S57Plugins xError = parseLeader(_xData, xLeader);
		if (xError != xeError_S57Plugins_OK)
			return xError;

		// 解析数据结构信息
		if (_xData.count() < 4)
			return xeError_S57Plugins_FileParseFaild;
		if (_xData.left(4) != "0001")
			return xeError_S57Plugins_FileParseFaild;	// 格式错误
		QString strKey = _xData.left(4);
		quint32 unEndIdx = _xData.indexOf(xRS);		// Find ‘RS’记录分离符
		if (unEndIdx < 0 || unEndIdx < quint32(+xLeader.unFieldLenSize + xLeader.unFieldPosSize))
			return xeError_S57Plugins_FileParseFaild;
		QByteArray xStructBytes = _xData.left(unEndIdx + 1);
		_xData.remove(0, unEndIdx + 1);

		quint32 unSizeLen = xLeader.unFieldLenSize;
		quint32 unSizePos = xLeader.unFieldPosSize;

		// 循环获取数据结构信息
		QStringList strKeySeq;
		QString strFieldKey = QString();
		quint32 unLen = 0;
		quint32 unPos = 0;
		XFieldRecord xRootRecord;
		XFieldRecord xIdentifyRecord;
		bool bIsReadOne = false;
		while ((quint32)xStructBytes.count() >= unSizeLen + unSizePos + 4 && xStructBytes.at(0) != xRS)
		{
			strFieldKey = xStructBytes.left(4);
			unLen = xStructBytes.mid(4, unSizeLen).toUInt();
			unPos = xStructBytes.mid(4 + unSizeLen, unSizePos).toUInt();
			xStructBytes.remove(0, 4 + unSizeLen + unSizePos);

			if (_pRefFileInfo->m_xStructInfo.xFieldMaps.contains(strFieldKey) == false)
				return xeError_S57Plugins_DataDefNotFound;
			
			// 读取数据区相应记录
			QByteArray xFileBytes = _xData.left(unLen);
			XFieldRecord xResultRecord;
			xError = takeFieldData(xFileBytes, _pRefFileInfo->m_xStructInfo.xFieldMaps[strFieldKey], xResultRecord);
			if (xError != xeError_S57Plugins_OK)
				return xError;
			if (strFieldKey == "0001")
				xIdentifyRecord = xResultRecord;
			else if (bIsReadOne == false )
			{
				bIsReadOne = true;
				xRootRecord = xResultRecord;
				xRootRecord.varIdentifyID = xIdentifyRecord.varIdentifyID;
				xRootRecord.strStructKey = strFieldKey;
			}
			else
			{
				xResultRecord.varIdentifyID = xIdentifyRecord.varIdentifyID;
				xResultRecord.strStructKey = strFieldKey;
				xRootRecord.xSubRecordList.push_back(xResultRecord);
			}
			_xData.remove(0, unLen);
		}
		if (_xData.count() != 0)
			return xeError_S57Plugins_FileParseFaild;
		
		xError = procNewRecords(xRootRecord,_pRefFileInfo);
		if (xError != xeError_S57Plugins_OK)
			return xError;

		return xeError_S57Plugins_OK;
	}

	XSpace_Plugins::xeError_S57Plugins XS57Reader::takeFieldData(QByteArray& _xBytes, const XField& _xFieldDef, XFieldRecord& _xRefRecord)
	{
		quint32 unCount = _xBytes.count();
		
		/* 简单校验 */
		{
			if (unCount <= 0)
				return xeError_S57Plugins_FileParseFaild;

			if (_xBytes.at(unCount - 1) == xRS)
			{
				_xBytes.remove(unCount - 1, 1);
				unCount -= 1;
			}
			if (unCount <= 0)
				return xeError_S57Plugins_FileParseFaild;
		}
		
		quint32 unFormatWidth = 0;
		QVariant::Type eValType = QVariant::Invalid;
		unFormatWidth = _xFieldDef.unFormatWidth;
		eValType = _xFieldDef.eValType;
		QVariant varTemp;
		if (unFormatWidth > 0)
		{
			// 单字段
			varTemp = QVariant();
			xeError_S57Plugins xError = dataConverVarByType(_xBytes, unFormatWidth, _xFieldDef.eValType,
				_xFieldDef.unStructType, _xFieldDef.unDataType, _xFieldDef.bIsBinary,_xFieldDef.strEscapeCharSet, varTemp);
			if (xError != xeError_S57Plugins_OK)
				return xError;

			if (!varTemp.isValid())
				return xeError_S57Plugins_FormatNoSup;
			if (_xFieldDef.strKey == "0001")
			{
				_xRefRecord.varIdentifyID = varTemp;
				_xRefRecord.strStructKey = _xFieldDef.strKey;
			}				
		}
		
		// 解析多字段数据
		quint32 unFieldLen = 0;
		QList<XField>::const_iterator itr = _xFieldDef.xValFields.constBegin();
		XFieldRecord xManyDimRecordTemp = XFieldRecord();
		bool bIsManyDim = false;
		while (itr != _xFieldDef.xValFields.constEnd())
		{
			varTemp = QVariant();
			eValType = itr->eValType;
			unFormatWidth = itr->unFormatWidth;
			QString str = itr->strKey;
			xeError_S57Plugins xError = dataConverVarByType(_xBytes, unFormatWidth, itr->eValType,
				_xFieldDef.unStructType, _xFieldDef.unDataType, itr->bIsBinary,_xFieldDef.strEscapeCharSet, varTemp);
			if (xError != xeError_S57Plugins_OK)
				return xError;
			_xRefRecord.varFieldList.push_back(varTemp);
			_xRefRecord.strFieldListKeys.push_back(itr->strKey);
			unFieldLen += unFormatWidth;
			itr += 1;

			// 多维数据处理
			if (itr == _xFieldDef.xValFields.constEnd()
				&& _xFieldDef.unStructType == 2 && _xBytes.count() > 0/*&&(quint32)_xBytes.count()>=unFieldLen && _xBytes.count()%unFieldLen == 0*/)
			{
				if (xManyDimRecordTemp.bIsManyDim == false)
				{
					xManyDimRecordTemp = _xRefRecord;
					xManyDimRecordTemp.bIsManyDim = true;
					bIsManyDim = true;
				}
				else
					xManyDimRecordTemp.xManyDimRecordList.push_back(_xRefRecord);
				_xRefRecord = XFieldRecord();
				itr = _xFieldDef.xValFields.constBegin();
				unFieldLen = 0;
			}
		}
		if (bIsManyDim)
		{
			xManyDimRecordTemp.xManyDimRecordList.push_back(_xRefRecord);
			_xRefRecord = xManyDimRecordTemp;
		}
			
		if (_xBytes.count() > 0)
			return xeError_S57Plugins_FileParseFaild;
		return xeError_S57Plugins_OK;
	}

	XSpace_Plugins::xeError_S57Plugins XS57Reader::dataConverVarByType(QByteArray& _xBytes,
		quint32& _unFByteWidth, const QVariant::Type& _eType, const quint32& _unStructCode,
		const quint32& _unDataType, const bool& _bIsBinary, const QString& _strEscapeCharSet, QVariant& _xVarRef)
	{
		int nTemp = 0;
		QString strTemp = QString();
		double dTemp = 0.0;
		QByteArray bytesTemp;
		quint64 unTemp64 = 0;

		quint32 unFBytes = _unFByteWidth;
		QVariant::Type eType = _eType;
		switch (eType)
		{
		case QVariant::Int:
		{
			if (_bIsBinary == 0) // 字符串代码
				nTemp = _xBytes.left(unFBytes).toInt();
			else if (_bIsBinary) // 二进制代码
			{
				if (unFBytes > 4)
					return xeError_S57Plugins_FileParseFaild;
				int nBitMove = 0;
				for (quint32 unIdx = 0; unIdx < unFBytes; unIdx++)
				{
					nTemp = nTemp | (uchar)(_xBytes.at(unIdx)) << nBitMove;
					nBitMove += 8;
				}
			}
			_xVarRef.setValue(nTemp);
			_xBytes.remove(0, unFBytes);
		}break;
		case QVariant::String:
		{
			if (unFBytes == 0)
			{
				quint32 unIdex = _xBytes.indexOf(xUS);
					
				if (unIdex == -1)
					unIdex = _xBytes.length();
				if (unIdex < 0)
					return xeError_S57Plugins_FileFormatFaild;
				for (quint32 unTemIdx = 0; unTemIdx < unIdex; unTemIdx ++)
				{
					if (_strEscapeCharSet == "-A " || _strEscapeCharSet == "   ") // 2级词汇
						strTemp.push_back((uchar)_xBytes.at(unTemIdx));
					else{
						strTemp = QString::fromLocal8Bit(_xBytes.left(unIdex));
						break;
					}
				}
				_xVarRef.setValue(strTemp);
				_xBytes.remove(0, unIdex+1);
				_unFByteWidth = unIdex;
			}
			else
			{
				if ((quint32)_xBytes.count() < unFBytes)
					return xeError_S57Plugins_FileFormatFaild;
				for (quint32 unTemIdx = 0; unTemIdx < unFBytes; unTemIdx++)
				{
					if (_strEscapeCharSet == "-A ") // 2级词汇
						strTemp.push_back((uchar)_xBytes.at(unTemIdx));
					else{
						strTemp = QString::fromLocal8Bit(_xBytes.left(unFBytes));
						break;
					}
				}
				_xVarRef.setValue(strTemp);
				_xBytes.remove(0, unFBytes);
			}
		}break;
		case QVariant::Double:
		{
			if (unFBytes == 0 || (quint32)_xBytes.count() < unFBytes)
				return xeError_S57Plugins_FileFormatFaild;
			if (_bIsBinary)
			{
				if (unFBytes > 8)
					return xeError_S57Plugins_FileParseFaild;
				int nBitMove = 0;
				for (quint32 unIdx = 0; unIdx < unFBytes; unIdx++)
				{
					unTemp64 = unTemp64 | ((uchar)(_xBytes.at(unIdx)) << nBitMove);
					nBitMove += 8;
				}
				XBitsConver xConv;
				xConv.unInt64 = unTemp64;
				if (unFBytes <= 4)
					dTemp = xConv.fFloat;
				else
					dTemp = xConv.dDouble;
			}else
				dTemp = _xBytes.left(unFBytes).toDouble();
		
			_xVarRef.setValue(dTemp);
			_xBytes.remove(0, unFBytes);
		}break;
		case QVariant::BitArray:
		{
			if ((quint32)_xBytes.count() < unFBytes)
				return xeError_S57Plugins_FileFormatFaild;
			bytesTemp = _xBytes.left(unFBytes);
			_xVarRef.setValue(bytesTemp);
			_xBytes.remove(0, unFBytes);
		}break;
		default:
			return xeError_S57Plugins_FileFormatFaild;
			break;
		}
		return xeError_S57Plugins_OK;
	}

	XSpace_Plugins::xeError_S57Plugins XS57Reader::procNewRecords(const XFieldRecord& _xNewRecord, XS57FileInfo* _pRefFileInfo)
	{
		xeError_S57Plugins xError = xeError_S57Plugins_OK;
		/* DSID 数据集通用信息结构 */
		if (_xNewRecord.strStructKey == "DSID")
		{
			_pRefFileInfo->m_xDSCommonInfo.unIdentifyID = _xNewRecord.varIdentifyID.toUInt();
			xError = procDSID(_xNewRecord, _pRefFileInfo->m_xDSCommonInfo);
			if (xError != xeError_S57Plugins_OK)
				return xError;
		}
		else if (_xNewRecord.strStructKey == "DSPM")
		{
			_pRefFileInfo->m_xDSGeoInfo.unIdentifyID = _xNewRecord.varIdentifyID.toUInt();
			xError = procDSPM(_xNewRecord, _pRefFileInfo->m_xDSGeoInfo);
			if (xError != xeError_S57Plugins_OK)
				return xError;
		}
		else if (_xNewRecord.strStructKey == "VRID")
		{
			XS57_Struct_VRID* pxNewVRID = new XS57_Struct_VRID();
			pxNewVRID->unIdentifyID = _xNewRecord.varIdentifyID.toUInt();
			xError = procVRID(_xNewRecord, _pRefFileInfo->m_xDSGeoInfo.unCOMF, _pRefFileInfo->m_xDSGeoInfo.unSOMF,*pxNewVRID);
			if (xError != xeError_S57Plugins_OK){
				delete pxNewVRID;
				pxNewVRID = NULL;
				return xError;
			}
			_pRefFileInfo->m_xVRIDs.push_back(pxNewVRID);
			pxNewVRID = NULL;
		}
		else if (_xNewRecord.strStructKey == "FRID")
		{
			XS57_Struct_FRID* pxNewFRID = new XS57_Struct_FRID();
			pxNewFRID->unIdentifyID = _xNewRecord.varIdentifyID.toUInt();
			xError = procFRID(_xNewRecord, *pxNewFRID);
			if (xError != xeError_S57Plugins_OK){
				delete pxNewFRID;
				pxNewFRID = NULL;
				return xError;
			}

			_pRefFileInfo->m_xFRIDs.push_back(pxNewFRID);
			pxNewFRID = NULL;
		}
		else
			_pRefFileInfo->m_xErrorRecords.push_back(_xNewRecord);


		return xeError_S57Plugins_OK;
	}

	XSpace_Plugins::xeError_S57Plugins XS57Reader::procDSSI(const XFieldRecord& _xDSSIRec, XS57_Struct_DSSI& _xRefDSSI)
	{
		if (_xDSSIRec.strStructKey != "DSSI")
			return xeError_S57Plugins_FileParseFaild;

		quint32 unIndex = 0;
		QString strKey = QString();
		QString strTemp = QString();
		QVariant::Type varType = QVariant::Invalid;
		for each (QVariant var in _xDSSIRec.varFieldList)
		{
			strKey = _xDSSIRec.strFieldListKeys.at(unIndex);
			varType = var.type();
			if (strKey == "DSTR"){
				if (varType == QVariant::Int)
					_xRefDSSI.eDSTR = XS57_Struct_DSSI::xeDSTR(var.toUInt());
				else if (varType == QVariant::String){
					strTemp = var.toString();
					if (strTemp == "CS")
						_xRefDSSI.eDSTR = XS57_Struct_DSSI::xeDSTR_CS;
					else if (strTemp == "CN")
						_xRefDSSI.eDSTR = XS57_Struct_DSSI::xeDSTR_CN;
					else if (strTemp == "PG")
						_xRefDSSI.eDSTR = XS57_Struct_DSSI::xeDSTR_PG;
					else if (strTemp == "FT")
						_xRefDSSI.eDSTR = XS57_Struct_DSSI::xeDSTR_FT;
					else if (strTemp == "NO")
						_xRefDSSI.eDSTR = XS57_Struct_DSSI::xeDSTR_NO;
				}
			}
			else if (strKey == "AALL")
				_xRefDSSI.eATTFWordLv = xeWordLv(var.toUInt());
			else if (strKey == "NALL")
				_xRefDSSI.eNALLWordLv = xeWordLv(var.toUInt());
			else if (strKey == "NOMR")
				_xRefDSSI.unNOMR = var.toUInt();
			else if (strKey == "NOCR")
				_xRefDSSI.unNOCR = var.toUInt();
			else if (strKey == "NOGR")
				_xRefDSSI.unNOGR = var.toUInt();
			else if (strKey == "NOLR")
				_xRefDSSI.unNOLR = var.toUInt();
			else if (strKey == "NOIN")
				_xRefDSSI.unNOIN = var.toUInt();
			else if (strKey == "NOCN")
				_xRefDSSI.unNOCN = var.toUInt();
			else if (strKey == "NOED")
				_xRefDSSI.unNOED = var.toUInt();
			else if (strKey == "NOFA")
				_xRefDSSI.unNOFA = var.toUInt();

			unIndex++;
		}

		return xeError_S57Plugins_OK;
	}

	XSpace_Plugins::xeError_S57Plugins XS57Reader::procDSID(const XFieldRecord& _xDSIDRec, XS57_Struct_DSID& _xRefDSID)
	{
		if (_xDSIDRec.strStructKey != "DSID")
			return xeError_S57Plugins_FileParseFaild;

		quint32 unIndex = 0;
		QString strKey = QString();
		QString strTemp = QString();
		for each (QVariant var in _xDSIDRec.varFieldList)
		{
			strKey = _xDSIDRec.strFieldListKeys.at(unIndex);
			if (strKey == "RCNM")
				_xRefDSID.strName = var.toString();
			else if (strKey == "RCID")
				_xRefDSID.unID = var.toUInt();
			else if (strKey == "EXPP"){
				if (var.type() == QVariant::Int)
					_xRefDSID.eEXPP = XS57_Struct_DSID::xeEXPP(var.toUInt());
				else{
					strTemp = var.toString();
					if (strTemp == "N")
						_xRefDSID.eEXPP = XS57_Struct_DSID::xeEXPP_NEW;
					else if (strTemp == "R")
						_xRefDSID.eEXPP = XS57_Struct_DSID::xeEXPP_R;
				}
			}
			else if (strKey == "INTU")
				_xRefDSID.eINTU = XS57_Struct_DSID::xeINTU(var.toUInt());
			else if (strKey == "DSNM")
				_xRefDSID.strDSName = var.toString();
			else if (strKey == "EDTN")
				_xRefDSID.strEDTN = var.toString();
			else if (strKey == "UPDN")
				_xRefDSID.strUPDN = var.toString();
			else if (strKey == "UADT")
				_xRefDSID.xUPDT = QDate::fromString(var.toString(), "yyyyMMdd");
			else if (strKey == "ISDT")
				_xRefDSID.xISDT = QDate::fromString(var.toString(), "yyyyMMdd");
			else if (strKey == "STED")
				_xRefDSID.dS57Number = var.toDouble();
			else if (strKey == "PRSP"){
				if (var.type() == QVariant::Int)
					_xRefDSID.ePRSP = XS57_Struct_DSID::xePRSP(var.toUInt());
				else{
					strTemp = var.toString();
					if (strTemp == "ENC")
						_xRefDSID.ePRSP = XS57_Struct_DSID::xePRSP_ENC;
					else if (strTemp == "ODD")
						_xRefDSID.ePRSP = XS57_Struct_DSID::xePRSP_ODD;
				}
			}
			else if (strKey == "PSDN")
				_xRefDSID.strPSDN = var.toString();
			else if (strKey == "PRED")
				_xRefDSID.strPRED = var.toString();
			else if (strKey == "PROF"){
				if (var.type() == QVariant::Int)
					_xRefDSID.ePROF = XS57_Struct_DSID::xePROF(var.toUInt());
				else{
					strTemp = var.toString();
					if (strTemp == "EN")
						_xRefDSID.ePROF = XS57_Struct_DSID::xePROF_EN;
					else if (strTemp == "ER")
						_xRefDSID.ePROF = XS57_Struct_DSID::xePROF_ER;
					else if (strTemp == "DD")
						_xRefDSID.ePROF = XS57_Struct_DSID::xePROF_DD;
				}
			}
			else if (strKey == "AGEN")
				_xRefDSID.strAGEN = var.toString();
			else if (strKey == "COMT")
				_xRefDSID.strCOMT = var.toString();

			unIndex++;
		}

		// 获取DSSI结构信息
		bool bIsSuccess = false;
		for each (XFieldRecord xRec in _xDSIDRec.xSubRecordList)
		{
			if (xRec.strStructKey == "DSSI")
			{
				if (procDSSI(xRec, _xRefDSID.xDSetStructInfo) == xeError_S57Plugins_OK)
					bIsSuccess = true;
				break;
			}
		}
		if (bIsSuccess == false)
			return xeError_S57Plugins_FileParseFaild;

		return xeError_S57Plugins_OK;
	}

	XSpace_Plugins::xeError_S57Plugins XS57Reader::procDSPM(const XFieldRecord& _xDSPMRec, XS57_Struct_DSPM& _xREfDSPM)
	{
		if (_xDSPMRec.strStructKey != "DSPM")
			return xeError_S57Plugins_FileParseFaild;

		quint32 unIndex = 0;
		QString strKey = QString();
		QString strTemp = QString();
		for each (QVariant var in _xDSPMRec.varFieldList)
		{
			strKey = _xDSPMRec.strFieldListKeys.at(unIndex);
			if (strKey == "RCNM")
				_xREfDSPM.strRCNM = var.toString();
			else if (strKey == "RCID")
				_xREfDSPM.unRCID = var.toUInt();
			else if (strKey == "HDAT")
				_xREfDSPM.eHDAT = xeHORDAT(var.toUInt());
			else if (strKey == "VDAT")
				_xREfDSPM.eVDAT = xeVERDAT(var.toUInt());
			else if (strKey == "SDAT")
				_xREfDSPM.eSDAT = xeVERDAT(var.toUInt());
			else if (strKey == "CSCL")
				_xREfDSPM.unCSCL = var.toUInt();
			else if (strKey == "DUNI")
				_xREfDSPM.eDUNI = xeDUNITS(var.toUInt());
			else if (strKey == "HUNI")
				_xREfDSPM.eHUNI = xeHUNITS(var.toUInt());
			else if (strKey == "COUN")
				_xREfDSPM.eCOUN = xeCUNITS(var.toUInt());
			else if (strKey == "COMF")
				_xREfDSPM.unCOMF = var.toUInt();
			else if (strKey == "SOMF")
				_xREfDSPM.unSOMF = var.toUInt();
			else if (strKey == "COMT")
				_xREfDSPM.strCOMT = var.toString();
			unIndex++;
		}

		// 子字段暂时不解析--Lost
		_xREfDSPM.xDSPR = XS57_Struct_DSPR();
		_xREfDSPM.xDSRC = XS57_Struct_DSRC();

		return xeError_S57Plugins_OK;
	}

	XSpace_Plugins::xeError_S57Plugins XS57Reader::procVRID(const XFieldRecord& _xVRIDRec, const quint32& _unCOUN, const quint32& _unSOMF, XS57_Struct_VRID& _xRefVRID)
	{
		if (_xVRIDRec.strStructKey != "VRID")
			return xeError_S57Plugins_FileParseFaild;

		quint32 unIndex = 0;
		QString strKey = QString();
		QString strTemp = QString();
		QVariant::Type eType = QVariant::Invalid;
		for each (QVariant var in _xVRIDRec.varFieldList)
		{
			strKey = _xVRIDRec.strFieldListKeys.at(unIndex);
			eType = var.type();
			if (strKey == "RCNM")
			{
				if (eType == QVariant::Int)
				{
					_xRefVRID.eRCNM = XS57_Struct_VRID::xeRCNM(var.toUInt());
					_xRefVRID.strKey += QChar(var.toUInt()).cell();
				}
				else if (eType == QVariant::String)
				{
					strTemp = var.toString();
					if (strTemp == "VI")
						_xRefVRID.eRCNM = XS57_Struct_VRID::xeVI;
					else if (strTemp == "VC")
						_xRefVRID.eRCNM = XS57_Struct_VRID::xeVC;
					else if (strTemp == "VE")
						_xRefVRID.eRCNM = XS57_Struct_VRID::xeVE;
					else if (strTemp == "VF")
						_xRefVRID.eRCNM = XS57_Struct_VRID::xeVF;
					strKey += strTemp;
				}
			}
			else if (strKey == "RCID")
			{
				_xRefVRID.unRCID = var.toUInt();
				XBitsConver xConv;
				xConv.nInt32 = _xRefVRID.unRCID;
				_xRefVRID.strKey += xConv.ucChar[0];
				_xRefVRID.strKey += xConv.ucChar[1];
				_xRefVRID.strKey += xConv.ucChar[2];
				_xRefVRID.strKey += xConv.ucChar[3];
			}
			else if (strKey == "RVER")
				_xRefVRID.unRVER = var.toUInt();
			else if (strKey == "RUIN"){
				if (eType == QVariant::Int)
					_xRefVRID.eRUIN = xeUPCODE(var.toUInt());
				else if (eType == QVariant::String){
					strTemp = var.toString();
					if (strTemp == "I")
						_xRefVRID.eRUIN = XSpace_Plugins::xeUPCODE_I;
					else if (strTemp == "D")
						_xRefVRID.eRUIN = XSpace_Plugins::xeUPCODE_D;
					else if (strTemp == "M")
						_xRefVRID.eRUIN = XSpace_Plugins::xeUPCODE_M;
				}
			}
			unIndex++;
		}
		

		// 解析子字段
		bool bIsSuccess = false;
		for each (XFieldRecord xRec in _xVRIDRec.xSubRecordList)
		{
			if (xRec.strStructKey == "SG3D")
			{
				if (procSG3D(xRec, _unCOUN, _unSOMF, _xRefVRID.xSG3Ds) != xeError_S57Plugins_OK)
					return xeError_S57Plugins_FileParseFaild;
			}else if (xRec.strStructKey == "ATTV"){
				if (procATTV(xRec, _xRefVRID.xATTVs) != xeError_S57Plugins_OK)
					return xeError_S57Plugins_FileParseFaild;
			}else if (xRec.strStructKey == "SG2D"){
				if (procSG2D(xRec, _unCOUN, _unSOMF, _xRefVRID.xSG2Ds) != xeError_S57Plugins_OK)
					return xeError_S57Plugins_FileParseFaild;
			}else if (xRec.strStructKey == "VRPT"){
				if (procVRPT(xRec, _xRefVRID.xVRPTs) != xeError_S57Plugins_OK)
					return xeError_S57Plugins_FileParseFaild;
			}
			else
				int md = 0;
		}
		//if (bIsSuccess == false)
		//	return xeError_S57Plugins_FileParseFaild;

		return xeError_S57Plugins_OK;
	}

	XSpace_Plugins::xeError_S57Plugins XS57Reader::procSG3D(const XFieldRecord& _xSG3DRec, const quint32& _unCOUN, const quint32& _unSOMF, QList<QVector3D>& _xSG3Ds)
	{
		if (_xSG3DRec.strStructKey != "SG3D")
			return xeError_S57Plugins_FileParseFaild;

		QVector3D ver3D = QVector3D();
		quint32 unIndex = 0;
		QString strKey = QString();
		double dTemp = 0;
		QVariant::Type varType = QVariant::Invalid;
		for each (QVariant var in _xSG3DRec.varFieldList)
		{
			strKey = _xSG3DRec.strFieldListKeys.at(unIndex);
			varType = var.type();
			if (strKey == "*YCOO")
				ver3D.setY(var.toInt()/(double)_unCOUN);
			else if (strKey == "XCOO")
				ver3D.setX(var.toDouble() / (double)_unCOUN);
			else if (strKey == "VE3D")
				ver3D.setZ(var.toDouble() / (double)_unSOMF);

			unIndex++;
		}
		_xSG3Ds.push_back(ver3D);

		if (_xSG3DRec.bIsManyDim)
		{
			for each (XFieldRecord xMany3D in _xSG3DRec.xManyDimRecordList)
			{
				unIndex = 0;
				ver3D = QVector3D();

				for each (QVariant var in xMany3D.varFieldList)
				{
					strKey = xMany3D.strFieldListKeys.at(unIndex);
					varType = var.type();
					if (strKey == "*YCOO")
						ver3D.setY(var.toInt() / (double)_unCOUN);
					else if (strKey == "XCOO")
						ver3D.setX(var.toDouble() / (double)_unCOUN);
					else if (strKey == "VE3D")
						ver3D.setZ(var.toDouble() / (double)_unSOMF);

					unIndex++;
				}
				_xSG3Ds.push_back(ver3D);
			}
		}

		return xeError_S57Plugins_OK;
	}

	XSpace_Plugins::xeError_S57Plugins XS57Reader::procATTV(const XFieldRecord& _xATTVRec, QList<XS57_Struct_ATTVF>& _xRefATTVs)
	{
		XS57_Struct_ATTVF xATTV = XS57_Struct_ATTVF();
		quint32 unIndex = 0;
		QString strKey = QString();
		double dTemp = 0;
		QVariant::Type varType = QVariant::Invalid;
		for each (QVariant var in _xATTVRec.varFieldList)
		{
			strKey = _xATTVRec.strFieldListKeys.at(unIndex);
			varType = var.type();
			if (strKey == "*ATTL")
				xATTV.unATTL = var.toUInt();
			else if (strKey == "ATVL")
				xATTV.strATVL = var.toString();

			unIndex++;
		}
		_xRefATTVs.push_back(xATTV);

		if (_xATTVRec.bIsManyDim)
		{
			for each (XFieldRecord xMany3D in _xATTVRec.xManyDimRecordList)
			{
				unIndex = 0;
				xATTV = XS57_Struct_ATTVF();

				for each (QVariant var in xMany3D.varFieldList)
				{
					strKey = xMany3D.strFieldListKeys.at(unIndex);
					varType = var.type();
					if (strKey == "*ATTL")
						xATTV.unATTL = var.toUInt();
					else if (strKey == "ATVL")
						xATTV.strATVL = var.toString();

					unIndex++;
				}
				_xRefATTVs.push_back(xATTV);
			}
		}

		return xeError_S57Plugins_OK;
	}

	XSpace_Plugins::xeError_S57Plugins XS57Reader::procSG2D(const XFieldRecord& _xSG2DRec, const quint32& _unCOUN, const quint32& _unSOMF, QList<QVector2D>& _xSG2Ds)
	{
		if (_xSG2DRec.strStructKey != "SG2D")
			return xeError_S57Plugins_FileParseFaild;

		QVector2D ver2D = QVector2D();
		quint32 unIndex = 0;
		QString strKey = QString();
		double dTemp = 0;
		QVariant::Type varType = QVariant::Invalid;
		for each (QVariant var in _xSG2DRec.varFieldList)
		{
			strKey = _xSG2DRec.strFieldListKeys.at(unIndex);
			varType = var.type();
			if (strKey == "*YCOO")
				ver2D.setY(var.toInt() / (double)_unCOUN);
			else if (strKey == "XCOO")
				ver2D.setX(var.toDouble() / (double)_unCOUN);

			unIndex++;
		}
		_xSG2Ds.push_back(ver2D);

		if (_xSG2DRec.bIsManyDim)
		{
			for each (XFieldRecord xMany2D in _xSG2DRec.xManyDimRecordList)
			{
				unIndex = 0;
				ver2D = QVector2D();

				for each (QVariant var in xMany2D.varFieldList)
				{
					strKey = xMany2D.strFieldListKeys.at(unIndex);
					varType = var.type();
					if (strKey == "*YCOO")
						ver2D.setY(var.toInt() / (double)_unCOUN);
					else if (strKey == "XCOO")
						ver2D.setX(var.toDouble() / (double)_unCOUN);

					unIndex++;
				}
				_xSG2Ds.push_back(ver2D);
			}
		}

		return xeError_S57Plugins_OK;
	}

	XSpace_Plugins::xeError_S57Plugins XS57Reader::procVRPT(const XFieldRecord& _xVRPTRec, QList<XS57_Struct_VRPT>& _xRefVRPTs)
	{
		if (_xVRPTRec.strStructKey != "VRPT")
			return xeError_S57Plugins_FileParseFaild;

		XS57_Struct_VRPT xVRPT = XS57_Struct_VRPT();
		quint32 unIndex = 0;
		QString strKey = QString();
		QString strTemp = QString();
		double dTemp = 0;
		QVariant::Type varType = QVariant::Invalid;
		for each (QVariant var in _xVRPTRec.varFieldList)
		{
			strKey = _xVRPTRec.strFieldListKeys.at(unIndex);
			varType = var.type();
			if (strKey == "*NAME")
			{
				QByteArray strTemp = var.toByteArray();
				foreach(QChar ucCode, strTemp)
				{
					xVRPT.strNAME.append(ucCode.cell());
				}
			}
			else if (strKey == "ORNT"){
				if (varType == QVariant::Int)
					xVRPT.eORNT = xeORNT(var.toUInt());
				else if (varType == QVariant::String){
					strTemp = var.toString();
					if (strTemp == "F")
						xVRPT.eORNT = xeORNT_F;
					else if (strTemp == "R")
						xVRPT.eORNT = xeORNT_R;
					else if (strTemp == "N")
						xVRPT.eORNT = xeORNT_N;
				}
			}else if (strKey == "USAG"){
				if (varType == QVariant::Int)
					xVRPT.eUSAG = xeUSAG(var.toUInt());
				else if (varType == QVariant::String){
					strTemp = var.toString();
					if (strTemp == "E")
						xVRPT.eUSAG = xeUSAG_E;
					else if (strTemp == "T")
						xVRPT.eUSAG = xeUSAG_T;
					else if (strTemp == "C")
						xVRPT.eUSAG = xeUSAG_C;
					else if (strTemp == "N")
						xVRPT.eUSAG = xeUSAG_N;
				}
			}else if (strKey == "TOPI"){
				if (varType == QVariant::Int)
					xVRPT.eTOPI = xeTOPI(var.toUInt());
				else if (varType == QVariant::String){
					strTemp = var.toString();
					if (strTemp == "B")
						xVRPT.eTOPI = xeTOPI_B;
					else if (strTemp == "E")
						xVRPT.eTOPI = xeTOPI_E;
					else if (strTemp == "S")
						xVRPT.eTOPI = xeTOPI_S;
					else if (strTemp == "D")
						xVRPT.eTOPI = xeTOPI_D;
					else if (strTemp == "F")
						xVRPT.eTOPI = xeTOPI_F;
					else if (strTemp == "N")
						xVRPT.eTOPI = xeTOPI_N;
				}
			}else if (strKey == "MASK"){
				if (varType == QVariant::Int)
					xVRPT.eMask = xeMASK(var.toUInt());
				else if (varType == QVariant::String){
					strTemp = var.toString();
					if (strTemp == "M")
						xVRPT.eMask = xeMASK_M;
					else if (strTemp == "S")
						xVRPT.eMask = xeMASK_S;
					else if (strTemp == "N")
						xVRPT.eMask = xeMASK_N;
				}
			}

			unIndex++;
		}
		_xRefVRPTs.push_back(xVRPT);

		if (_xVRPTRec.bIsManyDim)
		{
			for each (XFieldRecord xMany3D in _xVRPTRec.xManyDimRecordList)
			{
				unIndex = 0;
				xVRPT = XS57_Struct_VRPT();

				for each (QVariant var in xMany3D.varFieldList)
				{
					strKey = _xVRPTRec.strFieldListKeys.at(unIndex);
					varType = var.type();
					if (strKey == "*NAME")
					{
						QByteArray strTemp = var.toByteArray();
						foreach(QChar ucCode, strTemp)
						{
							xVRPT.strNAME.append(ucCode.cell());
						}
					}
					else if (strKey == "ORNT"){
						if (varType == QVariant::Int)
							xVRPT.eORNT = xeORNT(var.toUInt());
						else if (varType == QVariant::String){
							strTemp = var.toString();
							if (strTemp == "F")
								xVRPT.eORNT = xeORNT_F;
							else if (strTemp == "R")
								xVRPT.eORNT = xeORNT_R;
							else if (strTemp == "N")
								xVRPT.eORNT = xeORNT_N;
						}
					}
					else if (strKey == "USAG"){
						if (varType == QVariant::Int)
							xVRPT.eUSAG = xeUSAG(var.toUInt());
						else if (varType == QVariant::String){
							strTemp = var.toString();
							if (strTemp == "E")
								xVRPT.eUSAG = xeUSAG_E;
							else if (strTemp == "T")
								xVRPT.eUSAG = xeUSAG_T;
							else if (strTemp == "C")
								xVRPT.eUSAG = xeUSAG_C;
							else if (strTemp == "N")
								xVRPT.eUSAG = xeUSAG_N;
						}
					}
					else if (strKey == "TOPI"){
						if (varType == QVariant::Int)
							xVRPT.eTOPI = xeTOPI(var.toUInt());
						else if (varType == QVariant::String){
							strTemp = var.toString();
							if (strTemp == "B")
								xVRPT.eTOPI = xeTOPI_B;
							else if (strTemp == "E")
								xVRPT.eTOPI = xeTOPI_E;
							else if (strTemp == "S")
								xVRPT.eTOPI = xeTOPI_S;
							else if (strTemp == "D")
								xVRPT.eTOPI = xeTOPI_D;
							else if (strTemp == "F")
								xVRPT.eTOPI = xeTOPI_F;
							else if (strTemp == "N")
								xVRPT.eTOPI = xeTOPI_N;
						}
					}
					else if (strKey == "MASK"){
						if (varType == QVariant::Int)
							xVRPT.eMask = xeMASK(var.toUInt());
						else if (varType == QVariant::String){
							strTemp = var.toString();
							if (strTemp == "M")
								xVRPT.eMask = xeMASK_M;
							else if (strTemp == "S")
								xVRPT.eMask = xeMASK_S;
							else if (strTemp == "N")
								xVRPT.eMask = xeMASK_N;
						}
					}

					unIndex++;
				}

				_xRefVRPTs.push_back(xVRPT);
			}
		}

		return xeError_S57Plugins_OK;
	}

	XSpace_Plugins::xeError_S57Plugins XS57Reader::procFRID(const XFieldRecord& _xFRIDRec, XS57_Struct_FRID& _xRefFRID)
	{
		if (_xFRIDRec.strStructKey != "FRID")
			return xeError_S57Plugins_FileParseFaild;

		quint32 unIndex = 0;
		QString strKey = QString();
		QString strTemp = QString();
		QVariant::Type eType = QVariant::Invalid;
		for each (QVariant var in _xFRIDRec.varFieldList)
		{
			strKey = _xFRIDRec.strFieldListKeys.at(unIndex);
			eType = var.type();
			if (strKey == "RCNM")
			{
				_xRefFRID.strRCNM = var.toString();
			}
			else if (strKey == "RCID")
			{
				_xRefFRID.unRCID = var.toUInt();
			}
			else if (strKey == "PRIM"){
				if (eType == QVariant::Int)
					_xRefFRID.ePRIM = xePRIM(var.toUInt());
				else if (eType == QVariant::String){
					strTemp = var.toString();
					if (strTemp == "P")
						_xRefFRID.ePRIM = XSpace_Plugins::xePRIM_P;
					else if (strTemp == "L")
						_xRefFRID.ePRIM = XSpace_Plugins::xePRIM_L;
					else if (strTemp == "A")
						_xRefFRID.ePRIM = XSpace_Plugins::xePRIM_A;
					else if (strTemp == "N")
						_xRefFRID.ePRIM = XSpace_Plugins::xePRIM_N;
				}
			}
			else if (strKey == "GRUP")
				_xRefFRID.unGROUP = var.toUInt();
			else if (strKey == "OBJL")
				_xRefFRID.unOBJL = var.toUInt();
			else if (strKey == "RVER")
				_xRefFRID.unRVER = var.toUInt();
			else if (strKey == "RUIN"){
				if (eType == QVariant::Int)
					_xRefFRID.eRUIN = xeUPCODE(var.toUInt());
				else if (eType == QVariant::String){
					strTemp = var.toString();
					if (strTemp == "I")
						_xRefFRID.eRUIN = XSpace_Plugins::xeUPCODE_I;
					else if (strTemp == "D")
						_xRefFRID.eRUIN = XSpace_Plugins::xeUPCODE_D;
					else if (strTemp == "M")
						_xRefFRID.eRUIN = XSpace_Plugins::xeUPCODE_M;
				}
			}
			unIndex++;
		}

		// 解析子字段
		bool bIsSuccess = false;
		for each (XFieldRecord xRec in _xFRIDRec.xSubRecordList)
		{
			if (xRec.strStructKey == "FOID")
			{
				if (procFOID(xRec,_xRefFRID.xFOID) != xeError_S57Plugins_OK)
					return xeError_S57Plugins_FileParseFaild;
				_xRefFRID.strKey = _xRefFRID.xFOID.strKey;
			}
			else if (xRec.strStructKey == "ATTF")
			{
				if (procATTV(xRec, _xRefFRID.xATTFs) != xeError_S57Plugins_OK)
					return xeError_S57Plugins_FileParseFaild;
			}
			else if (xRec.strStructKey == "FSPT")
			{
				if (procFSPT(xRec, _xRefFRID.xFSPTs) != xeError_S57Plugins_OK)
					return xeError_S57Plugins_FileParseFaild;
			}
			else if (xRec.strStructKey == "FFPT")
			{
				if (procFFPT(xRec, _xRefFRID.xFFPTs) != xeError_S57Plugins_OK)
					return xeError_S57Plugins_FileParseFaild;
			}
			else
				int md = 0;
		}

		return xeError_S57Plugins_OK;
	}

	XSpace_Plugins::xeError_S57Plugins XS57Reader::procFOID(const XFieldRecord& _xFOIDRec, XS57_Struct_FOID& _xRefFOID)
	{
		if (_xFOIDRec.strStructKey != "FOID")
			return xeError_S57Plugins_FileParseFaild;

		quint32 unIndex = 0;
		QString strKey = QString();
		QString strTemp = QString();
		QVariant::Type eType = QVariant::Invalid;
		for each (QVariant var in _xFOIDRec.varFieldList)
		{
			strKey = _xFOIDRec.strFieldListKeys.at(unIndex);
			eType = var.type();
			if (strKey == "AGEN")
			{
				_xRefFOID.strAGEN = var.toString();
				if (eType == QVariant::Int)
				{
					XBitsConver xConv;
					xConv.unInt16 = var.toUInt();
					_xRefFOID.strKey += xConv.ucChar[0];
					_xRefFOID.strKey += xConv.ucChar[1];
				}
				else
					_xRefFOID.strKey += var.toString();
			}
				
			else if (strKey == "FIDN")
			{
				_xRefFOID.unFIDN = var.toUInt();
				if (eType == QVariant::Int)
				{
					XBitsConver xConv;
					xConv.unInt32 = var.toUInt();
					_xRefFOID.strKey += xConv.ucChar[0];
					_xRefFOID.strKey += xConv.ucChar[1];
					_xRefFOID.strKey += xConv.ucChar[2];
					_xRefFOID.strKey += xConv.ucChar[3];
				}
				else
				{
					strTemp = var.toString();
					while (strTemp.count() < 10)
						strTemp.push_back('0');
					_xRefFOID.strKey += strTemp;
				}
			}
			else if (strKey == "FIDS")
			{
				_xRefFOID.unFIDS = var.toUInt();
				if (eType == QVariant::Int)
				{
					XBitsConver xConv;
					xConv.unInt32 = var.toUInt();
					_xRefFOID.strKey += xConv.ucChar[0];
					_xRefFOID.strKey += xConv.ucChar[1];
				}
				else
				{
					strTemp = var.toString();
					while (strTemp.count() < 5)
						strTemp.push_back('0');
					_xRefFOID.strKey += strTemp;
				}
			}
			unIndex++;
		}
		
		return xeError_S57Plugins_OK;
	}

	XSpace_Plugins::xeError_S57Plugins XS57Reader::procFSPT(const XFieldRecord& _xFSPTRec, QList<XS57_Struct_FSPT>& _xRefFSPTs)
	{
		if (_xFSPTRec.strStructKey != "FSPT")
			return xeError_S57Plugins_FileParseFaild;

		XS57_Struct_FSPT xFSPT = XS57_Struct_FSPT();
		quint32 unIndex = 0;
		QString strKey = QString();
		QString strTemp = QString();
		double dTemp = 0;
		QVariant::Type varType = QVariant::Invalid;
		for each (QVariant var in _xFSPTRec.varFieldList)
		{
			strKey = _xFSPTRec.strFieldListKeys.at(unIndex);
			varType = var.type();
			if (strKey == "*NAME")
			{
				QByteArray strTemp = var.toByteArray();
				foreach(QChar ucCode, strTemp)
				{
					xFSPT.strNAME.append(ucCode.cell());
				}
			}
			else if (strKey == "ORNT"){
				if (varType == QVariant::Int)
					xFSPT.eORNT = xeORNT(var.toUInt());
				else if (varType == QVariant::String){
					strTemp = var.toString();
					if (strTemp == "F")
						xFSPT.eORNT = xeORNT_F;
					else if (strTemp == "R")
						xFSPT.eORNT = xeORNT_R;
					else if (strTemp == "N")
						xFSPT.eORNT = xeORNT_N;
				}
			}
			else if (strKey == "USAG"){
				if (varType == QVariant::Int)
					xFSPT.eUSAG = xeUSAG(var.toUInt());
				else if (varType == QVariant::String){
					strTemp = var.toString();
					if (strTemp == "E")
						xFSPT.eUSAG = xeUSAG_E;
					else if (strTemp == "T")
						xFSPT.eUSAG = xeUSAG_T;
					else if (strTemp == "C")
						xFSPT.eUSAG = xeUSAG_C;
					else if (strTemp == "N")
						xFSPT.eUSAG = xeUSAG_N;
				}
			}
			else if (strKey == "MASK"){
				if (varType == QVariant::Int)
					xFSPT.eMask = xeMASK(var.toUInt());
				else if (varType == QVariant::String){
					strTemp = var.toString();
					if (strTemp == "M")
						xFSPT.eMask = xeMASK_M;
					else if (strTemp == "S")
						xFSPT.eMask = xeMASK_S;
					else if (strTemp == "N")
						xFSPT.eMask = xeMASK_N;
				}
			}

			unIndex++;
		}
		_xRefFSPTs.push_back(xFSPT);

		if (_xFSPTRec.bIsManyDim)
		{
			for each (XFieldRecord xManyFSPT in _xFSPTRec.xManyDimRecordList)
			{
				unIndex = 0;
				xFSPT = XS57_Struct_FSPT();

				for each (QVariant var in xManyFSPT.varFieldList)
				{
					strKey = _xFSPTRec.strFieldListKeys.at(unIndex);
					varType = var.type();
					if (strKey == "*NAME")
					{
						QByteArray strTemp = var.toByteArray();
						foreach(QChar ucCode, strTemp)
						{
							xFSPT.strNAME.append(ucCode.cell());
						}
					}
					else if (strKey == "ORNT"){
						if (varType == QVariant::Int)
							xFSPT.eORNT = xeORNT(var.toUInt());
						else if (varType == QVariant::String){
							strTemp = var.toString();
							if (strTemp == "F")
								xFSPT.eORNT = xeORNT_F;
							else if (strTemp == "R")
								xFSPT.eORNT = xeORNT_R;
							else if (strTemp == "N")
								xFSPT.eORNT = xeORNT_N;
						}
					}
					else if (strKey == "USAG"){
						if (varType == QVariant::Int)
							xFSPT.eUSAG = xeUSAG(var.toUInt());
						else if (varType == QVariant::String){
							strTemp = var.toString();
							if (strTemp == "E")
								xFSPT.eUSAG = xeUSAG_E;
							else if (strTemp == "T")
								xFSPT.eUSAG = xeUSAG_T;
							else if (strTemp == "C")
								xFSPT.eUSAG = xeUSAG_C;
							else if (strTemp == "N")
								xFSPT.eUSAG = xeUSAG_N;
						}
					}
					else if (strKey == "MASK"){
						if (varType == QVariant::Int)
							xFSPT.eMask = xeMASK(var.toUInt());
						else if (varType == QVariant::String){
							strTemp = var.toString();
							if (strTemp == "M")
								xFSPT.eMask = xeMASK_M;
							else if (strTemp == "S")
								xFSPT.eMask = xeMASK_S;
							else if (strTemp == "N")
								xFSPT.eMask = xeMASK_N;
						}
					}

					unIndex++;
				}

				_xRefFSPTs.push_back(xFSPT);
			}
		}

		return xeError_S57Plugins_OK;
	}

	XSpace_Plugins::xeError_S57Plugins XS57Reader::procFFPT(const XFieldRecord& _xFFPTRec, QList<XS57_Struct_FFPT>& _xRefFFPTs)
	{
		if (_xFFPTRec.strStructKey != "FFPT")
			return xeError_S57Plugins_FileParseFaild;

		XS57_Struct_FFPT xFFPT = XS57_Struct_FFPT();
		quint32 unIndex = 0;
		QString strKey = QString();
		QString strTemp = QString();
		double dTemp = 0;
		QVariant::Type varType = QVariant::Invalid;
		for each (QVariant var in _xFFPTRec.varFieldList)
		{
			strKey = _xFFPTRec.strFieldListKeys.at(unIndex);
			varType = var.type();
			if (strKey == "LNAM")
				xFFPT.strLNAM = var.toString();
			else if (strKey == "RIND"){
				if (varType == QVariant::Int)
					xFFPT.eRIND = xeRIND(var.toUInt());
				else if (varType == QVariant::String){
					strTemp = var.toString();
					if (strTemp == "M")
						xFFPT.eRIND = XSpace_Plugins::xeRIND_M;
					else if (strTemp == "S")
						xFFPT.eRIND = XSpace_Plugins::xeRIND_S;
					else if (strTemp == "P")
						xFFPT.eRIND = XSpace_Plugins::xeRIND_P;
				}
			}
			else if (strKey == "COMT")
				xFFPT.strCOMT = var.toString();

			unIndex++;
		}
		_xRefFFPTs.push_back(xFFPT);

		if (_xFFPTRec.bIsManyDim)
		{
			for each (XFieldRecord xManyFSPT in _xFFPTRec.xManyDimRecordList)
			{
				unIndex = 0;
				xFFPT = XS57_Struct_FFPT();

				for each (QVariant var in xManyFSPT.varFieldList)
				{
					strKey = _xFFPTRec.strFieldListKeys.at(unIndex);
					varType = var.type();
					if (strKey == "LNAM")
						xFFPT.strLNAM = var.toString();
					else if (strKey == "RIND"){
						if (varType == QVariant::Int)
							xFFPT.eRIND = xeRIND(var.toUInt());
						else if (varType == QVariant::String){
							strTemp = var.toString();
							if (strTemp == "M")
								xFFPT.eRIND = XSpace_Plugins::xeRIND_M;
							else if (strTemp == "S")
								xFFPT.eRIND = XSpace_Plugins::xeRIND_S;
							else if (strTemp == "P")
								xFFPT.eRIND = XSpace_Plugins::xeRIND_P;
						}
					}
					else if (strKey == "COMT")
						xFFPT.strCOMT = var.toString();

					unIndex++;
				}

				_xRefFFPTs.push_back(xFFPT);
			}
		}

		return xeError_S57Plugins_OK;
	}

}