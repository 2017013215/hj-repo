#ifndef MEASUREACTIONS_H
#define MEASUREACTIONS_H

#include "ByyAction.h"

class ByyLineMeasureAction : public ByyAction
{
public:
	ByyLineMeasureAction(ByyApp& app,QWidget* parent=0);
	~ByyLineMeasureAction();

protected slots:
	void on_triggered();

};

class ByyPolygonMeasureAction : public ByyAction
{
public:
	ByyPolygonMeasureAction(ByyApp& app,QWidget* parent=0);
	~ByyPolygonMeasureAction();

protected slots:
	void on_triggered();

};

class ByyIntervisibilityLineAction : public ByyAction
{
public:
	ByyIntervisibilityLineAction(ByyApp& app,QWidget* parent=0);
	~ByyIntervisibilityLineAction();

protected slots:
	void on_triggered();

};

class ByyIntervisibilityFanAction : public ByyAction
{
public:
	ByyIntervisibilityFanAction(ByyApp& app,QWidget* parent=0);
	~ByyIntervisibilityFanAction();

	protected slots:
		void on_triggered();

};
#endif // MEASUREACTIONS_H
