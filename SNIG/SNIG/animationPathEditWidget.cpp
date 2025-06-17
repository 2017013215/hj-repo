#include "stdafx.h"
#include "animationPathEditWidget.h"

ByyAnimationPathEditWidget::ByyAnimationPathEditWidget( ByyIG& ig,QWidget * parent /*= 0*/, Qt::WindowFlags f /*= 0*/ )
	: QWidget(parent,f),needSave(false),myIG(ig)
{
	ui.setupUi(this);

	ui.newButton->setIcon(QIcon(ICONPATH+"/EarthTool/new.png"));
	ui.openButton->setIcon(QIcon(ICONPATH+"/EarthTool/open.png"));
	ui.saveButton->setIcon(QIcon(ICONPATH+"/EarthTool/save.png"));
	ui.saveAsButton->setIcon(QIcon(ICONPATH+"/EarthTool/saveas.png"));
	ui.addButton->setIcon(QIcon(ICONPATH+"UtilityAdd.png"));
	ui.removeButton->setIcon(QIcon(ICONPATH+"UtilityMinus.png"));

	connect(ui.newButton,SIGNAL(clicked()),SLOT(newFile()));
	connect(ui.openButton,SIGNAL(clicked()),SLOT(openFile()));
	connect(ui.saveButton,SIGNAL(clicked()),SLOT(saveFile()));
	connect(ui.saveAsButton,SIGNAL(clicked()),SLOT(saveAsFile()));
	connect(ui.addButton,SIGNAL(clicked()),SLOT(add()));
	connect(ui.removeButton,SIGNAL(clicked()),SLOT(remove()));

	connect(ui.tableWidget,SIGNAL(cellClicked(int,int)),SLOT(cellClicked(int,int)));
}

ByyAnimationPathEditWidget::~ByyAnimationPathEditWidget()
{

}

void ByyAnimationPathEditWidget::newFile()
{
	if (okToContinue())
	{
		clear();

		currentFilename="";
		needSave=false;
	}
}

void ByyAnimationPathEditWidget::openFile()
{
	if (okToContinue())
	{
		QString fileName = QFileDialog::getOpenFileName(this, tr("Open File"),
			VIEWPOINTPATH,
			tr("Path Navi File (*.spn)"));

		if (fileName.isEmpty())
		{
			return;
		}

		ByyAnimationPath _animationPath;

		if (!_animationPath.read(fileName.toStdString()))
		{
			QMessageBox::warning(this,QString::fromLocal8Bit("提示"),QString::fromLocal8Bit("文件打开失败！"));
			return;
		}

		clear();

		QVector<MyControlPoint>& cpList=_animationPath.getControlPointList();

		for (int i=0;i!=cpList.count();++i)
		{
			addToTable(cpList[i]);
		}

		currentFilename=fileName;
		needSave=false;
	}
}

void ByyAnimationPathEditWidget::clear()
{
	while(ui.tableWidget->rowCount())
	{
		ui.tableWidget->removeRow(0);
	}
}

bool ByyAnimationPathEditWidget::saveFile()
{
	return currentFilename.isEmpty()?saveAsFile():saveFile(currentFilename);
}

bool ByyAnimationPathEditWidget::saveFile( const QString& filename )
{
	ByyAnimationPath _animationPath;

	for (int i=0;i!=ui.tableWidget->rowCount();++i)
	{
		_animationPath.push_back(getControlPointFromTable(i));
	}

	if (!_animationPath.write(filename.toStdString()))
	{
		QMessageBox::warning(this,QString::fromLocal8Bit("提示"),QString::fromLocal8Bit("文件打开失败！"));
		return false;
	}

	needSave=false;

	return true;
}

bool ByyAnimationPathEditWidget::saveAsFile()
{
	QString filename = QFileDialog::getSaveFileName(this, tr("Save File"),
		VIEWPOINTPATH,
		tr("Path Navi File (*.spn)"));

	if (filename.isEmpty())
	{
		return false;
	}

	currentFilename=filename;

	return saveFile(currentFilename);
}

void ByyAnimationPathEditWidget::add()
{
	osg::Matrixd m=myIG.inputController().currentObserver()->manip()->getMatrix();

	MyControlPoint cp;

	cp.name=QString("Point%1").arg(ui.tableWidget->rowCount()).toStdString();

	cp.time=ui.tableWidget->rowCount()==0?0:10;

	cp.ctrlPoints=osg::AnimationPath::ControlPoint(m.getTrans(), m.getRotate());

	addToTable(cp);
}

void ByyAnimationPathEditWidget::remove()
{
	int row=ui.tableWidget->currentRow();

	if (row==-1)
	{
		return;
	}

	ui.tableWidget->removeRow(row);

	needSave=true;
}

bool ByyAnimationPathEditWidget::okToContinue()
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

void ByyAnimationPathEditWidget::closeEvent( QCloseEvent *e )
{
	okToContinue()?e->accept():e->ignore();
}

void ByyAnimationPathEditWidget::addToTable( const MyControlPoint& cp )
{
	int row=ui.tableWidget->rowCount();

	ui.tableWidget->insertRow(row);

	QTableWidgetItem *item=new QTableWidgetItem(QString::fromStdString(cp.name));

	QVariant cpVar;
	cpVar.setValue(cp.ctrlPoints);

	item->setData(Qt::UserRole,cpVar);

	ui.tableWidget->setItem(row,0,item);

	item=new QTableWidgetItem(QString::number(cp.time));
	ui.tableWidget->setItem(row,1,item);

}

MyControlPoint ByyAnimationPathEditWidget::getControlPointFromTable( int row )
{
	MyControlPoint _result;

	_result.name=ui.tableWidget->item(row,0)->text().toStdString();
	_result.ctrlPoints=ui.tableWidget->item(row,0)->data(Qt::UserRole).value<osg::AnimationPath::ControlPoint>();
	_result.time=ui.tableWidget->item(row,1)->text().toDouble();

	return _result;
}

void ByyAnimationPathEditWidget::cellClicked( int row,int column )
{
	if (column==0)
	{
		osg::AnimationPath::ControlPoint ctrlPoints=ui.tableWidget->item(row,0)->data(Qt::UserRole).value<osg::AnimationPath::ControlPoint>();

		osg::Matrixd _matrix;

		_matrix.makeRotate(ctrlPoints.getRotation());
		_matrix.postMultTranslate(ctrlPoints.getPosition());
	}
}
