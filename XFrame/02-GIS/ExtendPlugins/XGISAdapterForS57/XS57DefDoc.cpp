#include "XS57DefDoc.h"
#include <QtCore/QTextStream>

namespace XSpace_Plugins
{
	xIMPLEMENT_SERIAL(XS57ClassDef, XDocument, xeVersion_V1)
	XS57ClassDef::XS57ClassDef()
	{
		m_xS57ClassVec.clear();
		m_xS57CodeToStrs.clear();
		m_xS57StrToCodes.clear();
	}

	XS57ClassDef::~XS57ClassDef()
	{
		m_xS57ClassVec.clear();
		m_xS57CodeToStrs.clear();
		m_xS57StrToCodes.clear();
	}

	void XS57ClassDef::Serialize(XArchive& _xArchive)
	{
		if (!IsSerialize())
			return;

		int nClassVersion = 0xFFFF;
		if (_xArchive.IsStoring())
		{
			nClassVersion = GetRuntimeClass()->GetClassVersion();
			if (nClassVersion == xBASE_VERSION_1_0)
			{
				XObject::Serialize(_xArchive);
			}
		}
		else if (_xArchive.IsLoading())
		{
			nClassVersion = _xArchive.GetObjectVersion();
			if (nClassVersion == xBASE_VERSION_1_0)
			{
				XObject::Serialize(_xArchive);
			}
		}
	}

	bool XS57ClassDef::InitFromPrim(const QString& _strFilePath)
	{
		QFile xFile(_strFilePath);
		if (!xFile.open(QIODevice::ReadOnly))
			return false;

		QString strLine = QString();
		QTextStream stream(&xFile);
		strLine = stream.readLine();
		if (strLine != "Identify 20160120"){
			// 识别码错误 用以简单校验文件内容
			xFile.close();
			return false;
		}
		strLine = stream.readLine();

		QString strTemp = QString();
		while (!stream.atEnd())
		{
			QStringList strOneRec = stream.readLine().split("	");
			if (strOneRec.count() >= 8){
				XS57Class xNewClass;
				xNewClass.unCode = strOneRec.at(0).toUInt();
				xNewClass.strName = strOneRec.at(1);
				xNewClass.strAcronym = strOneRec.at(2);
				xNewClass.strAttsA = strOneRec.at(3).split(";");
				xNewClass.strAttsB = strOneRec.at(4).split(";");
				xNewClass.strAttsC = strOneRec.at(5).split(";");
				xNewClass.strPRIMTypes = strOneRec.at(7).split(";");
				if (strOneRec.at(6).count()==1)
					xNewClass.ucClassTypes = strOneRec.at(6).at(0).cell();
				if (strOneRec.size() >= 10)
				{
					xNewClass.strLayers = strOneRec.at(8).split(";");
					xNewClass.strRendererLevs = strOneRec.at(9).split(";");
				}

				m_xS57ClassVec.insert(xNewClass.unCode,xNewClass);
				m_xS57CodeToStrs.insert(xNewClass.unCode, xNewClass.strAcronym);
				m_xS57StrToCodes.insert(xNewClass.strAcronym, xNewClass.unCode);
			}
		}
		xFile.close();

		return true;
	}

}