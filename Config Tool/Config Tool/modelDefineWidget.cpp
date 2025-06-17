#include "modelDefineWidget.h"
#include "mappingQuery.h"
#include <QInputDialog>
#include <QMessageBox>
#include <QFileDialog>

#define ModelRole Qt::UserRole 
#define IconRole Qt::UserRole+1 
#define IdRole Qt::UserRole+2 

ByyModelDefineWidget::ByyModelDefineWidget(QWidget *parent)
	: QWidget(parent)
{
	ui.setupUi(this);

	ui.pushButton_1->setIcon(QIcon("../data/icons/UtilityAdd.png"));
	ui.pushButton_2->setIcon(QIcon("../data/icons/UtilityMinus.png"));
	ui.pushButton_3->setIcon(QIcon("../data/icons/UtilityRename.png"));

	init();

	connect(ui.lineEdit,SIGNAL(textChanged(const QString&)),this,SLOT(modelFilter(const QString&)));
	connect(ui.listWidget,SIGNAL(currentItemChanged(QListWidgetItem*,QListWidgetItem*)),this,SLOT(currentModelDefineChanged(QListWidgetItem*,QListWidgetItem*)));
	connect(ui.pushButton_3,SIGNAL(clicked()),this,SLOT(modelRename()));
	connect(ui.pushButton_1,SIGNAL(clicked()),this,SLOT(addModelDefine()));
	connect(ui.pushButton_2,SIGNAL(clicked()),this,SLOT(removeModelDefine()));
	connect(ui.tableWidget,SIGNAL(itemDoubleClicked(QTableWidgetItem*)),SLOT(editFileName(QTableWidgetItem*)));
	connect(ui.listWidget,SIGNAL(itemDoubleClicked(QListWidgetItem*)),this,SLOT(modelRename()));
}

ByyModelDefineWidget::~ByyModelDefineWidget()
{

}

void ByyModelDefineWidget::init()
{
	QVector<ModelDefine> defines=ByyMappingQuery::instance()->getModelDefines();

	for (int i=0;i!=defines.size();++i)
	{
		QListWidgetItem *item=new QListWidgetItem(defines[i].name);
		item->setData(IdRole,defines[i].id);
		item->setData(IconRole,defines[i].icon_filepath);
		item->setData(ModelRole,defines[i].model_filepath);
		ui.listWidget->addItem(item);
	}
}

void ByyModelDefineWidget::modelFilter( const QString& text )
{
	for (int i=0;i!=ui.listWidget->count();++i)
	{
		ui.listWidget->setRowHidden(i,text.isEmpty()?false:!ui.listWidget->item(i)->text().toLower().contains(text.toLower()));
	}
}

void ByyModelDefineWidget::currentModelDefineChanged( QListWidgetItem * current, QListWidgetItem * previous )
{
	if (current)
	{
		ui.tableWidget->item(0,1)->setText(current->data(ModelRole).toString());
		ui.tableWidget->item(1,1)->setText(current->data(IconRole).toString());
	}
}

void ByyModelDefineWidget::modelRename()
{
	if (ui.listWidget->currentRow()==-1)
	{
		return;
	}
	QListWidgetItem *current=ui.listWidget->currentItem();

	bool ok=false;

	QString name=QInputDialog::getText(this,QString::fromLocal8Bit("重命名模型定义"),
		QString::fromLocal8Bit("新模型定义名称:"),QLineEdit::Normal,
		current->text(),&ok);

	if (!ok||name==current->text())
	{
		return;
	}

	if (ByyMappingQuery::instance()->findModelName(name))
	{
		QMessageBox::warning(this,QString::fromLocal8Bit("模型定义"),
			QString::fromLocal8Bit("该名称已经存在！"));
		return;
	}

	ByyMappingQuery::instance()->renameModel(current->data(IdRole).toInt(),name);	

	current->setText(name);

	//emit modified();
}

void ByyModelDefineWidget::addModelDefine()
{
	bool ok=false;
	QString name=QInputDialog::getText(this,QString::fromLocal8Bit("添加模型定义"),
		QString::fromLocal8Bit("模型定义名称:"),QLineEdit::Normal,QString(),&ok);

	if (!ok)
	{
		return;
	}

	if (ByyMappingQuery::instance()->findModelName(name))
	{
		QMessageBox::warning(this,QString::fromLocal8Bit("模型定义"),
			QString::fromLocal8Bit("该名称已经存在！"));
		return;
	}

	QString filename=openModelFile();

	if (filename.isEmpty())
	{
		return;
	}

	ModelDefine md;
	md.name=name;
	md.model_filepath=filename;

	int id=ByyMappingQuery::instance()->addModelDefine(md);

	QListWidgetItem *item=new QListWidgetItem(name);
	item->setData(ModelRole,filename);
	item->setData(IconRole,"");
	item->setData(IdRole,id);
	ui.listWidget->addItem(item);
	ui.listWidget->sortItems();
	ui.listWidget->setCurrentItem(item);

	myLastModelPath=filename;

	//emit modified();
}

void ByyModelDefineWidget::addModelDefine( const ModelDefine& md )
{
	QListWidgetItem *item=new QListWidgetItem(md.name);
	item->setData(ModelRole,md.model_filepath);
	item->setData(IconRole,md.icon_filepath);
	item->setData(IdRole,md.id);
	ui.listWidget->addItem(item);
}

void ByyModelDefineWidget::removeModelDefine()
{
	QListWidgetItem *current=ui.listWidget->currentItem();

	if (!current)
	{
		return;
	}

	int r=QMessageBox::warning(
		this,QString::fromLocal8Bit("确认"),QString::fromLocal8Bit(
		"你确认删除\"%1\"吗?").arg(current->text()),QMessageBox::Yes|QMessageBox::No);

	if (QMessageBox::Yes==r)
	{
		ByyMappingQuery::instance()->removeModelDefine(current->data(IdRole).toInt());

		//emit modified();

		delete ui.listWidget->takeItem(ui.listWidget->row(current));
	}
}

void ByyModelDefineWidget::editFileName( QTableWidgetItem * item )
{
	if (item->column()==0)
	{
		return;
	}

	QListWidgetItem* current=ui.listWidget->currentItem();

	if (!current)
	{
		return;
	}

	if (item->row()==ModelRow)
	{
		QString filename=openModelFile(item->text());

		if (filename.isEmpty())
		{
			return;
		}

		item->setText(filename);
		current->setData(ModelRole,filename);

		ByyMappingQuery::instance()->repathModel(current->data(IdRole).toInt(),filename);
	}
	else if (item->row()==IconRow)
	{
		QString filename=openIconFile(item->text());

		if (filename.isEmpty())
		{
			return;
		}

		item->setText(filename);
		current->setData(IconRole,filename);

		ByyMappingQuery::instance()->repathModelIcon(current->data(IdRole).toInt(),filename);
	}
	//emit modified();
}

QString ByyModelDefineWidget::openModelFile( const QString& path )
{
	QString filepath(path);

	if (path.isEmpty())
	{
		filepath=myLastModelPath.isEmpty()?"../data/models":myLastModelPath;
	}

	QString filename=QFileDialog::getOpenFileName(this, QString::fromLocal8Bit("选择文件"),
		filepath,tr("Model File (*.osg *.ive *.flt *.3ds)"));

	if (!filename.isEmpty())
	{
		filename=QDir("../data").exists(filename)?QDir(".").relativeFilePath(filename):filename;
		myLastModelPath=filename;
	}

	return filename;
}

QString ByyModelDefineWidget::openIconFile( const QString& path/*=QString()*/ )
{
	QString filepath(path);

	if (path.isEmpty())
	{
		filepath=myLastIconPath.isEmpty()?"../data/models":myLastIconPath;
	}

	QString filename=QFileDialog::getOpenFileName(this, QString::fromLocal8Bit("选择文件"),
		filepath,tr("Icon File (*.png)"));

	if (!filename.isEmpty())
	{
		filename=QDir("../data").exists(filename)?QDir(".").relativeFilePath(filename):filename;
		myLastIconPath=filename;
	}

	return filename;
}
