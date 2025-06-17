#include "stdafx.h"
#include "modelWidget.h"
#include "model.h"
#include "modelPlace.h"
#include "modelEditPlace.h"
#include "sceneController.h"
#include "videoEditDialog.h"
#include "buildingInfoDialog.h"

#include <osgEarth/Registry>

ByyModelWidget::ByyModelWidget( ByySceneController* s,QWidget *parent /*= 0*/ ) 
	: ByyOperateWidget(s,parent)
{
	ui.setupUi(this);

	myBuildingInfoWidget=new ByyBuildingInfoDialog(this);
	myBuildingInfoWidget->setWindowFlags(Qt::SubWindow);
	layout()->addWidget(myBuildingInfoWidget);
	myBuildingInfoWidget->setEnabled(false);
	connect(myBuildingInfoWidget,SIGNAL(modified()),SIGNAL(modified()));

	myVideoEditDialog=new ByyVideoEditDialog(this);
	myVideoEditDialog->setWindowFlags(Qt::SubWindow);
	layout()->addWidget(myVideoEditDialog);
	myVideoEditDialog->setEnabled(false);
	connect(myVideoEditDialog,SIGNAL(modified()),SIGNAL(modified()));

	myVideoEditDialog->setVisible(false);
	myBuildingInfoWidget->setVisible(false);
	ui.myModelPropertyWidget->setVisible(false);

	ui.pushButton->setIcon(QIcon("../data/icons/EarthTool/find.png"));
	ui.pushButton_2->setIcon(QIcon("../data/icons/UtilityMinus.png"));
	ui.pushButton_3->setIcon(QIcon("../data/icons/EarthTool/cursor_drag_arrow.png"));
	ui.pushButton_6->setIcon(QIcon("../data/icons/UtilityRename.png"));

	connect(ui.listWidget,SIGNAL(currentRowChanged(int)),SLOT(zoomto()));
	connect(ui.listWidget,SIGNAL(currentRowChanged(int)),SLOT(currentModelChanged(int)));
	connect(ui.doubleSpinBox,SIGNAL(valueChanged(double)),SLOT(setPostionX(double)));
	connect(ui.doubleSpinBox_2,SIGNAL(valueChanged(double)),SLOT(setPostionY(double)));
	connect(ui.doubleSpinBox_3,SIGNAL(valueChanged(double)),SLOT(setPostionZ(double)));
	connect(ui.doubleSpinBox_4,SIGNAL(valueChanged(double)),SLOT(setRoatate()));
	connect(ui.doubleSpinBox_5,SIGNAL(valueChanged(double)),SLOT(setRoatate()));
	connect(ui.doubleSpinBox_6,SIGNAL(valueChanged(double)),SLOT(setRoatate()));
	connect(ui.doubleSpinBox_7,SIGNAL(valueChanged(double)),SLOT(setScaleX(double)));
	connect(ui.doubleSpinBox_8,SIGNAL(valueChanged(double)),SLOT(setScaleY(double)));
	connect(ui.doubleSpinBox_9,SIGNAL(valueChanged(double)),SLOT(setScaleZ(double)));
	connect(ui.pushButton,SIGNAL(clicked()),SLOT(zoomto()));
	connect(ui.pushButton_2,SIGNAL(clicked()),SLOT(remove()));
	connect(ui.pushButton_3,SIGNAL(clicked()),SLOT(move()));
	connect(ui.pushButton_6,SIGNAL(clicked()),SLOT(rename()));
	connect(ui.checkBox,SIGNAL(stateChanged(int)),SLOT(setBuilding(int)));
	connect(ui.checkBox_2,SIGNAL(stateChanged(int)),SLOT(setVideo(int)));
	

	connect(ui.checkBox_3,SIGNAL(stateChanged(int)),SLOT(setEntityHeight(int)));
	connect(ui.doubleSpinBox_10,SIGNAL(valueChanged(double)),SLOT(setEntityHeightValue(double)));

	connect(ui.myLodCheckBox,SIGNAL(stateChanged(int)),SLOT(setLod(int)));
	connect(ui.myLodRange,SIGNAL(valueChanged(double)),SLOT(setLodRange(double)));

	connect(ui.terrainCheckBox,SIGNAL(stateChanged(int)),SLOT(setTerrain(int)));

	modelPick=new ByyModelPicking(&modelList);

	connect(modelPick,SIGNAL(currentModelChanged(int)),SLOT(pickingModel(int)));
}

ByyModelWidget::~ByyModelWidget()
{

}

void ByyModelWidget::init()
{
	release();

	osgEarth::MapNode* mapNode=d_scene->getMapNode();

	osgEarth::Config& externals=mapNode->externalConfig();
	osgEarth::Config models=externals.child(getConfigKey());
	const osgEarth::ConfigSet& children = models.children();

	for(osgEarth::ConfigSet::const_iterator i = children.begin(); i != children.end(); ++i )
	{
		if (!i->empty())
		{
			addItem(new ByyModel(mapNode,*i));
		}
	}
	d_scene->getViewer()->addEventHandler(modelPick);
}

void ByyModelWidget::release()
{
	d_scene->getViewer()->removeEventHandler(modelPick);
	d_root->removeChildren(0,d_root->getNumChildren());

	qDeleteAll(modelList);
	ui.listWidget->clear();
	modelList.clear();
}

std::string ByyModelWidget::getConfigKey()
{
	return "models";
}

void ByyModelWidget::writeToMapNode()
{
	osgEarth::Config& externals=d_scene->getMapNode()->externalConfig();

	externals.remove(getConfigKey());

	osgEarth::Config newConf(getConfigKey());

	for (int i=0;i!=modelList.count();++i)
	{
		newConf.add(modelList[i]->getConfig());
	}

	externals.add(newConf);
}

void ByyModelWidget::currentModelChanged( int row )
{
	ui.groupBox_6->setEnabled(row!=-1);
	ui.groupBox->setEnabled(row!=-1);
	ui.groupBox_3->setEnabled(row!=-1);
	ui.groupBox_2->setEnabled(row!=-1);

	ui.checkBox_3->setEnabled(row!=-1);
	ui.doubleSpinBox_10->setEnabled(row!=-1);



	ui.myLodCheckBox->setEnabled(row!=-1);
	ui.myLodRange->setEnabled(row!=-1);

	ui.terrainCheckBox->setEnabled(row!=-1);

	myBuildingInfoWidget->setEnabled(row!=-1);

	myVideoEditDialog->setEnabled(row!=-1);

	if (row==-1)
	{
		return;
	}

	ByyModel *model=modelList[row];

	myBuildingInfoWidget->setEnabled(model->isBuilding());

	myBuildingInfoWidget->setCurrentModel(model);

	myVideoEditDialog->setEnabled(model->isVideo());

	myVideoEditDialog->setCurrentModel(model);


	blockAllSignals(true);

	ui.checkBox->setChecked(model->isBuilding());


	ui.checkBox_2->setChecked(model->isVideo());


	ui.checkBox_3->setChecked(model->isEntHeight());
	ui.doubleSpinBox_10->setValue(model->getEntHeightValue());
	

	ui.doubleSpinBox->setValue(model->positionX());
	ui.doubleSpinBox_2->setValue(model->positionY());
	ui.doubleSpinBox_3->setValue(model->positionZ());
	ui.doubleSpinBox_4->setValue(osg::inRadians(model->rotationX()));
	ui.doubleSpinBox_5->setValue(osg::inRadians(model->rotationY()));
	ui.doubleSpinBox_6->setValue(osg::inRadians(model->rotationZ()));
	ui.doubleSpinBox_7->setValue(model->scaleX());
	ui.doubleSpinBox_8->setValue(model->scaleY());
	ui.doubleSpinBox_9->setValue(model->scaleZ());

	ui.myLodCheckBox->setChecked(model->useLOD());
	ui.myLodRange->setValue(model->lodRange());

	ui.terrainCheckBox->setChecked(model->isTerrain());

	blockAllSignals(false);
}

void ByyModelWidget::setPostionX( double value )
{
	modelList[ui.listWidget->currentRow()]->setPositionX(value);
	emit modified();
}

void ByyModelWidget::setPostionY( double value )
{
	modelList[ui.listWidget->currentRow()]->setPositionY(value);
	emit modified();
}

void ByyModelWidget::setPostionZ( double value )
{
	modelList[ui.listWidget->currentRow()]->setPositionZ(value);
	emit modified();
}

void ByyModelWidget::setRoatate()
{
	modelList[ui.listWidget->currentRow()]->setRotation(osg::inDegrees(ui.doubleSpinBox_4->value()),osg::inDegrees(ui.doubleSpinBox_5->value()),osg::inDegrees(ui.doubleSpinBox_6->value()));
	emit modified();
}

void ByyModelWidget::setScaleX( double value )
{
	modelList[ui.listWidget->currentRow()]->setScaleX(value);
	emit modified();
}

void ByyModelWidget::setScaleY( double value )
{
	modelList[ui.listWidget->currentRow()]->setScaleY(value);
	emit modified();
}

void ByyModelWidget::setScaleZ( double value )
{
	modelList[ui.listWidget->currentRow()]->setScaleZ(value);
	emit modified();
}

void ByyModelWidget::zoomto()
{
	int row=ui.listWidget->currentRow();

	if (row==-1)
	{
		return;
	}

	ByyModel* model=modelList[row];

	osgEarth::GeoPoint geoPoint=model->getModel()->getPosition();

	geoPoint.alt()+=model->getModel()->getBound().radius()*3;

	d_scene->changeViewpoint(geoPoint.vec3d());
}

void ByyModelWidget::remove()
{
	int row=ui.listWidget->currentRow();

	if (row==-1)
	{
		return;
	}
	int r=QMessageBox::warning(0,QString::fromLocal8Bit("提示"),QString::fromLocal8Bit("确认删除吗？"),QMessageBox::Yes|QMessageBox::No);

	if (r!=QMessageBox::Yes)
	{
		return;
	}

	d_root->removeChild(row);
	delete modelList.takeAt(row);
	delete ui.listWidget->takeItem(row);

	emit modified();
}

void ByyModelWidget::move()
{
	int row=ui.listWidget->currentRow();

	if (row==-1)
	{
		return;
	}

	ByyModel* model=modelList[row];

	ByyModelEditPlace *place=new ByyModelEditPlace(model->getModel(),d_scene);
	connect(place,SIGNAL(placeFinished(bool)),SLOT(moveFinish(bool)),Qt::QueuedConnection);

	d_scene->getViewer()->addEventHandler(place);

	setEnabled(false);
}

void ByyModelWidget::add( const QString& fileName )
{
	ByyModelPlace *modelPlace=new ByyModelPlace(fileName,d_scene);
	connect(modelPlace,SIGNAL(placePosition(const osgEarth::GeoPoint&)),SLOT(newModelPlacePosition(const osgEarth::GeoPoint&)),Qt::QueuedConnection);
	connect(modelPlace,SIGNAL(placeFinished()),SLOT(newModelPlaceFinished()),Qt::QueuedConnection);
	d_scene->getViewer()->addEventHandler(modelPlace);
}

void ByyModelWidget::newModelPlacePosition( const osgEarth::GeoPoint& geoPoint )
{
	ByyModelPlace *modelPlace=static_cast<ByyModelPlace*>(sender());
	QString modelFile=modelPlace->getModelFileName();

	std::string _referrer=d_scene->getMap()->getMapOptions().getConfig().referrer();

	osgEarth::Config conf("model");
	conf.setReferrer(_referrer);

	osgEarth::Config styleConf("style");
	//styleConf.setReferrer(_referrer);

	osgEarth::Config symbols("symbols");
	//symbols.setReferrer(_referrer);

	osgEarth::Config modelSymbolConf("model");
	//modelSymbolConf.setReferrer(_referrer);
	modelSymbolConf.add("url",QDir(QFileInfo(QString::fromStdString(_referrer)).path()).relativeFilePath(modelFile).toStdString());

	symbols.add(modelSymbolConf);
	styleConf.add(symbols);
	conf.add(styleConf);
	conf.add("name",QFileInfo(modelFile).fileName().toStdString());

	ByyModel* model=new ByyModel(d_scene->getMapNode(),conf);
	model->setPosition(geoPoint);

	addItem(model);

	//ui.listWidget->setCurrentRow(ui.listWidget->count()-1);

	emit modified();
}

void ByyModelWidget::newModelPlaceFinished()
{
	osg::ref_ptr<ByyModelPlace> modelPlace=static_cast<ByyModelPlace*>(sender());
	d_scene->getViewer()->removeEventHandler(modelPlace);
	modelPlace=NULL;
}

void ByyModelWidget::moveFinish( bool ok )
{
	osg::ref_ptr<ByyModelEditPlace> place=static_cast<ByyModelEditPlace*>(sender());
	d_scene->getViewer()->removeEventHandler(place);

	if (ok)
	{
		osgEarth::Annotation::ModelNode *modelNode=place->getModelNode();
		osgEarth::GeoPoint geoPoint=modelNode->getPosition();
		modelNode->setPosition(geoPoint);

		currentModelChanged(ui.listWidget->currentRow());

		emit modified();
	}

	setEnabled(true);

	place=NULL;
}

void ByyModelWidget::setBuilding( int checkState )
{
	myBuildingInfoWidget->setEnabled(checkState);
	ByyModel* model=modelList[ui.listWidget->currentRow()];
	model->setBuilding(checkState);

	emit modified();
}

void ByyModelWidget::setVideo( int checkState )
{
	ByyModel* model=modelList[ui.listWidget->currentRow()];
	model->setVideo(checkState);
	myBuildingInfoWidget->setEnabled(checkState);
	emit modified();
}

void ByyModelWidget::addItem( ByyModel* model )
{
	modelList.append(model);
	d_root->addChild(model->getModel());

	QListWidgetItem *item=new QListWidgetItem(model->name());
	ui.listWidget->addItem(item);

	std::cout<<"Add Model:"<<qPrintable(model->name())<<std::endl;
}

void ByyModelWidget::rename()
{
	int row=ui.listWidget->currentRow();
	if (row==-1)
	{
		return;
	}

	bool ok;
	QString text = QInputDialog::getText(this, QString::fromLocal8Bit("重命名"),
		QString::fromLocal8Bit("名称:"), QLineEdit::Normal,
		ui.listWidget->item(row)->text(), &ok);

	if (ok && !text.isEmpty())
	{
		ui.listWidget->item(row)->setText(text);
		modelList[row]->setName(text);
		emit modified();
	}
}

void ByyModelWidget::setEntityHeight( int checkState )
{
	ByyModel* model=modelList[ui.listWidget->currentRow()];
	model->setEntHeight(checkState==Qt::Checked);
	emit modified();
}

void ByyModelWidget::setEntityHeightValue( double value )
{
	ByyModel* model=modelList[ui.listWidget->currentRow()];
	model->setEntHeightValue(value);
	emit modified();
}

void ByyModelWidget::setLod( int checkState )
{
	ByyModel* model=modelList[ui.listWidget->currentRow()];
	model->setLOD(checkState==Qt::Checked);
	model->setLodRange(ui.myLodRange->value());
	emit modified();
}

void ByyModelWidget::setLodRange( double value )
{
	ByyModel* model=modelList[ui.listWidget->currentRow()];
	model->setLodRange(value);
	emit modified();
}

void ByyModelWidget::setTerrain( int checkState )
{
	ByyModel* model=modelList[ui.listWidget->currentRow()];
	model->setIsTerrain(checkState==Qt::Checked);
	emit modified();
}

void ByyModelWidget::blockAllSignals( bool bl )
{
	ui.checkBox->blockSignals(bl);
	ui.checkBox_2->blockSignals(bl);
	ui.checkBox_3->blockSignals(bl);
	ui.doubleSpinBox->blockSignals(bl);
	ui.doubleSpinBox_2->blockSignals(bl);
	ui.doubleSpinBox_3->blockSignals(bl);
	ui.doubleSpinBox_4->blockSignals(bl);
	ui.doubleSpinBox_5->blockSignals(bl);
	ui.doubleSpinBox_6->blockSignals(bl);
	ui.doubleSpinBox_7->blockSignals(bl);
	ui.doubleSpinBox_8->blockSignals(bl);
	ui.doubleSpinBox_9->blockSignals(bl);
	ui.doubleSpinBox_10->blockSignals(bl);
	ui.myLodCheckBox->blockSignals(bl);
	ui.myLodRange->blockSignals(bl);
	ui.terrainCheckBox->blockSignals(bl);
}

void ByyModelWidget::pickingModel( int index )
{
	ui.listWidget->setCurrentRow(index);
}

void ByyModelWidget::enableModelEdit( bool e )
{
	ui.myModelPropertyWidget->setVisible(e);
}

void ByyModelWidget::enableVideoEdit( bool e )
{
	myVideoEditDialog->setVisible(e);
}

void ByyModelWidget::enableInfoEdit( bool e )
{
	myBuildingInfoWidget->setVisible(e);
}
