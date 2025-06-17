#ifndef INDICATEVISUALIZER_H
#define INDICATEVISUALIZER_H

#include "stateVisualizer.h"

#include "ByySelectionManager.h"

namespace osgEarth 
{
	namespace Annotation
	{
		class ByyPlaceNode;
	}
}

class ByyIndicateVisualizer : public ByyStateVisualizer
{
	Q_OBJECT
public:
	ByyIndicateVisualizer(ByyEntityObject& entObj,ByyIG& ig);
	~ByyIndicateVisualizer();

	static const QString& theType();
	virtual const QString& type();

public slots:
	void setText(const QString& text);
	void setIconVisible(bool v);
	void setTextVisible(bool v);
	void setPosition(const osg::Vec3d& pos);
	void set2D(bool is2d);

	void updateForceType();
	
	void setVisible(bool bl);

	void setOrientation( const osg::Vec3d& ori );
	void	selectChanged(bool selected);
public slots:
		void onSelectionChanged(ByySelectionManager::SelectionType,unsigned int);
protected:
	osg::Image* loadIcon();

protected:
	osg::ref_ptr<osgEarth::Annotation::ByyPlaceNode> myIndicateNode;
	osg::ref_ptr<osgEarth::Annotation::ByyPlaceNode> mySelectionNode;

	bool		m_isSelected;
};

typedef ByyStateVisualizerCreatorTemplate<ByyIndicateVisualizer> ByyIndicateVisualizerCreator;

#endif // INDICATEVISUALIZER_H
