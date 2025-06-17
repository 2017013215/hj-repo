#ifndef EARTHMANIPULATOR_H
#define EARTHMANIPULATOR_H

class ByyEarthManipulator : public osgEarth::Util::EarthManipulator
{
public:
	ByyEarthManipulator();
	~ByyEarthManipulator();

	virtual void  pan (double dx, double dy) ;
	void setTetherNode( osg::Node* node );
	//virtual float getFusionDistanceValue() const { return 100000; }
	void setViewpoint (const osgEarth::Viewpoint &vp, double duration_s=0.0);

protected:
	virtual bool  handleKeyboardAction (const Action &action, double duration_s=DBL_MAX) ;
	virtual bool  handleMouseAction (const Action &action, osg::View *view);
	//virtual void  handleContinuousAction (const Action &action, osg::View *view) ;
	void adjustViewpoint();
	void rotate(double dx,double dy);
	

private:
	void adjustDistance();
};

#endif // EARTHMANIPULATOR_H
