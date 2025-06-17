#include "stdafx.h"
#include "imageLayerWidget.h"

#include <osgDB/FileNameUtils>
#include <osgEarthDrivers/gdal/GDALOptions>
#include "variantmanager.h"
#include "qtpropertybrowser-2.5_1/qtvariantproperty.h"
#include "qtpropertybrowser-2.5_1/qttreepropertybrowser.h"
#include "sceneController.h"

ByyImageLayerWidget::ByyImageLayerWidget(ByySceneController *scene,QWidget *parent)
	: ByyAbstractManageWidget(scene,parent)
{
	
}

ByyImageLayerWidget::~ByyImageLayerWidget()
{

}

void ByyImageLayerWidget::init()
{
	release();
	d_map=d_scene->getMap();

	osgEarth::ImageLayerVector out_layers;
	d_map->getImageLayers(out_layers);

	for (int i=0;i!=out_layers.size();++i)
	{
		addImageLayerToWidget(out_layers[i]);
	}
}

void ByyImageLayerWidget::addImageLayerToWidget( osgEarth::ImageLayer* imageLayer )
{
	variantManager->blockSignals(true);

	QtProperty *topItem = variantManager->addProperty(QtVariantPropertyManager::groupTypeId(),
		QString::fromLocal8Bit("Í¼Ïñ"));

	QtVariantProperty *nameItem = variantManager->addProperty(QVariant::String, QString::fromLocal8Bit("Ãû³Æ"));
	nameItem->setValue(QString::fromStdString(imageLayer->getName()));
	topItem->addSubProperty(nameItem);

	QtVariantProperty *driverItem = variantManager->addProperty(QVariant::String, QString::fromLocal8Bit("Çý¶¯"));
	driverItem->setValue(QString::fromStdString(imageLayer->getImageLayerOptions().driver()->getDriver()));
	driverItem->setEnabled(false);
	topItem->addSubProperty(driverItem);

	QtVariantProperty *urlItem = variantManager->addProperty(QVariant::String, QString::fromLocal8Bit("URL"));
	urlItem->setValue(QString::fromStdString(imageLayer->getImageLayerOptions().driver()->getConfig().child("url").value()));
	urlItem->setEnabled(false);
	topItem->addSubProperty(urlItem);

	QtVariantProperty *minLevelItem = variantManager->addProperty(QVariant::Int, QString::fromLocal8Bit("Min-Level"));
	minLevelItem->setValue(imageLayer->getImageLayerOptions().minLevel().get());
	topItem->addSubProperty(minLevelItem);

	QtVariantProperty *maxLevelItem = variantManager->addProperty(QVariant::Int, QString::fromLocal8Bit("Max-Level"));
	maxLevelItem->setValue(imageLayer->getImageLayerOptions().maxLevel().get());
	topItem->addSubProperty(maxLevelItem);

	variantEditor->addProperty(topItem);

	variantManager->blockSignals(false);
}

void ByyImageLayerWidget::moveUp()
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
			d_map->moveImageLayer(d_map->getImageLayerAt(index),index-1);
			emit modified();
		}
	}
}

void ByyImageLayerWidget::moveDown()
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
			d_map->moveImageLayer(d_map->getImageLayerAt(index),index+1);
			emit modified();
		}
	}
}

void ByyImageLayerWidget::removeToggle()
{
	if (variantEditor->currentItem()->parent()==NULL)
	{
		QList<QtBrowserItem *> topItems=variantEditor->topLevelItems();
		int index=topItems.indexOf(variantEditor->currentItem());
		if (index!=-1)
		{
			QtProperty *property=variantEditor->currentItem()->property();
			variantEditor->removeProperty(property);
			d_map->removeImageLayer(d_map->getImageLayerAt(index));
			emit modified();
		}
	}
}

void ByyImageLayerWidget::release()
{
	variantEditor->clear();
}

void ByyImageLayerWidget::valueChanged( QtProperty *property, const QVariant &val )
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
				d_map->getImageLayerAt(i)->setName(val.toString().toStdString());
			}
			else if (index==3&&val.toInt()>0)
			{
				osgEarth::optional<unsigned>& minLevel=const_cast<osgEarth::optional<unsigned>&>(d_map->getImageLayerAt(i)->getImageLayerOptions().minLevel());
				minLevel=val.toInt();
			}
			else if (index==4&&val.toInt()<30)
			{
				osgEarth::optional<unsigned>& maxLevel=const_cast<osgEarth::optional<unsigned>&>(d_map->getImageLayerAt(i)->getImageLayerOptions().maxLevel());
				maxLevel=val.toInt();
			}
		}
	}
	emit modified();
}

void ByyImageLayerWidget::add( const QStringList& filePaths )
{
	d_map->beginUpdate();

	std::string _referrer=d_scene->getMap()->getMapOptions().getConfig().referrer();

	for (int i = 0; i < filePaths.size(); i++)
	{
		QString filePath = filePaths.at(i);

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

			d_map->addImageLayer(newLayer);


			addImageLayerToWidget(newLayer);
			emit modified();
		}
	}

	d_map->endUpdate();
}
