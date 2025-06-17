#include "StdAfx.h"
#include "assembleWidget.h"
#include "modelPlacer.h"
#include "scenarioManager.h"
#include "ByyApp.h"
#include "mainWindow.h"
#include "utils.h"
#include "ByyEntityOperator.h"
#include "ByyHdose.h"
#include "simuController.h"
#include "ByyRemoteEntityCreateWidget.h"
#include "entitySettings.h"
#include <QHBoxLayout>
#include "ByyIG.h"
#include <osgEarth/MapNode>
#include <osgEarth/Terrain>
ByyAssembleWidget::ByyAssembleWidget( ByyApp& app,QWidget *parent /*= 0*/ ) 
	: QWidget(parent)
	,myApp(app)
	,myPlaceAssemble(0)
	,myEventProcessor(this)
{
	ui.setupUi(this);
	load();

	ui.categoriesComboBox->addItem(QString::fromLocal8Bit("全部"),-1);
	ui.categoriesComboBox->addItem(QString::fromLocal8Bit("陆上"),11);
	ui.categoriesComboBox->addItem(QString::fromLocal8Bit("空中"),12);
	ui.categoriesComboBox->addItem(QString::fromLocal8Bit("水面"),13);
	ui.categoriesComboBox->addItem(QString::fromLocal8Bit("水下"),14);
	ui.categoriesComboBox->addItem(QString::fromLocal8Bit("太空"),15);
	ui.categoriesComboBox->addItem(QString::fromLocal8Bit("武器"),20);
	ui.categoriesComboBox->addItem(QString::fromLocal8Bit("其他"),0);

	ui.forceComboBox->addItem(QString::fromLocal8Bit("红方"),ByyForceFriendly);
	ui.forceComboBox->addItem(QString::fromLocal8Bit("蓝方"),ByyForceOpposing);
	ui.forceComboBox->addItem(QString::fromLocal8Bit("中立"),ByyForceNeutral);

	connect(ui.treeWidget,SIGNAL(itemClicked(QTreeWidgetItem*,int)),SLOT(itemClicked(QTreeWidgetItem*,int)));

	connect(&(ByyModelPlacer::instance(myApp.ig())),SIGNAL(push(const osgEarth::GeoPoint&,const osg::Vec3d&,int)),SLOT(pushModel(const osgEarth::GeoPoint&,const osg::Vec3d&,int)));
	connect(&(ByyModelPlacer::instance(myApp.ig())),SIGNAL(released()),SLOT(pushRelease()));

	connect(&myApp.hdose().scenarioManager(),SIGNAL(signal_scenarioChanged()),SLOT(checkEnabled()));
	connect(&myApp.hdose(),SIGNAL(signal_runningChanged()),SLOT(checkEnabled()));

	connect(ui.categoriesComboBox,SIGNAL(currentIndexChanged(int)),SLOT(filter()));
	connect(ui.forceComboBox,SIGNAL(currentIndexChanged(int)),SLOT(filter()));
	connect(ui.searchEdit,SIGNAL(textChanged(const QString&)),SLOT(filter()));

	//checkEnabled();
	filter();

	ByyIG::IGInstance()->inputController().addEventProcessor(&myEventProcessor);

	_length=0;
	_width=0;
	_radioDeploy=0;
	_radioCheck=0;
	_posLon=0;
	_posLat=0;
	_canDeploy=0;
	_enabled=false;

	_opeDeploy=true;
	_opeCheck=false;
	ByyModelPlacer::instance()->enableDeploy(true);
	if(!ByyEntitySettings::instance().terrainDeployChecking()||ByyEntitySettings::instance().terrainObstacleLayers().size()==0)
	{
		checkEnabled();
		//return;
	}
	QGroupBox *groupBox = new QGroupBox(QString::fromLocal8Bit("操作方法"));
	_radioDeploy = new QRadioButton(QString::fromLocal8Bit("部署"));
	_radioCheck = new QRadioButton(QString::fromLocal8Bit("地形检测"));

	_radioDeploy->setChecked(_opeDeploy);

	QHBoxLayout *vbox = new QHBoxLayout;
	vbox->addWidget(_radioDeploy);
	vbox->addWidget(_radioCheck);
	groupBox->setLayout(vbox);

	ui.verticalLayout->addWidget(groupBox);

	connect(_radioDeploy,SIGNAL(clicked(bool)),this,SLOT(setdeploy(bool)));
	connect(_radioCheck,SIGNAL(clicked(bool)),this,SLOT(setterraincheck(bool)));

	vbox = new QHBoxLayout;
	vbox->addWidget(new QLabel(QString::fromLocal8Bit("实体类型：")));
	_entityStr=new QLabel;
	vbox->addWidget(_entityStr);
	ui.verticalLayout->addLayout(vbox);

	vbox = new QHBoxLayout;
	vbox->addWidget(new QLabel(QString::fromLocal8Bit("长[米]：")));
	_length=new QLineEdit;
	vbox->addWidget(_length,2);

	vbox->addWidget(new QLabel(QString::fromLocal8Bit("宽[米]：")));
	_width=new QLineEdit;
	vbox->addWidget(_width,2);
	ui.verticalLayout->addLayout(vbox);

	connect(_length,SIGNAL(textEdited(const QString&)),this,SLOT(userChangeSize(const QString&)));
	connect(_width,SIGNAL(textEdited(const QString&)),this,SLOT(userChangeSize(const QString&)));

	QHBoxLayout* upLay=new QHBoxLayout;
	ui.verticalLayout->addLayout(upLay);

	QVBoxLayout* sublayLay=new QVBoxLayout;
	upLay->addLayout(sublayLay);


	vbox = new QHBoxLayout;
	vbox->addWidget(new QLabel(QString::fromLocal8Bit("经度：")));
	_posLon=new QLabel;
	vbox->addWidget(_posLon,2);
	sublayLay->addLayout(vbox);

	vbox = new QHBoxLayout;
	vbox->addWidget(new QLabel(QString::fromLocal8Bit("纬度：")));
	_posLat=new QLabel;
	vbox->addWidget(_posLat,2);
	sublayLay->addLayout(vbox);

	_canDeploy=new QLabel;
	upLay->addWidget(_canDeploy);

	//opeChanged();


	checkEnabled();
}
bool ByyAssembleWidget::processEvent(const osgGA::GUIEventAdapter& ea,osgGA::GUIActionAdapter& aa)
{
	if (isHidden()) return false;
	if(!_enabled) return false;

	if(_opeDeploy)
		return false;
	if(!_opeCheck)
		return false;
	if(ea.getEventType()!=ea.RELEASE&&ea.getEventType()!=ea.MOVE)
		return false;
	osgEarth::MapNode *mapNode=ByyIG::IGInstance()->terrain()->mapNode();
	if (!mapNode) return false;

	osg::Vec3d world;
	if (!mapNode->getTerrain()->getWorldCoordsUnderMouse(aa.asView(), ea.getX(), ea.getY(), world))
		return false;
	static osgEarth::GeoPoint map;
	map.fromWorld( mapNode->getMapSRS(), world );
	bool canDepoy=ByyModelPlacer::instance()->checkTerrainObstacle(osg::DegreesToRadians(map.x()),osg::DegreesToRadians(map.y()),_length->text().toDouble(),_width->text().toDouble());
	if(ea.getEventType()==ea.RELEASE)
	{
		if(ea.getButton()==osgGA::GUIEventAdapter::LEFT_MOUSE_BUTTON)
		{
			_posLon->setText(QString::number(map.x(),'f',8));
			_posLat->setText(QString::number(map.y(),'f',8));		
			_canDeploy->setText(canDepoy?QString::fromLocal8Bit("可部署"):QString::fromLocal8Bit("不可部署"));
			ByyIG::IGInstance()->sceneWidget()->setCursor(Qt::ArrowCursor);
		}
		else
		{
			ByyIG::IGInstance()->sceneWidget()->setCursor(Qt::ArrowCursor);
			_opeCheck=false;
		}
	}		
	else if(ea.getEventType()==ea.MOVE)
	{
		if(canDepoy)
		{
			ByyIG::IGInstance()->sceneWidget()->setCursor(Qt::PointingHandCursor);
		}
		else
		{
			ByyIG::IGInstance()->sceneWidget()->setCursor(Qt::ForbiddenCursor);
		}
	}

	return false;
}


void ByyAssembleWidget::userChangeSize(const QString&)
{
	ui.treeWidget->clearSelection();
	_entityStr->setText("");
	_opeCheck=true;
}
//void ByyAssembleWidget::opeChanged()
//{

//}

void ByyAssembleWidget::setdeploy(bool status)
{
	_opeDeploy=true;
	checkEnabled();
	ByyModelPlacer::instance()->enableDeploy(true);
}

void ByyAssembleWidget::setterraincheck(bool)
{
	_opeDeploy=false;
	_opeCheck=true;
	checkEnabled();
	ByyModelPlacer::instance()->enableDeploy(false);
}

ByyAssembleWidget::~ByyAssembleWidget()
{

}

void ByyAssembleWidget::load()
{
	ByyOpd& opd=myApp.hdose().opd();

	const ByyOpd::Assembles& assembles=opd.assembles();

	/*QList<QTreeWidgetItem*> items;

	{
		ByyOpd::Assembles::iterator it=assembles.begin();

		for (;it!=assembles.end();++it)
		{
			items.append(new QTreeWidgetItem);
		}
	}

	ui.treeWidget->addTopLevelItems(items);*/

	{
		ByyOpd::Assembles::iterator it=assembles.begin();

		for (;it!=assembles.end();++it)
		{
			ByyEntityType entType;

			if (it.value().valid())
			{
				entType=it.value().getEntityType();
			}

			if (entType.valid()&&(entType.kind==1 || entType.kind==3|| entType.kind==2))
			{
				QTreeWidgetItem *item=new QTreeWidgetItem;

				item->setText(0,it->myTag);
				item->setData(0,Qt::UserRole,(int)&it.value());

				item->setIcon(0,QIcon(loadEntityIcon(entType.string(),ByyForceNeutral)));

				ui.treeWidget->addTopLevelItem(item);
			}
			

			//item->setHidden(!(it.value().valid()&&it.value().getEntityType().valid()&&it.value().getEntityType().kind==1));
		}
	}
}

void ByyAssembleWidget::itemClicked( QTreeWidgetItem * item, int column )
{
	ByyAssemble* assemble=(ByyAssemble*)item->data(0,Qt::UserRole).toInt();

	ByyEntityType entityType=assemble->getEntityType();

	_opeCheck=true;

	if (!entityType.valid())
	{
		QMessageBox::warning(this,QString::fromLocal8Bit("提示"),
			QString::fromLocal8Bit("未找到对应的实体类型！"));

		return ;
	}

	_entityStr->setText(item->text(0));
	ByyOpdEntry* opd=ByyOpd::instance()->findOpdEntry(entityType);
	double len=0;
	double width=0;
	if(opd!=0)
	{
		len=opd->value("Length").toDouble();
		width=opd->value("Width").toDouble();
		if(width==0)
		{
			if(entityType.kind==1 && (entityType.domain==2 || entityType.domain==5) )
				width=opd->value("Span").toDouble();
			if(entityType.kind==1 && (entityType.domain==3 || entityType.domain==4) )
				width=opd->value("Beam").toDouble();
			if(entityType.kind==2)
				width=opd->value("Span").toDouble();
			if(entityType.kind==3)
			{
				len=0.6;
				width=0.6;
			}
		}
	}
	_length->setText(QString::number(len,'f',2));
	_width->setText(QString::number(width,'f',2));

	if(_opeDeploy)
	{
		
		if (ByyModelPlacer::instance(myApp.ig()).setModelType(entityType))
		{
			myPlaceAssemble=assemble;
			{
				ByyIG::IGInstance()->sceneWidget()->setCursor(ByyModelPlacer::instance(myApp.ig()).cursor());
			}
		}
		else
		{
			QMessageBox::warning(this,QString::fromLocal8Bit("提示"),
				QString::fromLocal8Bit("加载模型失败！"));
		}
	}
}

#include "ByyEditEntityWidget.h"

void ByyAssembleWidget::pushModel( const osgEarth::GeoPoint& geoPoint, const osg::Vec3d& orientation, int carryID)
{
	ByyEntityType entType=ByyModelPlacer::instance(myApp.ig()).modelType();

	ByyForceType ft=(ByyForceType)ui.forceComboBox->itemData(ui.forceComboBox->currentIndex(),Qt::UserRole).toInt();

	ByyEntityObject *entObj=myApp.hdose().createEntity(myPlaceAssemble->myItemClass,ft,osg::Vec3d(geoPoint.x(),geoPoint.y(),geoPoint.alt()));

	entObj->setOrientation(orientation);
	ByyModelPlacer::instance(myApp.ig()).release();

	//设置有效搭载对象
	ByyLocalEntity * lEntObj = dynamic_cast<ByyLocalEntity*>(entObj);
	if((carryID >= 0 ) && lEntObj)
	{
		ParameterList& editParameters = lEntObj->getParameters();

		ParameterList::iterator it= editParameters.begin(); 

		for (;it!=editParameters.end();++it)
		{
			//qDebug() << __FUNCTION__ << __LINE__ << (*it)->Group << "xxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxx" << QString::fromLocal8Bit("搭载关系");
			if (QString::fromLocal8Bit("搭载关系") == (*it)->Group)
			{
				if("Embarkation" == (*it)->Name)
				{
					(*it)->setValue("1");
				}
				else if("EmbarkedEntityName" == (*it)->Name)
				{
					QString opdName;
					ByyEntityObject* obj = ByyIG::IGInstance()->core().localObjectManager().getObjectByGlobalId(carryID);
					if(obj)
					{
						opdName = obj->name();
					}
					(*it)->setValue(opdName);
				}
			}
		}
	}

	if (myApp.hdose().simuController().scnLoaded())
	{
		ByyEntityOperator::instance().edit(entObj,this,SLOT(onEntityEditFinished(ByyLocalEntity*)));
	}
	else
	{
		ByyEntityOperator::instance().edit(entObj);
	}
}

void ByyAssembleWidget::checkEnabled()
{
	_enabled=false;
	if(myApp.hdose().started())
	{
		_enabled=true;
	}
	else
		_enabled=myApp.hdose().scenarioManager().scenario();
	ui.treeWidget->setEnabled(_enabled);
	if(_radioCheck==0)
		return;
	_radioCheck->setEnabled(_enabled);
	_radioDeploy->setEnabled(_enabled);
	_length->setEnabled(_enabled&&!_opeDeploy);
	_width->setEnabled(_enabled&&!_opeDeploy);
}

void ByyAssembleWidget::filter()
{
	ByyOpd& opd=myApp.hdose().opd();

	QTreeWidgetItem *item=0;

	ByyAssemble* assemble;

	int currentCategoriy=ui.categoriesComboBox->itemData(ui.categoriesComboBox->currentIndex(),Qt::UserRole).toInt();

	for (int i=0;i!=ui.treeWidget->topLevelItemCount();++i)
	{
		bool _show=true;

		item=ui.treeWidget->topLevelItem(i);

		if (_show&&!ui.searchEdit->text().isEmpty())
		{
			_show=item->text(0).toLower().contains(ui.searchEdit->text().toLower());
		}

		if (_show&&currentCategoriy!=-1)
		{
			assemble=(ByyAssemble*)item->data(0,Qt::UserRole).toInt();
			if(currentCategoriy<20){
				if(assemble->getEntityType().kind==1)
				{
					short domian=currentCategoriy-10;
					_show=assemble->getEntityType().domain==domian;
				}
				else
					_show=false;
			}
			else{
				short kind=currentCategoriy/10;
				_show=assemble->getEntityType().kind==kind;
			}
		}

		item->setHidden(!_show);
	}
}

void ByyAssembleWidget::pushRelease()
{
	qApp->restoreOverrideCursor();
}

void ByyAssembleWidget::onEntityEditFinished( ByyLocalEntity* entObj )
{
	myApp.hdose().simuController().createObject(1,entObj->itemClass(),entObj->getAttributeValueSet());
}
