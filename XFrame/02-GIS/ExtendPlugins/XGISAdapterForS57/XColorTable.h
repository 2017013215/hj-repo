#ifndef XCOLORTABLE_HEADER
#define XCOLORTABLE_HEADER

#include <QtCore/QStringList>
#include <QtGui/QVector3D>
#include <QtCore/qmath.h>
#include <QtCore/QMap>
#include <QtGui/QColor>

namespace XSpace_Plugins
{
	struct XColor
	{
		XColor(){
			strKey = QString();
			strName = QString();
			sRGB = QVector3D();
			xCIEYxy = QVector3D();
		}
		XColor(const QString& _strKey, const QString& _strName, const QColor& _xSRGB){
			strKey = _strKey;
			strName = _strName;
			sRGB.setX(_xSRGB.red());
			sRGB.setY(_xSRGB.green());
			sRGB.setZ(_xSRGB.blue());

			xCIEYxy = QVector3D();
		}
		QString		strKey;
		QString		strName;
		QVector3D	sRGB;
		QVector3D	xCIEYxy;
	};
	class XColorTable
	{
	private:
		XColorTable();
	public:
		~XColorTable();

	public:
		static XColorTable* fromString(const QStringList& _strColorStr);

		const bool GetColor(const QString& _strKey, QColor& _xRefCol);

		quint32 m_unID;
		QString m_strObjType;
		QString	m_strName;
		QStringList m_strRecord;

		QMap<QString, XColor> m_xColorVec;	// ÑÕÉ«¼¯ºÏ


	private:
		static QVector3D Yxy2XYZ(const QVector3D& _xCIEColorYxy);
		static QVector3D XYZ2sRGB(const QVector3D& _xColorXYZ);
		static QVector3D Yxy2sRGB(const QVector3D& _xCIEColorYxy);
	};
}
#endif