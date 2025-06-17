#ifndef KEYVALUELIST_H
#define KEYVALUELIST_H

#pragma warning(disable:4661)

#include "snigcore_global.h"

#include <QList>
#include <QPair>
#include <QTextStream>

#include <osg/Vec3d>

typedef QPair<QString,QString> ByyKeyValue;

struct PointStu
{
	double lon;
	double lat;
	double elev;
	double speed;
	double bcross;
	double waittime;
	double btimectrl;
	double arrivetime;
	double iround;
	double rsurround;
};

class SNIGCORE_EXPORT ByyKeyValueList : public QList<ByyKeyValue>
{
public:
	ByyKeyValueList();
	~ByyKeyValueList();

	template<class T>
	void add(const QString& key,const T& value)
	{
		QString str;

		QTextStream(&str)<<value;

		append(qMakePair(key, str));
	}

	template<class T>
	void get(const QString& key,T& value)const 
	{
		QString temp;
		for (int i=0;i!=size();++i)
		{
			if (at(i).first==key)
			{
				temp=(*this)[i].second;

				QTextStream(&temp)>>value;
				break;
			}
		}
	}

	void get(const QString& key,QString& value)const 
	{
		for (int i=0;i!=size();++i)
		{
			if (at(i).first==key)
			{
				value=(*this)[i].second;
				break;
			}
		}
	}

	void get(const QString& key,bool& value)const 
	{
		for (int i=0;i!=size();++i)
		{
			if (at(i).first==key)
			{
				value=(*this)[i].second.toInt();
				break;
			}
		}
	}

	template<class T>
	QList<T> getValues(const QString& key)const
	{
		QList<T> result;

		T t;
		QString temp;

		for (int i=0;i!=size();++i)
		{
			if (at(i).first==key)
			{
				temp=(*this)[i].second;
				QTextStream(&temp)>>t;

				result.append(t);
			}
		}

		return result;
	}

	void remove(const QString& key);

private:
	
};

SNIGCORE_EXPORT QTextStream& operator<<(QTextStream& stream,const osg::Vec3d& vector);
SNIGCORE_EXPORT QTextStream& operator>>(QTextStream& stream,osg::Vec3d& vector);

SNIGCORE_EXPORT QTextStream& operator<<(QTextStream& stream,const PointStu& vector);
SNIGCORE_EXPORT QTextStream& operator>>(QTextStream& stream,PointStu& vector);
#endif // KEYVALUELIST_H
