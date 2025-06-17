#ifndef DETECTINDICATEVISUALIZER_H
#define DETECTINDICATEVISUALIZER_H

#include <QObject>

namespace osgEarth 
{
	namespace Annotation
	{
		class ByyPlaceNode;
	}
}

class ByyIG;
class ByyDetectObject;

class ByyDetectIndicateVisualizer: public QObject
{
	Q_OBJECT
public:
	ByyDetectIndicateVisualizer(ByyDetectObject& obj,ByyIG& ig);
	~ByyDetectIndicateVisualizer();

	static const QString& theType();
	virtual const QString& type();

	osg::Group * root();

public slots:
	void setText(const QString& text);
	void setIconVisible(bool v);
	void setTextVisible(bool v);
	void setPosition(osg::Vec3d pos);
	void set2D(bool is2d);

	void updateForceType();
	
	void setVisible(bool bl);

protected:
	osg::Image* loadIcon();

protected:
	ByyDetectObject& myObj;
	ByyIG& myIG;

	osg::ref_ptr<osg::Group> myRoot;

	osg::observer_ptr<osgEarth::MapNode> myMapNode;

protected:
	osg::ref_ptr<osgEarth::Annotation::ByyPlaceNode> myIndicateNode;
};

#endif // DETECTINDICATEVISUALIZER_H
