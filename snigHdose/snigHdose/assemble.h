#ifndef ASSEMBLE_H
#define ASSEMBLE_H

#include <QMultiMap>
#include "ByyDescribeEntry.h"
#include "opdEntry.h"
#include "snighdose_global.h"

class SNIGHDOSE_EXPORT ByyAssemble
{
public:
	ByyAssemble();
	~ByyAssemble();

	ByyDescribeEntry toDescribeEntry();

	bool valid();

	void fromConfig(QDomElement& element,QMap<QString,ByyOpdEntry>& myOpdEntrys);

	ByyOpdEntry* getEsrOpd();
	QString getEsrItemClass();

	ByyEntityType getEntityType();

	QString myItemClass;
	QString myType;
	QString myTag;
	QString myOpd;

	ByyOpdEntry* myOpdEntry;

	QMultiMap<QString,ByyAssemble> myChildren;

	int index;
};

#endif // ASSEMBLE_H
