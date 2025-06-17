#ifndef COMAPP_H
#define COMAPP_H

#include <QObject>

class ByyApp;
class comApp : public QObject
{
	Q_OBJECT

public:
	~comApp();

	static comApp* instance();
	
	void setApp(ByyApp* app);

	ByyApp* getApp();

private:
	comApp(QObject *parent = 0);

private:
	
	ByyApp* myApp;
	
};

#endif // COMAPP_H
