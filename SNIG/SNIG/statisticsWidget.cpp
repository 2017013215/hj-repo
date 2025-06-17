#include "StdAfx.h"
#include "statisticsWidget.h"
#include "ByyApp.h"
#include "ByyHdose.h"
#include "scenarioManager.h"
#include "scenarioV2.h"
#include "simuController.h"

Statisticswidget::Statisticswidget(QWidget *parent)
	: QDialog(parent)
{
	ui.setupUi(this);
	
	//setWindowFlags(Qt::FramelessWindowHint);
	//setWindowModality(Qt::WindowModal);

	this->setAttribute(Qt::WA_QuitOnClose,false);

	myApp = NULL;

	
	m_missileNumFJ = 0;  //反舰导弹数量
	m_missileNumLJ = 0;//拦截弹数量
	m_missileNumMZ = 0;//命中数量
	m_missileNumBL = 0;//被拦截数量
	m_missileNumTF = 0;//突防数量
	m_missileNumGR = 0;//被干扰数量
	m_tfgl = 0;//突防概率
	m_ljgl = 0;//拦截成功率

	
	connect(&timer,SIGNAL(timeout()),SLOT(onTick()));
}

// 主窗口关闭时，此窗口如果没有关闭，则此析构函数有问题
// 20231102
Statisticswidget::~Statisticswidget()
{
}

Statisticswidget* Statisticswidget::instance()
{
	static Statisticswidget t_instance;
	return &t_instance;
}


void Statisticswidget::setApp(ByyApp* app)
{
	myApp = app;
	ui.widget_ddhj->setApp(app);
	connect(&app->hdose(), SIGNAL(signal_missileTJ(int)), this, SLOT(slt_missileTJ(int)));
	connect(&app->hdose(), SIGNAL(signal_entityDm(int, QString)), this, SLOT(slt_entityDm(int, QString)));
	connect(&app->hdose().simuController(), SIGNAL(signal_reloadScn()), this, SLOT(slt_started()));

	timer.start(1000);
}

void Statisticswidget::updataInfo()
{
	ui.lineEdit_fjd->setText(QString::number(m_missileNumFJ));
	ui.lineEdit_fkd->setText(QString::number(m_missileNumLJ));
	ui.lineEdit_mzd->setText(QString::number(m_missileNumMZ));
	ui.lineEdit_blj->setText(QString::number(m_missileNumBL));
	ui.lineEdit_tfd->setText(QString::number(m_missileNumFJ-m_missileNumBL));
	ui.lineEdit_bgr->setText(QString::number(m_missileNumFJ-m_missileNumMZ-m_missileNumBL));
	double tf;
	if(m_missileNumFJ != 0)
		tf = (m_missileNumFJ-m_missileNumBL)/(double)m_missileNumFJ;
	else
		tf = 0;
	ui.lineEdit_tfgl->setText(QString::number(tf));
	double lj;
	if(m_missileNumFJ != 0)
		lj = m_missileNumBL/(double)m_missileNumFJ;
	else
		lj = 0;
	ui.lineEdit_ljl->setText(QString::number(lj));
}

void Statisticswidget::slt_missileTJ(int type)
{
	switch(type)
	{
	case 1:
		m_missileNumFJ++;
		break;
	case 2:
		m_missileNumLJ++;
		break;
	case 3:
		m_missileNumMZ++;
		break;
	case 4:
		m_missileNumBL++;
		break;
	default:
		break;

	}

	updataInfo();
}


void Statisticswidget::slt_entityDm(int type, QString name)
{
	//qDebug()<<"----slt_entityDm------"<<type<<name;

	if(m_dmNamList.contains(name))
		return;

	qDebug()<<"----slt_entityDm------"<<type<<name; // xks

	m_dmNamList.append(name);

	if(type == 1)
	{
		ui.textEdit_red->append(name);
	}
	else
	{
		ui.textEdit_blue->append(name);
	}

}

void Statisticswidget::slt_started()
{
	m_missileNumFJ = 0;  //反舰导弹数量
	m_missileNumLJ = 0;//拦截弹数量
	m_missileNumMZ = 0;//命中数量
	m_missileNumBL = 0;//被拦截数量
	m_missileNumTF = 0;//突防数量
	m_missileNumGR = 0;//被干扰数量
	m_tfgl = 0;//突防概率
	m_ljgl = 0;//拦截成功率

	updataInfo();
	
	QString taskName = myApp->hdose().scenarioManager().scenario()->filename();
	ui.lineEdit->setText(QFileInfo(taskName).baseName());


	ui.widget_ddhj->clearUi();

	ui.textEdit_red->clear();
	ui.textEdit_blue->clear();
}


void Statisticswidget::onTick()
{
	if(!myApp)
		return;
	
	ui.lineEdit_2->setText(QString::number(myApp->hdose().simTime()) + "s");

}