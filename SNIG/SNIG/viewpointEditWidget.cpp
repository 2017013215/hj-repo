#include "stdafx.h"
#include "viewpointEditWidget.h"

ByyViewpointEditWidget::ByyViewpointEditWidget( ByyIG& ig,QWidget * parent /*= 0*/, Qt::WindowFlags f /*= 0*/ )
	: QWidget(parent,f),needSave(false),myIG(ig),_srs(osgEarth::SpatialReference::create("wgs84"))
{
	ui.setupUi(this);

	ui.newButton->setIcon(QIcon(ICONPATH+"/EarthTool/new.png"));
	ui.openButton->setIcon(QIcon(ICONPATH+"/EarthTool/open.png"));
	ui.saveButton->setIcon(QIcon(ICONPATH+"/EarthTool/save.png"));
	ui.saveAsButton->setIcon(QIcon(ICONPATH+"/EarthTool/saveas.png"));
	ui.addButton->setIcon(QIcon(ICONPATH+"UtilityAdd.png"));
	ui.removeButton->setIcon(QIcon(ICONPATH+"UtilityMinus.png"));
	ui.moveUpButton->setIcon(QIcon(ICONPATH+"/EarthTool/moveup.png"));
	ui.moveDownButton->setIcon(QIcon(ICONPATH+"/EarthTool/movedown.png"));

	connect(ui.newButton,SIGNAL(clicked()),SLOT(newFile()));
	connect(ui.openButton,SIGNAL(clicked()),SLOT(openFile()));
	connect(ui.saveButton,SIGNAL(clicked()),SLOT(saveFile()));
	connect(ui.saveAsButton,SIGNAL(clicked()),SLOT(saveAsFile()));
	connect(ui.addButton,SIGNAL(clicked()),SLOT(add()));
	connect(ui.removeButton,SIGNAL(clicked()),SLOT(remove()));
	connect(ui.moveUpButton,SIGNAL(clicked()),SLOT(moveUp()));
	connect(ui.moveDownButton,SIGNAL(clicked()),SLOT(moveDown()));

	ui.moveUpButton->setVisible(false);
	ui.moveDownButton->setVisible(false);

	connect(ui.tableWidget,SIGNAL(cellClicked(int,int)),SLOT(cellClicked(int,int)));
}

ByyViewpointEditWidget::~ByyViewpointEditWidget()
{

}

void ByyViewpointEditWidget::newFile()
{
	if (okToContinue())
	{
		clear();

		currentFilename="";
		needSave=false;
	}
}

void ByyViewpointEditWidget::openFile()
{
	if (okToContinue())
	{
		QString fileName = QFileDialog::getOpenFileName(this, QString::fromLocal8Bit("打开文件"),
			VIEWPOINTPATH,
			tr("Viewpoint Records (*.vpr)"));

		if (fileName.isEmpty())
		{
			return;
		}

		ByyVprFile vprFile;

		QList<osgEarth::Viewpoint> vps;

		if (!vprFile.read(fileName,vps))
		{
			QMessageBox::warning(this,QString::fromLocal8Bit("提示"),QString::fromLocal8Bit("文件打开失败！"));
			return;
		}

		clear();

		for (int i=0;i!=vps.count();++i)
		{
			addToTable(vps[i]);
		}

		currentFilename=fileName;
		needSave=false;
	}
}

bool ByyViewpointEditWidget::saveFile()
{
	return currentFilename.isEmpty()?saveAsFile():saveFile(currentFilename);
}

bool ByyViewpointEditWidget::saveFile( const QString& filename )
{
	QList<osgEarth::Viewpoint> vps;

	for (int i=0;i!=ui.tableWidget->rowCount();++i)
	{
		vps.append(getViewpointFromTable(i));
	}

	ByyVprFile vprFile;

	if (!vprFile.write(filename,vps))
	{
		QMessageBox::warning(this,QString::fromLocal8Bit("提示"),QString::fromLocal8Bit("文件打开失败！"));
		return false;
	}
	
	needSave=false;

	return true;
}

bool ByyViewpointEditWidget::saveAsFile()
{
	QString filename = QFileDialog::getSaveFileName(this, QString::fromLocal8Bit("保存视点"),
		VIEWPOINTPATH,
		tr("Viewpoint Records (*.vpr)"));

	if (filename.isEmpty())
	{
		return false;
	}

	currentFilename=filename;

	 return saveFile(currentFilename);
}

void ByyViewpointEditWidget::add()
{
	ByyObserver *observer=myIG.inputController().currentObserver();
	if (observer)
	{
		osgEarth::Viewpoint vp=observer->getViewpoint();
		vp.name()="Viewpoint";
		addToTable(vp);

		needSave=true;
	}
}

void ByyViewpointEditWidget::remove()
{
	int row=ui.tableWidget->currentRow();

	if (row==-1)
	{
		return;
	}

	ui.tableWidget->removeRow(row);

	needSave=true;
}

void ByyViewpointEditWidget::moveUp()
{

}

void ByyViewpointEditWidget::moveDown()
{

}

bool ByyViewpointEditWidget::okToContinue()
{
	if (needSave)
	{
		int r=QMessageBox::warning(this,QString::fromLocal8Bit("提示"),QString::fromLocal8Bit("是否将更改保存到文件?"),QMessageBox::Yes|QMessageBox::No|QMessageBox::Cancel);

		if (r==QMessageBox::Cancel)
		{
			return false;
		}
		else if (r==QMessageBox::Yes)
		{
			return saveFile();
		}
	}
	
	return true;
}

void ByyViewpointEditWidget::closeEvent( QCloseEvent *e )
{
	okToContinue()?e->accept():e->ignore();
}

void ByyViewpointEditWidget::addToTable( const osgEarth::Viewpoint& vp )
{
	int row=ui.tableWidget->rowCount();
	ui.tableWidget->insertRow(row);

	QTableWidgetItem* item=new QTableWidgetItem(QString::fromUtf8(vp.name()->c_str()));
	ui.tableWidget->setItem(row,0,item);

	item=new QTableWidgetItem(QString::number(vp.focalPoint()->x(),'f',10));
	ui.tableWidget->setItem(row,1,item);

	item=new QTableWidgetItem(QString::number(vp.focalPoint()->y(),'f',10));
	ui.tableWidget->setItem(row,2,item);

	item=new QTableWidgetItem(QString::number(vp.focalPoint()->z(),'f'));
	ui.tableWidget->setItem(row,3,item);

	item=new QTableWidgetItem(QString::number(vp.getHeading(),'f'));
	ui.tableWidget->setItem(row,4,item);

	item=new QTableWidgetItem(QString::number(vp.getPitch(),'f'));
	ui.tableWidget->setItem(row,5,item);

	item=new QTableWidgetItem(QString::number(vp.getRange(),'f'));
	ui.tableWidget->setItem(row,6,item);
}

osgEarth::Viewpoint ByyViewpointEditWidget::getViewpointFromTable( int row )
{
	return osgEarth::Viewpoint(ui.tableWidget->item(row,0)->text().toUtf8().data(),
		ui.tableWidget->item(row,1)->text().toDouble(),
		ui.tableWidget->item(row,2)->text().toDouble(),
		ui.tableWidget->item(row,3)->text().toDouble(),
		ui.tableWidget->item(row,4)->text().toDouble(),
		ui.tableWidget->item(row,5)->text().toDouble(),
		ui.tableWidget->item(row,6)->text().toDouble()
		);
}

void ByyViewpointEditWidget::clear()
{
	while(ui.tableWidget->rowCount())
	{
		ui.tableWidget->removeRow(0);
	}
}

void ByyViewpointEditWidget::cellClicked( int row,int column )
{
	ByyObserver *observer=myIG.inputController().currentObserver();

	if (observer&&column==0)
	{
		observer->setViewpoint(getViewpointFromTable(row));
	}
}

bool ByyVprFile::read( const QString& filename,QList<osgEarth::Viewpoint>& out_vps )
{
	QFile file(filename);

	if (!file.open(QFile::ReadOnly))
	{
		return false;
	}

	QTextStream in(&file);

	in.setCodec("UTF-8");

	while(!in.atEnd())
	{
		QString _line=in.readLine();

		if (!_line.isEmpty())
		{
			osgEarth::Config conf;
			conf.fromJSON(_line.toUtf8().data());

			out_vps.append(osgEarth::Viewpoint(conf));
		}
	}

	file.close();

	return true;
}

bool ByyVprFile::write( const QString& filename,const QList<osgEarth::Viewpoint>& out_vps )
{
	QFile file(filename);

	if (!file.open(QFile::WriteOnly|QFile::Truncate))
	{
		return false;
	}

	QTextStream out(&file);

	out.setCodec("UTF-8");

	for (int i=0;i!=out_vps.count();++i)
	{
		out<<QString::fromUtf8(out_vps[i].getConfig().toJSON().c_str());
	}

	file.close();

	return true;
}
