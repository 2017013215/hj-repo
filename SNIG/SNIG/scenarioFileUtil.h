#ifndef FILEUTIL_H
#define FILEUTIL_H

class ByyApp;

class ByyScenarioFileUtil
{
public:
	ByyScenarioFileUtil();
	~ByyScenarioFileUtil();

	static bool doSave(ByyApp& myApp);
	static bool saveAs(ByyApp& myApp);
	static bool save(ByyApp& myApp);
	static bool okToContinue(ByyApp& myApp);

private:
	
};

#endif // FILEUTIL_H
