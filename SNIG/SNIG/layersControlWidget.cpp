#include "stdafx.h"
#include "layersControlWidget.h"
#include <osgDB/FileNameUtils>

ByyLayersControlWidget::ByyLayersControlWidget( ByyIG& ig,QWidget * parent /*= 0*/, Qt::WindowFlags f /*= 0*/ )
	: QWidget(parent,f),
	myIG(ig),
	ui(new Ui::ByyLayersControlWidget)
{
	ui->setupUi(this);
	createWidgets();
	refresh();
}

ByyLayersControlWidget::~ByyLayersControlWidget()
{

}

void ByyLayersControlWidget::createWidgets()
{

	imageTopItem=new QTreeWidgetItem(ui->treeWidget);
	imageTopItem->setText(0,QString::fromLocal8Bit("图层"));
	imageTopItem->setFlags(Qt::ItemIsEnabled);

	elevationTopItem=new QTreeWidgetItem(ui->treeWidget);
	elevationTopItem->setText(0,QString::fromLocal8Bit("高程"));
	elevationTopItem->setFlags(Qt::ItemIsEnabled);

	modelTopItem=new QTreeWidgetItem(ui->treeWidget);
	modelTopItem->setText(0,QString::fromLocal8Bit("矢量"));
	modelTopItem->setFlags(Qt::ItemIsEnabled);

	ui->treeWidget->installEventFilter(this);

	connect(ui->treeWidget,SIGNAL(itemChanged(QTreeWidgetItem*,int)),this,SLOT(itemChanged(QTreeWidgetItem*,int)));

	moveUpAction=new QAction(QIcon("../data/icons/earthtool/moveup.png"),QString::fromLocal8Bit("上移"),this);
	connect(moveUpAction, SIGNAL(triggered()), this, SLOT(moveUp()));

	moveDownAction=new QAction(QIcon("../data/icons/earthtool/movedown.png"),QString::fromLocal8Bit("下移"),this);
	connect(moveDownAction, SIGNAL(triggered()), this, SLOT(moveDown()));

	removeAction=new QAction(QIcon("../data/icons/earthtool/movedown.png"),QString::fromLocal8Bit("删除"),this);
	connect(removeAction, SIGNAL(triggered()), this, SLOT(remove()));

	ui->pushButton->setIcon(QIcon(ICONPATH+"import.png"));
	ui->pushButton_2->setIcon(QIcon(ICONPATH+"import.png"));
	ui->pushButton_3->setIcon(QIcon(ICONPATH+"remove.png"));

	removeAction->setIcon(QIcon(ICONPATH+"remove.png"));

	connect(ui->pushButton,SIGNAL(clicked()),SLOT(addImageToggled()));
	connect(ui->pushButton_2,SIGNAL(clicked()),SLOT(addElevationToggled()));
	connect(ui->pushButton_3,SIGNAL(clicked()),SLOT(remove()));
}

void ByyLayersControlWidget::refresh()
{
	ui->treeWidget->blockSignals(true);
	
	qDeleteAll(imageTopItem->takeChildren());
	qDeleteAll(elevationTopItem->takeChildren());
	qDeleteAll(modelTopItem->takeChildren());

	layersMap.clear();

	osgEarth::Map* _map=myIG.terrain()->getMap();

	if (_map)
	{
		osgEarth::ImageLayerVector out_layers;
		_map->getImageLayers(out_layers);

		for (int i=0;i!=out_layers.size();++i)
		{
			addImageLayer(out_layers[i].get());
		}
		imageTopItem->setExpanded(true);

		osgEarth::ElevationLayerVector out_layers1;
		_map->getElevationLayers(out_layers1);

		for (int i=0;i!=out_layers1.size();++i)
		{
			addElevationLayer(out_layers1[i].get());
		}
		elevationTopItem->setExpanded(true);

		osgEarth::ModelLayerVector out_layers2;
		_map->getModelLayers(out_layers2);

		for (int i=0;i!=out_layers2.size();++i)
		{
			QTreeWidgetItem *modelItem=new QTreeWidgetItem(modelTopItem);
			modelItem->setCheckState(0,Qt::Checked);
			modelItem->setText(0,QString::fromLocal8Bit(out_layers2[i]->getName().c_str()));

			layersMap.insert(modelItem,out_layers2[i].get());
		}
		modelTopItem->setExpanded(true);
	}
	ui->treeWidget->blockSignals(false);
}

void ByyLayersControlWidget::addImageToggled()
{
	QStringList files = QFileDialog::getOpenFileNames(this, QString::fromLocal8Bit("添加图像"),
		"../data/imageLayer",
		tr("GeoTiff (*.tif)"));

	if (files.isEmpty())
		return;

	std::string _referrer=myIG.terrain()->getMap()->getMapOptions().getConfig().referrer();

	myIG.terrain()->getMap()->beginUpdate();

	foreach(const QString& filePath,files)
	{
		if (!filePath.isEmpty())
		{
			QString imageAbsolutePath=QDir("../data/imageLayer").absolutePath();
			bool inImagePath=QDir("../data/imageLayer").exists(filePath);
			osgEarth::Config conf("image");
			conf.setReferrer(_referrer);

			conf.add("url",(inImagePath?QDir("../data/earthFile").relativeFilePath(filePath):filePath).toStdString());

			std::string fileName = osgDB::getSimpleFileName(filePath.toStdString());

			conf.add("driver","gdal");
			conf.add("name",fileName);

			osgEarth::ImageLayerOptions imageLayerOptions(conf);

			osg::ref_ptr<osgEarth::ImageLayer> newLayer = new osgEarth::ImageLayer(imageLayerOptions);

			myIG.terrain()->getMap()->addImageLayer(newLayer);

			addImageLayer(newLayer);
		}
	}
	myIG.terrain()->getMap()->endUpdate();

	//sceneController->earth()->setChanged();
}

void ByyLayersControlWidget::addElevationToggled()
{
	QStringList files = QFileDialog::getOpenFileNames(this, QString::fromLocal8Bit("添加高程"),
		"../data/elevationLayer",
		tr("GeoTiff (*.tif)"));

	if (files.isEmpty())
		return;

	std::string _referrer=myIG.terrain()->getMap()->getMapOptions().getConfig().referrer();

	myIG.terrain()->getMap()->beginUpdate();

	foreach(const QString& filePath,files)
	{
		if (!filePath.isEmpty())
		{
			QString elevationAbsolutePath=QDir("../data/ElevationLayer").absolutePath();

			bool inElevationPath=QDir("../data/ElevationLayer").exists(filePath);

			osgEarth::Config conf("elevation");

			conf.setReferrer(_referrer);

			conf.add("url",(inElevationPath?QDir("../data/earthFile").relativeFilePath(filePath):filePath).toStdString());

			std::string fileName = osgDB::getSimpleFileName(filePath.toStdString());
			conf.add("name",fileName);
			conf.add("driver","gdal");

			osg::ref_ptr<osgEarth::ElevationLayer> newLayer = new osgEarth::ElevationLayer(osgEarth::ElevationLayerOptions(conf));

			myIG.terrain()->getMap()->addElevationLayer(newLayer);

			addElevationLayer(newLayer);
		}
	}

	myIG.terrain()->getMap()->endUpdate();
	//sceneController->earth()->setChanged();
}

void ByyLayersControlWidget::addImageLayer( osgEarth::ImageLayer* layer )
{
	QTreeWidgetItem *imageItem=new QTreeWidgetItem(imageTopItem);
	imageItem->setCheckState(0,Qt::Checked);
	imageItem->setText(0,QString::fromLocal8Bit(layer->getName().c_str()));

	layersMap.insert(imageItem,layer);
}

void ByyLayersControlWidget::addElevationLayer( osgEarth::ElevationLayer* layer )
{
	QTreeWidgetItem *elevationItem=new QTreeWidgetItem(elevationTopItem);
	elevationItem->setCheckState(0,Qt::Checked);
	elevationItem->setText(0,QString::fromLocal8Bit(layer->getName().c_str()));

	layersMap.insert(elevationItem,layer);
}

void ByyLayersControlWidget::itemChanged( QTreeWidgetItem * item,int column )
{
	if (layersMap.contains(item))
	{
		if (item->parent()==modelTopItem)
		{
			osgEarth::ModelLayer* modelLayer=dynamic_cast<osgEarth::ModelLayer*>(layersMap[item]);

			modelLayer->setVisible(item->checkState(0)==Qt::Checked);
		}
		else
		{
			osgEarth::TerrainLayer* terrainLayer=dynamic_cast<osgEarth::TerrainLayer*>(layersMap[item]);

			terrainLayer->setVisible(item->checkState(0)==Qt::Checked);
		}
	}	
}

bool ByyLayersControlWidget::eventFilter( QObject *obj, QEvent *event )
{
	if (obj==ui->treeWidget&&event->type()==QEvent::ContextMenu)
	{
		QTreeWidgetItem *currentItem=ui->treeWidget->itemAt(ui->treeWidget->viewport()->mapFromGlobal(QCursor::pos()));
		QTreeWidgetItem *currentParent=currentItem?currentItem->parent():NULL;

		if (currentParent)
		{
			QMenu *menu=new QMenu(this);

			menu->setAttribute(Qt::WA_DeleteOnClose);

			menu->addAction(moveUpAction);
			menu->addAction(moveDownAction);
			menu->addSeparator();
			menu->addAction(removeAction);
			moveUpAction->setEnabled(currentParent->indexOfChild(currentItem)!=0);
			moveDownAction->setEnabled(currentParent->indexOfChild(currentItem)!=currentParent->childCount()-1);
			menu->popup(QCursor::pos());
		}
		return true;
	}
	else
		return QObject::eventFilter(obj, event);
}

void ByyLayersControlWidget::moveUp()
{
	QTreeWidgetItem *currentItem=ui->treeWidget->currentItem();

	if (!layersMap.contains(currentItem))
	{
		return;
	}

	osgEarth::Map* _map=myIG.terrain()->getMap();

	QTreeWidgetItem *currentParent=currentItem->parent();

	int index=currentParent->indexOfChild(currentItem);
	currentParent->removeChild(currentItem);
	currentParent->insertChild(index-1,currentItem);
	ui->treeWidget->setCurrentItem(currentItem);

	osgEarth::Layer* layer=layersMap[currentItem];

	if (currentParent==imageTopItem)
	{
		osgEarth::ImageLayer *imageLayer=dynamic_cast<osgEarth::ImageLayer*>(layer);
		if (imageLayer)
		{
			_map->moveImageLayer(imageLayer,index-1);
		}
	}
	else if(currentParent==elevationTopItem)
	{
		osgEarth::ElevationLayer *elevationLayer=dynamic_cast<osgEarth::ElevationLayer*>(layer);
		if (elevationLayer)
		{
			_map->moveElevationLayer(elevationLayer,index-1);
		}
	}
	else if (currentParent==modelTopItem)
	{
		osgEarth::ModelLayer *modelLayer=dynamic_cast<osgEarth::ModelLayer*>(layer);
		if (modelLayer)
		{
			_map->moveModelLayer(modelLayer,index-1);
		}
	}
//	sceneController->earth()->setChanged();
}

void ByyLayersControlWidget::moveDown()
{
	QTreeWidgetItem *currentItem=ui->treeWidget->currentItem();

	if (!layersMap.contains(currentItem))
	{
		return;
	}

	osgEarth::Map* _map=myIG.terrain()->getMap();

	QTreeWidgetItem *currentParent=currentItem->parent();

	int index=currentParent->indexOfChild(currentItem);
	currentParent->removeChild(currentItem);
	currentParent->insertChild(index+1,currentItem);
	ui->treeWidget->setCurrentItem(currentItem);

	osgEarth::Layer* layer=layersMap[currentItem];

	if (currentParent==imageTopItem)
	{
		osgEarth::ImageLayer *imageLayer=dynamic_cast<osgEarth::ImageLayer*>(layer);
		if (imageLayer)
		{
			_map->moveImageLayer(imageLayer,index+1);
		}
	}
	else if(currentParent==elevationTopItem)
	{
		osgEarth::ElevationLayer *elevationLayer=dynamic_cast<osgEarth::ElevationLayer*>(layer);
		if (elevationLayer)
		{
			_map->moveElevationLayer(elevationLayer,index+1);
		}
	}
	else if (currentParent==modelTopItem)
	{
		osgEarth::ModelLayer *modelLayer=dynamic_cast<osgEarth::ModelLayer*>(layer);
		if (modelLayer)
		{
			_map->moveModelLayer(modelLayer,index+1);
		}
	}
	//sceneController->earth()->setChanged();
}

void ByyLayersControlWidget::remove()
{
	QTreeWidgetItem* item=ui->treeWidget->currentItem();

	if (!layersMap.contains(item))
		return;

	QTreeWidgetItem* parent=item?item->parent():NULL;

	if (parent)
	{
		int r=QMessageBox::warning(this,QString::fromLocal8Bit("提示"),QString::fromLocal8Bit("是否移除所选地形？"),QMessageBox::Yes|QMessageBox::No);

		if (r==QMessageBox::No)
			return;

	}

	if (parent==imageTopItem)
	{
		myIG.terrain()->getMap()->removeImageLayer(dynamic_cast<osgEarth::ImageLayer*>(layersMap[item]));
	}
	else if (parent==elevationTopItem)
	{
		myIG.terrain()->getMap()->removeElevationLayer(dynamic_cast<osgEarth::ElevationLayer*>(layersMap[item]));
	}
	else if (parent==modelTopItem)
	{
		myIG.terrain()->getMap()->removeModelLayer(dynamic_cast<osgEarth::ModelLayer*>(layersMap[item]));
	}

	layersMap.remove(item);

	parent->removeChild(item);

	//sceneController->earth()->setChanged();
}
