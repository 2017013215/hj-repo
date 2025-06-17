#ifndef BYYDESCPARAMLIST_H
#define BYYDESCPARAMLIST_H

#include "snigcore_global.h"

#include <QString>
#include <QMap>
#include <QList>


class SNIGCORE_EXPORT ByyDescParamList
{
public:
	ByyDescParamList(const QString& name=QString());
	virtual ~ByyDescParamList();

	void setName(const QString& name);

	QString getName()const;

	void addParameter(const QString& name,const QString& value); 

	int count()const;

	QString value(const QString& name)const;
	QString value(int index)const;
	QString name(int index)const;

	bool hasParameter(const QString& name);

	typedef QMap<QString,QString> Key_Value_List;

	typedef QList<QString> Order_List;

private:
	QString				myName;

	Key_Value_List		myParameters;

	Order_List			myOrder;

};

#endif // BYYDESCPARAMLIST_H
