#include "StdAfx.h"
#include "showControlWidget.h"
#include "entitySettings.h"
#include "observerSettings.h"

ByyShowControlWidget::ByyShowControlWidget(QWidget *parent)
	: QWidget(parent)
{
	ui.setupUi(this);

	ui.entityCheckBox->setChecked(ByyObserverSettings::instance().entityVisible());
	connect(ui.entityCheckBox,SIGNAL(clicked(bool)),&ByyObserverSettings::instance(),SLOT(setEntityVisible(bool)));

	ui.ctrlObjCheckBox->setChecked(ByyObserverSettings::instance().ctrlObjVisible());
	connect(ui.ctrlObjCheckBox,SIGNAL(clicked(bool)),&ByyObserverSettings::instance(),SLOT(setCtrlObjVisible(bool)));

	ui.ctrlObjLableCheckBox->setChecked(ByyObserverSettings::instance().ctrlObjLableVisible());
	connect(ui.ctrlObjLableCheckBox,SIGNAL(clicked(bool)),&ByyObserverSettings::instance(),SLOT(setCtrlObjLableVisible(bool)));

	ui.entityLabelCheckBox->setChecked(ByyEntitySettings::instance().entityLabelEnabled());
	connect(ui.entityLabelCheckBox,SIGNAL(clicked(bool)),&ByyEntitySettings::instance(),SLOT(setEntityLabelEnabled(bool)));

	ui.entityNameCheckBox->setChecked(ByyEntitySettings::instance().entityNameEnabled());
	connect(ui.entityNameCheckBox,SIGNAL(clicked(bool)),&ByyEntitySettings::instance(),SLOT(setEntityNameEnabled(bool)));

	ui.entityInfoCheckBox->setChecked(ByyEntitySettings::instance().entityInfoEnabled());
	connect(ui.entityInfoCheckBox,SIGNAL(clicked(bool)),&ByyEntitySettings::instance(),SLOT(setEntityInfoEnabled(bool)));

	ui.electronicCheckBox->setChecked(ByyEntitySettings::instance().electronicEnabled());
	connect(ui.electronicCheckBox,SIGNAL(clicked(bool)),&ByyEntitySettings::instance(),SLOT(setElectronicEnabled(bool)));

	ui.ribbonCheckBox->setChecked(ByyEntitySettings::instance().trackHistoryEnabled());
	connect(ui.ribbonCheckBox,SIGNAL(clicked(bool)),&ByyEntitySettings::instance(),SLOT(setTrackHistoryEnabled(bool)));

	ui.graticuleCheckBox->setChecked(ByyObserverSettings::instance().graticuleVisible());
	connect(ui.graticuleCheckBox,SIGNAL(clicked(bool)),&ByyObserverSettings::instance(),SLOT(setGraticuleVisible(bool)));

	ui.satelliteOribtCheckBox->setChecked(ByyEntitySettings::instance().satelliteOribtVisible());
	connect(ui.satelliteOribtCheckBox,SIGNAL(clicked(bool)),&ByyEntitySettings::instance(),SLOT(setSatelliteOribtVisible(bool)));

	ui.radioCommCheckBox->setChecked(ByyObserverSettings::instance().radioCommVisible());
	connect(ui.radioCommCheckBox,SIGNAL(clicked(bool)),&ByyObserverSettings::instance(),SLOT(setRadioCommVisible(bool)));

	ui.subjectionCheckBox->setChecked(ByyObserverSettings::instance().subjectionVisible());
	connect(ui.subjectionCheckBox,SIGNAL(clicked(bool)),&ByyObserverSettings::instance(),SLOT(setSubjectionVisible(bool)));

	ui.trackCheckBox->setChecked(ByyObserverSettings::instance().trackVisible());
	connect(ui.trackCheckBox,SIGNAL(clicked(bool)),&ByyObserverSettings::instance(),SLOT(setTrackVisible(bool)));

	ui.guidanceCheckBox->setChecked(ByyObserverSettings::instance().guidanceVisible());
	connect(ui.guidanceCheckBox,SIGNAL(clicked(bool)),&ByyObserverSettings::instance(),SLOT(setGuidanceVisible(bool)));

	ui.routePlanCheckBox->setChecked(ByyObserverSettings::instance().routePlanVisible());
	connect(ui.routePlanCheckBox,SIGNAL(clicked(bool)),&ByyObserverSettings::instance(),SLOT(setRoutePlanVisible(bool)));
}

ByyShowControlWidget::~ByyShowControlWidget()
{

}
