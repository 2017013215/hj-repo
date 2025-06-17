#ifndef SCNENTRY_H
#define SCNENTRY_H

#include <QString>
#include <hash_map>

#include "parameter.h"
#include "snighdose_global.h"

class QDomElement;

class SNIGHDOSE_EXPORT ByyScnEntry
{
public:
	ByyScnEntry();
	~ByyScnEntry();

	void fromConfig(QDomElement& element);

public:
	QString myName;
	QString myParent;

	ParameterList myParams;

};

#endif // SCNENTRY_H
