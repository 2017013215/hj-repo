#ifndef ENTOBJEDITWIDGET_H
#define ENTOBJEDITWIDGET_H

#include "ByyEntityObserver.h"
#include "ui_entObjEditWidget.h"
#include "entityObjectFrame.h"
#include "CustomDelegate.h"

class ByyLocalEntity;
class BehaviorTreeEditWdg;

class ByyEditEntityWidget : public ByyEntityObserver
{
	Q_OBJECT

public:
	ByyEditEntityWidget(ByyCore& core,ByyAttributeEditMng &atmng,ByyLocalEntity* entObj,QWidget *parent = 0);
	~ByyEditEntityWidget();

	ByyLocalEntity* currentEnt(){return myEntObj;}

public slots:
	void updateObj();
	void cancel();

	void setName(const QString& name);
	void setHeading(double value);
	void setSideCurrentIndex(int index);

	void setLat(const QString& lat);
	void setLon(const QString& lon);
	void setAlt(const QString& alt);

	void setSpeed(const QString& speed);

	//ÐÐÎªÊ÷±à¼­
	void onBehaviorTreeEdit();
	void onUpdateBehaviorTreeInfo(QString &path,QString &content);

protected slots:
	void onItemDbClicked(QTreeWidgetItem* item,int column);
	void onItemChanged(QTreeWidgetItem* item,int column);
	
	void addArrayChild();
	void removeArrayChild();

	void onEntityRemove(ByyEntityObject* entObj);

protected:
	bool eventFilter(QObject *obj, QEvent *ev);

	void updateParam(const QString& name,const QString& value);

private:
	QTreeWidgetItem* addChild(ByyParameter* param,QTreeWidgetItem* parent);

	void updateParent(QTreeWidgetItem *item);
	void updateChildren(QTreeWidgetItem *item);

signals:
	void editFinished(ByyLocalEntity*);

private:
	Ui::ByyEntObjEditWidget ui;
	ByyLocalEntity *myEntObj;
	bool myEdited;
	QWidget *m_Parent;

	enum{Name,Tag,Value,Type,Desc, Design};

	ParameterList myEditParameters;

	QAbstractItemDelegate * m_defaultDelegates;
	CCustomDelegate * m_delegates;

	BehaviorTreeEditWdg *m_BehaviorTreeEdit;
	ByyAttributeEditMng& myATMng;
};

#endif // ENTOBJEDITWIDGET_H
