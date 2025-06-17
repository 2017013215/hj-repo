#include "StdAfx.h"
#include "scnProfileWidget.h"
#include "scenarioManager.h"
#include "scenarioV2.h"
#include "ByyApp.h"
#include "../snigCore/coreUtils.h"
#include "SignalSingleton.h"

ByyScnProfileWidget::ByyScnProfileWidget( ByyApp& app,QWidget *parent /*= 0*/ ) 
	: QWidget(parent)
	,myApp(app)
{
	ui.setupUi(this);

	ByyScenarioV2 *scn=myApp.hdose().scenarioManager().scenario();

	if (scn)
	{
		const ByyScenarioV2::ScenarioInfo& scnInfo=scn->getScnInfo();

		const QVector<osg::Vec3d>& pts=scnInfo.mySimulationRect.points();

		QList<double> lons,lats;

		if (pts.size()>=4)
		{
			for (int i=0;i!=4;++i)
			{
				lons.push_back(pts[i].x());
				lats.push_back(pts[i].y());
			}

			qSort(lons.begin(),lons.end());


			ui.northSpinBox->setValue(lats.last());
			ui.southSpinBox->setValue(lats.first());
			ui.westSpinBox->setValue(lons.first());
			ui.eastSpinBox->setValue(lons.last());
		}

		ui.nameEdit->setText(scnInfo.myName);
		ui.outlineEdit->setText(scnInfo.myOutline);
		
		ui.startTimeEdit->setDateTime(scnInfo.myStartDateTime);
		ui.endTimeEdit->setDateTime(scnInfo.myEndDateTime);

		ui.scnRunNSpinBox->setValue(scnInfo.m_ScnRunN);

		ui.radomSeedgroupBox->setChecked(scnInfo.m_UseRandomSeed);

		ui.radomSeedspinBox->setValue(scnInfo.m_RandomSeed);

		ui.tSpinBox->setValue(scnInfo.m_T);

		ui.simNetworkCommCheckBox->setChecked(scnInfo.m_SimNetworkComm);
	}


	connect(ui.pushButton,SIGNAL(clicked()),SLOT(onOkClicked()));
	connect(ui.pushButton_2,SIGNAL(clicked()),SLOT(close()));

	connect(ui.northSpinBox,SIGNAL(valueChanged(double)),SLOT(onValueChanged()));
	connect(ui.southSpinBox,SIGNAL(valueChanged(double)),SLOT(onValueChanged()));
	connect(ui.westSpinBox,SIGNAL(valueChanged(double)),SLOT(onValueChanged()));
	connect(ui.eastSpinBox,SIGNAL(valueChanged(double)),SLOT(onValueChanged()));
	connect(ui.nameEdit,SIGNAL(textChanged(const QString&)),SLOT(onValueChanged()));
	connect(ui.outlineEdit,SIGNAL(textChanged()),SLOT(onValueChanged()));
	connect(ui.startTimeEdit,SIGNAL(dateTimeChanged(const QDateTime&)),SLOT(onValueChanged()));
	connect(ui.endTimeEdit,SIGNAL(dateTimeChanged(const QDateTime&)),SLOT(onValueChanged()));
	connect(ui.scnRunNSpinBox,SIGNAL(valueChanged(int)),SLOT(onValueChanged()));
	connect(ui.radomSeedgroupBox,SIGNAL(clicked()),SLOT(onValueChanged()));
	connect(ui.radomSeedspinBox,SIGNAL(valueChanged(int)),SLOT(onValueChanged()));
	connect(ui.tSpinBox,SIGNAL(valueChanged(double)),SLOT(onValueChanged()));
	connect(ui.simNetworkCommCheckBox,SIGNAL(clicked()),SLOT(onValueChanged()));

	connect(&myApp.hdose().scenarioManager(),SIGNAL(signal_scenarioClosed()),SLOT(close()));

	ui.pushButton->setEnabled(true);
	setWindowFlags(Qt::Window);
	setAttribute(Qt::WA_DeleteOnClose);
}

ByyScnProfileWidget::~ByyScnProfileWidget()
{

}

void ByyScnProfileWidget::onOkClicked()
{
	ByyScenarioV2 *scn=myApp.hdose().scenarioManager().scenario();

	if (scn)
	{
		QVector<osg::Vec3d> pts;
		pts.push_back(osg::Vec3d(ui.westSpinBox->value(),ui.southSpinBox->value(),0));
		pts.push_back(osg::Vec3d(ui.eastSpinBox->value(),ui.southSpinBox->value(),0));
		pts.push_back(osg::Vec3d(ui.eastSpinBox->value(),ui.northSpinBox->value(),0));
		pts.push_back(osg::Vec3d(ui.westSpinBox->value(),ui.northSpinBox->value(),0));

		ByyScenarioV2::ScenarioInfo scnInfo;

		scnInfo.mySimulationRect.setPoints(pts);

		scnInfo.myName=ui.nameEdit->text();
		scnInfo.myOutline=ui.outlineEdit->toPlainText();

		scnInfo.myStartDateTime=ui.startTimeEdit->dateTime();
		scnInfo.myEndDateTime=ui.endTimeEdit->dateTime();

		scnInfo.m_ScnRunN=ui.scnRunNSpinBox->value();

		scnInfo.m_UseRandomSeed=ui.radomSeedgroupBox->isChecked();

		scnInfo.m_RandomSeed=ui.radomSeedspinBox->value();

		scnInfo.m_T=ui.tSpinBox->value();

		scnInfo.m_SimNetworkComm=ui.simNetworkCommCheckBox->isChecked();

		scn->setScnInfo(scnInfo);

		frameWnd()->setWindowTitle(scnInfo.myName);

		signalSingleton::instance()->triggerLoadSuccess();  // 发送加载成功信号
	}

	close();
}

void ByyScnProfileWidget::onValueChanged()
{
	ui.pushButton->setEnabled(true);
}
