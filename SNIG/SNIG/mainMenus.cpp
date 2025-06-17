#include "StdAfx.h"
#include "mainMenus.h"
#include "stereoSettings.h"

ByyFileMenu::ByyFileMenu( QWidget* parent/*=0*/ )
	:ByyMenu("ByyFileMenu")
{
	setTitle(QString::fromLocal8Bit("文件(&F)"));
}

ByyFileMenu::~ByyFileMenu()
{

}

ByyCreateMenu::ByyCreateMenu( QWidget* parent/*=0*/ )
	:ByyMenu("ByyCreateMenu")
{
	setTitle(QString::fromLocal8Bit("创建(&C)"));
}

ByyCreateMenu::~ByyCreateMenu()
{

}

ByySettingsMenu::ByySettingsMenu(QWidget* parent)
	:ByyMenu("ByySettingsMenu")
{
	setTitle(QString::fromLocal8Bit("设置(&T)"));
}

ByySettingsMenu::~ByySettingsMenu()
{

}

ByySimulationMenu::ByySimulationMenu(QWidget* parent)
	: ByyMenu("ByySimulationMenu")
{
	setTitle(QString::fromLocal8Bit("仿真(&S)"));
}

ByySimulationMenu::~ByySimulationMenu()
{

}

ByyMeasureMenu::ByyMeasureMenu( QWidget* parent/*=0*/ )
	: ByyMenu("ByyMeasureMenu")
{
	setTitle(QString::fromLocal8Bit("测量(&M)"));
}

ByyMeasureMenu::~ByyMeasureMenu()
{

}

ByyHelpMenu::ByyHelpMenu(QWidget* parent)
	: ByyMenu("ByyHelpMenu")
{
	setTitle(QString::fromLocal8Bit("帮助(&H)"));
}

ByyHelpMenu::~ByyHelpMenu()
{

}

ByySimpleMenu::ByySimpleMenu( const QString& title,const QString& objectName,QWidget* parent/*=0*/ ) 
	:ByyMenu(objectName,parent)
{
	setTitle(title);
}

ByySimpleMenu::~ByySimpleMenu()
{

}

ByyStereoModeMenu::ByyStereoModeMenu( ByyApp& app,QWidget* parent/*=0*/ )
		:ByyMenu("ByyStereoModeMenu",parent)
{
	setTitle(QString::fromLocal8Bit("立体模式(&M)"));
	connect(&ByyStereoSettings::instance(),SIGNAL(enableChanged(bool)),SLOT(onStereoChanged(bool)));
}

ByyStereoModeMenu::~ByyStereoModeMenu()
{

}

void ByyStereoModeMenu::onStereoChanged( bool enable )
{
	setEnabled(!enable);
}

ByyFormationMenu::ByyFormationMenu(QWidget* parent)
	: ByyMenu("ByyFormationMenu")
{
	setTitle(QString::fromLocal8Bit("编队(&F)"));
}

ByyFormationMenu::~ByyFormationMenu()
{
	
}

//裁决 9/7
ByyVerdictMenu::ByyVerdictMenu(QWidget* parent)
	:ByyMenu("ByyVerdictMenu")
{
	setTitle(QString::fromLocal8Bit("裁决(&V)"));
}
ByyVerdictMenu::~ByyVerdictMenu()
{

}
//探测预警判断 2024/04/22
ByyDetectiomEarlywaringJudgmentMenu::ByyDetectiomEarlywaringJudgmentMenu(ByyApp& app,QWidget* parent/*=0*/)
	: ByyMenu("ByyDetectiomEarlywaringJudgmentMenu")
{
	setTitle(QString::fromLocal8Bit("探测预警判断"));
	//connect(&ByyStereoSettings::instance(),SIGNAL(enableChanged(bool)),SLOT(onDetectiomEarlywaringJudgmentChanged(bool)));
}

ByyDetectiomEarlywaringJudgmentMenu::~ByyDetectiomEarlywaringJudgmentMenu()
{

}

void ByyDetectiomEarlywaringJudgmentMenu::onDetectiomEarlywaringJudgmentChanged(bool enable)
{
	setEnabled(!enable);
}

//目标跟踪判断 2024/04/22
ByyTargetTrackingJudgmentMenu::ByyTargetTrackingJudgmentMenu(ByyApp& app,QWidget* parent/*=0*/)
	: ByyMenu("ByyTargetTrackingJudgmentMenu")
{
	setTitle(QString::fromLocal8Bit("目标跟踪判断"));
	//connect(&ByyStereoSettings::instance(),SIGNAL(enableChanged(bool)),SLOT(onDetectiomEarlywaringJudgmentChanged(bool)));
}

ByyTargetTrackingJudgmentMenu::~ByyTargetTrackingJudgmentMenu()
{

}

void ByyTargetTrackingJudgmentMenu::onTargetTrackingJudgmentChanged(bool enable)
{
	setEnabled(!enable);
}

// 优势计算 2024/04/22
ByyAdvantageCalculationMenu::ByyAdvantageCalculationMenu(ByyApp& app,QWidget* parent/*=0*/)
	: ByyMenu("ByyAdvantageCalculationMenu")
{
	setTitle(QString::fromLocal8Bit("优势计算"));
	//connect(&ByyStereoSettings::instance(),SIGNAL(enableChanged(bool)),SLOT(onDetectiomEarlywaringJudgmentChanged(bool)));
}

ByyAdvantageCalculationMenu::~ByyAdvantageCalculationMenu()
{

}

void ByyAdvantageCalculationMenu::onAdvantageCalculationChanged(bool enable)
{
	setEnabled(!enable);
}

//毁伤计算 2024/04/22
ByyDamageCalculationMenu::ByyDamageCalculationMenu(ByyApp& app,QWidget* parent/*=0*/)
	: ByyMenu("ByyDamageCalculationMenu")
{
	setTitle(QString::fromLocal8Bit("毁伤计算"));
	//connect(&ByyStereoSettings::instance(),SIGNAL(enableChanged(bool)),SLOT(onDetectiomEarlywaringJudgmentChanged(bool)));
}

ByyDamageCalculationMenu::~ByyDamageCalculationMenu()
{

}

void ByyDamageCalculationMenu::onDemageCalculationChanged(bool enable)
{
	setEnabled(!enable);
}


//实体状态裁决计算 2024/04/22
ByyEntityStatusVerdictMenu::ByyEntityStatusVerdictMenu(ByyApp& app,QWidget* parent/*=0*/)
	: ByyMenu("ByyEntityStatusVerdictMenu")
{
	setTitle(QString::fromLocal8Bit("实体状态评估"));
	//connect(&ByyStereoSettings::instance(),SIGNAL(enableChanged(bool)),SLOT(onDetectiomEarlywaringJudgmentChanged(bool)));
}

ByyEntityStatusVerdictMenu::~ByyEntityStatusVerdictMenu()
{

}

void ByyEntityStatusVerdictMenu::onEntityStatusVerdictChanged(bool enable)
{
	setEnabled(!enable);
}
