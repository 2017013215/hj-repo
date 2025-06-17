#ifndef CANVASEVENTHANDLERS_H
#define CANVASEVENTHANDLERS_H

class ByyChannel;
class ByyObserver;

class ByyChannelCompass : public osgGA::GUIEventHandler
{
public:
	ByyChannelCompass(ByyChannel& channel,ByyObserver& observer);
	~ByyChannelCompass();

	virtual bool  handle(const osgGA::GUIEventAdapter &ea, osgGA::GUIActionAdapter &aa);

private:
	osg::ref_ptr<osgEarth::Util::Controls::ImageControl> myCompassControl;
	double d_lastHeading;
};


#endif // CANVASEVENTHANDLERS_H
