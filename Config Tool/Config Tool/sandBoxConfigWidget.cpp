#include "sandBoxConfigWidget.h"

#include <QFileDialog>
#include <QInputDialog>
#include <QMessageBox>
#include <QToolButton>

#define COLUMN 4

ByySandBoxConfigWidget::ByySandBoxConfigWidget(QWidget *parent)
	: QWidget(parent),d_lastName("../data/sandbox")
{
	ui.setupUi(this);

	addElementButton=new QToolButton;
	addElementButton->setIcon(QIcon("../data/icons/UtilityAdd.png"));
	addElementButton->setIconSize(QSize(64,64));
	addElementButton->setToolButtonStyle(Qt::ToolButtonTextUnderIcon);
	addElementButton->setAutoRaise(true);

	elementEditAction=new QAction(QString::fromLocal8Bit("编辑"),this);

	elementRemoveAction=new QAction(QString::fromLocal8Bit("删除"),this);
	
	categoryAddAction=new QAction(QString::fromLocal8Bit("添加类型"),this);
	categoryRemoveAction=new QAction(QString::fromLocal8Bit("删除"),this);
	categoryRenameAction=new QAction(QString::fromLocal8Bit("重命名"),this);

	//elementMenu.addAction(elementEditAction);
	elementMenu.addAction(elementRemoveAction);

	init();

	connect(categoryAddAction,SIGNAL(triggered()),SLOT(addCategory()));
	connect(categoryRemoveAction,SIGNAL(triggered()),SLOT(removeCategory()));
	connect(categoryRenameAction,SIGNAL(triggered()),SLOT(renameCategory()));

	connect(addElementButton,SIGNAL(clicked()),SLOT(addClicked()));
	connect(ui.treeWidget,SIGNAL(currentItemChanged(QTreeWidgetItem*,QTreeWidgetItem*)),SLOT(update1()));

	connect(ui.radioButton,SIGNAL(clicked()),SLOT(updateKind()));
	connect(ui.radioButton_2,SIGNAL(clicked()),SLOT(updateKind()));

	ui.treeWidget->installEventFilter(this);
}

ByySandBoxConfigWidget::~ByySandBoxConfigWidget()
{

}

void ByySandBoxConfigWidget::init()
{
	QVector<SandboxElement> es=ByyMappingQuery::instance()->getSandboxElements();

	for(int i=0;i!=es.size();++i)
	{
		add(es[i]);
	}

	QTreeWidgetItem *biaoPeiTopItem=new QTreeWidgetItem(ui.treeWidget,0);
	biaoPeiTopItem->setText(0,QString::fromLocal8Bit("标配军标"));
	biaoPeiTopItem->setData(0,Qt::UserRole,BiaoPei);

	QTreeWidgetItem *xiangXingTopItem=new QTreeWidgetItem(ui.treeWidget,0);
	xiangXingTopItem->setText(0,QString::fromLocal8Bit("象形军标"));
	xiangXingTopItem->setData(0,Qt::UserRole,XiangXing);

	QMap<int,QTreeWidgetItem*> biaoPeiForceTypeItems,xiangXingForceTypeItems;

	QVector<ElementForceType> forceTypes=ByyMappingQuery::instance()->getForecTypes();

	for (int i=0;i!=forceTypes.size();++i)
	{
		QTreeWidgetItem *biaoPeiItem=new QTreeWidgetItem(biaoPeiTopItem,1);
		biaoPeiItem->setText(0,forceTypes[i].name);
		biaoPeiItem->setData(0,Qt::UserRole,forceTypes[i].id);

		biaoPeiForceTypeItems.insert(forceTypes[i].id,biaoPeiItem);

		QTreeWidgetItem *xiangXingItem=new QTreeWidgetItem(xiangXingTopItem,1);
		xiangXingItem->setText(0,forceTypes[i].name);
		xiangXingItem->setData(0,Qt::UserRole,forceTypes[i].id);

		xiangXingForceTypeItems.insert(forceTypes[i].id,xiangXingItem);
	}

	QVector<SandboxCategory> categories=ByyMappingQuery::instance()->getElementCategories();

	for(int i=0;i!=categories.size();++i)
	{
		QTreeWidgetItem* item=new QTreeWidgetItem(2);

		item->setText(0,categories.at(i).name);
		item->setData(0,Qt::UserRole,categories[i].id);

		if (categories[i].kindType==BiaoPei)
		{
			biaoPeiForceTypeItems[categories[i].forceType]->addChild(item);
		}
		else
		{
			xiangXingForceTypeItems[categories[i].forceType]->addChild(item);
		}
	}

	updateKind();
}

void ByySandBoxConfigWidget::add( SandboxElement& e )
{
	ElementToolbutton *elementButton=new ElementToolbutton(e);
	elementButton->setMenu(&elementMenu);

	connect(elementButton,SIGNAL(triggered(QAction*)),this,SLOT(elementTriggered(QAction*)));
	connect(elementButton,SIGNAL(clicked()),elementButton,SLOT(showMenu()));

	d_elementGroup.push_back(elementButton);
}

void ByySandBoxConfigWidget::addClicked()
{
/*	ByySandBoxEditDialog dlg(this,ui.comboBox->model(),ui.comboBox_2->model());

	if (dlg.exec()!=QDialog::Accepted)
	{
		return;
	}

	SandboxElement e=dlg.getElement();

	e.elementType=ui.radioButton->isChecked()?BiaoPei:XiangXing;

	e.id=ByyMappingQuery::instance()->addSandboxElement(e);

	add(e);

	update1();

	emit modified();*/

	if (!ui.treeWidget->currentItem()||ui.treeWidget->currentItem()->type()!=2)
	{
		return;
	}

	QString iconFileName=QFileDialog::getOpenFileName(this,tr("OpenFile"),d_lastName,
		tr("Image(*.png)"));

	if(iconFileName.isEmpty())
		return;

	d_lastName=iconFileName;

	iconFileName=QDir("../data").exists(iconFileName)?QDir(".").relativeFilePath(iconFileName):iconFileName;


	bool ok;
	QString text = QInputDialog::getText(this, QString::fromLocal8Bit("输入名称"),
		QString::fromLocal8Bit("名称:"), QLineEdit::Normal,QFileInfo(iconFileName).fileName(), &ok);

	if (!ok || text.isEmpty())
		return;

	SandboxElement se;

	se.name=text;
	se.iconFilename=iconFileName;

	se.category.id=ui.treeWidget->currentItem()->data(0,Qt::UserRole).toInt();

	se.category.forceType=ui.treeWidget->currentItem()->parent()->data(0,Qt::UserRole).toInt();
	se.category.kindType=ui.treeWidget->currentItem()->parent()->parent()->data(0,Qt::UserRole).toInt();

	se.id=ByyMappingQuery::instance()->addSandboxElement(se);

	add(se);

	update1();

	emit modified();
}

void ByySandBoxConfigWidget::elementTriggered( QAction *action )
{
	ElementToolbutton *_btn=dynamic_cast<ElementToolbutton*>(sender());

	if (action==elementEditAction)
	{
		elementEdit(_btn);
	}
	else if (action==elementRemoveAction)
	{
		remove(_btn);
	}
}

void ByySandBoxConfigWidget::elementEdit( ElementToolbutton *_btn )
{
/*	ByySandBoxEditDialog dlg(this,ui.comboBox->model(),ui.comboBox_2->model(),_btn->getElement());

	if (dlg.exec()==QDialog::Accepted)
	{
		ByyMappingQuery::instance()->changeSandboxElement(dlg.getElement());

		_btn->setElement(dlg.getElement());

		update1();
	}*/
}

void ByySandBoxConfigWidget::remove( ElementToolbutton *_btn )
{
	ByyMappingQuery::instance()->removeSandboxElement(_btn->getElement().id);

	d_elementGroup.removeOne(_btn);

	_btn->deleteLater();

	update1();

	emit modified();
}

void ByySandBoxConfigWidget::updateKind()
{
	ui.treeWidget->topLevelItem(0)->setHidden(!ui.radioButton->isChecked());
	ui.treeWidget->topLevelItem(1)->setHidden(ui.radioButton->isChecked());

	ui.treeWidget->topLevelItem(0)->setExpanded(true);
	ui.treeWidget->topLevelItem(1)->setExpanded(true);

	update1();
}

void ByySandBoxConfigWidget::update1()
{
	int index=0;

	ui.gridLayout->removeWidget(addElementButton);

	foreach(ElementToolbutton* btn,d_elementGroup)
	{
		ui.gridLayout->removeWidget(btn);

		bool _show=needShow(btn);

		if (_show)
		{
			ui.gridLayout->addWidget(btn,index/COLUMN,index%COLUMN);
			++index;
		}
		btn->setVisible(_show);
	}

	QTreeWidgetItem* item=ui.treeWidget->currentItem();

	if(item&&item->type()==2)
	{
		ui.gridLayout->addWidget(addElementButton,index/COLUMN,index%COLUMN);
	}

	addElementButton->setVisible(item&&item->type()==2);
}

bool ByySandBoxConfigWidget::needShow( ElementToolbutton* btn )
{
	bool _show=ui.radioButton->isChecked()?btn->kindType()==BiaoPei:btn->kindType()==XiangXing;

	QTreeWidgetItem* item=ui.treeWidget->currentItem();

	if (item)
	{
		switch(item->type())
		{
		case 2:
			_show=_show&&btn->category()==item->data(0,Qt::UserRole).toInt();
			item=item->parent();

		case 1:
			_show=_show&&btn->forceType()==item->data(0,Qt::UserRole).toInt();
			break;
		}
	}

	return _show;
}

bool ByySandBoxConfigWidget::eventFilter( QObject *obj, QEvent *event )
{
	if (obj==ui.treeWidget&&event->type()==QEvent::ContextMenu)
	{
		QTreeWidgetItem *item=ui.treeWidget->itemAt(ui.treeWidget->viewport()->mapFromGlobal(QCursor::pos()));

		if (item)
		{
			QMenu treeWidgetMenu;

			if (item->type()==1)
			{
				treeWidgetMenu.addAction(categoryAddAction);
			}
			else if (item->type()==2)
			{
				treeWidgetMenu.addAction(categoryRemoveAction);
				treeWidgetMenu.addAction(categoryRenameAction);
			}
			treeWidgetMenu.exec(QCursor::pos());
		}
	}

	return QWidget::eventFilter(obj,event);
}

void ByySandBoxConfigWidget::addCategory()
{
	QTreeWidgetItem* currentItem=ui.treeWidget->currentItem();

	if (currentItem==NULL||currentItem->type()!=1)
	{
		return;
	}

	bool ok;

	QString categoryName=QInputDialog::getText(this,QString::fromLocal8Bit("添加类型"),
		QString::fromLocal8Bit("类型名称"),
		QLineEdit::Normal,QString(""),&ok);

	if(ok&&!categoryName.isEmpty())
	{
		SandboxCategory category;

		category.name=categoryName;
		category.forceType=currentItem->data(0,Qt::UserRole).toInt();
		category.kindType=currentItem->parent()->data(0,Qt::UserRole).toInt();

		int id=ByyMappingQuery::instance()->addCategory(category);

		QTreeWidgetItem* item=new QTreeWidgetItem(currentItem,2);

		item->setText(0,categoryName);
		item->setData(0,Qt::UserRole,id);

		currentItem->setExpanded(true);
	}
}

void ByySandBoxConfigWidget::removeCategory()
{
	QTreeWidgetItem* item=ui.treeWidget->currentItem();

	if (item==NULL||item->type()!=2)
		return;

	int categoryId=item->data(0,Qt::UserRole).toInt();

	if (ByyMappingQuery::instance()->getElementCount(categoryId)>0)
	{
		QMessageBox::warning(this,QString::fromLocal8Bit("提示"),QString::fromLocal8Bit("该类型包含军标,不能删除！"));
		return;
	}

	ByyMappingQuery::instance()->removeCategory(categoryId);

	delete item;
}

void ByySandBoxConfigWidget::renameCategory()
{
	QTreeWidgetItem* item=ui.treeWidget->currentItem();

	if (item==NULL||item->type()!=2)
		return;

	int armyId=item->data(0,Qt::UserRole).toInt();

	bool ok;

	QString categoryName=QInputDialog::getText(this,QString::fromLocal8Bit("编辑类型"),
		QString::fromLocal8Bit("类型名称"),
		QLineEdit::Normal,item->text(0),&ok);

	if(ok&&!categoryName.isEmpty())
	{
		ByyMappingQuery::instance()->changeCategory(armyId,categoryName);


		item->setText(0,categoryName);
	}
}

