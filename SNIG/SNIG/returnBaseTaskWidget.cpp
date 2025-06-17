#include "StdAfx.h"
#include "returnBaseTaskWidget.h"
#include "returnBaseTask.h"
#include <osgEarth/Terrain>
#include <osgEarth/MapNode>

ByyReturnBaseTaskWidget::ByyReturnBaseTaskWidget( ByyIG& ig,QWidget *parent /*= 0*/ )
	: ByyTaskEditorWidget(ig,parent)
	,myTask(NULL)
	,myEventProcessor(this)
{
	ui.setupUi(this);

	ByyLocalObjectManager::EntityObjectMap& entitys=ig.core().localObjectManager().entityObjects();

	ByyLocalObjectManager::EntityObjectMap::iterator it=entitys.begin();

	for (;it!=entitys.end();++it)
	{
		ui.selectBaseEntComboBox->addItem(it.value()->name());
	}

	QObject::connect(ui.pushButton, SIGNAL(clicked()), this, SLOT(accept()));
	QObject::connect(ui.pushButton_2, SIGNAL(clicked()), this, SLOT(reject()));
}

ByyReturnBaseTaskWidget::~ByyReturnBaseTaskWidget()
{

}

void ByyReturnBaseTaskWidget::init( ByyTask* task )
{
	myTask=dynamic_cast<ByyReturnBaseTask*>(task);

	ui.nameEdit->setText(myTask->name());
	ui.startTimeSpinBox->setValue(myTask->startTime());

	ui.myAssignBaseTypeComboBox->setCurrentIndex(myTask->getAssignBaseType());

	ui.lonEdit->setText(QString::number(myTask->getBasePos().x(),'f',10));
	ui.latEdit->setText(QString::number(myTask->getBasePos().y(),'f',10));
	ui.altEdit->setText(QString::number(myTask->getBasePos().z(),'f',10));

	int index=ui.selectBaseEntComboBox->findText(myTask->getBaseEnt());
	ui.selectBaseEntComboBox->setCurrentIndex(index);
}

ByyTask* ByyReturnBaseTaskWidget::task()
{
	return myTask;
}

void ByyReturnBaseTaskWidget::accept()
{
	myTask->setName(ui.nameEdit->text());
	myTask->setStartTime(ui.startTimeSpinBox->value());

	myTask->setAssignBaseType((ByyReturnBaseTask::AssignBaseType)ui.myAssignBaseTypeComboBox->currentIndex());

	osg::Vec3d pos;
	pos.x()=ui.lonEdit->text().toDouble();
	pos.y()=ui.latEdit->text().toDouble();
	pos.z()=ui.altEdit->text().toDouble();

	myTask->setBasePos(pos);

	myTask->setBaseEnt(ui.selectBaseEntComboBox->currentText());

	close();
	emit accepted();
}

void ByyReturnBaseTaskWidget::reject()
{
	close();
	emit rejected();
}

void ByyReturnBaseTaskWidget::mouseClicked( int button,int x,int y,osg::View *view )
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

bool ByyReturnBaseTaskWidget::MyEventProcessor::processEvent( const osgGA::GUIEventAdapter& ea,ByyChannel &channel )
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
