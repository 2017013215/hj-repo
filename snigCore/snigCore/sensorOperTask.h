#ifndef BYYSENSOROPERTASK_H
#define BYYSENSOROPERTASK_H

#include "task.h"

class SNIGCORE_EXPORT ByySensorOperTask : public ByyTask
{
public:
	ByySensorOperTask(ByyEntityObject& entObj);
	~ByySensorOperTask();

	static const TypeInfo& theTypeInfo();
	virtual const TypeInfo& typeInfo();

	virtual QString string();

	virtual ByyKeyValueList getConfig();
	virtual void fromConfig(const ByyKeyValueList& kvs);

	QString getDeviceName() const { return myDeviceName; }
	void setDeviceName(const QString& val) { myDeviceName = val; }

	bool getPowerOn() const { return myPowerOn; }
	void setPowerOn(bool val) { myPowerOn = val; }

private:
	QString myDeviceName;
	
	int mySensorOperType;
	bool myPowerOn;
	
};

typedef ByyTaskCreatorTemplate<ByySensorOperTask> ByySensorOperTaskCreator;

#endif // BYYSENSOROPERTASK_H
