#include "stdafx.h"
#include "abstractManageWidget.h"

#include <osgDB/FileNameUtils>
#include <osgEarthDrivers/gdal/GDALOptions>
#include "variantmanager.h"
#include "qtpropertybrowser-2.5_1/qtvariantproperty.h"
#include "qtpropertybrowser-2.5_1/qttreepropertybrowser.h"

ByyAbstractManageWidget::ByyAbstractManageWidget(ByySceneController *scene,QWidget *parent)
	: ByyOperateWidget(scene,parent)
{
	createWidgets();
}

ByyAbstractManageWidget::~ByyAbstractManageWidget()
{

}

void ByyAbstractManageWidget::createWidgets()
{
	variantManager = new ByyVariantManager(this);

	connect(variantManager,SIGNAL(valueChanged(QtProperty*,const QVariant&)),SLOT(valueChanged(QtProperty*,const QVariant&)));

	variantFactory = new QtVariantEditorFactory(this);

	variantEditor = new QtTreePropertyBrowser();
	 
	variantEditor->setFactoryForManager((QtVariantPropertyManager*)variantManager, variantFactory);

	variantEditor->setPropertiesWithoutValueMarked(true);
	variantEditor->setRootIsDecorated(false);

	QVBoxLayout *mainLayout=new QVBoxLayout;
	mainLayout->addWidget(variantEditor);
	mainLayout->setContentsMargins(0, 0, 0, 0);
	mainLayout->setSpacing(0);

	setLayout(mainLayout);

	variantEditor->installEventFilter(this);

	moveUpAction=new QAction(QIcon("../data/icons/earthtool/moveup.png"),QString::fromLocal8Bit("ÉÏÒÆ"),this);
	connect(moveUpAction, SIGNAL(triggered()), this, SLOT(moveUp()));

	moveDownAction=new QAction(QIcon("../data/icons/earthtool/movedown.png"),QString::fromLocal8Bit("ÏÂÒÆ"),this);
	connect(moveDownAction, SIGNAL(triggered()), this, SLOT(moveDown()));

	removeAction=new QAction(QString::fromLocal8Bit("ÒÆ³ý"),this);
	connect(removeAction, SIGNAL(triggered()), this, SLOT(removeToggle()));
}

bool ByyAbstractManageWidget::eventFilter( QObject *obj, QEvent *event )
{
	if (obj==variantEditor&&event->type()==QEvent::ContextMenu)
	{
		if (variantEditor->currentItem()->parent()==NULL)
		{
			QMenu popMenu;
			popMenu.addAction(moveUpAction);
			popMenu.addAction(moveDownAction);
			popMenu.addSeparator();
			popMenu.addAction(removeAction);
			popMenu.exec(QCursor::pos());
		}

		return true;
	}
	else
		return QObject::eventFilter(obj, event);
}