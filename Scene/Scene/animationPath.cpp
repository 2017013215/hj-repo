#include "stdafx.h"
#include "animationPath.h"

ByyAnimationPath::ByyAnimationPath()
{
}

ByyAnimationPath::~ByyAnimationPath()
{

}

bool ByyAnimationPath::read( const std::string& filename )
{
	osgDB::ifstream in(filename.c_str());

	controlPointList.clear();

	if (!in)
	{
		return false;
	}

	while (!in.eof())
	{
		MyControlPoint cp;
		osg::Vec3d position;
		osg::Quat rotation;
		in >> cp.name >> cp.time>> position.x() >> position.y() >> position.z() >> rotation.x() >> rotation.y() >> rotation.z() >> rotation.w();
		if(!in.eof())
		{
			cp.ctrlPoints=osg::AnimationPath::ControlPoint(position,rotation);
			controlPointList.push_back(cp);
		}
	}

	in.close();

	return true;
}

bool ByyAnimationPath::write( const std::string& filename )
{
	osgDB::ofstream	_fout;
	_fout.open( filename.c_str() );

	_fout.precision( 15 );

	for (int i=0;i!=controlPointList.size();++i)
	{
		_fout<<controlPointList[i].name<<" "<<controlPointList[i].time<<" "<<controlPointList[i].ctrlPoints.getPosition()<<" "<<controlPointList[i].ctrlPoints.getRotation()<<std::endl;
	}

	_fout.close();

	return true;
}

void ByyAnimationPath::push_back( const MyControlPoint& cp )
{
	controlPointList.append(cp);
}

std::vector<std::string> ByyAnimationPath::getNameList()
{
	std::vector<std::string> strs;
	strs.resize(controlPointList.size());

	for (int i=0;i!=controlPointList.size();++i)
	{
		strs[i]=controlPointList[i].name;
	}

	return strs;
}

void ByyAnimationPath::remove( int index )
{
	controlPointList.remove(index);
}

double ByyAnimationPath::totalTime()
{
	double result=0;

	for(int i=0;i!=controlPointList.size();++i)
	{
		result+=controlPointList[i].time;
	}

	return result;
}

MyControlPoint& ByyAnimationPath::getControlPoint( int index )
{
	return controlPointList[index];
}
