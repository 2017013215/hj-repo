#include "stdafx.h"
#include "viewpointWidget.h"

#include <osgDB/FileNameUtils>
#include <osgEarthDrivers/gdal/GDALOptions>
#include "variantmanager.h"
#include "qtvariantproperty.h"
#include "qttreepropertybrowser.h"
#include "sceneController.h"

ByyViewpointWidget::ByyViewpointWidget(ByySceneController *scene,QWidget *parent)
	: ByyAbstractManageWidget(scene,parent)
{
	gotoAction=new QAction(QString::fromLocal8Bit("前往"),this);
	connect(gotoAction, SIGNAL(triggered()), this, SLOT(gotoToggled()));
}

ByyViewpointWidget::~ByyViewpointWidget()
{

}

void ByyViewpointWidget::init()
{
	release();
	d_map=d_scene->getMap();
	d_mapNode=d_scene->getMapNode();

	osgEarth::Config& externals=d_mapNode->externalConfig();
	osgEarth::Config viewpointsConf=externals.child("viewpoints");
	const osgEarth::ConfigSet& children = viewpointsConf.children();

	for( osgEarth::ConfigSet::const_iterator i = children.begin(); i != children.end(); ++i )
	{
		addViewpointToWidget(*i);
	}
}

void ByyViewpointWidget::addViewpointToWidget( const osgEarth::Config& conf )
{
	variantManager->blockSignals(true);

	QtProperty *topItem = variantManager->addProperty(QtVariantPropertyManager::groupTypeId(),
		QString::fromLocal8Bit("视点"));

	QtVariantProperty *nameItem = variantManager->addProperty(QVariant::String, QString::fromLocal8Bit("名称"));
	nameItem->setValue(QString::fromUtf8(conf.value("name").c_str()));
	topItem->addSubProperty(nameItem);

	QtVariantProperty *longItem = variantManager->addProperty(QVariant::Double, QString::fromLocal8Bit("经度"));
	longItem->setValue(QString::fromStdString(conf.value("long")).toDouble());
	topItem->addSubProperty(longItem);

	QtVariantProperty *latItem = variantManager->addProperty(QVariant::Double, QString::fromLocal8Bit("纬度"));
	latItem->setValue(QString::fromStdString(conf.value("lat")).toDouble());
	topItem->addSubProperty(latItem);

	QtVariantProperty *altItem = variantManager->addProperty(QVariant::Double, QString::fromLocal8Bit("高度"));
	altItem->setValue(QString::fromStdString(conf.value("range")).toDouble());
	topItem->addSubProperty(altItem);

	QtVariantProperty *headingItem = variantManager->addProperty(QVariant::Double, QString::fromLocal8Bit("Heading"));
	headingItem->setValue(QString::fromStdString(conf.value("heading")).toDouble());
	topItem->addSubProperty(headingItem);

	QtVariantProperty *pitchItem = variantManager->addProperty(QVariant::Double, QString::fromLocal8Bit("Pitch"));
	pitchItem->setValue(QString::fromStdString(conf.value("pitch")).toDouble());
	topItem->addSubProperty(pitchItem);

	QtVariantProperty *heightItem = variantManager->addProperty(QVariant::Double, QString::fromLocal8Bit("Height"));
	heightItem->setValue(QString::fromStdString(conf.value("height")).toDouble());
	topItem->addSubProperty(heightItem);


	variantEditor->addProperty(topItem);

	variantManager->blockSignals(false);
}

void ByyViewpointWidget::moveUp()
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
			osgEarth::Config& externals=d_mapNode->externalConfig();
			osgEarth::Config* viewpointsConf=externals.mutable_child("viewpoints");
			osgEarth::ConfigSet& children = const_cast<osgEarth::ConfigSet&>(viewpointsConf->children());
			int i=0;
			for( osgEarth::ConfigSet::iterator it = children.begin(); it != children.end(); ++it )
			{
				if (i==index)
				{
					osgEarth::Config conf=*it;
					it=children.erase(it);
					--it;
					children.insert(it,conf);
					break;
				}
				++i;
			}
			emit modified();
		}
	}
}

void ByyViewpointWidget::moveDown()
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
			osgEarth::Config& externals=d_mapNode->externalConfig();
			osgEarth::Config* viewpointsConf=externals.mutable_child("viewpoints");
			osgEarth::ConfigSet& children = const_cast<osgEarth::ConfigSet&>(viewpointsConf->children());
			int i=0;
			for( osgEarth::ConfigSet::iterator it = children.begin(); it != children.end(); ++it )
			{
				if (i==index)
				{
					osgEarth::Config conf=*it;
					it=children.erase(it);
					++it;
					children.insert(it,conf);
					break;
				}
				++i;
			}
			emit modified();
		}
	}
}

void ByyViewpointWidget::removeToggle()
{
	if (variantEditor->currentItem()->parent()==NULL)
	{
		QList<QtBrowserItem *> topItems=variantEditor->topLevelItems();
		int index=topItems.indexOf(variantEditor->currentItem());
		QtProperty *property=variantEditor->currentItem()->property();
		variantEditor->removeProperty(property);
		if (index!=-1)
		{
			osgEarth::Config& externals=d_mapNode->externalConfig();
			osgEarth::Config* viewpointsConf=externals.mutable_child("viewpoints");
			osgEarth::ConfigSet& children = const_cast<osgEarth::ConfigSet&>(viewpointsConf->children());
			int i=0;
			for( osgEarth::ConfigSet::iterator it = children.begin(); it != children.end(); ++it )
			{
				if (i==index)
				{
					children.erase(it);
					break;
				}
				++i;
			}
			emit modified();
		}
	}
}

void ByyViewpointWidget::release()
{
	variantEditor->clear();
}

void ByyViewpointWidget::valueChanged( QtProperty *property, const QVariant &val )
{
	QList<QtBrowserItem *> topItems=variantEditor->topLevelItems();
	for (int i=0;i!=topItems.count();++i)
	{
		QList<QtProperty *> subProp=topItems[i]->property()->subProperties();
		if (subProp.contains(property))
		{
			osgEarth::Config& externals=d_mapNode->externalConfig();
			osgEarth::Config* viewpointsConf=externals.mutable_child("viewpoints");
			osgEarth::ConfigSet& children = const_cast<osgEarth::ConfigSet&>(viewpointsConf->children());
			int j=0;
			for( osgEarth::ConfigSet::iterator it = children.begin(); it != children.end(); ++it )
			{
				if (j==i)
				{
					int index=subProp.indexOf(property);
					if (index==0)
					{
						(*it).set("name",val.toString().toUtf8().data());
					}
					else if (index==1)
					{
						(*it).set("long",val.toDouble());
					}
					else if (index==2)
					{
						(*it).set("lat",val.toDouble());
					}
					else if (index==3)
					{
						(*it).set("alt",val.toDouble());
					}
					else if (index==4)
					{
						(*it).set("heading",val.toDouble());
					}
					else if (index==5)
					{
						(*it).set("pitch",val.toDouble());
					}
					else if (index==6)
					{
						(*it).set("height",val.toDouble());
					}
					break;
				}
				++j;
			}
		}
	}
	emit modified();
}

void ByyViewpointWidget::add( const osgEarth::Viewpoint& viewPoint )
{
	osgEarth::Config conf=viewPoint.getConfig();
	conf.remove("srs");
	addViewpointToWidget(conf);
	osgEarth::Config& externals=d_mapNode->externalConfig();
	osgEarth::Config* viewpointsConf=externals.mutable_child("viewpoints");
	if (viewpointsConf)
	{
		viewpointsConf->add(conf);
	}
	else
	{
		osgEarth::Config newConf("viewpoints");
		newConf.add(conf);
		externals.add(newConf);
	}
	
}

bool ByyViewpointWidget::eventFilter( QObject *obj, QEvent *event )
{
	if (obj==variantEditor&&event->type()==QEvent::ContextMenu)
	{
		if (variantEditor->currentItem()->parent()==NULL)
		{
			QMenu popMenu;
			popMenu.addAction(gotoAction);
			popMenu.addSeparator();
			popMenu.addAction(moveUpAction);
			popMenu.addAction(moveDownAction);
			popMenu.addSeparator();
			popMenu.addAction(removeAction);
			popMenu.exec(QCursor::pos());
		}

		return true;
	}
	else
		return QObject::eventFilter(obj, event);
}

void ByyViewpointWidget::gotoToggled()
{
	if (variantEditor->currentItem()->parent()==NULL)
	{
		QList<QtBrowserItem *> topItems=variantEditor->topLevelItems();
		int index=topItems.indexOf(variantEditor->currentItem());
		if (index!=-1)
		{
			osgEarth::Config& externals=d_mapNode->externalConfig();
			osgEarth::Config* viewpointsConf=externals.mutable_child("viewpoints");
			osgEarth::ConfigSet& children = const_cast<osgEarth::ConfigSet&>(viewpointsConf->children());
			int i=0;
			for( osgEarth::ConfigSet::iterator it = children.begin(); it != children.end(); ++it )
			{
				if (i==index)
				{
					d_scene->changeViewpoint(osgEarth::Viewpoint(*it));
					break;
				}
				++i;
			}
		}
	}
}
