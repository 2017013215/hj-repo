#include "stdafx.h"
#include "environmentWidget.h"
#include "ByyEnvironment.h"

ByyEnvironmentWidget::ByyEnvironmentWidget( ByyEnvironment& environment,QWidget *parent /*= 0*/ ) 
	: QWidget(parent,Qt::Window),
	tickInterval(500),
	myEnvironment(environment)
{
	ui.setupUi(this);

	initCmpass();

	init();

	connect(ui.doubleSpinBox,SIGNAL(valueChanged(double)),&myEnvironment,SLOT(setTimeScale(double)));
	connect(ui.doubleSpinBox,SIGNAL(valueChanged(double)),SLOT(timeScaleChanged(double)));
	connect(ui.oceanHeightSlider,SIGNAL(valueChanged(int)),&myEnvironment,SLOT(setSeaLevel(int)));
	connect(ui.visibilitySpinBox,SIGNAL(valueChanged(double)),&myEnvironment,SLOT(setVisibility(double)));

	connect(ui.precipitationType,SIGNAL(currentIndexChanged(int)),this,SLOT(updatePrecipitation()));
	connect(ui.preciIntensitySlider,SIGNAL(valueChanged(int)),this,SLOT(updatePrecipitation()));
	connect(ui.precipitationNear,SIGNAL(valueChanged(int)),this,SLOT(updatePrecipitation()));
	connect(ui.precipitationFar,SIGNAL(valueChanged(int)),this,SLOT(updatePrecipitation()));

	connect(&timer,SIGNAL(timeout()),SLOT(updateDateTime()));

	if (myEnvironment.timeScale()>0)
	{
		timer.start(tickInterval);
	}


	connect(ui.calendarWidget,SIGNAL(clicked(const QDate&)),SLOT(calendarToggled(const QDate&)));
	connect(ui.timeEdit,SIGNAL(timeChanged(const QTime&)),SLOT(timeEditToggled(const QTime&)));
	connect(ui.horizontalSlider,SIGNAL(valueChanged(int)),SLOT(timeSliderChanged()));
	connect(ui.horizontalSlider_2,SIGNAL(valueChanged(int)),SLOT(timeSliderChanged()));
	connect(ui.horizontalSlider_3,SIGNAL(valueChanged(int)),SLOT(ambientValueChanged(int)));

	
	connect(ui.pushButton_3,SIGNAL(clicked()),SLOT(useSystemSettings()));
	
	connect(ui.windDirectionCompass,SIGNAL(valueChanged(double)),this,SLOT(updateWind()));
	connect(ui.windSpeedSpinBox,SIGNAL(valueChanged(double)),this,SLOT(updateWind()));
	

	connect(ui.visibilitySlider,SIGNAL(valueChanged(int)),SLOT(visibilitySliderChanged(int)));

	connect(ui.cloudTypesComboBox,SIGNAL(currentIndexChanged(int)),this,SLOT(onCloudLayerTypeChanged(int)));
}

ByyEnvironmentWidget::~ByyEnvironmentWidget()
{

}

void ByyEnvironmentWidget::init()
{
	updateDateTime();

	ui.doubleSpinBox->setValue(myEnvironment.timeScale());

	ui.windDirectionCompass->setValue(0);

	ui.windSpeedSpinBox->setValue(10);

	ui.oceanHeightSlider->setValue(myEnvironment.seaLevel());
}

void ByyEnvironmentWidget::initCmpass()
{
	ui.windDirectionCompass->setLineWidth(0);
	ui.windDirectionCompass->setFrameShadow(QwtCompass::Raised);

	QwtCompassScaleDraw *scaleDraw = new QwtCompassScaleDraw();
	//scaleDraw->enableComponent( QwtAbstractScaleDraw::Ticks, true );
	//scaleDraw->enableComponent( QwtAbstractScaleDraw::Labels, true );
	//scaleDraw->enableComponent( QwtAbstractScaleDraw::Backbone, false );
	scaleDraw->setTickLength( QwtScaleDiv::MinorTick, 0 );
	scaleDraw->setTickLength( QwtScaleDiv::MediumTick, 0 );
	scaleDraw->setTickLength( QwtScaleDiv::MajorTick, 3 );

	ui.windDirectionCompass->setScaleDraw( scaleDraw );

	ui.windDirectionCompass->setNeedle( new QwtCompassMagnetNeedle(
		QwtCompassMagnetNeedle::TriangleStyle,Qt::transparent, Qt::red ) );
}

void ByyEnvironmentWidget::useSystemSettings()
{
	myEnvironment.setDateTime(QDateTime::currentDateTime());

	updateDateTime();
}

void ByyEnvironmentWidget::timeScaleChanged( double scale )
{
	if (scale==0&&timer.isActive())
	{
		timer.stop();
	}
	else if (scale!=0&&!timer.isActive())
	{
		timer.start(tickInterval);
	}
}

void ByyEnvironmentWidget::updateDateTime()
{
	ui.calendarWidget->blockSignals(true);
	ui.timeEdit->blockSignals(true);
	ui.horizontalSlider->blockSignals(true);
	ui.horizontalSlider_2->blockSignals(true);

	if (myEnvironment.dateTime().date()!=ui.calendarWidget->selectedDate())
	{
		ui.calendarWidget->setSelectedDate(myEnvironment.dateTime().date());
	}

	
	ui.timeEdit->setTime(myEnvironment.dateTime().time());
	ui.horizontalSlider->setValue(myEnvironment.dateTime().time().hour());
	ui.horizontalSlider_2->setValue(myEnvironment.dateTime().time().minute());
	ui.AnalogClock->setTime(myEnvironment.dateTime().time());


	ui.calendarWidget->blockSignals(false);
	ui.timeEdit->blockSignals(false);
	ui.horizontalSlider->blockSignals(false);
	ui.horizontalSlider_2->blockSignals(false);
}

void ByyEnvironmentWidget::calendarToggled( const QDate& date )
{
	QDateTime dt=myEnvironment.dateTime();
	dt.setDate(date);

	myEnvironment.setDateTime(dt);
}

void ByyEnvironmentWidget::timeEditToggled( const QTime & time )
{
	QDateTime dt=myEnvironment.dateTime();
	dt.setTime(time);
	myEnvironment.setDateTime(dt);

	updateDateTime();
}

void ByyEnvironmentWidget::timeSliderChanged()
{
	QTime oldTime=ui.timeEdit->time();

	QTime newTime(ui.horizontalSlider->value(),ui.horizontalSlider_2->value(),oldTime.second(),oldTime.msec());

	ui.AnalogClock->setTime(newTime);
	ui.timeEdit->setTime(newTime);
}

void ByyEnvironmentWidget::ambientValueChanged( int value )
{
	myEnvironment.setAmbient(value/100.0);
}

void ByyEnvironmentWidget::visibilitySliderChanged( int value )
{
	ui.visibilitySpinBox->setValue(value/100.0*ui.visibilitySpinBox->maximum());
}

void ByyEnvironmentWidget::onCloudLayerTypeChanged( int index )
{
	if (index>0)
	{
		myEnvironment.setCloudLayerType(index-1);
	}
	else if(index==0)
		myEnvironment.removeCloudLayer();
}

void ByyEnvironmentWidget::updatePrecipitation()
{
	myEnvironment.setPrecipitation(ui.precipitationType->currentIndex(),
		ui.preciIntensitySlider->value(),
		ui.precipitationNear->value(),
		ui.precipitationFar->value());
}

void ByyEnvironmentWidget::updateWind()
{
	myEnvironment.setWind(ui.windSpeedSpinBox->value(),ui.windDirectionCompass->value());
}
