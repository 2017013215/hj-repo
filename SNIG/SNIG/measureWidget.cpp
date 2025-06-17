#include "stdafx.h"
#include "measureWidget.h"
#include "annotationDialogs.h"
#include "annotationComponent.h"
#include "geoMath.h"

#include <osgEarth/Utils>
#include <osgEarth/ElevationQuery>

/*
template <class T>
static T* getItemData(const QTreeWidgetItem *item)
{
	return reinterpret_cast<T*>(item->data(0, Qt::UserRole).toInt());
}*/

ByyMeasureWidget::ByyMeasureWidget( ByyIG& ig,QWidget *parent /*= 0*/ ) 
	: QWidget(parent),
	myIG(ig),
	d_annoComponent(new ByyAnnotationComponent(ig)),
	currentColorIcon(ICONPATH+"arrow_right.png")
{
	ui.setupUi(this);

	connect(d_annoComponent,SIGNAL(initialized()),SLOT(init()));
	connect(d_annoComponent,SIGNAL(released()),SLOT(release()));

	ui.pathButton->setIcon(QIcon("../data/icons/draw_line_bg.png"));
	ui.polygonButton->setIcon(QIcon("../data/icons/draw_poly_bg.png"));
	ui.ellipseButton->setIcon(QIcon("../data/icons/draw_circle_bg.png"));

	QVector<QColor> colorList(9);

	colorList[0]=Qt::red;
	colorList[1]=QColor(255,165,0);
	colorList[2]=Qt::yellow;
	colorList[3]=Qt::green;
	colorList[4]=Qt::cyan;
	colorList[5]=Qt::blue;
	colorList[6]=Qt::magenta;
	colorList[7]=Qt::white;
	colorList[8]=Qt::black;

	for (int i=0;i!=colorList.size();++i)
	{
		QListWidgetItem* item=new QListWidgetItem;

		item->setBackground(QBrush(colorList[i]));

		ui.listWidget->addItem(item);
	}

	connect(ui.listWidget,SIGNAL(currentItemChanged(QListWidgetItem*,QListWidgetItem*)),SLOT(currentColorItemChanged(QListWidgetItem*,QListWidgetItem*)));
	connect(ui.listWidget,SIGNAL(itemClicked(QListWidgetItem*)),SLOT(colorItemClicked(QListWidgetItem*)));

	pathTopItem=new QTreeWidgetItem(0);
	pathTopItem->setText(0,QString::fromLocal8Bit("Ïß¶Î"));
	
	polygonTopItem=new QTreeWidgetItem(0);
	polygonTopItem->setText(0,QString::fromLocal8Bit("¶à±ßÐÎ"));

	ellipseTopItem=new QTreeWidgetItem(0);
	ellipseTopItem->setText(0,QString::fromLocal8Bit("Ô²ÐÎ"));

	ui.treeWidget->addTopLevelItem(pathTopItem);
	ui.treeWidget->addTopLevelItem(polygonTopItem);
	ui.treeWidget->addTopLevelItem(ellipseTopItem);

	pathTopItem->setFlags(Qt::ItemIsEnabled);
	polygonTopItem->setFlags(Qt::ItemIsEnabled);
	ellipseTopItem->setFlags(Qt::ItemIsEnabled);

	connect(d_annoComponent,SIGNAL(mouseDown(osg::Node*)),this,SLOT(itemClickedFrom3D(osg::Node*)),Qt::QueuedConnection);

	connect(ui.pathButton,SIGNAL(clicked()),SLOT(addPath()));
	connect(ui.polygonButton,SIGNAL(clicked()),SLOT(addPolygon()));
	connect(ui.ellipseButton,SIGNAL(clicked()),SLOT(addEllipse()));

	editAction= new QAction(QIcon("../data/icons/edit.png"),QString::fromLocal8Bit("±à¼­"),this);
	removeAction= new QAction(QIcon("../data/icons/UtilityMinus.png"),QString::fromLocal8Bit("É¾³ý"),this);

	connect(editAction,SIGNAL(triggered()),SLOT(edit()),Qt::QueuedConnection);
	connect(removeAction,SIGNAL(triggered()),SLOT(remove()),Qt::QueuedConnection);

	connect(ui.treeWidget,SIGNAL(itemClicked(QTreeWidgetItem*,int)),SLOT(slot_itemClicked(QTreeWidgetItem*,int)));

	connect(ui.treeWidget,SIGNAL(itemChanged(QTreeWidgetItem*,int)),SLOT(itemChanged(QTreeWidgetItem*,int)));

	ui.treeWidget->installEventFilter(this);
}

ByyMeasureWidget::~ByyMeasureWidget()
{
	delete d_annoComponent;
}

void ByyMeasureWidget::init()
{
	release();

	osg::Group *annoRoot=d_annoComponent->annoGroup();

	for (int i=0;i!=annoRoot->getNumChildren();++i)
	{
		osg::Node* node=annoRoot->getChild(i);

		osgEarth::Annotation::AnnotationNode *annoNode=dynamic_cast<osgEarth::Annotation::AnnotationNode*>(node);

		addItem(annoNode);
	}

	ui.treeWidget->expandAll();
}

void ByyMeasureWidget::release()
{
	_annoNodeMap.clear();

	qDeleteAll(pathTopItem->takeChildren());
	qDeleteAll(polygonTopItem->takeChildren());
	qDeleteAll(ellipseTopItem->takeChildren());
}

osgEarth::QtGui::ViewVector Views2QtViewVector(osgViewer::ViewerBase::Views& views)
{
	osgEarth::QtGui::ViewVector rel(views.size());

	for (int i=0;i!=rel.size();++i)
	{
		rel[i]=views[i];
	}

	return rel;
}

void ByyMeasureWidget::addPath()
{
	d_annoComponent->setLabelVisible(false);
	d_annoComponent->setPathHeightEnabled(false);

	_activeDialog = new AddPathDialog(myIG.renderer()->getControlObjectRoot(), myIG.terrain()->mapNode(), Views2QtViewVector(myIG.display().getViews()));

	setEnables(false);

	connect(_activeDialog, SIGNAL(finished(int)), this, SLOT(onAddFinished(int)));

	_activeDialog->setWindowTitle(tr(" "));
	_activeDialog->setWindowFlags(Qt::Tool | Qt::WindowTitleHint | Qt::CustomizeWindowHint| Qt::WindowStaysOnTopHint);
	_activeDialog->setAttribute(Qt::WA_DeleteOnClose);
	//_activeDialog->move(mapToGlobal(QPoint(0,0)));
	ui.verticalLayout->addWidget(_activeDialog);
	//_activeDialog->show();
}

void ByyMeasureWidget::addPolygon()
{
	d_annoComponent->setLabelVisible(false);
	d_annoComponent->setPathHeightEnabled(false);

	_activeDialog = new AddPolygonDialog(myIG.renderer()->getControlObjectRoot(), myIG.terrain()->mapNode(), Views2QtViewVector(myIG.display().getViews()));

	setEnables(false);

	connect(_activeDialog, SIGNAL(finished(int)), this, SLOT(onAddFinished(int)));

	_activeDialog->setWindowTitle(tr(" "));
	_activeDialog->setWindowFlags(Qt::Tool | Qt::WindowTitleHint | Qt::CustomizeWindowHint| Qt::WindowStaysOnTopHint);
	_activeDialog->setAttribute(Qt::WA_DeleteOnClose);
	//_activeDialog->move(mapToGlobal(QPoint(0,0)));
	ui.verticalLayout->addWidget(_activeDialog);
	//_activeDialog->show();
}

void ByyMeasureWidget::addEllipse()
{
	d_annoComponent->setLabelVisible(false);
	d_annoComponent->setPathHeightEnabled(false);

	_activeDialog = new AddEllipseDialog(myIG.renderer()->getControlObjectRoot(), myIG.terrain()->mapNode(), Views2QtViewVector(myIG.display().getViews()));

	setEnables(false);

	connect(_activeDialog, SIGNAL(finished(int)), this, SLOT(onAddFinished(int)));

	_activeDialog->setWindowTitle(tr(" "));
	_activeDialog->setWindowFlags(Qt::Tool | Qt::WindowTitleHint | Qt::CustomizeWindowHint| Qt::WindowStaysOnTopHint);
	_activeDialog->setAttribute(Qt::WA_DeleteOnClose);
	//_activeDialog->move(mapToGlobal(QPoint(0,0)));
	ui.verticalLayout->addWidget(_activeDialog);
	//_activeDialog->show();
}

void ByyMeasureWidget::edit()
{
	QTreeWidgetItem *item=ui.treeWidget->currentItem();

	if (!item||!_annoNodeMap.contains(item))
		return;

	osgEarth::Annotation::AnnotationNode* node=_annoNodeMap[item];

	osgEarth::Annotation::FeatureNode* featureNode = dynamic_cast<osgEarth::Annotation::FeatureNode*>(node);

	if (featureNode)
	{
		const osgEarth::Features::Feature* feat = featureNode->getFeature();

		if (feat)
		{
			const osgEarth::Symbology::LineString* pathLine = dynamic_cast<const osgEarth::Symbology::LineString*>(feat->getGeometry());

			if (pathLine)
			{
				_activeDialog = new AddPathDialog(featureNode->getParent(0), myIG.terrain()->mapNode(), Views2QtViewVector(myIG.display().getViews()), featureNode);
			}
			else
			{
				const osgEarth::Symbology::Polygon* polygon = dynamic_cast<const osgEarth::Symbology::Polygon*>(feat->getGeometry());
				if (polygon)
				{
					_activeDialog = new AddPolygonDialog(featureNode->getParent(0), myIG.terrain()->mapNode(), Views2QtViewVector(myIG.display().getViews()), featureNode);
				}
			}
		}
	}
	else
	{
		osgEarth::Annotation::EllipseNode* ellipse = dynamic_cast<osgEarth::Annotation::EllipseNode*>(node);
		if (ellipse)
		{
			_activeDialog = new AddEllipseDialog(ellipse->getParent(0), myIG.terrain()->mapNode(), Views2QtViewVector(myIG.display().getViews()), ellipse);
		}
	}

	if (!_activeDialog.isNull())
	{
		setEnables(false);

		connect(_activeDialog, SIGNAL(finished(int)), this, SLOT(onEditFinished(int)));

		_activeDialog->setWindowTitle(tr(" "));
		_activeDialog->setWindowFlags(Qt::Tool | Qt::WindowTitleHint | Qt::CustomizeWindowHint| Qt::WindowStaysOnTopHint);
		_activeDialog->setAttribute(Qt::WA_DeleteOnClose);
		//_activeDialog->move(mapToGlobal(QPoint(0,0)));
		ui.verticalLayout->addWidget(_activeDialog);
		//_activeDialog->show();

		d_annoComponent->setLabelVisible(false);
		d_annoComponent->setPathHeightEnabled(false);
	}
}

void ByyMeasureWidget::remove()
{
	QTreeWidgetItem *item=ui.treeWidget->currentItem();

	if (!item)
		return;

	d_annoComponent->removeChild(_annoNodeMap[item]);

	_annoNodeMap.remove(item);

	item->parent()->removeChild(item);

	delete item;

	d_annoComponent->setLabelVisible(false);
	d_annoComponent->setPathHeightEnabled(false);

	clickCurrentItem=NULL;
}

void ByyMeasureWidget::onAddFinished( int result )
{
	setEnables(true);

	if (result == QDialog::Accepted)
	{
		osgEarth::Annotation::AnnotationNode* annotation = _activeDialog->getAnnotation();

		if (annotation)
		{
			d_annoComponent->addChild(annotation);

			ui.treeWidget->setCurrentItem(addItem(annotation));

			slot_itemClicked(ui.treeWidget->currentItem(),TextColumn,false);
		}
	}
	else
	{
		showOrHideLabel(ui.treeWidget->currentItem());
	}
}

void ByyMeasureWidget::onEditFinished( int result )
{
	setEnables(true);

	if (result == QDialog::Accepted)
	{
		d_annoComponent->setChanged();

		QTreeWidgetItem* item=ui.treeWidget->currentItem();

		osgEarth::Annotation::AnnotationNode* node=_annoNodeMap[item];

		item->setText(0,QString::fromUtf8(node->getName().c_str()));

		d_annoComponent->updateLabel(node);

		slot_itemClicked(ui.treeWidget->currentItem(),TextColumn,false);
	}
	else
	{
		showOrHideLabel(ui.treeWidget->currentItem());
	}
}

void ByyMeasureWidget::setEnables( bool enable )
{
	ui.pathButton->setEnabled(enable);
	ui.polygonButton->setEnabled(enable);
	ui.ellipseButton->setEnabled(enable);
	ui.treeWidget->setEnabled(enable);
}

void ByyMeasureWidget::slot_itemClicked( QTreeWidgetItem *item,int column/*=TextColumn*/,bool scaleTo/*=true*/ )
{
	if (item->type()==0)
	{
		return;
	}

	emit itemClicked();

	clickCurrentItem=item;

	osgEarth::Viewpoint vp;

	if (scaleTo)
	{
		vp=getItemCenter(item);
	}

	std::vector<osg::Vec3d> positionVec;

	osgEarth::Annotation::FeatureNode* _currentPathHeightNode=NULL;


	osgEarth::Symbology::Color _color;

	if (item->parent()==pathTopItem)
	{
		osgEarth::Annotation::FeatureNode* node=dynamic_cast<osgEarth::Annotation::FeatureNode*>(_annoNodeMap[item]);
		positionVec=node->getFeature()->getGeometry()->asVector();

		positionVec.erase(positionVec.begin());

		_color=node->getStyle().get<osgEarth::Symbology::LineSymbol>()->stroke()->color();
	}
	else
	{
		if (!scaleTo)
		{
			vp=getItemCenter(item);
		}

		positionVec.push_back(vp.focalPoint()->vec3d());

		_color=_annoNodeMap[item]->getStyle().get<osgEarth::Symbology::PolygonSymbol>()->fill()->color();
	}

	if (scaleTo)
	{
		ByyObserver *observer=myIG.inputController().currentObserver();

		if (observer)
		{
			observer->setViewpoint(vp);
		}
	}

	d_annoComponent->setLabelText(ByyGeoMath::getAnnotationLabelText(_annoNodeMap[item]));

	d_annoComponent->setLabelPosition(positionVec);

	showOrHideLabel(item);

	setCurrentColor(_color);
}

void ByyMeasureWidget::setCurrentColor( const osgEarth::Symbology::Color& color )
{
	QColor qColor;

	qColor.setRgbF(color.x(),color.y(),color.z());

	for (int i=0;i!=ui.listWidget->count();++i)
	{
		if (ui.listWidget->item(i)->background().color()==qColor)
		{
			ui.listWidget->setCurrentRow(i);
			break;
		}
	}
}

void ByyMeasureWidget::showOrHideLabel( QTreeWidgetItem* item )
{
	d_annoComponent->setLabelVisible(item&&item->checkState(TextColumn)==Qt::Checked);

	d_annoComponent->setPathHeightEnabled(item&&item->checkState(TextColumn)==Qt::Checked&&(item->parent()==pathTopItem||item->parent()==polygonTopItem),dynamic_cast<osgEarth::Annotation::FeatureNode*>(_annoNodeMap[item]));
}

osgEarth::Viewpoint ByyMeasureWidget::getItemCenter( QTreeWidgetItem * item )
{
	osgEarth::Annotation::AnnotationNode* node=_annoNodeMap[item];

	osgEarth::Annotation::EllipseNode* ellipse=dynamic_cast<osgEarth::Annotation::EllipseNode*>(node);

	osgEarth::Viewpoint vp;

	ByyObserver *observer=myIG.inputController().currentObserver();

	if (observer)
	{
		vp=observer->getViewpoint();
	}

	if (ellipse)
	{
		osgEarth::GeoPoint position=ellipse->getPosition();

		vp.focalPoint()->x()=position.x();
		vp.focalPoint()->y()=position.y();

		vp.setRange(position.z()+ellipse->getRadiusMajor().as(osgEarth::Units::METERS)*2.5);
	}
	else
	{
		osgEarth::Annotation::FeatureNode* featureNode = dynamic_cast<osgEarth::Annotation::FeatureNode*>(node);

		if (featureNode)
		{
			const osgEarth::Symbology::Geometry* geom=featureNode->getFeature()->getGeometry();
			osgEarth::Bounds bounds=geom->getBounds();

			vp.focalPoint()->vec3d().x()=bounds.center2d().x();
			vp.focalPoint()->vec3d().y()=bounds.center2d().y();

			std::vector< double >  out_elevations; 

			osgEarth::ElevationQuery _query(myIG.terrain()->getMap());

			_query.getElevations(geom->asVector(),myIG.terrain()->getMap()->getSRS(),out_elevations);

			std::sort(out_elevations.begin(),out_elevations.end());

			osg::ref_ptr<osg::Vec3dArray> _boundsArray=new osg::Vec3dArray;
			_boundsArray->push_back(osg::Vec3d(bounds.xMin(),bounds.yMax(),0));
			_boundsArray->push_back(osg::Vec3d(bounds.xMax(),bounds.yMax(),0));
			_boundsArray->push_back(osg::Vec3d(bounds.xMax(),bounds.yMin(),0));
			_boundsArray->push_back(osg::Vec3d(bounds.xMin(),bounds.yMin(),0));

			if (geom->getType()==geom->TYPE_LINESTRING)
			{
				double _distance=osgEarth::GeoMath::distance(geom->asVector());

				vp.setRange(out_elevations.back()+_distance*1.5);
			}
			else
			{
				double boundsArea=ByyGeoMath::calcPolygonArea(_boundsArray->asVector());

				vp.setRange(out_elevations.back()+sqrt(boundsArea)*2.5);
			}
		}
	}

	return vp;
}

void ByyMeasureWidget::itemChanged( QTreeWidgetItem * item,int column )
{
	printf("itemChanged\n");

	osgEarth::Annotation::AnnotationNode *node=_annoNodeMap[item];

	if (item->text(TextColumn)!=QString::fromUtf8(node->getName().c_str()))
	{
		node->setName(item->text(TextColumn).toUtf8().data());
		d_annoComponent->setChanged();
	}
	else
	{
		node->setNodeMask(item->checkState(TextColumn)==Qt::Checked);

		if (item==clickCurrentItem)
		{
			showOrHideLabel(item);
		}
	}
}

QTreeWidgetItem* ByyMeasureWidget::addItem( osgEarth::Annotation::AnnotationNode *annoNode )
{
	QTreeWidgetItem *item=new QTreeWidgetItem(1);
	//item->setData(TextColumn,Qt::UserRole,(int)annoNode);
	item->setText(TextColumn,QString::fromUtf8(annoNode->getName().c_str()));
	item->setCheckState(TextColumn,Qt::Checked);
	item->setFlags(item->flags()|Qt::ItemIsEditable);

	_annoNodeMap.insert(item,annoNode);


	osgEarth::Annotation::FeatureNode* featureNode = dynamic_cast<osgEarth::Annotation::FeatureNode*>(annoNode);

	if (featureNode)
	{
		const osgEarth::Features::Feature* feat = featureNode->getFeature();

		if (feat)
		{
			const osgEarth::Symbology::LineString* pathLine = dynamic_cast<const osgEarth::Symbology::LineString*>(feat->getGeometry());

			if (pathLine)
			{
				pathTopItem->addChild(item);
			}
			else
			{
				const osgEarth::Symbology::Polygon* polygon = dynamic_cast<const osgEarth::Symbology::Polygon*>(feat->getGeometry());
				if (polygon)
				{
					polygonTopItem->addChild(item);
				}
			}
		}
	}
	else
	{
		osgEarth::Annotation::EllipseNode* ellipse = dynamic_cast<osgEarth::Annotation::EllipseNode*>(annoNode);
		if (ellipse)
		{
			ellipseTopItem->addChild(item);
		}
	}

	return item;
}

bool ByyMeasureWidget::eventFilter( QObject *obj, QEvent *event )
{
	if (obj==ui.treeWidget&&ui.treeWidget->isEnabled()&&event->type()==QEvent::ContextMenu)
	{
		QTreeWidgetItem *item=ui.treeWidget->itemAt(ui.treeWidget->viewport()->mapFromGlobal(QCursor::pos()));

		if (item&&item->type()==1)
		{
			showContextMenu();
		}
	}

	return QWidget::eventFilter(obj,event);
}

void ByyMeasureWidget::itemClickedFrom3D( osg::Node* node )
{
	if (!ui.treeWidget->isEnabled())
		return;

	for (int i=0;i!=ui.treeWidget->topLevelItemCount();++i)
	{
		QTreeWidgetItem *top=ui.treeWidget->topLevelItem(i);

		for (int j=0;j!=top->childCount();++j)
		{
			QTreeWidgetItem *item=top->child(j);

			if (node==_annoNodeMap[item])
			{
				ui.treeWidget->setCurrentItem(item);

				showContextMenu();

				break;
			}
		}
	}
}

void ByyMeasureWidget::showEvent( QShowEvent *e )
{
	d_annoComponent->setConextMenuEnabled(true);
	QWidget::showEvent(e);
}

void ByyMeasureWidget::hideEvent( QHideEvent *e )
{
	d_annoComponent->setLabelVisible(false);
	d_annoComponent->setPathHeightEnabled(false);
	d_annoComponent->setConextMenuEnabled(false);
	QWidget::hideEvent(e);
}

void ByyMeasureWidget::showContextMenu()
{
	QMenu *_menu=new QMenu;
	_menu->setAttribute(Qt::WA_DeleteOnClose);

	_menu->addAction(editAction);
	_menu->addAction(removeAction);

	_menu->popup(QCursor::pos());
}

void ByyMeasureWidget::colorItemClicked( QListWidgetItem * item )
{
	QTreeWidgetItem *treeItem=ui.treeWidget->currentItem();

	if (treeItem==NULL||treeItem->type()!=1)
		return;

	QColor color=item->background().color();
	osgEarth::Symbology::Color _color(color.redF(),color.greenF(),color.blueF());

	osgEarth::Annotation::AnnotationNode* node=_annoNodeMap[treeItem];

	osgEarth::Symbology::Style _style=node->getStyle();

	if (treeItem->parent()==pathTopItem)
	{
		_style.getOrCreate<osgEarth::Symbology::LineSymbol>()->stroke()->color()=_color;
	}
	else if (treeItem->parent())
	{
		_style.getOrCreate<osgEarth::Symbology::PolygonSymbol>()->fill()->color()=osgEarth::Symbology::Color(_color,0.5);
	}
	
	node->setStyle(_style);

	d_annoComponent->setChanged();
}

void ByyMeasureWidget::currentColorItemChanged( QListWidgetItem * current, QListWidgetItem * previous )
{
	if (current)
	{
		current->setIcon(currentColorIcon);
	}

	if(previous)
	{
		previous->setIcon(QIcon());
	}
}
