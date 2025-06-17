#ifndef SCENARIO_H
#define SCENARIO_H

#include <QString>
#include <QDateTime>
#include <QVector>
#include "snighdose_global.h"
#include "ByyRectangle.h"

class ByyCore;
class ByyHdose;
class QDomDocument;
class QDomElement;

class SNIGHDOSE_EXPORT ByyScenarioV2
{
public:
	ByyScenarioV2(ByyHdose& hdose);
	virtual ~ByyScenarioV2();

	virtual bool load (const QString &filename); 
	virtual bool save();

	

	//void setStartDateTime(const QDateTime& dt);
	//const QDateTime& startDateTime()const;

	//void setEndDateTime(const QDateTime& dt);
	//const QDateTime& endDateTime()const;

	void setFilename(const QString& filename);
	QString filename()const;

	static bool valid(const QString& filename);

	bool modified();

	void setModified(bool modified=true);

	//void setSimulationRect(const ByyRectangle& rect);
	//const ByyRectangle& simulationRect();


	struct ScenarioInfo 
	{
		QString myName;
		QString myOutline;

		QDateTime myStartDateTime;
		QDateTime myEndDateTime;

		ByyRectangle mySimulationRect;

		long	m_ScnRunN;
		long	m_UseRandomSeed;
		long	m_RandomSeed;
		double	m_T;
		long	m_SimNetworkComm;

		bool operator !=(const ScenarioInfo& e)
		{
			return myName!=e.myName||myOutline!=e.myOutline
				||myStartDateTime!=e.myStartDateTime||myEndDateTime!=e.myEndDateTime
				||mySimulationRect!=e.mySimulationRect||m_ScnRunN!=e.m_ScnRunN
				||m_UseRandomSeed!=e.m_UseRandomSeed||m_RandomSeed!=e.m_RandomSeed
				||m_T!=e.m_T||m_SimNetworkComm!=e.m_SimNetworkComm;
		}

	};

	void setScnInfo(const ScenarioInfo& scnInfo);
	const ScenarioInfo& getScnInfo()const;

	struct  Environment
	{
		ByyRectangle myRectangle;
		int mySeaStateLevel;
		int myWindCou;
		int myWindVel;
		QString myName;

		Environment()
		{
			mySeaStateLevel=0;
			myWindCou=0;
			myWindVel=0;
		}

		Environment(const Environment& e)
		{
			myRectangle.setPoints(e.myRectangle.points());
			mySeaStateLevel=e.mySeaStateLevel;
			myWindCou=e.myWindCou;
			myWindVel=e.myWindVel;
			myName=e.myName;
		}

		bool operator !=(const Environment& e)
		{
			return myRectangle.points()!=e.myRectangle.points()||mySeaStateLevel!=e.mySeaStateLevel
				||myWindCou!=e.myWindCou||myWindVel!=e.myWindVel||myName!=e.myName;
		}

		bool operator ==(const Environment& e)
		{
			return myRectangle.points()==e.myRectangle.points()&&mySeaStateLevel==e.mySeaStateLevel
				&&myWindCou==e.myWindCou&&myWindVel==e.myWindVel&&myName==e.myName;
		}

		Environment& operator=(const Environment& e)
		{
			myRectangle.setPoints(e.myRectangle.points());
			mySeaStateLevel=e.mySeaStateLevel;
			myWindCou=e.myWindCou;
			myWindVel=e.myWindVel;
			myName=e.myName;
			return *this;
		}

	};

	typedef QList<Environment> EnvironmentList;

	void setEnvironmentList(const EnvironmentList& e);
	const EnvironmentList& environmentList()const;

	//编队设置
	struct FormationPara
	{
		QString m_MemberName;//成员名称
		int m_index;//成员所在的位序号（从0开始）
		double m_x;//前（单位：m）
		double m_y;//上（单位：m）
		double m_z;//右（单位：m）
		FormationPara()
		{
			m_index = 0;
			m_x = 0.0;
			m_y = 0.0;
			m_z = 0.0;
		}
		FormationPara(const FormationPara& p)
		{
			m_MemberName=p.m_MemberName;
			m_index=p.m_index;
			m_x=p.m_x;
			m_y=p.m_y;
			m_z=p.m_z;
		}
		bool operator !=(const FormationPara& p)
		{
			return m_MemberName!=p.m_MemberName
				||m_index!=p.m_index||m_x!=p.m_x||m_y!=p.m_y||m_z!=p.m_z;
		}

		bool operator ==(const FormationPara& p)
		{
			return m_MemberName==p.m_MemberName
				&&m_index==p.m_index&&m_x==p.m_x&&m_y==p.m_y&&m_z==p.m_z;
		}

		FormationPara& operator=(const FormationPara& p)
		{
			m_MemberName=p.m_MemberName;
			m_index=p.m_index;
			m_x=p.m_x;
			m_y=p.m_y;
			m_z=p.m_z;
			return *this;
		}
	};
	struct Formation
	{
		QString m_FormationName;//编队名称
		QString m_LeaderName;//领队名称
		int m_MemberCount;//编队成员数量
		QList<FormationPara> m_FormationPara;//编队成员位置信息
		Formation()
		{
			m_MemberCount = 0;
		}
		Formation(const Formation& f)
		{
			m_FormationName=f.m_FormationName;
			m_LeaderName=f.m_LeaderName;
			m_MemberCount=f.m_MemberCount;
			m_FormationPara.clear();
			for (unsigned int i=0;i<f.m_FormationPara.size();i++)
			{
				m_FormationPara.append(f.m_FormationPara.at(i));
			}
		}

		bool operator !=(const Formation& f)
		{
			return m_FormationName!=f.m_FormationName
				||m_LeaderName!=f.m_LeaderName||m_MemberCount!=f.m_MemberCount||m_FormationPara!=f.m_FormationPara;
		}

		bool operator ==(const Formation& f)
		{
			return m_FormationName==f.m_FormationName
				&&m_LeaderName==f.m_LeaderName&&m_MemberCount==f.m_MemberCount&&m_FormationPara==f.m_FormationPara;
		}

		Formation& operator=(const Formation& f)
		{
			m_FormationName=f.m_FormationName;
			m_LeaderName=f.m_LeaderName;
			m_MemberCount=f.m_MemberCount;
			m_FormationPara.clear();
			for (unsigned int i=0;i<f.m_FormationPara.size();i++)
			{
				m_FormationPara.append(f.m_FormationPara.at(i));
			}
			return *this;
		}
	};
	typedef QList<Formation> FormationList;
	void setFormationList(const FormationList& e);
	const FormationList& formationList()const;

protected:
	QDomElement serializeProfile(QDomDocument& doc);
	QDomElement serializeEnvironment(Environment& environment,QDomDocument& doc);

	QVector<QDomElement> serializeEntityObjects(QDomDocument& doc,ByyForceType forceType=ByyForceAll);
	QVector<QDomElement> serializeControlObjects(QDomDocument& doc,ByyForceType forceType=ByyForceAll);

	QVector<QDomElement> serializeEntityTask(QDomDocument& doc);

	QVector<QDomElement> serializeFormation(QDomDocument& doc);

	void readProfile(QDomElement& item);
	Environment readEnvironment(QDomElement& item);
	Formation readFormation(QDomElement& item);

protected:
	ByyHdose& myHdose;
	ByyCore& myCore;


	ScenarioInfo myScnInfo;
	
	EnvironmentList myEnvironmentList;

	QString myFilename;

	bool myModified;

	FormationList myFormationList;
	
};

#endif // SCENARIO_H
