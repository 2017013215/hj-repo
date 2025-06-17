#ifndef HDOSESETTINGS_H
#define HDOSESETTINGS_H

#include <QObject>
#include "snighdose_global.h"

class SNIGHDOSE_EXPORT ByyHdoseSettings : public QObject
{
	Q_OBJECT

public:
	~ByyHdoseSettings();

	static ByyHdoseSettings& instance();

	QString getCreateWorkDir() const { return myCreateWorkDir; }
	void setCreateWorkDir(const QString& val) { myCreateWorkDir = val; }

	QString getCreateSimFile() const { return myCreateSimFile; }
	void setCreateSimFile(const QString& val) { myCreateSimFile = val; }

	QString getJoinWorkDir() const { return myJoinWorkDir; }
	void setJoinWorkDir(const QString& val) { myJoinWorkDir = val; }

	QString getJoinSimFile() const { return myJoinSimFile; }
	void setJoinSimFile(const QString& val) { myJoinSimFile = val; }

	QString getSingleWorkDir() const { return mySingleWorkDir; }
	void setSingleWorkDir(const QString& val) { mySingleWorkDir = val; }

	QString getSingleSimFile() const { return mySingleSimFile; }
	void setSingleSimFile(const QString& val) { mySingleSimFile = val; }

	QString getOpdFile() const { return myOpdFile; }
	void setOpdFile(const QString& val) { myOpdFile = val; }

	QString getRsmFile() const { return myRsmFile; }
	void setRsmFile(const QString& val) { myRsmFile = val; }

	QString getEditTypeFile() const { return myEditTypeFile; }
	void setEditTypeFile(const QString& val) { myEditTypeFile = val; }

	QString getLocalSvr() const { return myLocalNetSvr; }

	QString getRemoteSvrIP() const { return myRemoteSvrIP; }
	void setRemoteSvrIP(const QString& val) { myRemoteSvrIP = val; }

protected:
	void readSettings();
	void writeSettings();

private:
	ByyHdoseSettings();

private:
	QString	myCreateWorkDir;
	QString	myCreateSimFile;

	QString myJoinWorkDir;
	QString myJoinSimFile;
	QString mySingleWorkDir;
	QString mySingleSimFile;
	
	QString myOpdFile;
	QString myRsmFile;
	QString myEditTypeFile;

	QString	myLocalNetSvr;
	QString	myRemoteSvrIP;
	
};

#endif // HDOSESETTINGS_H
