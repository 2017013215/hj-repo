#ifndef BYYAPP_H
#define BYYAPP_H

class ByyMainWindow;
class ByyPluginSystem;
class ByyApp
{
public:
	ByyApp();
	~ByyApp();

	void initialize();

	ByyCore& core();
	ByyHdose& hdose();
	ByyIG& ig();
	ByyMainWindow& mainWindow();

	bool hdoseValid();

	inline int aaa();

protected:
	ByyCore* myCore;
	ByyHdose* myHdose;
	ByyIG* myIG;
	ByyMainWindow* myMainWindow;
	ByyPluginSystem*	_pluginSystem;
};

#endif // BYYAPP_H
