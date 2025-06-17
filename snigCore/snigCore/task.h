#ifndef TASK_H
#define TASK_H

#include <QString>
#include "keyValueList.h"
#include "snigcore_global.h"

class ByyEntityObject;

class SNIGCORE_EXPORT ByyTask
{
public:
	ByyTask(ByyEntityObject& entObj);
	virtual ~ByyTask();

	struct TypeInfo
	{
		QString type;
		QString name;
		QString	clsify;
	};

	virtual	QString	scnClsName(){return "CSimTask";}

	virtual const TypeInfo& typeInfo()=0;
	virtual QString string()=0;

	void setName(const QString& str);
	const QString& name()const;

	void setStartTime(double t);
	double startTime()const;

	ByyEntityObject& entity();

	virtual ByyKeyValueList getConfig();
	virtual void fromConfig(const ByyKeyValueList& kvs);
	virtual	void	notifyCtrlObjNameChanged(QString old,QString cur){}

protected:
	ByyEntityObject& myEntObj;

	QString myName;

	double myStartTime;
};

class ByyTaskCreator
{
public:
	virtual ~ByyTaskCreator()
	{

	}

	virtual ByyTask* createTask(ByyEntityObject& entObj)=0;
	virtual const ByyTask::TypeInfo& typeInfo()=0;
};

template<class T>
class ByyTaskCreatorTemplate : public ByyTaskCreator
{
public:
	virtual ByyTask* createTask(ByyEntityObject& entObj)
	{
		return new T(entObj);
	}

	virtual const ByyTask::TypeInfo& typeInfo()
	{
		return T::theTypeInfo();
	}
};

#endif // TASK_H
