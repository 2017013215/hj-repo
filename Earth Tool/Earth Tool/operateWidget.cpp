#include "stdafx.h"
#include "operateWidget.h"
#include "sceneController.h"

ByyOperateWidget::ByyOperateWidget(ByySceneController *scene,QWidget *parent)
	: QWidget(parent),d_scene(scene)
{
	d_root=new osg::Group;
	d_scene->addChild(d_root);
}

ByyOperateWidget::~ByyOperateWidget()
{

}

void ByyOperateWidget::writeToMapNode()
{

}
