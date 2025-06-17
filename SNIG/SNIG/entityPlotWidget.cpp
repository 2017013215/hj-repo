#include "StdAfx.h"
#include "entityPlotWidget.h"
#include "plot/ByyPlot.h"
#include "entityObject.h"
#include "ByyApp.h"
#include "ByyCore.h"
#include "localObjectManager.h"
#include "remoteObjectManager.h"
#include <qwt_plot_renderer.h>

ByyEntityPlotWidget1::ByyEntityPlotWidget1( ByyApp& app,ByyEntityObject* entity/*=0*/,QWidget* parent/*=0*/,Qt::WindowFlags f /*= 0*/ ) 
	: ByyEntityObserver(app.core(),entity,parent,f)
	,myApp(app)
	,myMaxXAxis(60)
	,myMaxYAxis(10000)
{
	if (entity)
	{
		setWindowTitle(entity->name());
	}
	

	QVBoxLayout *mainLayout=new QVBoxLayout;
	mainLayout->setMargin(0);
	setLayout(mainLayout);

	QLabel*	label=new QLabel(QString::fromLocal8Bit("参数"));
	QComboBox* comboBox=new QComboBox;
	QPushButton* btn=new QPushButton(QString::fromLocal8Bit("添加"));

	QHBoxLayout* hboxLayout=new QHBoxLayout;
	hboxLayout->addWidget(label);
	hboxLayout->addWidget(comboBox);
	hboxLayout->addWidget(btn);
	hboxLayout->addStretch();

	mainLayout->addLayout(hboxLayout);

	QToolBar* toolBar=new QToolBar(this);
	mainLayout->addWidget(toolBar);

	myTabBar=new QTabBar;
	myTabBar->setExpanding(false);
	mainLayout->addWidget(myTabBar);
	connect(myTabBar,SIGNAL(currentChanged(int)),this,SLOT(onCurrentCurveChanged(int)));

	myPlot=new ByyPlot(this);
	myPlot->setAxisScale( QwtPlot::xBottom, 0, myMaxXAxis);
	myPlot->setAxisScale( QwtPlot::yLeft, 0.0, myMaxYAxis );

	mainLayout->addWidget(myPlot);

	addCurve(QString::fromLocal8Bit("高度"),myAlt);
	addCurve(QString::fromLocal8Bit("速度"),mySpeed);

	QToolButton *btnPicker=new QToolButton(this);
	btnPicker->setText(QString::fromLocal8Bit("撷取"));
	btnPicker->setCheckable(true);
	btnPicker->setToolButtonStyle( Qt::ToolButtonTextUnderIcon );
	toolBar->addWidget(btnPicker);
	connect(btnPicker,SIGNAL(clicked(bool)),myPlot,SLOT(enablePicker(bool)));

	QToolButton *btnExport = new QToolButton( toolBar );
	btnExport->setText( QString::fromLocal8Bit("导出") );
	//btnExport->setIcon( QPixmap( print_xpm ) );
	btnExport->setToolButtonStyle( Qt::ToolButtonTextUnderIcon );
	toolBar->addWidget( btnExport );
	connect( btnExport, SIGNAL( clicked() ), SLOT( exportDocument() ) );

	connect(&myApp.core(),SIGNAL(postTick()),this,SLOT(tick()));

	myElapsedTimer.start();

	resize(870,500);
}

ByyEntityPlotWidget1::~ByyEntityPlotWidget1()
{

}

void ByyEntityPlotWidget1::addCurve( const QString& name,SignalData& sd )
{
	int index=myTabBar->addTab(name);
	QwtPlotCurve* curve=myPlot->addCurve(name,sd,QPen(Qt::green,3));
	myCurveList.append(curve);

	if (myTabBar->currentIndex()!=index)
	{
		myPlot->setCurveVisible(curve,false);
	}
}

void ByyEntityPlotWidget1::tick()
{
	if (myCurrentEntity&&myElapsedTimer.elapsed()>=200)
	{
		double simTime=myApp.hdose().simTime();

		myAlt.append(QPointF(simTime,myCurrentEntity->position().z()));
		mySpeed.append(QPointF(simTime,myCurrentEntity->speed()));

		myPlot->tick(simTime);

		myElapsedTimer.restart();
	}
}

void ByyEntityPlotWidget1::exportDocument()
{
	QwtPlotRenderer renderer;
	renderer.exportTo( myPlot, "bode.pdf");
}

void ByyEntityPlotWidget1::onCurrentCurveChanged( int index )
{
	for (int i=0;i!=myCurveList.size();++i)
	{
		myPlot->setCurveVisible(myCurveList[i],i==index);
	}

	myPlot->fullView();
}
