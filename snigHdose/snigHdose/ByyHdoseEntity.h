#ifndef BYYHDOSEENTITY_H
#define BYYHDOSEENTITY_H

#include "snighdose_global.h"
#include "remoteEntity.h"

#include <QStringList>

class CObj;

class SNIGHDOSE_EXPORT ByyHdoseEntity : public ByyRemoteEntity
{
	Q_OBJECT

public:
	ByyHdoseEntity(CObj* obj);
	~ByyHdoseEntity();

	struct AttributeInfo_st
	{
		QString name;

		enum DataType{t_other,t_bool,t_int,t_long,t_double,t_float,t_char,t_BOOL,t_WORD,t_DWORD,t_GeoPt,t_Vec3};

		DataType myType;

		char* dataPt;
	};

	typedef QMap<QString,AttributeInfo_st> AttributeInfoList;

	QStringList attributeNameList(AttributeInfo_st::DataType type);

	QStringList attributeNameList();

	AttributeInfoList& attributeInfoList();

	double getAttributeValue(const QString& name);
	
	double getGeoPtValue(const QString& name,unsigned index);
	double getVec3Value(const QString& name,unsigned index);
	
	void generateAttributeInfo();

protected:

protected:
	CObj* myObj;

	AttributeInfoList myAttributeInfoList;
};

#endif // BYYHDOSEENTITY_H
