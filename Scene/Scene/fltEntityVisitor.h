#ifndef HLAENTITYVISITOR_H
#define HLAENTITYVISITOR_H

#include <osgSim/DOFTransform>
#include <osgSim/MultiSwitch>
#include <osg/NodeVisitor>
#include <QMultiMap>

class ByyFltEntityVisitor : public osg::NodeVisitor
{
public:
	ByyFltEntityVisitor();
	~ByyFltEntityVisitor();

	void apply(osg::Transform& node);
	void apply(osg::Group& node);

	osg::ref_ptr<osgSim::MultiSwitch> getDamageSwith();
	osg::ref_ptr<osgSim::MultiSwitch> getHealthSwith();
	osg::ref_ptr<osgSim::MultiSwitch> getMovingSwith();
	QMultiMap<int,osgSim::DOFTransform*> getArticulatedParts();

private:
	osg::ref_ptr<osgSim::MultiSwitch> damageSwitch;
	osg::ref_ptr<osgSim::MultiSwitch> healthSwitch;
	osg::ref_ptr<osgSim::MultiSwitch> movingSwitch;
	osg::ref_ptr<osgSim::MultiSwitch> weapon_1Switch;
	QMultiMap<int,osgSim::DOFTransform*> artMaps;
	
};

#endif // HLAENTITYVISITOR_H
