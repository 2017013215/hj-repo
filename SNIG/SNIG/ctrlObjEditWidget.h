#ifndef CTRLOBJEDITWIDGET_H
#define CTRLOBJEDITWIDGET_H

#include <QWidget>
#include "ui_ctrlObjEditWidget.h"
#include "ByyRoute.h"

class ByyControlObject;

class ByyCtrlObjEditWidget : public QWidget
{
	Q_OBJECT

public:
	ByyCtrlObjEditWidget(ByyControlObject* ctrlObj,QWidget *parent = 0);
	~ByyCtrlObjEditWidget();

protected slots:
	void updateObj();

protected:
	void setPointsToTableWidget(const ByyVec3dArray& points,const ByyDoubleArray& speedS, const ByyDoubleArray& BCrossS, const ByyDoubleArray& WaitTimeS, const ByyDoubleArray& BTimeCtrlS, const ByyDoubleArray& TimeArriveS,  const ByyDoubleArray& IRound, const ByyDoubleArray& RSurround);
	void setPointsToTableWidgetComm(const ByyVec3dArray& points);
	ByyVec3dArray tableWidgetToPoints();
	ByyDoubleArray tableWidetToSpeedS();
	ByyDoubleArray tableWidetToDoubleArray(int column);
	ByyDoubleArray tableWidetToBCrossS();

private:
	Ui::ByyCtrlObjEditWidget ui;
	ByyControlObject* myCtrlObj;
};

#endif // CTRLOBJEDITWIDGET_H
