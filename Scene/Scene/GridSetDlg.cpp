#include "StdAfx.h"
#include "GridSetDlg.h"
#include <QGridLayout>
#include "../snigCore/coreUtils.h"
#include <QVBoxLayout>
#include <QLabel>
#include <QPushButton>
#include <QHBoxLayout>
#include "observerSettings.h"

GridSetDlg* dlg=0;
void GridSetDlg::doSetting()
{
	if(dlg==0)
	{
		dlg=new GridSetDlg;
	}
	dlg->show();
}

GridSetDlg::GridSetDlg(void)
{
	setParent(frameWnd());
	setWindowTitle(CNTR("方里网设置"));
	QVBoxLayout* mainlay=new QVBoxLayout;
	setLayout(mainlay);
	QGridLayout* lay=new QGridLayout;
	mainlay->addLayout(lay);

	gridOpen=new QCheckBox(CNTR("显示局部网格"));
	lay->addWidget(gridOpen,0,0,1,2);

	clampToTerrain=new QCheckBox(CNTR("贴地"));
	lay->addWidget(clampToTerrain,0,2,1,2);

	

	lay->addWidget(new QLabel(CNTR("开始经度:")),1,0);
	lonStart=new QLineEdit;
	lay->addWidget(lonStart,1,1);

	lay->addWidget(new QLabel(CNTR("结束经度:")),1,2);
	lonEnd=new QLineEdit;
	lay->addWidget(lonEnd,1,3);

	lay->addWidget(new QLabel(CNTR("开始纬度:")),2,0);
	latStart=new QLineEdit;
	lay->addWidget(latStart,2,1);

	lay->addWidget(new QLabel(CNTR("结束纬度:")),2,2);
	latEnd=new QLineEdit;
	lay->addWidget(latEnd,2,3);

	lay->addWidget(new QLabel(CNTR("开关比例尺:")),3,0);
	openScale=new QLineEdit;
	lay->addWidget(openScale,3,1);

	lay->addWidget(new QLabel(CNTR("分割粒度:")),3,2);
	resolution=new QLineEdit;
	lay->addWidget(resolution,3,3);

	QHBoxLayout* hlay=new QHBoxLayout;
	mainlay->addLayout(hlay);
	QPushButton* btnCancle=new QPushButton(CNTR("取消"));
	hlay->addWidget(btnCancle);
	connect(btnCancle,SIGNAL(clicked(bool)),this,SLOT(accept()));

	QPushButton* btnOk=new QPushButton(CNTR("确定"));
	hlay->addWidget(btnOk);
	connect(btnOk,SIGNAL(clicked(bool)),this,SLOT(ok()));

	gridOpen->setChecked(ByyObserverSettings::instance().gridOpen());
	clampToTerrain->setChecked(ByyObserverSettings::instance().clampToTerrain());
	openScale->setText(QString::number(ByyObserverSettings::instance().openScale()));
	lonStart->setText(QString::number(ByyObserverSettings::instance().lonStart(),'f',2));
	lonEnd->setText(QString::number(ByyObserverSettings::instance().lonEnd(),'f',2));
	latStart->setText(QString::number(ByyObserverSettings::instance().latStart(),'f',2));
	latEnd->setText(QString::number(ByyObserverSettings::instance().latEnd(),'f',2));
	resolution->setText(QString::number(ByyObserverSettings::instance().resolution()));

	setWindowFlags(Qt::Dialog);
	
}


GridSetDlg::~GridSetDlg(void)
{
}

void GridSetDlg::ok()
{
	ByyObserverSettings::instance().setGridSettings(gridOpen->isChecked(),
		openScale->text().toInt(),
		lonStart->text().toDouble(),
		lonEnd->text().toDouble(),
		latStart->text().toDouble(),
		latEnd->text().toDouble(),
		resolution->text().toInt(),
		clampToTerrain->isChecked());
	accept();
}
