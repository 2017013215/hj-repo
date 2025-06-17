#include "XDocument.h"
#include <QtCore/QFile>
#include "XArchive.h"

namespace XSpace_Common
{
	xIMPLEMENT_SERIAL_VIRTURL(XDocument,XObject,0xFFFF)
	XDocument::XDocument(void)
	{
	}

	XDocument::~XDocument(void)
	{
	}

	int XDocument::OpenDocument(const QString& _strFilePath)
	{
		if (!QFile::exists(_strFilePath))
			return 1;

		QFile loadFile(_strFilePath);
		if (!loadFile.open(QIODevice::ReadOnly))
			return 2;

		// 序列化对象
		XArchive loadArchive(&loadFile,XArchive::EMode_Load);
		int nClassTag = 0;
		int nClassVersion = 0xFFFF;
		QString strClassName = QString();
		loadArchive>>nClassTag;
		loadArchive>>nClassVersion;
		loadArchive>>strClassName;
		loadArchive.SetObjectVersion(nClassVersion);
		Serialize(loadArchive);

		loadFile.close();
		return 0;
	}

	int XDocument::SaveDocument(const QString& _strFilePath, const bool& _bRefCover)
	{
		if (QFile::exists(_strFilePath) && _bRefCover == false)
			return 1;

		else if (QFile::exists(_strFilePath) && _bRefCover)
		{
			bool bRes = QFile::remove(_strFilePath);
			if (bRes == false)
				return 2;
		}

		QFile saveFile(_strFilePath);
		if (!saveFile.open(QIODevice::WriteOnly))
			return 3;

		// 序列化对象
		XArchive saveArchive(&saveFile,XArchive::EMode_Store);
		saveArchive<<(int)(0xFFFF);
		saveArchive<<GetRuntimeClass()->GetClassVersion();
		saveArchive<<GetRuntimeClass()->GetClassName();
		Serialize(saveArchive);

		saveFile.close();
		return 0;
	}

}
