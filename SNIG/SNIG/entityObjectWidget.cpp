#include "StdAfx.h"
#include "entityObjectWidget.h"
#include "ByyApp.h"
#include "utils.h"
#include "sceneEntity.h"
#include "ByyEntityOperator.h"
#include "ByyEntityInfoBoxWidgt.h"

Q_DECLARE_METATYPE(ByyEntityObject*)

ByyEntityObjectWidget::ByyEntityObjectWidget( ByyApp& app,QWidget *parent /*= 0*/ ) : QWidget(parent)
	,myApp(app)
{
	ui.setupUi(this);

	layout()->addWidget(new ByyEntityInfoBoxWidgt(app,0,this));

	ui.treeWidget->installEventFilter(this);

	connect(ui.treeWidget,SIGNAL(itemDoubleClicked(QTreeWidgetItem*,int)),SLOT(itemDoubleClicked(QTreeWidgetItem*,int)));

	connect(&myApp.core().remoteObjectManager(),SIGNAL(entityAppend(ByyEntityObject*)),this,SLOT(appendEntObject(ByyEntityObject*)));
	connect(&myApp.core().remoteObjectManager(),SIGNAL(entityRemoved(ByyEntityObject*)),this,SLOT(removeEntObject(ByyEntityObject*)));

	connect(&myApp.core().localObjectManager(),SIGNAL(entityAppend(ByyEntityObject*)),this,SLOT(appendEntObject(ByyEntityObject*)));
	connect(&myApp.core().localObjectManager(),SIGNAL(entityRemoved(ByyEntityObject*)),this,SLOT(removeEntObject(ByyEntityObject*)));

	connect(ui.treeWidget,SIGNAL(levelChanged(QTreeWidgetItem*)),SLOT(onLevelChanged(QTreeWidgetItem*)));

	connect(ui.treeWidget,SIGNAL(itemClicked(QTreeWidgetItem*,int)),
		this,SLOT(onItemClicked(QTreeWidgetItem*,int)));
}

ByyEntityObjectWidget::~ByyEntityObjectWidget()
{

}

void ByyEntityObjectWidget::appendEntObject( ByyEntityObject* entObj )
{
	QTreeWidgetItem *item=new QTreeWidgetItem;

	item->setIcon(0,QIcon(loadEntityIcon(entObj->entityType().string(),entObj->forceType())));

	item->setText(0,entObj->name());

	item->setData(0,Qt::UserRole,QVariant::fromValue<ByyEntityObject*>(entObj));

	connect(entObj,SIGNAL(nameChanged(const QString&)),SLOT(onNameChanged(const QString&)));
	connect(entObj,SIGNAL(visibleChanged(bool)),SLOT(onVisibleChanged(bool)));
	connect(entObj,SIGNAL(forceTypeChanged(ByyForceType)),SLOT(onForceTypeChanged(ByyForceType)));

	if (entObj->level()==1)
	{
		ui.treeWidget->addTopLevelItem(item);
	}
	else
	{
		ByyAbsObjectManager* objManager=dynamic_cast<ByyAbsObjectManager*>(sender());

		ByyEntityObject *parentEnt=objManager->findEntity(entObj->superiorName());

		QTreeWidgetItem* parentItem=findItem(parentEnt);

		if (parentItem)
		{
			parentItem->addChild(item);
		}
		else
		{
			ui.treeWidget->addTopLevelItem(item);
			myUnFindSuperiorItems.append(item);
		}
	}

	QList<QTreeWidgetItem*>::iterator it=myUnFindSuperiorItems.begin();

	for (;it!=myUnFindSuperiorItems.end();)
	{
		QTreeWidgetItem *unParentItem=*it;

		ByyEntityObject *itemEntObj=unParentItem->data(0,Qt::UserRole).value<ByyEntityObject*>();

		if (itemEntObj->level()-entObj->level()==1&&itemEntObj->superiorName()==entObj->name())
		{
			ui.treeWidget->takeTopLevelItem(ui.treeWidget->indexOfTopLevelItem(unParentItem));

			item->addChild(unParentItem);

			it=myUnFindSuperiorItems.erase(it);
		}
		else
			++it;
	}

	item->setHidden(!entObj->visible());
}

void ByyEntityObjectWidget::removeEntObject( ByyEntityObject* entObj )
{
	QList<QPointer<QWidget>> entObjInfoWidgets=myAttachEntWidgetMap.values(entObj);

	for (int i=0;i!=entObjInfoWidgets.size();++i)
	{
		if (!entObjInfoWidgets[i].isNull())
		{
			entObjInfoWidgets[i]->close();
		}
	}

	myAttachEntWidgetMap.remove(entObj);

	QTreeWidgetItem* item=findItem(entObj);

	if (!item)
		return;

	QTreeWidgetItem *itemParent=item->parent();

	if (itemParent)
	{
		itemParent->removeChild(item);

	}
	else
	{
		int index=ui.treeWidget->indexOfTopLevelItem(item);

		ui.treeWidget->takeTopLevelItem(index);

		myUnFindSuperiorItems.removeOne(item);
	}


	QList<QTreeWidgetItem*> children=item->takeChildren();


	foreach(QTreeWidgetItem* child,children)
	{
		if (itemParent)
		{
			itemParent->addChild(child);

		}
		else
		{
			ui.treeWidget->addTopLevelItem(child);
		}
	}

	delete item;
}

void ByyEntityObjectWidget::itemDoubleClicked( QTreeWidgetItem *item,int column )
{
	ByyEntityObject *entObj=item->data(0,Qt::UserRole).value<ByyEntityObject*>();

	ByySceneEntity* sceneObj=myApp.ig().sceneObjectManager()->findEntity(entObj);

	if (!sceneObj)
		return;

	osg::Vec3d focal=sceneObj->center();

	osgEarth::Viewpoint vp("",focal.x(),focal.y(),focal.z(),0,-89,sceneObj->radius()*3);

	myApp.ig().inputController().currentObserver()->setViewpoint(vp);
}

bool ByyEntityObjectWidget::eventFilter( QObject *obj, QEvent *ev )
{
	if (obj==ui.treeWidget&&ev->type()==QEvent::ContextMenu)
	{
		if (QTreeWidgetItem *item=ui.treeWidget->itemAt(ui.treeWidget->viewport()->mapFromGlobal(QCursor::pos())))

			ByyEntityOperator::instance().showContextMenu();

		return true;
	}

	return QWidget::eventFilter(obj,ev);
}

void ByyEntityObjectWidget::onNameChanged( const QString& name )
{
	ByyEntityObject* obj=dynamic_cast<ByyEntityObject*>(sender());

	QTreeWidgetItem* item=findItem(obj);

	if(item)
	{
		item->setText(0,name);
	}
}

void ByyEntityObjectWidget::onVisibleChanged( bool b )
{
	ByyEntityObject *ent=dynamic_cast<ByyEntityObject*>(sender());

	QTreeWidgetItem* item=findItem(ent);

	if(item)
	{
		item->setHidden(!b);
	}
}

QTreeWidgetItem* ByyEntityObjectWidget::findItem( ByyEntityObject* entObj )
{
	QTreeWidgetItemIterator it(ui.treeWidget);

	while(*it)
	{
		if (entObj==(*it)->data(0,Qt::UserRole).value<ByyEntityObject*>())
		{
			return *it;
		}
		++it;
	}
	return 0;
}

void ByyEntityObjectWidget::onLevelChanged( QTreeWidgetItem *item )
{
	ByyEntityObject *ent=item->data(0,Qt::UserRole).value<ByyEntityObject*>();

	int newLevel=1;

	QString superior;

	if (item->parent())
	{
		ByyEntityObject *parentEnt=item->parent()->data(0,Qt::UserRole).value<ByyEntityObject*>();

		newLevel=parentEnt->level()+1;
		superior=parentEnt->name();
	}

	ent->setLevel(newLevel);
	ent->setSuperiorName(superior);
}

void ByyEntityObjectWidget::onForceTypeChanged( ByyForceType type )
{
	ByyEntityObject *ent=dynamic_cast<ByyEntityObject*>(sender());

	QTreeWidgetItem* item=findItem(ent);

	if(item)
	{
		item->setIcon(0,QIcon(loadEntityIcon(ent->entityType().string(),ent->forceType())));
	}
}

void ByyEntityObjectWidget::onItemClicked( QTreeWidgetItem *current, int column )
{
	ByySelectionManager::SelectionType type=ByySelectionManager::NoValidSelection;

	unsigned int id=0;

	if (current)
	{
		ByyEntityObject *curEnt=current->data(0,Qt::UserRole).value<ByyEntityObject*>();
		type=ByySelectionManager::Entity;
		id=curEnt->objectId();

	}

	ByySelectionManager::instance().setCurrentSelection(type,id);
	
	//ui.infoBoxWidget->setEntity(curEnt);
}
