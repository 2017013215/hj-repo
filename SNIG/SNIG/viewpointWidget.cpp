#include "stdafx.h"
#include "viewpointWidget.h"
#include "viewpointEditWidget.h"

ByyViewpointWidget::ByyViewpointWidget(ByyIG& ig,QWidget *parent)
	: QWidget(parent),myIG(ig)
{
	ui.setupUi(this);

	QPalette palette;
	palette.setColor(QPalette::Background, QColor(255,255,255));
	setPalette(palette);

	ui.pushButton_2->setIcon(QIcon("../data/icons/UtilityMinus.png"));
	ui.pushButton_3->setIcon(QIcon("../data/icons/import.png"));

	connect(ui.pushButton_2,SIGNAL(clicked()),SLOT(remove()));
	connect(ui.pushButton_3,SIGNAL(clicked()),SLOT(importFile()));

	connect(ui.listWidget,SIGNAL(currentRowChanged(int)),SLOT(manualListCurrentChanged(int)),Qt::QueuedConnection);
	connect(ui.listWidget,SIGNAL(itemClicked(QListWidgetItem*)),SLOT(itemClicked(QListWidgetItem*)));

//	loadFile("../data/viewpoints/tw.vpr");
}

ByyViewpointWidget::~ByyViewpointWidget()
{

}

void ByyViewpointWidget::remove()
{
	int row=ui.listWidget->currentRow();

	if (row==-1)
	{
		return;
	}

	manualViewpoints.remove(row);

	delete ui.listWidget->takeItem(row);
}

void ByyViewpointWidget::importFile()
{
	QString fileName = QFileDialog::getOpenFileName(this, QString::fromLocal8Bit("导入视点"),
		VIEWPOINTPATH,
		tr("Viewpoint Records (*.vpr)"));

	if (fileName.isEmpty())
	{
		return;
	}

	loadFile(fileName);

}

void ByyViewpointWidget::addToManual( osgEarth::Viewpoint& vp )
{
	manualViewpoints.push_back(vp);

	QListWidgetItem *item=new QListWidgetItem(QString::fromUtf8(vp.name()->c_str()));
	//item->setFlags(item->flags()|Qt::ItemIsEditable);
	ui.listWidget->addItem(item);
}

void ByyViewpointWidget::manualListCurrentChanged( int current)
{
	ui.pushButton_2->setEnabled(current!=-1);
}

void ByyViewpointWidget::itemClicked( QListWidgetItem *item )
{
	emit viewpointClicked();

	ByyObserver *observer=myIG.inputController().currentObserver();

	if (observer)
	{
		observer->setViewpoint(manualViewpoints[ui.listWidget->row(item)]);
	}
}

void ByyViewpointWidget::loadFile( const QString& fileName )
{
	ByyVprFile vprFile;

	QList<osgEarth::Viewpoint> vps;

	if (!vprFile.read(fileName,vps))
	{
		QMessageBox::warning(this,QString::fromLocal8Bit("提示"),QString::fromLocal8Bit("文件打开失败！"));
		return;
	}

	for (int i=0;i!=vps.size();++i)
	{
		addToManual(vps[i]);
	}
}
