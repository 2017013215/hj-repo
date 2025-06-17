#include "keyValueList.h"

ByyKeyValueList::ByyKeyValueList()
{

}

ByyKeyValueList::~ByyKeyValueList()
{

}

void ByyKeyValueList::remove( const QString& key )
{
	for (int i=0;i!=size();++i)
	{
		if (at(i).first==key)
		{
			removeAt(i);
			break;
		}
	}
}

QTextStream& operator<<( QTextStream& stream,const osg::Vec3d& vector )
{
	stream.setRealNumberPrecision(12);
	stream<<vector.y()<<","<<vector.x()<<","<<vector.z();
	return stream;
}

SNIGCORE_EXPORT QTextStream& operator>>( QTextStream& stream,osg::Vec3d& vector )
{
	double x,y,z;
	stream>>y>>QChar()>>x>>QChar()>>z>>QChar();

	vector.x()=x;
	vector.y()=y;
	vector.z()=z;


	return stream;
}

QTextStream& operator<<( QTextStream& stream,const PointStu& vector )
{
	stream.setRealNumberPrecision(12);
	stream<<vector.lat<<","<<vector.lon<<","<<vector.elev<<","<<vector.speed<<","<<vector.bcross<<","<<vector.waittime<<","<<vector.btimectrl<<","<<vector.arrivetime<<","<<vector.iround<<","<<vector.rsurround;
	return stream;
}

SNIGCORE_EXPORT QTextStream& operator>>( QTextStream& stream,PointStu& vector )
{
	double x,y,z, speed;
	double bcross;
	double waittime;
	double btimectrl;
	double arrivetime;
	double iround;
	double rsurround;
	stream>>y>>QChar()>>x>>QChar()>>z>>QChar()>>speed>>QChar()>>bcross>>QChar()>>waittime>>QChar()>>btimectrl>>QChar()>>arrivetime>>QChar()>>iround>>QChar()>>rsurround>>QChar();

	vector.lon=x;
	vector.lat=y;
	vector.elev=z;
	vector.speed=speed;
	vector.bcross=bcross;
	vector.waittime=waittime;
	vector.btimectrl=btimectrl;
	vector.arrivetime=arrivetime;
	vector.iround=iround;
	vector.rsurround=rsurround;


	return stream;
}
