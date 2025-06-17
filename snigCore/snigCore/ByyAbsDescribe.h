#ifndef BYYABSDESCRIBE_H
#define BYYABSDESCRIBE_H

#include "snigcore_global.h"
#include <QString>

class SNIGCORE_EXPORT ByyAbsDescribe
{
public:
	ByyAbsDescribe();
	virtual ~ByyAbsDescribe();

	QString getItemClass()const;
	QString getType()const;
	QString getTag()const;

	void setItemClass(const QString& val);
	void setType(const QString& val);
	void setTag(const QString& val);

protected:
	QString myItemClass;
	QString myType;
	QString myTag;
};

#endif // BYYABSDESCRIBE_H
