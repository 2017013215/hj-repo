#include "hdoseSettings.h"
#include "ByyRelativePath.h"
#include <QSettings>

ByyHdoseSettings::ByyHdoseSettings()
	: QObject()
{
	readSettings();
}

ByyHdoseSettings::~ByyHdoseSettings()
{
	writeSettings();
}

ByyHdoseSettings& ByyHdoseSettings::instance()
{
	static ByyHdoseSettings _instance;
	return _instance;
}

void ByyHdoseSettings::readSettings()
{
	QSettings s(myRelativePath.resolvePath("../data/config/HDose.ini"),QSettings::IniFormat);
	s.beginGroup("Hdose");
	myCreateWorkDir=s.value("CreateWorkDir").toString();
	myCreateSimFile=s.value("CreateSimFile").toString();
	myJoinWorkDir=s.value("JoinWorkDir").toString();
	myJoinSimFile=s.value("JoinSimFile").toString();
	mySingleWorkDir=s.value("SingleWorkDir").toString();
	mySingleSimFile=s.value("SingleSimFile").toString();
	myOpdFile=s.value("OpdFile").toString();
	myRsmFile=s.value("RsmFile").toString();
	myEditTypeFile=s.value("EditType").toString();
	myLocalNetSvr=s.value("LocalNetSvr").toString();
	myRemoteSvrIP=s.value("RemoteSvrIP").toString();
}

void ByyHdoseSettings::writeSettings()
{
	QSettings s(myRelativePath.resolvePath("../data/config/HDose.ini"),QSettings::IniFormat);
	s.beginGroup("Hdose");

	s.setValue("CreateWorkDir",myCreateWorkDir);
	s.setValue("CreateSimFile",myCreateSimFile);
	s.setValue("JoinWorkDir",myJoinWorkDir);
	s.setValue("JoinSimFile",myJoinSimFile);
	s.setValue("SingleWorkDir",mySingleWorkDir);
	s.setValue("SingleSimFile",mySingleSimFile);
	s.setValue("OpdFile",myOpdFile);
	s.setValue("RsmFile",myRsmFile);
	s.setValue("LocalNetSvr",myLocalNetSvr);
	s.setValue("RemoteSvrIP",myRemoteSvrIP);
}
