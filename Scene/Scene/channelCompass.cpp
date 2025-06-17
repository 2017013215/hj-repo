#include "stdafx.h"
#include "channelCompass.h"
#include "ByyChannel.h"
#include "ByyObserver.h"

struct ResetViewpointHandler : public osgEarth::Util::Controls::ControlEventHandler
{
	ResetViewpointHandler(ByyObserver& observer)
		:myObserver(observer)
	{

	}

	ByyObserver& myObserver;

	void onClick( osgEarth::Util::Controls::Control* control, const osg::Vec2f& pos, int mouseButtonMask )
	{
		osgEarth::Viewpoint vp=myObserver.getViewpoint();
		vp.setHeading(0);
		myObserver.setViewpoint(vp);
	}
};


ByyChannelCompass::ByyChannelCompass( ByyChannel& channel,ByyObserver& observer )
{
	osg::ref_ptr<osg::Image> compassImage=osgDB::readImageFile("../data/icons/compass.png");

	myCompassControl=new osgEarth::Util::Controls::ImageControl(compassImage);

	myCompassControl->setHorizAlign( osgEarth::Util::Controls::Control::ALIGN_RIGHT );
	myCompassControl->setVertAlign( osgEarth::Util::Controls::Control::ALIGN_TOP);
	myCompassControl->setFixSizeForRotation(true);
	myCompassControl->setAbsorbEvents(false);
	myCompassControl->addEventHandler(new ResetViewpointHandler(observer));

	channel.addToCanvas(myCompassControl);
}

ByyChannelCompass::~ByyChannelCompass()
{

}

bool ByyChannelCompass::handle( const osgGA::GUIEventAdapter &ea, osgGA::GUIActionAdapter &aa )
{
	if (myCompassControl.valid()&&ea.getEventType()==osgGA::GUIEventAdapter::FRAME)
	{
		osgViewer::View *view=dynamic_cast<osgViewer::View*>(&aa);

		if (view)
		{
			osgEarth::Util::EarthManipulator* em=dynamic_cast<osgEarth::Util::EarthManipulator*>(view->getCameraManipulator());

			if (em&&em->getViewpoint().getHeading()!=-d_lastHeading)
			{
				d_lastHeading=-em->getViewpoint().getHeading();

				if (myCompassControl.valid())
				{
					myCompassControl->setRotation(osgEarth::Angular(d_lastHeading));
				}
			}
		}

	}

	return false;
}