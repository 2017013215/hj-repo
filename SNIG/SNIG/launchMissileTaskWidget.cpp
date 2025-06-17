#include "StdAfx.h"
#include "launchMissileTaskWidget.h"
#include "launchMissileTask.h"
#include <osgEarth/Terrain>
#include <osgEarth/MapNode>

ByyLaunchMissileTaskWidget::ByyLaunchMissileTaskWidget( ByyIG& ig,QWidget *parent /*= 0*/ ) 
	: ByyTaskEditorWidget(ig,parent)
	,myEventProcessor(this)
{
	ui.setupUi(this);

	QObject::connect(ui.pushButton, SIGNAL(clicked()), this, SLOT(accept()));
	QObject::connect(ui.pushButton_2, SIGNAL(clicked()), this, SLOT(reject()));

	ByyLocalObjectManager::EntityObjectMap& entitys=ig.core().localObjectManager().entityObjects();

	ByyLocalObjectManager::EntityObjectMap::iterator it=entitys.begin();

	QTreeWidgetItem *item=0;
	for (;it!=entitys.end();++it)
	{
		item=new QTreeWidgetItem;
		item->setText(0,it.value()->name());

		ui.treeWidget->addTopLevelItem(item);
	}

	ig.inputController().addEventProcessor(&myEventProcessor);
}

ByyLaunchMissileTaskWidget::~ByyLaunchMissileTaskWidget()
{
	myIG.inputController().removeEventProcessor(&myEventProcessor);
}

void ByyLaunchMissileTaskWidget::init( ByyTask* task )
{
	myTask=dynamic_cast<ByyLaunchMissileTask*>(task);

	ui.nameEdit->setText(myTask->name());
	ui.startTimeSpinBox->setValue(myTask->startTime());

	ui.targetAssignComboBox->setCurrentIndex(myTask->targetMode()-1);

	ui.lonEdit->setText(QString::number(myTask->targetPos().x(),'f',10));
	ui.latEdit->setText(QString::number(myTask->targetPos().y(),'f',10));
	ui.altEdit->setText(QString::number(myTask->targetPos().z(),'f',10));

	QTreeWidgetItem *item=0;

	QList<QTreeWidgetItem *> items=ui.treeWidget->findItems(myTask->targetObjName(),Qt::MatchExactly);

	if (items.size()>0)
	{
		ui.treeWidget->setCurrentItem(items[0]);
	}
}

ByyTask* ByyLaunchMissileTaskWidget::task()
{
	return myTask;
}

void ByyLaunchMissileTaskWidget::accept()
{
	myTask->setName(ui.nameEdit->text());
	myTask->setStartTime(ui.startTimeSpinBox->value());

	myTask->setTargetMode((ByyLaunchMissileTask::TargetMode)(ui.targetAssignComboBox->currentIndex()+1));

	QTreeWidgetItem* item=ui.treeWidget->currentItem();

	if (item)
	{
		myTask->setTargetObjName(item->text(0));
	}

	osg::Vec3d pos;
	pos.x()=ui.lonEdit->text().toDouble();
	pos.y()=ui.latEdit->text().toDouble();
	pos.z()=ui.altEdit->text().toDouble();

	myTask->setTargetPos(pos);

	close();
	emit accepted();
}

void ByyLaunchMissileTaskWidget::reject()
{
	close();
	emit rejected();
}

void ByyLaunchMissileTaskWidget::mouseClicked( int button,int x,int y,osg::View *view )
{
	osgEarth::MapNode *mapNode=myIG.terrain()->mapNode();

	if (!mapNode) return;

	osg::Vec3d world;
	if ( mapNode->getTerrain()->getWorldCoordsUnderMouse(view, x, y, world) )
	{
		static osgEarth::GeoPoint map;

		map.fromWorld( mapNode->getMapSRS(), world );

		ui.lonEdit->setText(QString::number(map.x(),'f',8));
		ui.latEdit->setText(QString::number(map.y(),'f',8));
		ui.altEdit->setText(QString::number(map.z(),'f',8));
	}
}

bool ByyLaunchMissileTaskWidget::MyEventProcessor::processEvent( const osgGA::GUIEventAdapter& ea,ByyChannel &channel )
{
	switch(ea.getEventType())
	{
	case osgGA::GUIEventAdapter::PUSH:
		myXdown=ea.getX();
		myYdown=ea.getY();
		break;
	case osgGA::GUIEventAdapter::RELEASE:
		if (ea.getX()==myXdown&&myYdown==ea.getY())
		{
			myWidget->mouseClicked(0,ea.getX(),ea.getY(),channel.view());
		}
		break;
	}

	return false;
}
