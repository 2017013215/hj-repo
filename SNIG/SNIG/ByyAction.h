#ifndef BYYMENUITEM_H
#define BYYMENUITEM_H

#include <QAction>

class ByyApp;

class ByyAction : public QAction
{
	Q_OBJECT

public:
	ByyAction(ByyApp& app,const QString& objectName,QWidget* parent=0);
	virtual ~ByyAction();

public slots:
	virtual void checkEnabled();

protected slots:
	virtual void on_triggered();

protected:
	ByyApp& myApp;
	
};

class ByySeparatorAction : public ByyAction
{
public:
	ByySeparatorAction(ByyApp& app,QWidget* parent);
	~ByySeparatorAction();
};

class ByyCheckableAction : public ByyAction
{
public:
	ByyCheckableAction(ByyApp& app,const QString& objectName,QWidget* parent=0);
	virtual ~ByyCheckableAction();

};

#endif // BYYMENUITEM_H
