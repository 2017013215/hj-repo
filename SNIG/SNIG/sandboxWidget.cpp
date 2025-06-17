#include "stdafx.h"
#include "sandboxWidget.h"
#include "sandboxPlacer.h"
#include "sandBoxComponent.h"

#include <osgEarthSymbology/TextSymbol>
#include <osgEarthSymbology/IconSymbol>
#include <osg/Depth>

ByySandboxWidget::ByySandboxWidget( ByyIG& ig,QWidget *parent /*= 0*/, Qt::WFlags flags /*= 0*/ ) 
	: QWidget(parent, flags),
	myIG(ig),
	d_sandbox(new ByySandBoxComponent(ig))
{
	ui.setupUi(this);

	QHash<int,SandboxElement>& sandboxElements=d_sandbox->elementsHash();

	QHash<int,SandboxElement>::iterator i=sandboxElements.begin();
	for (;i!=sandboxElements.end();++i)
	{
		ElementToolbutton *elementButton=new ElementToolbutton(i.value(),32);

		connect(elementButton,SIGNAL(clicked()),this,SLOT(add2D()));

		d_eleButtons.append(elementButton);
	}

	QVector<ElementForceType> forceTypes=ByyMappingQuery::instance()->getForecTypes();

	QVector<SandboxCategory> categories=ByyMappingQuery::instance()->getElementCategories();

	initTree(forceTypes,categories);

	connect(d_sandbox,SIGNAL(initialized()),SLOT(init()));
	connect(d_sandbox,SIGNAL(released()),SLOT(release()));

	ByySandboxDisplayControlWidget *displayControlWidget=new ByySandboxDisplayControlWidget(forceTypes,categories,d_sandbox,ui.tab_2);

	ui.verticalLayout_3->insertWidget(1,displayControlWidget);

	connect(d_sandbox,SIGNAL(mouseDown(osg::Node*)),this,SLOT(itemClickedFrom3D(osg::Node*)),Qt::QueuedConnection);

	moveItemAction=new QAction(QString::fromLocal8Bit("移动"),this);
	removeItemAction=new QAction(QString::fromLocal8Bit("删除"),this);
	renameItemAction=new QAction(QString::fromLocal8Bit("重命名"),this);

	connect(moveItemAction,SIGNAL(triggered()),SLOT(move()));
	connect(removeItemAction,SIGNAL(triggered()),SLOT(deleteItem()));
	connect(renameItemAction,SIGNAL(triggered()),SLOT(renameItem()));
	
	connect(ui.radioButton,SIGNAL(clicked()),this,SLOT(updateKind()));
	connect(ui.radioButton_2,SIGNAL(clicked()),this,SLOT(updateKind()));

	connect(ui.treeWidget,SIGNAL(currentItemChanged(QTreeWidgetItem*,QTreeWidgetItem*)),SLOT(update1()));

	connect(ui.listWidget,SIGNAL(currentRowChanged(int)),SLOT(listCurrentRowChanged(int)));
	connect(ui.listWidget,SIGNAL(itemDoubleClicked(QListWidgetItem*)),SLOT(itemDoubleClicked(QListWidgetItem*)));

	connect(ui.pushButton,SIGNAL(clicked()),SLOT(deleteItem()));
	connect(ui.pushButton_2,SIGNAL(clicked()),SLOT(renameItem()));
	connect(ui.pushButton_3,SIGNAL(clicked()),SLOT(move()));

	connect(ui.doubleSpinBox,SIGNAL(valueChanged(double)),SLOT(scaleIcon(double)));
}

ByySandboxWidget::~ByySandboxWidget()
{
	delete d_sandbox;
}

void ByySandboxWidget::initTree( QVector<ElementForceType> &forceTypes,QVector<SandboxCategory>& categories )
{
	QTreeWidgetItem *biaoPeiTopItem=new QTreeWidgetItem(ui.treeWidget,0);
	biaoPeiTopItem->setText(0,QString::fromLocal8Bit("标配军标"));
	biaoPeiTopItem->setData(0,Qt::UserRole,BiaoPei);

	QTreeWidgetItem *xiangXingTopItem=new QTreeWidgetItem(ui.treeWidget,0);
	xiangXingTopItem->setText(0,QString::fromLocal8Bit("象形军标"));
	xiangXingTopItem->setData(0,Qt::UserRole,XiangXing);

	QMap<int,QTreeWidgetItem*> biaoPeiForceTypeItems,xiangXingForceTypeItems;

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

void ByySandboxWidget::deleteItem()
{
	int row=ui.listWidget->currentRow();

	if (row==-1)
		return;

	ui.listWidget->takeItem(row);

	d_sandbox->removeChild(row);
}

void ByySandboxWidget::renameItem()
{
	int row=ui.listWidget->currentRow();

	if (row==-1)
		return;

	bool ok;
	QString _text = QInputDialog::getText(this, QString::fromLocal8Bit("输入"),
		QString::fromLocal8Bit("标签:"), QLineEdit::Normal,ui.listWidget->item(row)->text(), &ok);

	if (ok)
	{
		ui.listWidget->item(row)->setText(_text);

		d_sandbox->rename(row,_text);
	}
}

void ByySandboxWidget::add2D()
{
	emit placeStarted();
	ElementToolbutton *btn=dynamic_cast<ElementToolbutton*>(sender());

	SandboxElement& _element=btn->getElement();

	osgEarth::Symbology::Style& d_placeStyle=d_sandbox->getPlaceStyle();

	d_placeStyle.getOrCreate<osgEarth::Symbology::IconSymbol>()->url()->setLiteral(_element.iconFilename.toUtf8().data());
	d_placeStyle.getOrCreate<osgEarth::Symbology::IconSymbol>()->scale()=ui.doubleSpinBox->value();

	osgEarth::Annotation::PlaceNode *placeNode=new osgEarth::Annotation::PlaceNode(myIG.terrain()->mapNode(),osgEarth::GeoPoint(myIG.terrain()->mapNode()->getMapSRS(),-117,32),
		_element.name.toUtf8().data(),d_placeStyle);

	//osg::StateSet *stateSet=placeNode->getAttachPoint()->getChild(0)->asGeode()->getOrCreateStateSet();

	//osg::Depth *depth=dynamic_cast<osg::Depth*>(stateSet->getAttribute(osg::StateAttribute::DEPTH));

	//depth->setRange(0.5,1.0);

	//stateSet->setRenderingHint( osg::StateSet::TRANSPARENT_BIN );

	myIG.renderer()->addNodeToRoot(placeNode,ByyOsgRenderer::SceneRoot);

	ByySandBoxPlacer *placer=new ByySandBoxPlacer(placeNode);
	myIG.inputController().addEventProcessor(placer);

	connect(placer,SIGNAL(finish(int)),this,SLOT(placeFinished(int)),Qt::QueuedConnection);

	d_currentElement=&_element;
}

void ByySandboxWidget::placeFinished( int result )
{
	ByySandBoxPlacer* placer=dynamic_cast<ByySandBoxPlacer*>(sender());

	myIG.inputController().removeEventProcessor(placer);
	placer->deleteLater();

	osg::ref_ptr<osgEarth::Annotation::PlaceNode> _placeNode=dynamic_cast<osgEarth::Annotation::PlaceNode*>(placer->getNode());

	if (result==0)
		return;


	if (d_currentElement==NULL)
		return;

	//osg::StateSet *stateSet=_placeNode->getAttachPoint()->getChild(0)->asGeode()->getOrCreateStateSet();
	//stateSet->removeAttribute(osg::StateAttribute::DEPTH);
	//stateSet->setRenderingHint( osg::StateSet::TRANSPARENT_BIN );

	//osgEarth::Decluttering::setEnabled( _placeNode->getOrCreateStateSet(), true);

	QString _text=d_currentElement->name;

	bool ok;
	_text = QInputDialog::getText(this, QString::fromLocal8Bit("输入"),
		QString::fromLocal8Bit("标签:"), QLineEdit::Normal,_text, &ok);

	if(ok)
	{
		myIG.renderer()->removeNodeFromRoot(_placeNode,ByyOsgRenderer::SceneRoot);
		_placeNode->setText(_text.toUtf8().data());

		QListWidgetItem *newItem=new QListWidgetItem(_text);
		ui.listWidget->addItem(newItem);

		d_sandbox->addChild(_placeNode,_text,d_currentElement->id);
	}
	else
		myIG.renderer()->removeNodeFromRoot(_placeNode,ByyOsgRenderer::SceneRoot);
}

void ByySandboxWidget::move()
{
	int row=ui.listWidget->currentRow();

	if (row==-1)
		return;

	ByySandBoxPlacer *placer=new ByySandBoxPlacer(dynamic_cast<osgEarth::Annotation::GeoPositionNode*>(d_sandbox->getChild(row)));
	myIG.inputController().addEventProcessor(placer);
	connect(placer,SIGNAL(finish(int)),SLOT(moveFinish(int)),Qt::QueuedConnection);
}

void ByySandboxWidget::moveFinish( int result )
{
	ByySandBoxPlacer* placer=dynamic_cast<ByySandBoxPlacer*>(sender());

	myIG.inputController().removeEventProcessor(placer);

	placer->deleteLater();

	if (result==1)
	{
		d_sandbox->moved();
	}
}

void ByySandboxWidget::init()
{
	QVector<SandboxInfo>& _sandboxInfos=d_sandbox->getSandboxInfos();

	for (int i=0;i!=_sandboxInfos.size();++i)
	{
		QListWidgetItem *item=new QListWidgetItem(_sandboxInfos[i].name);

		ui.listWidget->addItem(item);
	}
}

void ByySandboxWidget::release()
{
	ui.listWidget->clear();
}

void ByySandboxWidget::scaleIcon( double spinValue )
{
	d_sandbox->setScale(spinValue);
}


void ByySandboxWidget::updateKind()
{
	ui.treeWidget->topLevelItem(0)->setHidden(!ui.radioButton->isChecked());
	ui.treeWidget->topLevelItem(1)->setHidden(ui.radioButton->isChecked());

	ui.treeWidget->topLevelItem(0)->setExpanded(true);
	ui.treeWidget->topLevelItem(1)->setExpanded(true);

	update1();
}

void ByySandboxWidget::update1()
{
	int index=0;

	foreach(ElementToolbutton* btn,d_eleButtons)
	{
		ui.gridLayout->removeWidget(btn);

		bool _show=needShow(btn->getElement());

		if (_show)
		{
			ui.gridLayout->addWidget(btn,index/2,index%2);
			++index;
		}
		btn->setVisible(_show);
	}

	QVector<SandboxInfo>& sandboxInfos=d_sandbox->getSandboxInfos();

	for (int i=0;i!=ui.listWidget->count();++i)
	{
		ui.listWidget->item(i)->setHidden(!needShow(*sandboxInfos[i]._element));
	}
}

bool ByySandboxWidget::needShow( SandboxElement& element )
{
	bool _show=ui.radioButton->isChecked()?element.category.kindType==BiaoPei:element.category.kindType==XiangXing;

	QTreeWidgetItem* item=ui.treeWidget->currentItem();

	if (item)
	{
		switch(item->type())
		{
		case 2:
			_show=_show&&element.category.id==item->data(0,Qt::UserRole).toInt();
			item=item->parent();

		case 1:
			_show=_show&&element.category.forceType==item->data(0,Qt::UserRole).toInt();
			break;
		}
	}

	return _show;
}

void ByySandboxWidget::listCurrentRowChanged( int row )
{
	ui.pushButton->setEnabled(row!=-1);
	ui.pushButton_2->setEnabled(row!=-1);
	ui.pushButton_3->setEnabled(row!=-1);
}

void ByySandboxWidget::itemClickedFrom3D( osg::Node* node )
{
	int index=d_sandbox->getChildIndex(node);

	if (index==ui.listWidget->count())
		return;

	emit placeStarted();

	ui.listWidget->setCurrentRow(index);

	QMenu menu;

	menu.addAction(moveItemAction);
	menu.addAction(removeItemAction);
	menu.addAction(renameItemAction);

	menu.exec(QCursor::pos());
}

void ByySandboxWidget::itemDoubleClicked( QListWidgetItem *item )
{
	int row=ui.listWidget->row(item);

	osgEarth::GeoPoint position=dynamic_cast<osgEarth::Annotation::GeoPositionNode*>(d_sandbox->getChild(row))->getPosition();

	ByyObserver *observer=myIG.inputController().currentObserver();

	if (observer)
	{
		osgEarth::Viewpoint vp=observer->getViewpoint();

		vp.focalPoint()->x()=position.x();
		vp.focalPoint()->y()=position.y();

		observer->setViewpoint(vp);
	}
}

void ByySandboxWidget::showEvent( QShowEvent *e )
{
	d_sandbox->setContextMenuEnabled(true);
	QWidget::showEvent(e);
}

void ByySandboxWidget::hideEvent( QHideEvent *e )
{
	d_sandbox->setContextMenuEnabled(false);
	QWidget::hideEvent(e);
}

ByySandboxForceDisplay::ByySandboxForceDisplay( ElementForceType& forceType,QVector<SandboxCategory>& categories,ByySandBoxComponent *sandbox ) :d_sandbox(sandbox)
{
	ui.setupUi(this);

	QPalette p=palette();

	p.setColor(QPalette::WindowText,forceType.color);

	setPalette(p);

	forceTypeId=forceType.id;

	ui.checkBox->setText(forceType.name);

	int biaoPeiIndex=0,xiangXingIndex=0;

	for (int i=0;i!=categories.count();++i)
	{
		if (forceType.id==categories[i].forceType)
		{
			QCheckBox *cb=new QCheckBox(categories[i].name);

			cb->setChecked(true);

			if (categories[i].kindType==BiaoPei)
			{
				ui.gridLayout->addWidget(cb,biaoPeiIndex/2,biaoPeiIndex%2);
				++biaoPeiIndex;
			}
			else
			{
				ui.gridLayout_2->addWidget(cb,xiangXingIndex/2,xiangXingIndex%2);
				++xiangXingIndex;
			}

			categoryCheckBoxMap.insert(cb,categories[i].id);

			connect(cb, SIGNAL(clicked(bool)),SLOT(categoryClicked(bool)));
		}
	}
	connect(ui.checkBox,SIGNAL(clicked(bool)),SLOT(forceTypeClicked(bool)));
}

void ByySandboxForceDisplay::forceTypeClicked( bool checked )
{
	QVector<SandboxInfo>& sbInfos=d_sandbox->getSandboxInfos();

	for (int i=0;i!=sbInfos.count();++i)
	{
		if (sbInfos[i]._element->category.forceType!=forceTypeId)
			continue;

		d_sandbox->setEnabled(i,checked);
	}

	QMap<QCheckBox*,int>::Iterator it=categoryCheckBoxMap.begin();

	for(;it!=categoryCheckBoxMap.end();++it)
	{
		it.key()->setChecked(checked);
	}

	ui.checkBox->setTristate(false);
}

void ByySandboxForceDisplay::categoryClicked( bool checked )
{
	QVector<SandboxInfo>& sbInfos=d_sandbox->getSandboxInfos();

	QCheckBox* cb=dynamic_cast<QCheckBox*>(sender());

	int categoryid=categoryCheckBoxMap[cb];

	for (int i=0;i!=sbInfos.count();++i)
	{
		if (sbInfos[i]._element->category.forceType!=forceTypeId)
			continue;

		if (sbInfos[i]._element->category.id!=categoryid)
			continue;

		d_sandbox->setEnabled(i,checked);
	}

	int checkedCount=0;

	QMap<QCheckBox*,int>::Iterator it=categoryCheckBoxMap.begin();

	for(;it!=categoryCheckBoxMap.end();++it)
	{
		if (it.key()->isChecked())
		{
			++checkedCount;
		}
	}

	if (checkedCount==0)
	{
		ui.checkBox->setCheckState(Qt::Unchecked);
	}
	else if (checkedCount==categoryCheckBoxMap.size())
	{
		ui.checkBox->setCheckState(Qt::Checked);
	}
	else
	{
		ui.checkBox->setCheckState(Qt::PartiallyChecked);
	}
}

ByySandboxDisplayControlWidget::ByySandboxDisplayControlWidget( QVector<ElementForceType> &forceTypes,QVector<SandboxCategory>& categories,ByySandBoxComponent *sandbox,QWidget* parent/*=NULL*/ ) :QWidget(parent)
{
	QVBoxLayout *_layout=new QVBoxLayout;
	setLayout(_layout);

	_layout->setContentsMargins(0,0,0,0);

	for (int i=0;i!=forceTypes.count();++i)
	{
		ByySandboxForceDisplay *display=new ByySandboxForceDisplay(forceTypes[i],categories,sandbox);

		_layout->addWidget(display);
		_layout->addStretch();
	}
}
