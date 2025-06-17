#include "stdafx.h"
#include "terrainEffectWidget.h"
#include "terrainEffect.h"
#include "../snigCore/coreUtils.h"

ByyTerrainEffectWidget::ByyTerrainEffectWidget( ByyTerrainEffect *effect,QWidget *parent,Qt::WindowFlags f ) 
	: QWidget(parent,f|Qt::Window),myEffect(effect)
{
	ui.setupUi(this);

	connect(ui.checkBox,SIGNAL(stateChanged(int)),SLOT(contourCheckChanged(int)));
	connect(ui.checkBox_2,SIGNAL(stateChanged(int)),SLOT(verticalCheckeChanged(int)));
	connect(ui.horizontalSlider,SIGNAL(valueChanged(int)),SLOT(contourSliderChanged(int)));
	connect(ui.horizontalSlider_2,SIGNAL(valueChanged(int)),SLOT(verticalSliderChanged(int)));
	connect(ui.doubleSpinBox,SIGNAL(valueChanged(double)),SLOT(contourSpinChanged(double)));
	connect(ui.doubleSpinBox_2,SIGNAL(valueChanged(double)),SLOT(verticalSpinChanged(double)));

	setWindowFlags(this->windowFlags()& ~Qt::WindowMaximizeButtonHint& ~Qt::WindowMinimizeButtonHint);

	openGrayEffect=new QCheckBox(CNTR("»Ò¶È±³¾°"));
	ui.verticalLayout->addWidget(openGrayEffect);
	connect(openGrayEffect,SIGNAL(stateChanged(int)),SLOT(openGrayEffectChanged(int)));

	openGrayEffect->setChecked(myEffect->isOpenGrayEffect());
	ui.checkBox->setChecked(myEffect->isEnableContour());
	ui.checkBox_2->setChecked(myEffect->isEnableVertical());
	
	ui.horizontalSlider->setValue(myEffect->contourVal()*100);
	ui.doubleSpinBox->setValue(myEffect->contourVal());
	ui.horizontalSlider_2->setValue(myEffect->verticalVal()*10);
	ui.doubleSpinBox_2->setValue(myEffect->verticalVal());
}

ByyTerrainEffectWidget::~ByyTerrainEffectWidget()
{
	//d_effectsComponent->enableContour(false);
	//d_effectsComponent->enableVertical(false);
}

void ByyTerrainEffectWidget::openGrayEffectChanged(int state)
{
	myEffect->openGrayEffect(state);
}
void ByyTerrainEffectWidget::contourSliderChanged( int slider_value )
{
	myEffect->setContourOpacity(slider_value/100.0);
	ui.doubleSpinBox->setValue(slider_value/100.0);
}

void ByyTerrainEffectWidget::verticalSliderChanged( int slider_value )
{
	myEffect->setVerticalScale(slider_value/10.0);
	ui.doubleSpinBox_2->setValue(slider_value/10.0);
}

void ByyTerrainEffectWidget::contourCheckChanged( int state )
{
	if(state==2)
	{
		myEffect->enableContour(true);
	}
	else
	{
		myEffect->enableContour(false);
	}
}

void ByyTerrainEffectWidget::verticalCheckeChanged( int state )
{
	if(state==2)
		myEffect->enableVertical(true);
	else
		myEffect->enableVertical(false);
}

void ByyTerrainEffectWidget::contourSpinChanged( double spin_value )
{
	ui.horizontalSlider->setValue(spin_value*100);
}

void ByyTerrainEffectWidget::verticalSpinChanged( double spin_value )
{
	ui.horizontalSlider_2->setValue(spin_value*10);
}
