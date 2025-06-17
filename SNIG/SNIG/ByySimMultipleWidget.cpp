#include "StdAfx.h"
#include "ByySimMultipleWidget.h"
#include "ByyHdose.h"
#include "simuController.h"
#include "scenarioManager.h"

ByySimMultipleWidget::ByySimMultipleWidget(ByyHdose& hdose,QWidget *parent)
	: QWidget(parent)
	,myHdose(hdose)
{
	ui.setupUi(this);
	setEnabled(false);

	connect(ui.doubleSpinBox,SIGNAL(valueChanged(double)),this,SLOT(onValueChanged(double)));

	connect(&myHdose,SIGNAL(signal_runningChanged()),SLOT(checkEnabled()));
	connect(&myHdose.scenarioManager(),SIGNAL(signal_scenarioChanged()),SLOT(checkEnabled()));
	connect(&myHdose.simuController(),SIGNAL(signal_StatusChanged(int)),SLOT(checkEnabled()));

	connect(&myHdose,SIGNAL(signal_runningChanged()),SLOT(resetUiValue()));
}

ByySimMultipleWidget::~ByySimMultipleWidget()
{

}

void ByySimMultipleWidget::onValueChanged( double val )
{
	myHdose.setSimMultiplier(val);
}

void ByySimMultipleWidget::checkEnabled()
{
	setEnabled(myHdose.started()&&
		(myHdose.mode()==ByyHdose::Single||myHdose.mode()==ByyHdose::Create)&&myHdose.scenarioManager().scenario()&&
		(myHdose.simuController().isRunning()||myHdose.simuController().isPaused()));
}

void ByySimMultipleWidget::resetUiValue()
{
	ui.doubleSpinBox->blockSignals(true);
	ui.doubleSpinBox->setValue(1);
	ui.doubleSpinBox->blockSignals(false);
}
