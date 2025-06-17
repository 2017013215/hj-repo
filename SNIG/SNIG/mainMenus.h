#ifndef HELPMENU_H
#define HELPMENU_H

#include "ByyMenu.h"

class ByyFileMenu : public ByyMenu
{
public:
	ByyFileMenu(QWidget* parent=0);
	~ByyFileMenu();
};

class ByyCreateMenu : public ByyMenu
{
public:
	ByyCreateMenu(QWidget* parent=0);
	~ByyCreateMenu();
};

class ByySettingsMenu : public ByyMenu
{
public:
	ByySettingsMenu(QWidget* parent=0);
	~ByySettingsMenu();
};

class ByySimulationMenu : public ByyMenu
{
public:
	ByySimulationMenu(QWidget* parent=0);
	~ByySimulationMenu();
};

class ByyMeasureMenu : public ByyMenu
{
public:
	ByyMeasureMenu(QWidget* parent=0);
	~ByyMeasureMenu();
};

class ByyHelpMenu : public ByyMenu
{
public:
	ByyHelpMenu(QWidget* parent=0);
	~ByyHelpMenu();
};

class ByyStereoModeMenu : public ByyMenu
{
	Q_OBJECT
public:
	ByyStereoModeMenu(ByyApp& app,QWidget* parent=0);
	~ByyStereoModeMenu();

protected slots:
	void onStereoChanged(bool enable);

};

class ByySimpleMenu : public ByyMenu
{
public:
	ByySimpleMenu(const QString& title,const QString& objectName,QWidget* parent=0);
	~ByySimpleMenu();
};

class ByyFormationMenu : public ByyMenu
{
public:
	ByyFormationMenu(QWidget* parent=0);
	~ByyFormationMenu();
};
//²Ã¾ö 9/7
class ByyVerdictMenu :public ByyMenu
{
public:
	ByyVerdictMenu(QWidget* parent=0);
	~ByyVerdictMenu();
};


//Ì½²âÔ¤¾¯ÅÐ¶Ï 2024/04/22
class ByyDetectiomEarlywaringJudgmentMenu : public ByyMenu
{
	Q_OBJECT
public:
	ByyDetectiomEarlywaringJudgmentMenu(ByyApp& app,QWidget* parent=0);
	~ByyDetectiomEarlywaringJudgmentMenu();

	protected slots:
		void onDetectiomEarlywaringJudgmentChanged(bool enable);

};

//Ä¿±ê¸ú×ÙÅÐ¶Ï 2024/04/22
class ByyTargetTrackingJudgmentMenu : public ByyMenu
{
	Q_OBJECT
public:
	ByyTargetTrackingJudgmentMenu(ByyApp& app,QWidget* parent=0);
	~ByyTargetTrackingJudgmentMenu();

	protected slots:
		void onTargetTrackingJudgmentChanged(bool enable);

};



//ÓÅÊÆ¼ÆËã 2024/04/22
class ByyAdvantageCalculationMenu : public ByyMenu
{
	Q_OBJECT
public:
	ByyAdvantageCalculationMenu(ByyApp& app,QWidget* parent=0);
	~ByyAdvantageCalculationMenu();

	protected slots:
		void onAdvantageCalculationChanged(bool enable);

};


//»ÙÉË¼ÆËã 2024/04/22
class ByyDamageCalculationMenu : public ByyMenu
{
	Q_OBJECT
public:
	ByyDamageCalculationMenu(ByyApp& app,QWidget* parent=0);
	~ByyDamageCalculationMenu();

	protected slots:
		void onDemageCalculationChanged(bool enable);

};


//ÊµÌå×´Ì¬²Ã¾ö¼ÆËã 2024/04/22
class ByyEntityStatusVerdictMenu : public ByyMenu
{
	Q_OBJECT
public:
	ByyEntityStatusVerdictMenu(ByyApp& app,QWidget* parent=0);
	~ByyEntityStatusVerdictMenu();

	protected slots:
		void onEntityStatusVerdictChanged(bool enable);

};

#endif // HELPMENU_H
