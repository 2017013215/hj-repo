#ifndef BYYENTITYINFOBOXWIDGT_H
#define BYYENTITYINFOBOXWIDGT_H

#include "ByyEntityObserver.h"

#include "ByySelectionManager.h"
#include "ui_ByyEntityInfoBoxWidgt.h"

class ByySensorSR;
class ByyApp;

class SensorPowerButton : public QPushButton
{
	Q_OBJECT
public:
	SensorPowerButton(ByyApp &app,const QString & text, QWidget * parent = 0)
		:QPushButton(text,parent)
		,myApp(app)
	{
		connect(this,SIGNAL(clicked()),this,SLOT(switchPower()));
	}
	~SensorPowerButton()
	{

	}

protected slots:
	void switchPower();

public:
	ByyApp&	myApp;
	ByyEntityObject *myEntObj;
	ByySensorSR* mySR;
	QString myDevice;
};


class ByyEntityInfoBoxWidgt : public ByyEntityObserver
{
	Q_OBJECT

public:
	ByyEntityInfoBoxWidgt(ByyApp& app,ByyEntityObject* entity=0,QWidget* parent=0,Qt::WindowFlags f = 0);
	~ByyEntityInfoBoxWidgt();

	void setEntity(ByyEntityObject *entity);

	QSize sizeHint()const;

public slots:
	void updateParam();

protected slots:
	void onCurrentSelectionChanged(ByySelectionManager::SelectionType type,unsigned int id);
	void onItemClicked(QTableWidgetItem *item);

protected:
	void updateSensor();

protected:
	Ui::ByyEntityInfoBoxWidgt	ui;
					   ByyApp&	myApp;	
	                   double	myLastUpdateTime;

};

#endif // BYYENTITYINFOBOXWIDGT_H
