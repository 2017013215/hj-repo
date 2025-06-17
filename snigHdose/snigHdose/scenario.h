#ifndef SCENARIO_H
#define SCENARIO_H

#include <QString>
#include <QDateTime>
#include "snighdose_global.h"
#include "ByyRectangle.h"

class ByyCore;
class ByyHdose;
class QDomDocument;
class QDomElement;

class SNIGHDOSE_EXPORT ByyScenario
{
public:
	ByyScenario(ByyHdose& hdose);
	virtual ~ByyScenario();

	virtual bool load (const QString &filename); 
	virtual bool save();

	void setName(const QString& n);
	const QString& name()const;

	void setStartDateTime(const QDateTime& dt);
	const QDateTime& startDateTime()const;

	void setEndDateTime(const QDateTime& dt);
	const QDateTime& endDateTime()const;

	void setFilename(const QString& filename);
	QString filename()const;

	static bool valid(const QString& filename);

	bool modified();

	void setModified(bool modified=true);

	void setSimulationRect(const ByyRectangle& rect);
	const ByyRectangle& simulationRect();

	struct  Environment
	{
		ByyRectangle myRectangle;
		int mySeaStateLevel;
		int myWindCou;
		int myWindVel;

		bool operator !=(const Environment& e)
		{
			return myRectangle.points()!=e.myRectangle.points()||mySeaStateLevel!=e.mySeaStateLevel
				||myWindCou!=e.myWindCou||myWindVel!=e.myWindVel;
		}

		Environment& operator=(const Environment& e)
		{
			myRectangle.setPoints(e.myRectangle.points());
			mySeaStateLevel=e.mySeaStateLevel;
			myWindCou=e.myWindCou;
			myWindVel=e.myWindVel;
			return *this;
		}

	};


	void setEnvironment(const Environment& e);
	const Environment& environment()const;

protected:
	QDomElement serializeCscheduler(QDomDocument& doc);
	QDomElement serializeProfile(QDomDocument& doc);
	QDomElement serializeEnvironment(QDomDocument& doc);

	QVector<QDomElement> serializeEntityObjects(QDomDocument& doc,ByyForceType forceType=ByyForceAll);
	QVector<QDomElement> serializeControlObjects(QDomDocument& doc,ByyForceType forceType=ByyForceAll);

	QVector<QDomElement> serializeEntityTask(QDomDocument& doc);

	void readProfile(QDomElement& item);
	void readEnvironment(QDomElement& item);

protected:
	ByyHdose& myHdose;
	ByyCore& myCore;

	QString myName;

	QDateTime myStartDateTime;
	QDateTime myEndDateTime;

	ByyRectangle mySimulationRect;
	
	Environment myEnvironment;

	QString myFilename;

	bool myModified;

	
};

#endif // SCENARIO_H
