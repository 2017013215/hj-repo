#ifndef BYYDETECTTARGETWIDGET_H
#define BYYDETECTTARGETWIDGET_H

#include "ByyEntityObserver.h"
#include "ui_ByyDetectTargetWidget.h"

class ByyApp;
class ByySensorSR;

class ByyDetectTargetWidget : public ByyEntityObserver
{
	Q_OBJECT

public:
	ByyDetectTargetWidget(ByyApp& app,ByyEntityObject* entity=0,QWidget* parent=0,Qt::WindowFlags f = 0);
	~ByyDetectTargetWidget();

	void setEntity(ByyEntityObject *entity);

protected slots:
	void appendDetectObject(ByyDetectObject* obj,ByySensorSR* sensor);
	void removeDetectObject(ByyDetectObject* obj,ByySensorSR* sensor);

	void updateTimeChanged(double val);
	void batchNoChanged(long val);
	void CIDChanged(long val);

	void entityTypeChanged(ByyEntityType val);
	void threatLevelChanged(long val);
	void posChanged(osg::Vec3d val);
	void velocityChanged(double val);
	void courseChanged(double val);
	void distanceChanged(double val);
	void elChanged(double val);
	void azChanged(double val);
	void radiationTypeChanged(long val);
	void radiationPtChanged(double val);
	void radiationFreqChanged(double val);
	void radiationTuChanged(double val);
	void radiationPRFChanged(double val);
	void radiationSignalTypeChanged(double val);
	void radiationPulseTypeChanged(double val);
	void forceTypeChanged(ByyForceType ft);
	void nameChanged(const QString&);

protected:
	void refresh();

	void append(ByyDetectObject* obj);

private:
	Ui::ByyDetectTargetWidget ui;
	ByyApp&	myApp;
	ByySensorSR* myCurrentSensorSR;

	typedef QList<ByyDetectObject*> DetectObjectList;

	DetectObjectList myDetectObjectList;

	enum DetectType{
		UpdateTime,
	BatchNo,
	TargetID,
	TargetName,
	From,
	CID,
	Side,
	EntityType,
	ThreatLevel,
	Pos,
	Velocity,
	Course,
	Distance,
	El,
	Az,
	RadiationType,
	RadiationPt,
	RadiationFreq,
	RadiationTu,
	RadiationPRF,
	RadiationSignalType,
	RadiationPulseType
	};
};

#endif // BYYDETECTTARGETWIDGET_H
