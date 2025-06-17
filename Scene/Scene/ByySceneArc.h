#pragma once
#include "scenecircle.h"
#include "ByyArc.h"
class ByySceneArc :	public ByySceneCircle
{
	Q_OBJECT
public:
	ByySceneArc(ByyArc *ci,ByyIG& ig);
	~ByySceneArc(void);

	virtual void initiate();
	static ByySceneControlObject* create(ByyControlObject* obj,ByyIG& ig);
public slots:
	void setStartAngle(double angle);
	void setEndAngle(double angle);

protected:
	ByyArc*	myArc;
};

