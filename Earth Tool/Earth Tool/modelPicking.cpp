#include "stdafx.h"
#include "modelPicking.h"
#include "model.h"
#include <osgViewer/View>

ByyModelPicking::ByyModelPicking( QList<ByyModel*> *ml ):modelList(ml)
{

}

ByyModelPicking::~ByyModelPicking()
{

}

bool ByyModelPicking::handle( const osgGA::GUIEventAdapter &ea, osgGA::GUIActionAdapter &aa )
{
	if (ea.getEventType()==osgGA::GUIEventAdapter::DOUBLECLICK)
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
						foreach(ByyModel *m,*modelList)
						{
							if (m->getModel()==np[i])
							{
								emit currentModelChanged(modelList->indexOf(m));
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
