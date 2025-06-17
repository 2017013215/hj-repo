#ifndef BYYATTRIBUTEEDITMNG_H
#define BYYATTRIBUTEEDITMNG_H

#include "snighdose_global.h"

#include <QMap>
#include <String>
#include <QVariant>
#include <QStringList>

typedef struct s_AttrEditType {
	QString description;	//编辑格式的一般描述
	QStringList sourceInfo;		//数据源说明
	QMap<QString, QVariant> values;	//支持此格式的值
} AttrEditType;
bool SNIGHDOSE_EXPORT operator==(const AttrEditType &p1,const AttrEditType &p2);

class SNIGHDOSE_EXPORT ByyAttributeEditMng 
{
public:
	static ByyAttributeEditMng* instance();
	void load(const QString& filepath);
	bool getRsmRefAttricbeEditType(const QString& attrName, AttrEditType& eType);
	bool getTaskRefAttricbeEditType(const QString& attrName, AttrEditType& eType);

private:
	ByyAttributeEditMng();
	void clear();
	//source format( st::xx+xx+xx.. ): sourceType dataType1+...
	QStringList parseSourceInfo(const QString& sourceInfo);

	QMap<QString,AttrEditType> m_rsmEnumTypes;
	QMap<QString,AttrEditType> m_taskEnumTypes;
};

#endif