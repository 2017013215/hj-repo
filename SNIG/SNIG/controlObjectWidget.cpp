#include "StdAfx.h"
#include "controlObjectWidget.h"
#include "ByyApp.h"
#include "objectIconManager.h"
#include "ByySelectionManager.h"
#include "ByyControlObjectOperator.h"

Q_DECLARE_METATYPE(ByyControlObject*)

	ByyControlObjectWidget::ByyControlObjectWidget( ByyApp& app,QWidget *parent /*= 0*/ )
	: QWidget(parent)
	,myApp(app)
{
	ui.setupUi(this);

	

	connect(&myApp.core().remoteObjectManager(),SIGNAL(contrlObjectAppend(ByyControlObject*)),this,SLOT(appendContrlObject(ByyControlObject*)));
	connect(&myApp.core().remoteObjectManager(),SIGNAL(contrlObjectRemoved(ByyControlObject*)),this,SLOT(removeContrlObject(ByyControlObject*)));

	connect(&myApp.core().localObjectManager(),SIGNAL(contrlObjectAppend(ByyControlObject*)),this,SLOT(appendContrlObject(ByyControlObject*)));
	connect(&myApp.core().localObjectManager(),SIGNAL(contrlObjectRemoved(ByyControlObject*)),this,SLOT(removeContrlObject(ByyControlObject*)));

	connect(ui.treeWidget,SIGNAL(itemDoubleClicked(QTreeWidgetItem*,int)),SLOT(itemDoubleClicked(QTreeWidgetItem*,int)));

	connect(ui.treeWidget,SIGNAL(itemClicked(QTreeWidgetItem*,int)),
		this,SLOT(onItemClicked(QTreeWidgetItem*,int)));

	ui.treeWidget->installEventFilter(this);
}

ByyControlObjectWidget::~ByyControlObjectWidget()
{

}

void ByyControlObjectWidget::appendContrlObject( ByyControlObject* obj )
{
	QTreeWidgetItem *item=new QTreeWidgetItem(ui.treeWidget);

	item->setData(0,Qt::UserRole,QVariant::fromValue<ByyControlObject*>(obj));

	item->setIcon(0,ByyObjectIconManager::instance().getIcon(obj->forceType()));

	item->setHidden(!obj->visible());

	item->setText(0,obj->name());

	myControlObjects.insert(obj,item);

	connect(obj,SIGNAL(nameChanged(const QString&)),SLOT(onNameChanged(const QString&)));
	connect(obj,SIGNAL(visibleChanged(bool)),SLOT(onVisibleChanged(bool)));
	connect(obj,SIGNAL(forceTypeChanged(ByyForceType)),SLOT(onForceTypeChanged(ByyForceType)));
}

void ByyControlObjectWidget::removeContrlObject( ByyControlObject* obj )
{
	ControlObjectMap::iterator it=myControlObjects.find(obj);

	if (it!=myControlObjects.end())
	{
		delete ui.treeWidget->takeTopLevelItem(ui.treeWidget->indexOfTopLevelItem(it.value()));
	}

	myControlObjects.remove(obj);
}

void ByyControlObjectWidget::onNameChanged( const QString& name )
{
	ByyControlObject* obj=dynamic_cast<ByyControlObject*>(sender());

	if(obj&&myControlObjects.contains(obj))
	{
		myControlObjects[obj]->setText(0,name);
	}
}

void ByyControlObjectWidget::itemDoubleClicked( QTreeWidgetItem *item,int column )
{
	ByyControlObject* obj=item->data(0,Qt::UserRole).value<ByyControlObject*>();
	
	ByySceneControlObject* sceneObj=myApp.ig().sceneObjectManager()->findCtrlObj(obj);

	if (!sceneObj)
		return;

	osg::Vec3d focal=sceneObj->center();

	osgEarth::Viewpoint vp("",focal.x(),focal.y(),focal.z(),0,-89,sceneObj->radius()*3);

	myApp.ig().inputController().currentObserver()->setViewpoint(vp);
}

bool ByyControlObjectWidget::eventFilter( QObject *obj, QEvent *ev )
{
	if (obj==ui.treeWidget&&ev->type()==QEvent::ContextMenu)
	{
		if (QTreeWidgetItem *item=ui.treeWidget->itemAt(ui.treeWidget->viewport()->mapFromGlobal(QCursor::pos())))
			
			ByyControlObjectOperator::instance().showContextMenu();

		return true;
	}

	return QWidget::eventFilter(obj,ev);
}

void ByyControlObjectWidget::onVisibleChanged( bool b )
{
	ByyControlObject *ent=dynamic_cast<ByyControlObject*>(sender());

	if (!ent||!myControlObjects.contains(ent))
		return;

	myControlObjects[ent]->setHidden(!b);
}

void ByyControlObjectWidget::onForceTypeChanged( ByyForceType type )
{
	ByyControlObject* obj=dynamic_cast<ByyControlObject*>(sender());

	if(obj&&myControlObjects.contains(obj))
	{
		myControlObjects[obj]->setIcon(0,ByyObjectIconManager::instance().getIcon(obj->forceType()));
	}
}

void ByyControlObjectWidget::onItemClicked( QTreeWidgetItem *current, int column )
{
	ByySelectionManager::SelectionType type=ByySelectionManager::NoValidSelection;

	unsigned int id=0;

	if (current)
	{
		ByyControlObject* obj=current->data(0,Qt::UserRole).value<ByyControlObject*>();
		type=ByySelectionManager::ControlObject;
		id=obj->objectId();

	}

	ByySelectionManager::instance().setCurrentSelection(type,id);
}
