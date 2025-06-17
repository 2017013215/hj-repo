#ifndef OPD_H
#define OPD_H

#include "snighdose_global.h"

#include <QMap>
#include <QStringList>
#include "opdEntry.h"
#include "assemble.h"

class QDomElement;

class SNIGHDOSE_EXPORT ByyOpd
{
public:
	ByyOpd();
	virtual ~ByyOpd();

	static ByyOpd*	instance();

	typedef QMap<QString,ByyAssemble> Assembles;
	typedef QMap<QString,ByyOpdEntry> OPDEntrys;

	const OPDEntrys& opdEntrys();
	const Assembles& assembles();

	ByyAssemble findAssemble(const QString& itemClass);
	ByyOpdEntry findOpdEntry( const QString& name );

	ByyAssemble findAssemble(const ByyEntityType& entType);
	ByyOpdEntry* findOpdEntry( const ByyEntityType& entType );

protected:
	void load();

protected:
	OPDEntrys myOpdEntrys;
	Assembles myAssembles;
};

#endif // OPD_H
