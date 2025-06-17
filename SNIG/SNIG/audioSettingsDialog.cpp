#include "stdafx.h"
#include "audioSettingsDialog.h"

#include "audioSettings.h"

ByyAudioSettingsDialog::ByyAudioSettingsDialog(QWidget *parent)
	: QDialog(parent,Qt::Dialog|Qt::WindowSystemMenuHint)
{
	ui.setupUi(this);

	ui.tabWidget->setTabIcon(0,QIcon("../data/icons/ConfigurationSound.png"));
	ui.tabWidget->setTabIcon(1,QIcon("../data/icons/DisplayEntity.png"));
	ui.tabWidget->setTabIcon(2,QIcon("../data/icons/DisplayFires.png"));
	ui.tabWidget->setTabIcon(3,QIcon("../data/icons/DisplayDetonations.png"));

	//ui.tab_2->init(ByyEntityAudioMapping::instance(),QString::fromLocal8Bit("ÊµÌåÉùÒôÉèÖÃ"));
	//ui.tab_3->init(ByyFireAudioMapping::instance(),QString::fromLocal8Bit("¿ª»ðÉùÒôÉèÖÃ"));
	//ui.tab_4->init(ByyDetonationAudioMapping::instance(),QString::fromLocal8Bit("±¬Õ¨ÉùÒôÉèÖÃ"));

	ui.checkBox->setChecked(ByyAudioSettings::instance().enabled());
	ui.horizontalSlider->setValue(ByyAudioSettings::instance().soundVolume()*10);
	ui.spinBox->setValue(ByyAudioSettings::instance().range());

	connect(ui.checkBox,SIGNAL(clicked(bool)),&ByyAudioSettings::instance(),SLOT(setEnabled(bool)));
	connect(ui.horizontalSlider,SIGNAL(valueChanged(int)),this,SLOT(soundVolumeToggled(int)));
	connect(ui.spinBox,SIGNAL(valueChanged(int)),&ByyAudioSettings::instance(),SLOT(setRange(int)));
}

ByyAudioSettingsDialog::~ByyAudioSettingsDialog()
{

}

void ByyAudioSettingsDialog::soundVolumeToggled( int value )
{
	ByyAudioSettings::instance().setSoundVolume(value/10.0);
}
