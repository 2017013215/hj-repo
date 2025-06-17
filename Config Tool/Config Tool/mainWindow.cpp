#include "mainWindow.h"
#include "sandboxConfig.h"
#include <QMessageBox>
#include <QCloseEvent>

ByyMainWindow::ByyMainWindow(QWidget *parent, Qt::WFlags flags)
	: QMainWindow(parent, flags)
{
	ui.setupUi(this);

	ui.listWidget->item(0)->setIcon(QIcon("../data/icons/ConfigurationDefinitions.svg"));
	ui.listWidget->item(1)->setIcon(QIcon("../data/icons/ConfigurationMappingEntity.svg"));
	ui.listWidget->item(2)->setIcon(QIcon("../data/icons/DisplayEntity.svg"));
	ui.listWidget->item(3)->setIcon(QIcon("../data/icons/DisplayFires.svg"));
	ui.listWidget->item(4)->setIcon(QIcon("../data/icons/DisplayDetonations.svg"));
	ui.listWidget->item(5)->setIcon(QIcon("../data/icons/Terrain3DView.svg"));

	for (int i=0;i!=ui.listWidget->count();++i)
	{
		ui.listWidget->item(i)->setSizeHint(QSize(105,100));
	}


	ui.entityAudioWidget->init("EntityAudioMapping",QString::fromLocal8Bit("ÊµÌåÉùÒôÉèÖÃ"));
	ui.fireAudioWidget->init("FireAudioMapping",QString::fromLocal8Bit("¿ª»ðÉùÒôÉèÖÃ"));
	ui.detonationAudioWidget->init("DetonationAudioMapping",QString::fromLocal8Bit("±¬Õ¨ÉùÒôÉèÖÃ"));

	ui.saveAction->setIcon(QIcon("../data/icons/EarthTool/save.png"));
	ui.mainToolBar->setVisible(false);
	connect(ui.saveAction,SIGNAL(triggered()),SLOT(save()));


	//connect(ui.modelDefineWidget,SIGNAL(modified()),SLOT(setModified()));
	//connect(ui.entityMappingWidget,SIGNAL(modified()),SLOT(setModified()));
	//connect(ui.entityAudioWidget,SIGNAL(modified()),SLOT(setModified()));
	//connect(ui.fireAudioWidget,SIGNAL(modified()),SLOT(setModified()));
	//connect(ui.detonationAudioWidget,SIGNAL(modified()),SLOT(setModified()));
	//connect(ui.sandBoxConfigWidget,SIGNAL(modified()),SLOT(setModified()));
}

ByyMainWindow::~ByyMainWindow()
{

}
void ByyMainWindow::closeEvent( QCloseEvent *event )
{
	if (isWindowModified()) 
	{
		int r = QMessageBox::warning(this,windowTitle(),QString::fromLocal8Bit("ÊÇ·ñ±£´æÐÞ¸Ä?"),QMessageBox::Yes | QMessageBox::No| QMessageBox::Cancel);

		switch(r)
		{
		case QMessageBox::Cancel:
			event->ignore();
			break;
		case QMessageBox::Yes:
			save();
		case QMessageBox::No:
			event->accept();
			break;
		}
	}
}
void ByyMainWindow::save()
{
	setWindowModified(false);
}

void ByyMainWindow::setModified()
{
	setWindowModified(true);
}
