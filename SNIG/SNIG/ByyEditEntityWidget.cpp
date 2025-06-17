#include "StdAfx.h"
#include "ByyEditEntityWidget.h"
#include "parameter.h"
#include "BTFile/behaviortreeeditwdg.h"
#include "ByyAttributeEditMng.h"

ByyEditEntityWidget::ByyEditEntityWidget( ByyCore& core,ByyAttributeEditMng &atmng,ByyLocalEntity* entObj,QWidget *parent /*= 0*/ ) 
	: ByyEntityObserver(core,entObj,parent)
	,myEntObj(entObj)
	,myEdited(false)
	,m_defaultDelegates(nullptr)
	,m_delegates(nullptr)
	,myATMng(atmng)
{
	ui.setupUi(this);

	ui.nameEdit->setText(myEntObj->name());
	ui.headingSpinBox->setValue(myEntObj->orientation().x());
	ui.sideComboBox->setCurrentIndex(myEntObj->forceType());
	ui.lonEdit->setText(QString::number(myEntObj->position().x(),'f',8));
	ui.latEdit->setText(QString::number(myEntObj->position().y(),'f',8));
	ui.altEdit->setText(QString::number(myEntObj->position().z(),'f',8));
	ui.speedEdit->setText(QString::number(myEntObj->speed(),'f',8));

	myEditParameters=myEntObj->getParameters();
	//界面控件代理
	m_defaultDelegates = ui.treeWidget->itemDelegate();

	QMap<QString,QTreeWidgetItem*> groupItems;

	ParameterList::iterator it=myEditParameters.begin();

	for (;it!=myEditParameters.end();++it)
	{
		if (!groupItems.contains((*it)->Group))
		{
			QTreeWidgetItem* item=new QTreeWidgetItem;

			ui.treeWidget->addTopLevelItem(item);

			item->setText(0,(*it)->Group);
			groupItems.insert((*it)->Group,item);
		}

		addChild(*it,groupItems[(*it)->Group]);
	}


	ui.treeWidget->setColumnWidth(Value,100);
	ui.treeWidget->setColumnWidth(Type,60);

	ui.treeWidget->installEventFilter(this);

	connect(ui.treeWidget,SIGNAL(itemDoubleClicked(QTreeWidgetItem*,int)),this,SLOT(onItemDbClicked(QTreeWidgetItem*,int)));     
	connect(ui.treeWidget,SIGNAL(itemChanged(QTreeWidgetItem*,int)),this,SLOT(onItemChanged(QTreeWidgetItem*,int)));

	connect(ui.nameEdit,SIGNAL(textEdited(const QString&)),SLOT(setName(const QString&)));
	connect(ui.headingSpinBox,SIGNAL(valueChanged(double)),SLOT(setHeading(double)));
	connect(ui.sideComboBox,SIGNAL(currentIndexChanged(int)),SLOT(setSideCurrentIndex(int)));
	connect(ui.latEdit,SIGNAL(textEdited(const QString&)),SLOT(setLat(const QString&)));
	connect(ui.lonEdit,SIGNAL(textEdited(const QString&)),SLOT(setLon(const QString&)));
	connect(ui.altEdit,SIGNAL(textEdited(const QString&)),SLOT(setAlt(const QString&)));
	connect(ui.speedEdit,SIGNAL(textEdited(const QString&)),SLOT(setSpeed(const QString&)));

	m_BehaviorTreeEdit = NULL;
	m_Parent = parent;
}

ByyEditEntityWidget::~ByyEditEntityWidget()
{

}

void ByyEditEntityWidget::updateObj()
{
	//myEntObj->setName(ui.nameEdit->text());
	//myEntObj->setForceType((ByyForceType)(ui.sideComboBox->currentIndex()));
	myEntObj->setParameters(myEditParameters);
	close();

	emit editFinished(myEntObj);
}

void ByyEditEntityWidget::cancel()
{
	if (myEdited)
	{
		//entObjFrame.restore(myEntObj);
	}

	close();

	emit editFinished(myEntObj);
}

void ByyEditEntityWidget::setName( const QString& name )
{
	updateParam("EntityName",name);
}

void ByyEditEntityWidget::setHeading( double value )
{
	osg::Vec3d& ori=myEntObj->orientation();
	ori.x()=value;

	//myEntObj->setOrientation(ori);

	updateParam("Heading",QString::number(value));

	myEdited=true;
}

void ByyEditEntityWidget::setSideCurrentIndex( int index )
{
	updateParam("Side",QString::number(index));
}

void ByyEditEntityWidget::setLat( const QString& lat )
{
	//myEntObj->position().y()=lat.toDouble();

	myEdited=true;

	updateParam("Position",QString("%1,%2,%3").arg(ui.latEdit->text()).arg(ui.lonEdit->text()).arg(ui.altEdit->text()));
}

void ByyEditEntityWidget::setLon( const QString& lon )
{
	//myEntObj->position().x()=lon.toDouble();

	myEdited=true;
	updateParam("Position",QString("%1,%2,%3").arg(ui.latEdit->text()).arg(ui.lonEdit->text()).arg(ui.altEdit->text()));
}

void ByyEditEntityWidget::setAlt( const QString& alt )
{
	//myEntObj->position().z()=alt.toDouble();

	myEdited=true;
	updateParam("Position",QString("%1,%2,%3").arg(ui.latEdit->text()).arg(ui.lonEdit->text()).arg(ui.altEdit->text()));
}

void ByyEditEntityWidget::setSpeed( const QString& speed )
{
	//myEntObj->setSpeed(speed.toDouble());
	updateParam("Velocity",speed);
}

void ByyEditEntityWidget::onBehaviorTreeEdit()
{
	//行为树
	m_BehaviorTreeEdit = NULL;
	m_BehaviorTreeEdit = new BehaviorTreeEditWdg(myCore,myATMng,myEntObj,m_Parent);
	connect(m_BehaviorTreeEdit,SIGNAL(signal_updateBehaviorTreeInfo(QString &,QString &)),this,SLOT(onUpdateBehaviorTreeInfo(QString &,QString &)));
	m_BehaviorTreeEdit->show();
}

void ByyEditEntityWidget::onUpdateBehaviorTreeInfo(QString &path,QString &content)
{
	updateParam("PlanRef",path);
	updateParam("Plan",content);
}

QTreeWidgetItem* ByyEditEntityWidget::addChild( ByyParameter* param,QTreeWidgetItem* parent )
{
	QTreeWidgetItem *item=new QTreeWidgetItem;

	item->setText(Name,param->Name);
	item->setText(Tag,param->Tag);
	item->setText(Value,param->getValue());
	item->setText(Type,param->Type);
	item->setText(Desc,param->Description);

	//是否可设计
	//if(param->IsCanDesign.toInt())
	//	item->setCheckState(Design, param->IsDesign.toInt() == 0 ? Qt::Unchecked : Qt::Checked);
	item->setData(Value,Qt::UserRole,(int)param);

	parent->addChild(item);

	//////////////////////////////////////////////////////////////////////////
	if (param)
	{
#if 0
		if (param->Type=="enum")
		{
			ByyEnumParameter* enumPara = (ByyEnumParameter*)param;
			QMap<int, QString> m_EnumMap = enumPara->getEnumMap();
			QString enumValue = m_EnumMap.value(param->getValue().toInt());
			item->setText(Value,enumValue);			
		}
		else
		{
			item->setText(Value,param->getValue());
		}
#endif

		AttrEditType eType;
		if(myATMng.getRsmRefAttricbeEditType(param->Name, eType))
		{
			if(!eType.sourceInfo.isEmpty() && !eType.values.isEmpty())
			{
				item->setText(Value,eType.values.key(param->getValue().toInt()));
			}
			else if(!eType.sourceInfo.isEmpty())
			{
				if(("ENTITY" == eType.sourceInfo.at(1)) && (eType.sourceInfo.size() >= 3))
				{
					QString friendState = eType.sourceInfo.at(2);
					if("FRIEND" == friendState)
					{
						item->setText(Value,param->getValue());
					}
				}
			}
		}
		else
		{
			item->setText(Value,param->getValue());
		}
	}
	//行为树按钮
	if (param->Tag == QString::fromLocal8Bit("行为树脚本文件编辑"))
	{
		QPushButton *editBtn = new QPushButton();
		editBtn->setText(QString::fromLocal8Bit("行为树编辑"));
		connect(editBtn,SIGNAL(clicked()),this,SLOT(onBehaviorTreeEdit()));
		ui.treeWidget->setItemWidget(item,Value,editBtn);
	}
	//////////////
	{
		ByyListParameter *listParam=dynamic_cast<ByyListParameter*>(param);

		if (listParam)
		{
			ParameterList::iterator it=listParam->children().begin();

			for (;it!=listParam->children().end();++it)
			{
				addChild(*it,item);
			}
		}
	}

	return item;
}

void ByyEditEntityWidget::onItemDbClicked( QTreeWidgetItem* item,int column )
{
	ui.treeWidget->blockSignals(true);

	/*if (column==Value)
	{
	item->setFlags(item->flags()|Qt::ItemIsEditable);
	}
	else
	{
	item->setFlags(item->flags()&~Qt::ItemIsEditable);
	}
	ui.treeWidget->blockSignals(false);*/

	if (column==Value)
	{
		item->setFlags(item->flags()|Qt::ItemIsEditable);
	}
	else
	{
		item->setFlags(item->flags()&~Qt::ItemIsEditable);
	}
	ByyParameter* param = (ByyParameter*)item->data(Value,Qt::UserRole).toInt();
	if (param)
	{
#if 0
		if (param->Type=="enum")
		{
			if(m_delegates) delete m_delegates; m_delegates=NULL;
			ByyEnumParameter* enumPara = (ByyEnumParameter*)param;
			m_delegates = new CCustomDelegate(ui.treeWidget, ENUM_TYPE);
			m_delegates->setEnumMap(enumPara->getEnumMap());
			ui.treeWidget->setItemDelegate(m_delegates);			
		}
#endif
		QString attrName = item->data(0,Qt::DisplayRole).toString();
		AttrEditType eType;
		if(myATMng.getRsmRefAttricbeEditType(attrName, eType) && param->Type!="path")
		{
			QStringList keyValues = eType.values.keys();
			int size = keyValues.size();
			QMap<int, QString> map;
			if(!eType.sourceInfo.isEmpty() && !eType.values.isEmpty())
			{
				for (int i = 0; i < size; i++)
				{
					map.insert(i, keyValues.at(i));
				}
				if(m_delegates) delete m_delegates; m_delegates=NULL;
				ByyEnumParameter* enumPara = (ByyEnumParameter*)param;
				m_delegates = new CCustomDelegate(ui.treeWidget, ENUM_TYPE);
				m_delegates->setEnumMap(map);
				ui.treeWidget->setItemDelegate(m_delegates);
			}
			else if(!eType.sourceInfo.isEmpty())
			{
				if(("ENTITY" == eType.sourceInfo.at(1)) && (eType.sourceInfo.size() >= 3))
				{
					QString friendState = eType.sourceInfo.at(2);
					//搭载对象
					QList<ByyEntityObject*> entityObjs = myCore.localObjectManager().entityObjects().values();
					QStringList objList;
					int mapNode = 0;
					for(int j = 0; j < entityObjs.size();j++)
					{
						ByyEntityObject* tmpObj = entityObjs.at(j);
						if(("FRIEND" == friendState) && (myEntObj->forceType() == tmpObj->forceType()))
						{
							if (myEntObj->name() != tmpObj->name())
							{
								map.insert(j, tmpObj->name());
								mapNode++;
							}
						}
					}
					if(m_delegates) delete m_delegates; m_delegates=NULL;
					ByyEnumParameter* enumPara = (ByyEnumParameter*)param;
					m_delegates = new CCustomDelegate(ui.treeWidget, ENUM_TYPE);
					m_delegates->setEnumMap(map);
					ui.treeWidget->setItemDelegate(m_delegates);
				}
			}
		}
		else if(param->Type=="path")
		{
			if(m_delegates) delete m_delegates; m_delegates=NULL;
			ByyPathParameter* pathPara = (ByyPathParameter*)param;
			m_delegates = new CCustomDelegate(ui.treeWidget, PATH_TYPE);
			m_delegates->setPathFileSuffix(pathPara->getFileSuffix());
			ui.treeWidget->setItemDelegate(m_delegates);
		}
		else
		{
			ui.treeWidget->setItemDelegate(m_defaultDelegates);
		}
	}
	
	ui.treeWidget->blockSignals(false);

}

void ByyEditEntityWidget::onItemChanged( QTreeWidgetItem* item,int column )
{
//	printf("onItemChanged:%s\n",item->text(Tag).toLocal8Bit().data());

	if (column==Value)
	{
		ByyParameter* param=(ByyParameter*)item->data(column,Qt::UserRole).toInt();
		if (param)
		{
#if 0
			if (param->Type=="enum")
			{
				ByyEnumParameter* enumPara = (ByyEnumParameter*)param;
				QMap<int, QString> m_EnumMap = enumPara->getEnumMap();
				QString enumValue = QString::number(m_EnumMap.key(item->text(column)));
				enumPara->setValue(enumValue);
				param->setValue(enumValue);
			}
#endif
			ByyEnumParameter* enumPara = (ByyEnumParameter*)param;
			AttrEditType eType;
			if(myATMng.getRsmRefAttricbeEditType(enumPara->Name, eType))
			{
				if(!eType.sourceInfo.isEmpty() && !eType.values.isEmpty())
				{
					QString enumValue = eType.values.value(item->text(column)).toString();
					enumPara->setValue(enumValue);
					param->setValue(enumValue);
				}
				else if(!eType.sourceInfo.isEmpty())
				{
					if(("ENTITY" == eType.sourceInfo.at(1)) && (eType.sourceInfo.size() >= 3))
					{
						QString friendState = eType.sourceInfo.at(2);
						if("FRIEND" == friendState)
						{
							param->setValue(item->text(column));
						}
					}
				}
			}
			else
			{
				param->setValue(item->text(column));
			}
		}

		ui.treeWidget->blockSignals(true);

		updateParent(item);
		updateChildren(item);

		ui.treeWidget->blockSignals(false);
	}
	else if(column==Design)
	{
		ByyParameter* param=(ByyParameter*)item->data(Value,Qt::UserRole).toInt();

		param->setDesign(item->checkState(column) == Qt::Unchecked ? QString("0") : QString("1"));
	}
}

void ByyEditEntityWidget::updateParent( QTreeWidgetItem *item )
{
	if (!item)
		return;

	QTreeWidgetItem *parentItem=item->parent();

	if (!parentItem)
		return;

	ByyParameter* param=(ByyParameter*)parentItem->data(Value,Qt::UserRole).toInt();

	if (param)
	{
		parentItem->setText(Value,param->getValue());

		updateParent(parentItem);
	}
}

void ByyEditEntityWidget::updateChildren( QTreeWidgetItem *item )
{
	QTreeWidgetItem *child;

	for (int i=0;i!=item->childCount();++i)
	{
		child=item->child(i);

		ByyParameter* param=(ByyParameter*)child->data(Value,Qt::UserRole).toInt();

		if (param)
		{
			child->setText(Value,param->getValue());

			updateChildren(child);
		}
	}
}

bool ByyEditEntityWidget::eventFilter( QObject *obj, QEvent *ev )
{
	if (obj==ui.treeWidget&&ev->type()==QEvent::ContextMenu)
	{
		QTreeWidgetItem *item=ui.treeWidget->itemAt(ui.treeWidget->viewport()->mapFromGlobal(QCursor::pos()));

		ByyParameter* param=item?(ByyParameter*)item->data(Value,Qt::UserRole).toInt():0;

		if (item&&param)
		{
			ByyStructArrayParameter *structArrayParam=dynamic_cast<ByyStructArrayParameter*>(param);

			QMenu *menu=new QMenu(this);

			if (structArrayParam)
			{
				menu->addAction(QString::fromLocal8Bit("添加")+structArrayParam->Name,this,SLOT(addArrayChild()));
			}

			QTreeWidgetItem *paraentItem=item->parent();

			ByyParameter* parentParam=paraentItem?(ByyParameter*)paraentItem->data(Value,Qt::UserRole).toInt():0;

			if (paraentItem&&parentParam)
			{
				ByyStructArrayParameter *parentArrayParam=dynamic_cast<ByyStructArrayParameter*>(parentParam);

				if (parentArrayParam)
				{
					menu->addAction(QString::fromLocal8Bit("删除"),this,SLOT(removeArrayChild()));
				}

			}

			menu->setAttribute(Qt::WA_DeleteOnClose);

			menu->popup(QCursor::pos());
		}

		return true;
	}

	return QWidget::eventFilter(obj,ev);
}

void ByyEditEntityWidget::addArrayChild()
{
	QTreeWidgetItem *item=ui.treeWidget->currentItem();

	ByyStructArrayParameter* param=item?(ByyStructArrayParameter*)item->data(Value,Qt::UserRole).toInt():0;

	if (!item||!param)
		return;

	param->push_back();

	ui.treeWidget->blockSignals(true);

	updateParent(addChild(param->children().last(),item));

	ui.treeWidget->blockSignals(false);
}

void ByyEditEntityWidget::removeArrayChild()
{
	QTreeWidgetItem *item=ui.treeWidget->currentItem();

	ByyParameter* param=item?(ByyParameter*)item->data(Value,Qt::UserRole).toInt():0;

	if (!item||!param)
		return;

	QTreeWidgetItem *paraentItem=item->parent();

	ByyStructArrayParameter* parentParam=paraentItem?(ByyStructArrayParameter*)paraentItem->data(Value,Qt::UserRole).toInt():0;

	if (!paraentItem||!parentParam)
		return;

	parentParam->removeOne(param);

	
	ui.treeWidget->blockSignals(true);

	updateParent(item);

	ui.treeWidget->blockSignals(false);

	paraentItem->removeChild(item);

	delete item;
}

void ByyEditEntityWidget::updateParam( const QString& name,const QString& value )
{
	ByyParameter *p=myEditParameters.findParameter(name);

	if (p)
	{
		p->setValue(value);

		QTreeWidgetItemIterator it(ui.treeWidget);

		while (*it)
		{
			if (p==(ByyParameter*)(*it)->data(Value,Qt::UserRole).toInt())
			{
				AttrEditType eType;
				if(myATMng.getRsmRefAttricbeEditType(name, eType))
				{
					if(!eType.sourceInfo.isEmpty() && !eType.values.isEmpty())
					{
						(*it)->setText(Value,eType.values.key(value));
					}
					else if(!eType.sourceInfo.isEmpty())
					{
						if(("ENTITY" == eType.sourceInfo.at(1)) && (eType.sourceInfo.size() >= 3))
						{
							QString friendState = eType.sourceInfo.at(2);
							//攻击对象
							QList<ByyEntityObject*> entityObjs = myCore.localObjectManager().entityObjects().values();
							if("FRIEND" == friendState)
							{
								(*it)->setText(Value,value);
							}
						}
					}
				}
				else
				{
					(*it)->setText(Value,value);
				}
			}
			++it;
		}
	}
}

void ByyEditEntityWidget::onEntityRemove( ByyEntityObject* entObj )
{
	if(entObj==myCurrentEntity)
	{
		close();
		deleteLater();
	}
}
