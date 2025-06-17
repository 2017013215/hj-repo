#include "stdafx.h"
#include "cameraAnimationWidget.h"
#include "cameraAnimationComponent.h"

ByyCameraAnimationWidget::ByyCameraAnimationWidget( ByyIG& ig,QWidget *parent /*= 0*/ ) 
	: QWidget(parent),d_vf(new ByyCameraAnimationComponent(ig)),playItem(NULL),playItemIcon(ICONPATH+"EarthTool/movedown.png")
{
	ui.setupUi(this);

	ui.closePathButton->setIcon(QIcon(ICONPATH+"UtilityMinus.png"));
	ui.loadPathButton->setIcon(QIcon(ICONPATH+"import.png"));

	ui.resetButton->setIcon(QIcon(ICONPATH+"reset.png"));
	ui.playButton->setIcon(QIcon(ICONPATH+"play.png"));
	ui.stopButton->setIcon(QIcon(ICONPATH+"stop.png"));

	ui.resetButton->setVisible(false);

	connect(ui.closePathButton,SIGNAL(clicked()),SLOT(closePath()));
	connect(ui.loadPathButton,SIGNAL(clicked()),SLOT(loadPath()));

	connect(ui.playButton,SIGNAL(clicked()),SLOT(playOrPause()));
	connect(ui.stopButton,SIGNAL(clicked()),SLOT(stop()));
	connect(ui.resetButton,SIGNAL(clicked()),SLOT(reset()));

	connect(ui.checkBox,SIGNAL(clicked(bool)),SLOT(enablePath(bool)));
	connect(ui.checkBox_2,SIGNAL(clicked(bool)),SLOT(enablePoints(bool)));
	connect(ui.checkBox_3,SIGNAL(clicked(bool)),SLOT(enableLoop(bool)));

	connect(ui.treeWidget,SIGNAL(currentItemChanged(QTreeWidgetItem*,QTreeWidgetItem*)),SLOT(currentItemChanged(QTreeWidgetItem*,QTreeWidgetItem*)));
	connect(ui.treeWidget,SIGNAL(itemClicked(QTreeWidgetItem*,int)),SLOT(itemClicked(QTreeWidgetItem*,int)));

	connect(ui.tabWidget,SIGNAL(currentChanged(int)),SLOT(autoFlyChanged(int)));
	connect(ui.horizontalSlider,SIGNAL(valueChanged(int)),SLOT(autoFlyScaleChanged(int)));

	connect(ui.horizontalSlider_2,SIGNAL(valueChanged(int)),SLOT(speedChanged(int)));

	connect(d_vf,SIGNAL(currentPointChanged(int,int)),SLOT(currentPointChanged(int,int)));
	connect(d_vf,SIGNAL(finish()),SLOT(stop()));
}

ByyCameraAnimationWidget::~ByyCameraAnimationWidget()
{
	delete d_vf;
}

void ByyCameraAnimationWidget::closePath()
{
	QTreeWidgetItem *item=ui.treeWidget->currentItem();

	if (!item)
	{
		return;
	}

	if (item->parent())
	{
		item=item->parent();
	}

	int index=ui.treeWidget->indexOfTopLevelItem(item);

	d_vf->closePath(index);

	delete ui.treeWidget->takeTopLevelItem(index);

	updateTime();
}

void ByyCameraAnimationWidget::loadPath()
{
	QString fileName = QFileDialog::getOpenFileName(this, tr("Open File"),
		"../data/viewpoints",
		tr("Path Navi File (*.spn)"));

	if (!fileName.isEmpty())
	{
		ByyAnimationPath& path=d_vf->addPath(fileName.toStdString());

		QTreeWidgetItem *pathItem=new QTreeWidgetItem;

		pathItem->setText(0,QFileInfo(fileName).fileName());

		QVector<MyControlPoint>& ctrlPoints=path.getControlPointList();

		for (int i=0;i!=ctrlPoints.size();++i)
		{
			QTreeWidgetItem *item=new QTreeWidgetItem(pathItem);
			item->setText(0,QString::fromStdString(ctrlPoints[i].name));
		}

		ui.treeWidget->addTopLevelItem(pathItem);

		ui.treeWidget->setCurrentItem(pathItem);

		ui.treeWidget->expandAll();

		updateTime();
	}
}

void ByyCameraAnimationWidget::enablePath( bool b )
{
	d_vf->enablePath(b);
}

void ByyCameraAnimationWidget::enablePoints( bool b )
{
	d_vf->enablePoints(b);
}

void ByyCameraAnimationWidget::playOrPause()
{
	if (d_vf->running())
	{
		d_vf->pause();

		ui.playButton->setIcon(QIcon(ICONPATH+"play.png"));
	}
	else
	{
		d_vf->start();

		ui.stopButton->setEnabled(true);
		ui.loadPathButton->setEnabled(false);
		ui.closePathButton->setEnabled(false);

		ui.tabWidget->setTabEnabled(1,false);

		ui.playButton->setIcon(QIcon(ICONPATH+"pause.png"));

		if (playItem==NULL)
		{
			playItem=ui.treeWidget->topLevelItem(0)->child(0);

			playItem->setIcon(0,playItemIcon);

			emit runningChanged(true);
		}
	}
	
}

void ByyCameraAnimationWidget::stop()
{
	d_vf->pause();

	reset();

	ui.loadPathButton->setEnabled(true);
	ui.closePathButton->setEnabled(true);
	ui.stopButton->setEnabled(false);
	ui.tabWidget->setTabEnabled(1,true);

	ui.playButton->setIcon(QIcon(ICONPATH+"play.png"));

	if (playItem)
	{
		playItem->setIcon(0,QIcon());

		playItem=NULL;
	}

	runningChanged(false);
}

void ByyCameraAnimationWidget::reset()
{
	d_vf->reset();
}

void ByyCameraAnimationWidget::enableLoop( bool b )
{
	d_vf->enableLoop(b);
}

void ByyCameraAnimationWidget::currentItemChanged( QTreeWidgetItem * current, QTreeWidgetItem * previous )
{
	ui.closePathButton->setEnabled(current);
}

void ByyCameraAnimationWidget::updateTime()
{
	bool canFly=false;

	for (int i=0;i!=ui.treeWidget->topLevelItemCount();++i)
	{
		QTreeWidgetItem* topItem=ui.treeWidget->topLevelItem(i);

		canFly=canFly||d_vf->getAnimationPath(i).totalTime()>0;

		topItem->setText(1,QString::number(d_vf->getAnimationPath(i).totalTime(),'f',1));

		for (int j=0;j!=topItem->childCount();++j)
		{
			topItem->child(j)->setText(1,QString::number(d_vf->getAnimationPath(i).getControlPoint(j).time,'f',1));
		}
	}

	ui.resetButton->setEnabled(canFly);
	ui.playButton->setEnabled(canFly);
	//ui.stopButton->setEnabled(canFly);
}

void ByyCameraAnimationWidget::itemClicked( QTreeWidgetItem *item,int column )
{
	if (d_vf->running())
	{
		int path,point;

		if (item->parent())
		{
			point=item->parent()->indexOfChild(item);

			path=ui.treeWidget->indexOfTopLevelItem(item->parent());
		}
		else
		{
			path=ui.treeWidget->indexOfTopLevelItem(item);
			point=0;
		}
		d_vf->setCurrentPathPoint(path,point);
	}
}

void ByyCameraAnimationWidget::autoFlyChanged( int index )
{
	emit runningChanged(index);

	d_vf->setAutoFly(index==1);
}

void ByyCameraAnimationWidget::autoFlyScaleChanged( int value )
{
	d_vf->setAutoFlyScale(value/10.0);
	ui.label->setText(QString("%1x").arg(value/10.0,3));
}

void ByyCameraAnimationWidget::speedChanged( int value )
{
	d_vf->setMultiplier(value/10.0);
	ui.label_3->setText(QString("%1x").arg(value/10.0,3));
}

void ByyCameraAnimationWidget::currentPointChanged( int path,int point )
{
	ui.horizontalSlider_2->setValue(10);

	if (playItem)
	{
		playItem->setIcon(0,QIcon());
	}

	playItem=ui.treeWidget->topLevelItem(path)->child(point);

	playItem->setIcon(0,playItemIcon);
}

void ByyCameraAnimationWidget::stopAll()
{
	stop();
	ui.tabWidget->setCurrentIndex(0);
}
