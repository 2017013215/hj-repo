#include "StdAfx.h"
#include "waitBackEngineWidget.h"
#include "ByyHdose.h"
#include "simuController.h"

ByyWaitBackEngineWidget::ByyWaitBackEngineWidget( ByyHdose& hdose,QWidget *parent /*= 0*/ ) 
	: QWidget(parent,Qt::Dialog|Qt::WindowCloseButtonHint|Qt::WindowStaysOnTopHint)
	,myHdose(hdose)
{
	ui.setupUi(this);

	setAttribute(Qt::WA_DeleteOnClose);

	connect(&myHdose,SIGNAL(signal_stopped()),SLOT(close()));

	connect(&myHdose.simuController(),SIGNAL(signal_StatusChanged(int)),SLOT(onBackStateChanged(int)));
}

ByyWaitBackEngineWidget::~ByyWaitBackEngineWidget()
{

}

void ByyWaitBackEngineWidget::onBackStateChanged( int state )
{
	if (state>0)
	{
		close();
	}
}
