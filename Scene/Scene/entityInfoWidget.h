#ifndef ENTITYINFOWIDGET_H
#define ENTITYINFOWIDGET_H

#include <osg/ref_ptr>

class ByyIG;

namespace osgEarth
{
	namespace Util
	{
		namespace Controls
		{
			class LabelControl;
		}
	}
}

class ByyEntityInfoWidget
{
public:
	~ByyEntityInfoWidget();

	static ByyEntityInfoWidget& instance(ByyIG& ig);

	void setText(const QString& text);
	void setBackColor(const osg::Vec4& color);
	void setPosition(const osg::Vec3d& world);
	void setVisible(bool v);

private:
	ByyEntityInfoWidget(ByyIG& ig);

private:
	ByyIG& myIG;
	osg::ref_ptr<osgEarth::Util::Controls::LabelControl> myLabel;

};

#endif // ENTITYINFOWIDGET_H
