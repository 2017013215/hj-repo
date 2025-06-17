#include "stdafx.h"
#include "elevationLayerWidget.h"

#include <osgDB/FileNameUtils>
#include <osgEarthDrivers/gdal/GDALOptions>
#include "variantmanager.h"
#include "qtpropertybrowser-2.5_1/qtvariantproperty.h"
#include "qtpropertybrowser-2.5_1/qttreepropertybrowser.h"
#include "sceneController.h"

ByyElevationLayerWidget::ByyElevationLayerWidget(ByySceneController *scene,QWidget *parent)
	: ByyAbstractManageWidget(scene,parent)
{
	
}

ByyElevationLayerWidget::~ByyElevationLayerWidget()
{

}

void ByyElevationLayerWidget::init()
{
	release();
	d_map=d_scene->getMap();

	osgEarth::ElevationLayerVector out_layers;
	d_map->getElevationLayers(out_layers);

	for (int i=0;i!=out_layers.size();++i)
	{
		addElevationLayerToWidget(out_layers[i]);
	}
}

void ByyElevationLayerWidget::addElevationLayerToWidget( osgEarth::ElevationLayer* elevationLayer )
{
	variantManager->blockSignals(true);

	QtProperty *topItem = variantManager->addProperty(QtVariantPropertyManager::groupTypeId(),
		QString::fromLocal8Bit("高程"));

	QtVariantProperty *nameItem = variantManager->addProperty(QVariant::String, QString::fromLocal8Bit("名称"));
	nameItem->setValue(QString::fromStdString(elevationLayer->getName()));
	topItem->addSubProperty(nameItem);

	QtVariantProperty *driverItem = variantManager->addProperty(QVariant::String, QString::fromLocal8Bit("驱动"));
	driverItem->setValue(QString::fromStdString(elevationLayer->getElevationLayerOptions().driver()->getDriver()));
	driverItem->setEnabled(false);
	topItem->addSubProperty(driverItem);

	QtVariantProperty *urlItem = variantManager->addProperty(QVariant::String, QString::fromLocal8Bit("URL"));
	urlItem->setValue(QString::fromStdString(elevationLayer->getElevationLayerOptions().driver()->getConfig().child("url").value()));
	urlItem->setEnabled(false);
	topItem->addSubProperty(urlItem);


	variantEditor->addProperty(topItem);

	variantManager->blockSignals(false);
}

void ByyElevationLayerWidget::moveUp()
{
	if (variantEditor->currentItem()->parent()==NULL)
	{
		QList<QtBrowserItem *> topItems=variantEditor->topLevelItems();
		int index=topItems.indexOf(variantEditor->currentItem());
		if (index!=-1&&index!=0)
		{
			QtProperty *prevProperty=topItems[index-1]->property();
			QtProperty *property=variantEditor->currentItem()->property();
			variantEditor->removeProperty(prevProperty);
			variantEditor->insertProperty(prevProperty,property);
			d_map->moveElevationLayer(d_map->getElevationLayerAt(index),index-1);
			emit modified();
		}
	}
}

void ByyElevationLayerWidget::moveDown()
{
	if (variantEditor->currentItem()->parent()==NULL)
	{
		QList<QtBrowserItem *> topItems=variantEditor->topLevelItems();
		int index=topItems.indexOf(variantEditor->currentItem());
		if (index!=-1&&index!=topItems.count()-1)
		{
			QtProperty *nextProperty=topItems[index+1]->property();
			QtProperty *property=variantEditor->currentItem()->property();
			variantEditor->removeProperty(property);
			variantEditor->insertProperty(property,nextProperty);
			d_map->moveElevationLayer(d_map->getElevationLayerAt(index),index+1);
			emit modified();
		}
	}
}

void ByyElevationLayerWidget::removeToggle()
{
	if (variantEditor->currentItem()->parent()==NULL)
	{
		QList<QtBrowserItem *> topItems=variantEditor->topLevelItems();
		int index=topItems.indexOf(variantEditor->currentItem());
		if (index!=-1)
		{
			QtProperty *property=variantEditor->currentItem()->property();
			variantEditor->removeProperty(property);
			d_map->removeElevationLayer(d_map->getElevationLayerAt(index));
			emit modified();
		}
	}
}

void ByyElevationLayerWidget::release()
{
	variantEditor->clear();
}

void ByyElevationLayerWidget::valueChanged( QtProperty *property, const QVariant &val )
{
	QList<QtBrowserItem *> topItems=variantEditor->topLevelItems();
	for (int i=0;i!=topItems.count();++i)
	{
		QList<QtProperty *> subProp=topItems[i]->property()->subProperties();
		if (subProp.contains(property))
		{
			int index=subProp.indexOf(property);
			if (index==0)
			{
				d_map->getElevationLayerAt(i)->setName(val.toString().toStdString());
			}
			else if (index==3&&val.toInt()>0)
			{
				osgEarth::optional<unsigned>& minLevel=const_cast<osgEarth::optional<unsigned>&>(d_map->getElevationLayerAt(i)->getElevationLayerOptions().minLevel());
				minLevel=val.toInt();
			}
			else if (index==4&&val.toInt()<30)
			{
				osgEarth::optional<unsigned>& maxLevel=const_cast<osgEarth::optional<unsigned>&>(d_map->getElevationLayerAt(i)->getElevationLayerOptions().maxLevel());
				maxLevel=val.toInt();
			}
		}
	}
	emit modified();
}

void ByyElevationLayerWidget::add( const QStringList& filePaths )
{
	d_map->beginUpdate();

	std::string _referrer=d_scene->getMap()->getMapOptions().getConfig().referrer();

	for (int i = 0; i < filePaths.size(); i++)
	{
		QString filePath = filePaths.at(i);
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

			d_map->addElevationLayer(newLayer);


			addElevationLayerToWidget(newLayer);
			emit modified();
		}
	}

	d_map->endUpdate();
}
