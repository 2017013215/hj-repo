#include "StdAfx.h"
#include "entityInfoWidget.h"
#include "ByyIG.h"
#include "ByyInputController.h"
#include "ByyChannel.h"
#include <osgEarthUtil/Controls>

ByyEntityInfoWidget::ByyEntityInfoWidget( ByyIG& ig )
	:myIG(ig)
{
	myLabel=new osgEarth::Util::Controls::LabelControl("Hello World!\nHHHHHH");
	myLabel->setFont(osgText::readFontFile("../data/fonts/simhei.ttf"));
	myLabel->setEncoding(osgText::String::ENCODING_UTF8);
	myLabel->setBorderWidth(1);
	myLabel->setSize(256,128);
	myLabel->setBorderColor(0,0,0,0.5);
	myLabel->setVisible(false);

	ig.inputController().currentChannel()->addToCanvas(myLabel);
}

ByyEntityInfoWidget::~ByyEntityInfoWidget()
{

}

ByyEntityInfoWidget& ByyEntityInfoWidget::instance( ByyIG& ig )
{
	static ByyEntityInfoWidget w(ig);
	return w;
}

void ByyEntityInfoWidget::setText( const QString& text )
{
	myLabel->setText(text.toUtf8().data());
}

void ByyEntityInfoWidget::setBackColor( const osg::Vec4& color )
{
	myLabel->setBackColor(color);
}

void ByyEntityInfoWidget::setPosition( const osg::Vec3d& world )
{
	ByyChannel *channel=myIG.inputController().currentChannel();

	if (channel)
	{
		osg::Vec3d v3=channel->worldToScreen(world,true);
		myLabel->setPosition(v3.x(),v3.y());
	}
}

void ByyEntityInfoWidget::setVisible( bool v )
{
	myLabel->setVisible(v);
}
