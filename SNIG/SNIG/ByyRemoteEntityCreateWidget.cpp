#include "StdAfx.h"
#include "ByyRemoteEntityCreateWidget.h"

#include "entityObject.h"

ByyRemoteEntityCreateWidget::ByyRemoteEntityCreateWidget( ByyApp& app,const QString& assemble,ByyForceType ft,const osg::Vec3d& position,QWidget *parent /*= 0*/ ) : QWidget(parent)
	,myApp(app)
	,myAssemble(assemble)
	,myForceType(ft)
	,myPos(position)
{
	ui.setupUi(this);

	ui.SideEdit->setText(QString::number(myForceType));

	ui.EntityNameEdit->setText(myAssemble);

	connect(ui.okButton,SIGNAL(clicked()),this,SLOT(accept()));
}

ByyRemoteEntityCreateWidget::~ByyRemoteEntityCreateWidget()
{

}

void ByyRemoteEntityCreateWidget::accept()
{

}
