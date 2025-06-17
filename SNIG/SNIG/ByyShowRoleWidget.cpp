#include "StdAfx.h"
#include "ByyShowRoleWidget.h"

#include "ByyApp.h"
#include "ByyCore.h"
#include "remoteObjectManager.h"
#include "settings.h"

ByyShowRoleWidget::ByyShowRoleWidget(ByyApp& app,QWidget *parent)
	: QWidget(parent)
	,myApp(app)
{
	ui.setupUi(this);

	connect(ui.comboBox,SIGNAL(currentIndexChanged(int)),this,SLOT(onRoleChanged(int)));

	if (ByyConfigIni::instance()->userRule()==QString::fromLocal8Bit("ºì·½"))
	{
		ui.comboBox->setCurrentIndex(ByyRemoteObjectManager::Red);
		ui.comboBox->setEnabled(false);
	}
	else if (ByyConfigIni::instance()->userRule()==QString::fromLocal8Bit("À¶·½"))
	{
		ui.comboBox->setCurrentIndex(ByyRemoteObjectManager::Blue);
		ui.comboBox->setEnabled(false);
	}
}

ByyShowRoleWidget::~ByyShowRoleWidget()
{

}

void ByyShowRoleWidget::onRoleChanged( int index )
{
	myApp.core().remoteObjectManager().setShowRole((ByyRemoteObjectManager::ShowRole)index);
}
