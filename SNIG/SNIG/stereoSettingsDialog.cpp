#include "stdafx.h"
#include "stereoSettingsDialog.h"

#include <osg/DisplaySettings>

ByyStereoSettingsDialog::ByyStereoSettingsDialog(QWidget *parent)
	: QWidget(parent,Qt::WindowSystemMenuHint|Qt::Dialog)
{
	ui.setupUi(this);

	QPalette palette;
	palette.setColor(QPalette::Background, QColor(255,255,255));
	setPalette(palette);

	ui.doubleSpinBox->setValue(osg::DisplaySettings::instance()->getEyeSeparation());
	ui.doubleSpinBox_2->setValue(osg::DisplaySettings::instance()->getScreenDistance());

	connect(ui.doubleSpinBox,SIGNAL(valueChanged(double)),SLOT(eyeSeparationChanged(double)));
	connect(ui.doubleSpinBox_2,SIGNAL(valueChanged(double)),SLOT(screenDistanceChanged(double)));
}

ByyStereoSettingsDialog::~ByyStereoSettingsDialog()
{

}

void ByyStereoSettingsDialog::eyeSeparationChanged( double value )
{
	osg::DisplaySettings::instance()->setEyeSeparation(value);
}

void ByyStereoSettingsDialog::screenDistanceChanged( double value )
{
	osg::DisplaySettings::instance()->setScreenDistance(value);
}
