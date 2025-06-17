#include "StdAfx.h"
#include "scnEnvironmentSettingsWidget.h"
#include "scenarioManager.h"
#include "ByyApp.h"
#include "sceneRectangle.h"
#include "coreUtils.h"

#include <QInputDialog>

ByyScnEnvironmentSettingsWidget::ByyScnEnvironmentSettingsWidget( ByyApp& app,QWidget *parent /*= 0*/ ) 
	: QWidget(parent)
	,myApp(app)
	,mySceneRect(0)
{
	ui.setupUi(this);

	ui.addButton->setIcon(QIcon("../data/icons/UtilityAdd.png"));
	ui.removeButton->setIcon(QIcon("../data/icons/UtilityMinus.png"));
	ui.renameButton->setIcon(QIcon("../data/icons/UtilityRename.png"));

	connect(ui.comboBox,SIGNAL(currentIndexChanged(int)),this,SLOT(onCurrentItemChanged(int)));

	ByyScenarioV2 *scn=myApp.hdose().scenarioManager().scenario();

	if (scn)
	{
		myEditEnviromentList=scn->environmentList();

		for (int i=0;i!=myEditEnviromentList.size();++i)
		{
			ui.comboBox->addItem(myEditEnviromentList[i].myName);
		}
	}

	udateEditWidget();

	connect(ui.addButton,SIGNAL(clicked()),SLOT(addItem()));
	connect(ui.removeButton,SIGNAL(clicked()),SLOT(removeCurrentItem()));
	connect(ui.renameButton,SIGNAL(clicked()),SLOT(renameCurrentItem()));

	connect(ui.okButton,SIGNAL(clicked()),SLOT(onOkClicked()));
	connect(ui.cancelButton,SIGNAL(clicked()),SLOT(close()));

	connect(ui.northSpinBox,SIGNAL(valueChanged(double)),SLOT(updateRect()));
	connect(ui.southSpinBox,SIGNAL(valueChanged(double)),SLOT(updateRect()));
	connect(ui.westSpinBox,SIGNAL(valueChanged(double)),SLOT(updateRect()));
	connect(ui.eastSpinBox,SIGNAL(valueChanged(double)),SLOT(updateRect()));

	connect(ui.seaStateLevel_spinBox,SIGNAL(valueChanged(int)),SLOT(updateSeaState(int)));
	connect(ui.windCou_spinBox,SIGNAL(valueChanged(int)),SLOT(updateWindDirection(int)));
	connect(ui.windVel_spinBox,SIGNAL(valueChanged(int)),SLOT(updateWindSpeed(int)));

	connect(&myApp.hdose().scenarioManager(),SIGNAL(signal_scenarioClosed()),SLOT(close()));
}

ByyScnEnvironmentSettingsWidget::~ByyScnEnvironmentSettingsWidget()
{
	if (mySceneRect)
	{
		myApp.ig().renderer()->removeNodeFromRoot(mySceneRect->root(),ByyOsgRenderer::ControlObject);
		delete mySceneRect;
		mySceneRect=0;
	}
}

void ByyScnEnvironmentSettingsWidget::onOkClicked()
{
	if (ByyScenarioV2 *scn=myApp.hdose().scenarioManager().scenario())
	{
		scn->setEnvironmentList(myEditEnviromentList);
	}

	close();
}

void ByyScnEnvironmentSettingsWidget::setValueChanged()
{
	ui.okButton->setEnabled(true);
}

void ByyScnEnvironmentSettingsWidget::udateEditWidget()
{
	ui.editWidget->setEnabled(ui.comboBox->count());
	ui.renameButton->setEnabled(ui.comboBox->count());
	ui.removeButton->setEnabled(ui.comboBox->count());
}

void ByyScnEnvironmentSettingsWidget::onCurrentItemChanged( int index )
{
	ui.seaStateLevel_spinBox->blockSignals(true);
	ui.windCou_spinBox->blockSignals(true);
	ui.windVel_spinBox->blockSignals(true);
	ui.northSpinBox->blockSignals(true);
	ui.westSpinBox->blockSignals(true);
	ui.southSpinBox->blockSignals(true);
	ui.eastSpinBox->blockSignals(true);

	removeCurrentSceneRect();

	if (index==-1)
	{
		ui.seaStateLevel_spinBox->clear();
		ui.windCou_spinBox->clear();
		ui.windVel_spinBox->clear();
		ui.northSpinBox->setValue(0);
		ui.westSpinBox->setValue(0);
		ui.southSpinBox->setValue(0);
		ui.eastSpinBox->setValue(0);
	}
	else
	{
		ByyScenarioV2::Environment& envir=myEditEnviromentList[index];

		QVector<osg::Vec3d> pts=envir.myRectangle.points();

		QList<double> lons,lats;

		if (pts.size()>=4)
		{
			for (int i=0;i!=4;++i)
			{
				lons.push_back(pts[i].x());
				lats.push_back(pts[i].y());
			}

			qSort(lons.begin(),lons.end());
			qSort(lats.begin(),lats.end());


			ui.northSpinBox->setValue(lats.last());
			ui.southSpinBox->setValue(lats.first());
			ui.westSpinBox->setValue(lons.first());
			ui.eastSpinBox->setValue(lons.last());
		}

		ui.seaStateLevel_spinBox->setValue(envir.mySeaStateLevel);
		ui.windCou_spinBox->setValue(envir.myWindCou);
		ui.windVel_spinBox->setValue(envir.myWindVel);

		mySceneRect=new ByySceneRectangle(&envir.myRectangle,myApp.ig());
		mySceneRect->initiate();
		mySceneRect->setForceType(ByyForceNeutral);
		myApp.ig().renderer()->addNodeToRoot(mySceneRect->root(),ByyOsgRenderer::ControlObject);
	}

	ui.seaStateLevel_spinBox->blockSignals(false);
	ui.windCou_spinBox->blockSignals(false);
	ui.windVel_spinBox->blockSignals(false);
	ui.northSpinBox->blockSignals(false);
	ui.westSpinBox->blockSignals(false);
	ui.southSpinBox->blockSignals(false);
	ui.eastSpinBox->blockSignals(false);
}

void ByyScnEnvironmentSettingsWidget::addItem()
{
	ByyScenarioV2::Environment envir;
	envir.myName=QString::fromLocal8Bit("环境区域");
	envir.myRectangle.append(osg::Vec3d(110,20,1000));
	envir.myRectangle.append(osg::Vec3d(120,20,1000));
	envir.myRectangle.append(osg::Vec3d(120,30,1000));
	envir.myRectangle.append(osg::Vec3d(110,30,1000));

	myEditEnviromentList.append(envir);

	ui.comboBox->addItem(envir.myName);

	ui.comboBox->setCurrentIndex(ui.comboBox->count()-1);

	setValueChanged();
	udateEditWidget();
}

void ByyScnEnvironmentSettingsWidget::removeCurrentItem()
{
	int index=ui.comboBox->currentIndex();

	if (index==-1)
		return;

	removeCurrentSceneRect();

	myEditEnviromentList.removeAt(ui.comboBox->currentIndex());

	ui.comboBox->removeItem(ui.comboBox->currentIndex());

	setValueChanged();
	udateEditWidget();
}

void ByyScnEnvironmentSettingsWidget::renameCurrentItem()
{
	int index=ui.comboBox->currentIndex();

	if (index==-1)
		return;

	bool ok;
	QString text = QInputDialog::getText(this, QString::fromLocal8Bit("重命名"),
		QString::fromLocal8Bit("名称:"), QLineEdit::Normal,
		ui.comboBox->itemText(index), &ok);

	if (ok && !text.isEmpty())
	{
		myEditEnviromentList[index].myName=text;

		ui.comboBox->setItemText(index,text);

		setValueChanged();
	}
	
}

void ByyScnEnvironmentSettingsWidget::updateSeaState( int seaState )
{
	int index=ui.comboBox->currentIndex();

	if (index==-1)
		return;

	myEditEnviromentList[index].mySeaStateLevel=seaState;
}

void ByyScnEnvironmentSettingsWidget::updateWindDirection( int windDir )
{
	int index=ui.comboBox->currentIndex();

	if (index==-1)
		return;

	myEditEnviromentList[index].myWindCou=windDir;
}

void ByyScnEnvironmentSettingsWidget::updateWindSpeed( int windSpeed )
{
	int index=ui.comboBox->currentIndex();

	if (index==-1)
		return;

	myEditEnviromentList[index].myWindVel=windSpeed;
}

void ByyScnEnvironmentSettingsWidget::updateRect()
{
	int index=ui.comboBox->currentIndex();

	if (index==-1)
		return;

	QVector<osg::Vec3d> pts;
	pts.push_back(osg::Vec3d(ui.westSpinBox->value(),ui.southSpinBox->value(),0));
	pts.push_back(osg::Vec3d(ui.eastSpinBox->value(),ui.southSpinBox->value(),0));
	pts.push_back(osg::Vec3d(ui.eastSpinBox->value(),ui.northSpinBox->value(),0));
	pts.push_back(osg::Vec3d(ui.westSpinBox->value(),ui.northSpinBox->value(),0));

	myEditEnviromentList[index].myRectangle.setPoints(pts);
}

void ByyScnEnvironmentSettingsWidget::removeCurrentSceneRect()
{
	if (mySceneRect)
	{
		myApp.ig().renderer()->removeNodeFromRoot(mySceneRect->root(),ByyOsgRenderer::ControlObject);
		delete mySceneRect;
		mySceneRect=0;
	}
}
