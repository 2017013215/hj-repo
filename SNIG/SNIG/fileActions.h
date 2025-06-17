#ifndef FILEACTIONS_H
#define FILEACTIONS_H

#include "ByyAction.h"

class ByyNewScenarioAction : public ByyAction
{
public:
	ByyNewScenarioAction(ByyApp& app,QWidget* parent=0);
	~ByyNewScenarioAction();

protected slots:
	void on_triggered();
	void checkEnabled();
};

class ByyLoadScenarioAction : public ByyAction
{
public:
	ByyLoadScenarioAction(ByyApp& app,QWidget* parent=0);
	~ByyLoadScenarioAction();

protected slots:
	void on_triggered();
	void checkEnabled();

};

class ByySaveScenarioAction : public ByyAction
{
public:
	ByySaveScenarioAction(ByyApp& app,QWidget* parent=0);
	~ByySaveScenarioAction();

protected slots:
	void on_triggered();
	void checkEnabled();
};

class ByySaveAsScenarioAction : public ByyAction
{
public:
	ByySaveAsScenarioAction(ByyApp& app,QWidget* parent=0);
	~ByySaveAsScenarioAction();

protected slots:
	void on_triggered();
	void checkEnabled();
};

class ByyCloseScenarioAction : public ByyAction
{
public:
	ByyCloseScenarioAction(ByyApp& app,QWidget* parent=0);
	~ByyCloseScenarioAction();

protected slots:
	void on_triggered();
	void checkEnabled();
};

class ByyOpenTerrainAction : public ByyAction
{
public:
	ByyOpenTerrainAction(ByyApp& app,QWidget* parent=0);
	~ByyOpenTerrainAction();

protected slots:
	void on_triggered();

};

class ByyCloseTerrainAction : public ByyAction
{
public:
	ByyCloseTerrainAction(ByyApp& app,QWidget* parent=0);
	~ByyCloseTerrainAction();

protected slots:
	void on_triggered();

};

class ByySaveTerrainAction : public ByyAction
{
public:
	ByySaveTerrainAction(ByyApp& app,QWidget* parent=0);
	~ByySaveTerrainAction();

protected slots:
	void on_triggered();

};

class ByySaveAsTerrainAction : public ByyAction
{
public:
	ByySaveAsTerrainAction(ByyApp& app,QWidget* parent=0);
	~ByySaveAsTerrainAction();

protected slots:
	void on_triggered();

};

class ByyViewpointCreateAction : public ByyAction
{
public:
	ByyViewpointCreateAction(ByyApp& app,QWidget* parent=0);
	~ByyViewpointCreateAction();

protected slots:
	void on_triggered();

};

class ByyAnimationPathCreateAction : public ByyAction
{
public:
	ByyAnimationPathCreateAction(ByyApp& app,QWidget* parent=0);
	~ByyAnimationPathCreateAction();

protected slots:
	void on_triggered();

};

class ByyTerrainEditAction : public ByyAction
{
public:
	ByyTerrainEditAction(ByyApp& app,QWidget* parent=0);
	~ByyTerrainEditAction();

protected slots:
	void on_triggered();

};

class ByySnapshootAction : public ByyAction
{
public:
	ByySnapshootAction(ByyApp& app,QWidget* parent=0);
	~ByySnapshootAction();

protected slots:
	void on_triggered();

};

class ByyExitAction : public ByyAction
{
public:
	ByyExitAction(ByyApp& app,QWidget* parent=0);
	~ByyExitAction();

protected slots:
	void on_triggered();

};
#endif // FILEACTIONS_H
