#include "stdafx.h"
#include "sandboxWidget.h"
#include "sceneController.h"
#include "sandboxPlacer.h"

#include <sandboxConfig.h>

#include <osgEarthAnnotation/PlaceNode>
#include <osgEarthAnnotation/ModelNode>
#include <osgEarth/MapNode>

ByySandboxWidget::ByySandboxWidget(ByySceneController *scene,QWidget *parent)
	:d_scene(scene)
{
	d_root=new osg::Group;
	d_scene->addChild(d_root);
}

ByySandboxWidget::~ByySandboxWidget()
{

}

void ByySandboxWidget::init()
{
	release();

	osgEarth::Config& externals=d_scene->getMapNode()->externalConfig();

	osgEarth::Config sandboxs=externals.child("sandboxs");
	const osgEarth::ConfigSet& children = sandboxs.children();

	for(osgEarth::ConfigSet::const_iterator i = children.begin(); i != children.end(); ++i )
	{
		if (!i->empty())
		{
			QString _elementName=QString::fromUtf8(i->value("element").c_str());

			if (!d_elementsHash.contains(_elementName))
				continue;

			SandboxElement* _element=d_elementsHash[_elementName];


			d_placeStyle.getOrCreate<osgEarth::Symbology::IconSymbol>()->url()->setLiteral(_element->iconfilename.toLocal8Bit().data() );
			d_placeStyle.getOrCreate<osgEarth::Symbology::IconSymbol>()->scale()=d_scaled;

			osgEarth::GeoPoint _pos(i->child("position"));
			osgEarth::Annotation::PlaceNode *placeNode=new osgEarth::Annotation::PlaceNode(d_scene->getMapNode(),osgEarth::GeoPoint(d_scene->getMapNode()->getMapSRS(),_pos.x(),_pos.y(),_pos.alt()),i->value("name").c_str(), d_placeStyle);
			placeNode->setDynamic(true);

			d_root->addChild(placeNode);

			osgEarth::Decluttering::setEnabled( placeNode->getOrCreateStateSet(), true ,80);

			QListWidgetItem* _item=new QListWidgetItem(QString::fromUtf8(i->value("name").c_str()));
			_item->setData(Qt::UserRole,_element->name);

			ui.listWidget->addItem(_item);
		}
	}
}

void ByySandboxWidget::release()
{
	d_root->removeChildren(0,d_root->getNumChildren());
	ui.listWidget->clear();
}

void ByySandboxWidget::writeToMapNode()
{	
	osgEarth::Config& externals=d_scene->getMapNode()->externalConfig();

	externals.remove("sandboxs");

	osgEarth::Config newConf("sandboxs");

	for (int i=0;i!=ui.listWidget->count();++i)
	{
		osgEarth::Config _sangboxConf("sandbox");

		_sangboxConf.add("name",ui.listWidget->item(i)->text().toUtf8().data());
		_sangboxConf.add("element",ui.listWidget->item(i)->data(Qt::UserRole).toString().toUtf8().data());
		
		osgEarth::Annotation::PlaceNode *_place=dynamic_cast<osgEarth::Annotation::PlaceNode*>(d_root->getChild(i));

		_sangboxConf.addObj("position",_place->getPosition());

		newConf.add(_sangboxConf);
	}
	externals.add(newConf);
}
#include <osg/Depth>
void ByySandboxWidget::add2D()
{
	if (!d_scene->getMapNode())
		return;

	SandboxElement *element=NULL;

	QToolButton *button=dynamic_cast<QToolButton*>(sender());

	if (button)
	{
		element=d_ElementsMap[button];
	}
	else
		return;

	d_placeStyle.getOrCreate<osgEarth::Symbology::IconSymbol>()->url()->setLiteral( element->iconfilename.toLocal8Bit().data() );
	d_placeStyle.getOrCreate<osgEarth::Symbology::IconSymbol>()->scale()=d_scaled;

	osgEarth::Annotation::PlaceNode *placeNode=new osgEarth::Annotation::PlaceNode(d_scene->getMapNode(),osgEarth::GeoPoint(d_scene->getMapNode()->getMapSRS(),-117.17, 32.72),element->name.toUtf8().data(), d_placeStyle);

	d_root->addChild(placeNode);

	ByySandBoxPlacer *placer=new ByySandBoxPlacer(placeNode);
	d_scene->getViewer()->addEventHandler(placer);
	connect(placer,SIGNAL(finish(int)),SLOT(placeFinish(int)),Qt::QueuedConnection);

	d_currentElement=element;
}

void ByySandboxWidget::placeFinish( int result )
{
	osg::ref_ptr<ByySandBoxPlacer> placer=dynamic_cast<ByySandBoxPlacer*>(sender());

	if (!placer)
		return;

	d_scene->getViewer()->removeEventHandler(placer);

	osg::ref_ptr<osgEarth::Annotation::PlaceNode> _placeNode=dynamic_cast<osgEarth::Annotation::PlaceNode*>(placer->getNode());

	if (result==0)
	{
		d_root->removeChild(_placeNode);
		return;
	}

	if (d_currentElement==NULL)
		return;

	

	osgEarth::Decluttering::setEnabled( _placeNode->getOrCreateStateSet(), true ,80);

	QString _text=d_currentElement->name;


	bool ok;
	_text = QInputDialog::getText(this, QString::fromLocal8Bit("输入"),
		QString::fromLocal8Bit("标签:"), QLineEdit::Normal,_text, &ok);


	if (ok)
	{
		_placeNode->setText(_text.toUtf8().data());
	}

	QListWidgetItem* _item=new QListWidgetItem(_text);
	_item->setData(Qt::UserRole,d_currentElement->name);

	ui.listWidget->addItem(_item);

	emit modified();
}

void ByySandboxWidget::move()
{
	int row=ui.listWidget->currentRow();

	if (row==-1)
		return;

	ByySandBoxPlacer *placer=new ByySandBoxPlacer(dynamic_cast<osgEarth::Annotation::PositionedAnnotationNode*>(d_root->getChild(row)));
	d_scene->getViewer()->addEventHandler(placer);
	connect(placer,SIGNAL(finish(int)),SLOT(moveFinish(int)),Qt::QueuedConnection);
}

void ByySandboxWidget::remove()
{
	int row=ui.listWidget->currentRow();

	if (row==-1)
		return;

	int r=QMessageBox::warning(0,QString::fromLocal8Bit("提示"),QString::fromLocal8Bit("确认删除吗？"),QMessageBox::Yes|QMessageBox::No);

	if (r!=QMessageBox::Yes)
	{
		return;
	}

	d_root->removeChild(row);

	delete ui.listWidget->takeItem(row);

	emit modified();
}

void ByySandboxWidget::rename()
{
	int row=ui.listWidget->currentRow();

	if (row==-1)
		return;

	bool ok;
	QString _text = QInputDialog::getText(this, QString::fromLocal8Bit("输入"),
		QString::fromLocal8Bit("标签:"), QLineEdit::Normal,ui.listWidget->item(row)->text(), &ok);

	if (ok)
	{
		osgEarth::Annotation::PlaceNode* _place=dynamic_cast<osgEarth::Annotation::PlaceNode*>(d_root->getChild(row));
		if (_place)
		{
			_place->setText(_text.toUtf8().data());
		}
		
		ui.listWidget->item(row)->setText(_text);

		emit modified();
	}
}

void ByySandboxWidget::moveFinish( int result )
{
	osg::ref_ptr<ByySandBoxPlacer> placer=dynamic_cast<ByySandBoxPlacer*>(sender());

	if (!placer)
		return;

	d_scene->getViewer()->removeEventHandler(placer);
}

void ByySandboxWidget::itemChanged()
{
	QStringList _hideForceType=ui.qxtCheckComboBox_3->checkedItems();
	QStringList _hideModelType=ui.qxtCheckComboBox_4->checkedItems();

	for (int i=0;i!=ui.listWidget->count();++i)
	{
		if (_hideForceType.contains(d_elementsHash[ui.listWidget->item(i)->data(Qt::UserRole).toString()]->forceType)||
			_hideModelType.contains(d_elementsHash[ui.listWidget->item(i)->data(Qt::UserRole).toString()]->modelType))
		{
			ui.listWidget->item(i)->setHidden(false);
			d_root->getChild(i)->setNodeMask(true);
		}
		else
		{
			ui.listWidget->item(i)->setHidden(true);
			d_root->getChild(i)->setNodeMask(false);
		}
	}
}

void ByySandboxWidget::processElementChanged()
{
	QStringList _hideForceType=ui.qxtCheckComboBox_3->checkedItems();
	QStringList _hideModelType=ui.qxtCheckComboBox_4->checkedItems();

	for (QMap<QToolButton*,SandboxElement*>::iterator it=d_ElementsMap.begin();it!=d_ElementsMap.end();++it)
	{
		if (_hideForceType.contains((*it)->forceType)&&
			_hideModelType.contains((*it)->modelType))
		{
			it.key()->setVisible(true);
		}
		else
		{
			it.key()->setVisible(false);
			//d_toolButtonList.removeOne(it.key());
		}
	}
}

void ByySandboxWidget::processScale( double value )
{
	for (int i=0;i!=d_root->getNumChildren();++i)
	{
		osgEarth::Annotation::PlaceNode *_placeNode=dynamic_cast<osgEarth::Annotation::PlaceNode*>(d_root->getChild(i));

		if (_placeNode)
		{
			osgEarth::Symbology::Style _style=_placeNode->getStyle();

			_style.getOrCreate<osgEarth::Symbology::IconSymbol>()->scale()=value;

			_placeNode->setStyle(_style);
		}
	}
}
