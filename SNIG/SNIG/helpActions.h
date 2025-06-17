#ifndef ABOUTMENUITEM_H
#define ABOUTMENUITEM_H

#include "ByyAction.h"

class ByyAboutAction : public ByyAction
{
public:
	ByyAboutAction(ByyApp& app,QWidget* parent=0);
	~ByyAboutAction();

protected slots:
	virtual void on_triggered();
	
};

class ByyShowDocAction : public ByyAction
{
public:
	ByyShowDocAction(ByyApp& app,const QString& objectName,const QString& text,const QString& file,QWidget* parent=0);
	~ByyShowDocAction();

protected slots:
	virtual void on_triggered();

protected:
	QString myDocFile;

};

#endif // ABOUTMENUITEM_H
