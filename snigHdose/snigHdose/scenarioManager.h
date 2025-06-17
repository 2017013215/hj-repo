#ifndef SCENARIOMANAGER_H
#define SCENARIOMANAGER_H

#include <QObject>
#include "snighdose_global.h"

class ByyScenarioV2;
class ByyCore;
class ByyHdose;

class SNIGHDOSE_EXPORT ByyScenarioManager : public QObject
{
	Q_OBJECT

public:
	ByyScenarioManager(ByyHdose& hdose,QObject *parent=0);
	~ByyScenarioManager();

	bool newScenario();
	bool loadScenario(const QString& filename);
	bool saveScenario();
	bool saveAsScenario(const QString& filename);
	bool closeScenario();
	bool reload();

	virtual ByyScenarioV2* scenario();

	bool scenarioModified();

public slots:
	void setScenarioModified(bool modified=true);

signals:
	void signal_scenarioCreated(ByyScenarioV2*);
	void signal_scenarioLoaded(ByyScenarioV2*);
	void signal_scenarioClosed();
	void signal_scenarioSaved();
	void signal_scenarioChanged();
	void signal_modifiedChanged(bool);

protected:
	ByyHdose& myHdose;
	ByyCore& myCore;
	ByyScenarioV2* myScenario;	
	
};

#endif // SCENARIOMANAGER_H
