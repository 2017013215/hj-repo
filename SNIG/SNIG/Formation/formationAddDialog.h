#ifndef FORMATIONADDDIALOG_H
#define FORMATIONADDDIALOG_H

#include <QDialog>
#include "ui_formationAddDialog.h"
#include "scenarioV2.h"
class ByyApp;

enum ADDTYPE
{
	ADD_MEMBER,
	ADD_FORMATION
};


class formationAddDialog : public QDialog
{
	Q_OBJECT

public:
	formationAddDialog(ByyApp* app, ByyScenarioV2::FormationList& list, ADDTYPE type, QWidget *parent = 0);
	~formationAddDialog();

	void init();
	void setFormationList(ByyScenarioV2::FormationList& list);

	ByyScenarioV2::FormationList getFormationList();

	QList<ByyEntityObject*> getSelectEntityObjs();

	ByyScenarioV2::Formation getFormation();
public slots:
	void on_pushButton_ok_clicked();

	void on_pushButton_cancel_clicked();

private:
	Ui::formationAddDialog ui;
	ByyApp* myApp;
	ByyScenarioV2::FormationList myFormationList;
	ADDTYPE myType;
	ByyScenarioV2::Formation myFormation;
	QList<ByyEntityObject*> remainder_objs;
};

#endif // FORMATIONADDDIALOG_H
