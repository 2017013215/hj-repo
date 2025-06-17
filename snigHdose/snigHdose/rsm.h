#ifndef RSM_H
#define RSM_H

#include <QMap>
#include "scnEntry.h"
#include "simEsr.h"

class ByyRsm
{
public:
	ByyRsm();
	~ByyRsm();

	typedef QMap<QString,ByyScnEntry> ScnEntrys;
	typedef QMap<QString,ByySimEsr> SimESRs;

	ParameterList getParametersFromEsr(const QString& esrItemClass);
	ParameterList getParametersFromScnEntry(const QString& scnEntry);

protected:
	void load();

private:
	ScnEntrys myScnEntrys;
	SimESRs mySimEsrs;
};

#endif // RSM_H
