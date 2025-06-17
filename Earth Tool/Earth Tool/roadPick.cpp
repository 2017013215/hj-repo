#include "stdafx.h"
#include "roadPick.h"

#include <osgViewer/View>

ByyRoadPick::ByyRoadPick(osg::Group *root)
	:d_root(root)
{

}

ByyRoadPick::~ByyRoadPick()
{

}

bool ByyRoadPick::handle( const osgGA::GUIEventAdapter &ea, osgGA::GUIActionAdapter &aa )
{
	if (ea.getEventType()==osgGA::GUIEventAdapter::PUSH)
	{
		osgViewer::View *view=dynamic_cast<osgViewer::View*>(&aa);
		if (view&&view->computeIntersections(ea.getX(), ea.getY(), intersections))
		{
			for(osgUtil::LineSegmentIntersector::Intersections::iterator hitr = intersections.begin();
				hitr != intersections.end();++hitr)
			{
				if (!hitr->nodePath.empty()) 
				{ 
					const osg::NodePath& np = hitr->nodePath ;
					for (int i=np.size()-1; i>=0; --i)
					{ 
						for (int i=0;i!=d_root->getNumChildren();++i)
						{
							if (d_root->getChild(i)==np[i])
							{
								emit currentRoadChanged(i);
								return true;
							}
						}
					}
				}
			}
		}
	}
	return false;
}
