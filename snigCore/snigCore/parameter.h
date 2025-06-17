#ifndef OBJECTPARAMETER_H
#define OBJECTPARAMETER_H

#pragma warning(disable:4661)

#include <QList>
#include <QString>
#include <QHash>
#include <QMap>

#include "snigcore_global.h"

class QDomElement;

class ByyParameter;

class SNIGCORE_EXPORT ParameterList : public QList<ByyParameter*>
{
public:
	ParameterList();
	ParameterList(const ParameterList& orig);
	~ParameterList();

	void setValue(const QString& name,const QString& value);
	void setDesign(const QString& name,const QString& value);

	const ParameterList& operator=(const ParameterList& orig);
	const ParameterList& operator+=(const ParameterList& orig);

	ByyParameter* findParameter(const QString& param);

protected:
	

protected:
	QHash<QString,ByyParameter*> myParamMap;
};

class SNIGCORE_EXPORT ByyParameter
{
public:
	ByyParameter();
	virtual ~ByyParameter();

	virtual QString getValue()=0;
	virtual void setValue(const QString& val)=0;

	virtual void setDesign(const QString& val)=0;
	void updataUUID();

	virtual ByyParameter* clone()=0;

	virtual void fromConfig(QDomElement& element);

public:
	QString Name;
	QString Tag;
	QString Type;
	QString Description;
	QString Group;
	QString myDefValue;
	QString IsDesign;//是否选择设计
	QString IsCanDesign;//是否可设计
	QString myUUID;
};

class SNIGCORE_EXPORT ByyValueParameter : public ByyParameter
{
public:
	virtual QString getValue();
	virtual void setValue(const QString& val);
	virtual void setDesign(const QString& val);

	virtual ByyParameter* clone();
	virtual void fromConfig(QDomElement& element);

public:
	QString myValue;

};

class SNIGCORE_EXPORT ByyListParameter : public ByyParameter
{
public:
	ByyListParameter();
	ByyListParameter(const ByyListParameter& orig);
	virtual ~ByyListParameter();

	const ParameterList& children()const {return myChildren;}
	ParameterList& children(){return myChildren;}

	virtual void fromConfig(QDomElement& element);

	void removeOne(ByyParameter* param);

	QString mySeparator;

protected:
	ParameterList myChildren;
};

class SNIGCORE_EXPORT ByyStructParameter : public ByyListParameter
{
public:
	ByyStructParameter();
	ByyStructParameter(const ByyStructParameter& orig);
	virtual ~ByyStructParameter();

	//const ByyStructParameter& operator=(const ByyStructParameter& orig);

	virtual QString getValue();
	virtual void setValue(const QString& val);
	virtual void setDesign(const QString& val);

	virtual ByyParameter* clone();

	virtual void fromConfig(QDomElement& element);
};

class SNIGCORE_EXPORT ByyStructArrayParameter : public ByyListParameter
{
public:
	ByyStructArrayParameter();
	ByyStructArrayParameter(const ByyStructArrayParameter& orig);
	virtual ~ByyStructArrayParameter();

	//const ByyStructArrayParameter& operator=(const ByyStructArrayParameter& orig);

	virtual QString getValue();
	virtual void setValue(const QString& val);
	virtual void setDesign(const QString& val);

	virtual ByyParameter* clone();

	virtual void fromConfig(QDomElement& element);

	void push_back();

public:
	QString myArraySeparator;

	ByyStructParameter myTemplate;
};

class SNIGCORE_EXPORT ByyEnumParameter : public ByyValueParameter
{
public:
	ByyEnumParameter();
	ByyEnumParameter(const ByyEnumParameter& orig);
	virtual ~ByyEnumParameter();

	virtual QString getValue();
	virtual void setValue(const QString& val);

	virtual ByyParameter* clone();

	virtual void fromConfig(QDomElement& element);

	QMap<int, QString> getEnumMap();
protected:
	QString FilePath;
	QString Keyword;
	QMap<int, QString> m_EnumMap;

};

class SNIGCORE_EXPORT ByyPathParameter : public ByyValueParameter
{
public:
	ByyPathParameter();
	ByyPathParameter(const ByyPathParameter& orig);
	virtual ~ByyPathParameter();

	virtual QString getValue();
	virtual void setValue(const QString& val);

	virtual ByyParameter* clone();

	virtual void fromConfig(QDomElement& element);

	QString getFileSuffix();
protected:
	QString FileSuffix;
};

class SNIGCORE_EXPORT ByyParameterFactory 
{
public:
	static ByyParameter* create(QDomElement& element); 
};


#endif // OBJECTPARAMETER_H
