#include "stdafx.h"
#include "fltEntityVisitor.h"

ByyFltEntityVisitor::ByyFltEntityVisitor()
	: osg::NodeVisitor(osg::NodeVisitor::TRAVERSE_ALL_CHILDREN)
{
	
}

ByyFltEntityVisitor::~ByyFltEntityVisitor()
{

}

void ByyFltEntityVisitor::apply( osg::Transform& node )
{
	osgSim::DOFTransform* dof = dynamic_cast<osgSim::DOFTransform*>(&node);
	if(dof != NULL)
	{
		int descriptionNum = dof->getNumDescriptions();
		if(descriptionNum > 0)
		{
			QString description(QString::fromStdString(dof->getDescription(0)));

			if (description.contains("@dis articulated_part"))
			{
				int partNumber=description.right(4).toInt();
				if (partNumber>=1000&&partNumber<=10000)
				{
					artMaps.insert(partNumber,dof);
				}
			}
			else if (description.contains("@dis weapon_effect"))
			{
				dof->setNodeMask(0x0);
			}
		}

	}

	traverse(node);
}

void ByyFltEntityVisitor::apply( osg::Group& node )
{
	osgSim::MultiSwitch* s = dynamic_cast<osgSim::MultiSwitch*>(&node);
	if(s != NULL)
	{
		int dpNum = s->getNumDescriptions();
		if(dpNum !=0)
		{
			QString description(QString::fromStdString(s->getDescription(0)));
			if (description.contains("@dis switch damage"))
			{
				damageSwitch=s;
			}
			else if (description.contains("@dis switch health"))
			{
				healthSwitch=s;
			}
			else if (description.contains("@dis switch moving"))
			{
				movingSwitch=s;
			}
			else if (description.contains("@dis switch weapon_1"))
			{
				weapon_1Switch=s;
			}
		}
	}
	traverse(node);
}

osg::ref_ptr<osgSim::MultiSwitch> ByyFltEntityVisitor::getDamageSwith()
{
	return damageSwitch;
}

QMultiMap<int,osgSim::DOFTransform*> ByyFltEntityVisitor::getArticulatedParts()
{
	return artMaps;
}

osg::ref_ptr<osgSim::MultiSwitch> ByyFltEntityVisitor::getHealthSwith()
{
	return healthSwitch;
}

osg::ref_ptr<osgSim::MultiSwitch> ByyFltEntityVisitor::getMovingSwith()
{
	return movingSwitch;
}
