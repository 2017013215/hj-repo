#ifndef ENTITYEDITWIDGET_H
#define ENTITYEDITWIDGET_H

#include <QWidget>

class ByyEntityObject;

class ByyEntityObserver : public QWidget
{
	Q_OBJECT
public:
	ByyEntityObserver(ByyCore& core,ByyEntityObject* entity=0,QWidget* parent=0,Qt::WindowFlags f = 0);
	virtual ~ByyEntityObserver();

	virtual void setEntity(ByyEntityObject *entity);

protected slots:
	virtual void onEntityRemove(ByyEntityObject* entObj);
	
public:
	ByyEntityObject* myCurrentEntity;

protected:
	ByyCore& myCore;
	
};

#endif // ENTITYEDITWIDGET_H
