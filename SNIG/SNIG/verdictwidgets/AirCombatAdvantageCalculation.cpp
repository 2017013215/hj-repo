#include "stdafx.h"
#include "AirCombatAdvantageCalculation.h"
#include "remoteEntity.h"
#include "JudgeMathFun.h"

static const int AXIS_MAX_X = 10, AXIS_MAX_Y = 1;/* 横纵坐标最大显示范围 */

#define GET_MIN(a,b) ((a)<(b)?(a):(b)) 
#define GET_MAX(a,b) ((a)>(b)?(a):(b))

QStringList getAirCombatAdvantageTypes()
{
	QStringList EntityList;
	EntityList<<QString::fromLocal8Bit("速度优势")<<QString::fromLocal8Bit("高度优势")<<QString::fromLocal8Bit("位置优势")<<QString::fromLocal8Bit("角度优势")<<QString::fromLocal8Bit("综合优势");
	return EntityList;
};

double weightedAverage(const double values[],const double weights[],int size)
{
	double sum = 0.0;
	double totalWeight = 0.0;

	for (int i = 0;i<size;++i)
	{
		sum += values[i]*weights[i];//分子
		totalWeight +=weights[i];//分母
	}
	if (totalWeight!=0)
	{
		return sum/totalWeight;//计算加权平均数
	}else{
		return 0.0;
	}
}

double AngleAdvantageFunction(ByyEntityObject* selfEntObject,ByyEntityObject* tgtEntObject)
{
	double agl_search_max= DegreesToRadians(45);
	double agl_lch_max = DegreesToRadians(30);
	double agl_T_max =  DegreesToRadians(20);
	JGeoCal::GeoPt selfPos,tgtPos;
	ByyRemoteEntity *selfRemoteEnt=dynamic_cast<ByyRemoteEntity*>(selfEntObject);
	ByyRemoteEntity *tgtRemoteEnt=dynamic_cast<ByyRemoteEntity*>(tgtEntObject);

	selfPos.x = selfRemoteEnt->position().y();
	selfPos.y = selfRemoteEnt->position().x();
	selfPos.z = selfRemoteEnt->position().z();
	tgtPos.x = tgtRemoteEnt->position().y();
	tgtPos.y = tgtRemoteEnt->position().x();
	tgtPos.z = tgtRemoteEnt->position().z();
	JGeoCal::TopocentricPolar tcp;
	JGeoCal::GeodeticInDegreeToTopocentricPolar(tcp,tgtPos,selfPos);
	JudgeMathFun::CJudgeMathFun fun;
	double agl_p = JGeoCal::Math::negMaxToMax(selfRemoteEnt->orientation().x()-  JGeoCal::Math::zeroTo360(tcp.Az), 180.0) * Deg2Rad;
	double agl_q = JGeoCal::Math::negMaxToMax(tgtRemoteEnt->orientation().x()-JGeoCal::Math::zeroTo360(tcp.Az), 180.0)* Deg2Rad;
	double  angleAdvantge = fun.AngleAdvantageFunction(1.0, agl_p, agl_search_max,agl_lch_max, agl_T_max,agl_q);
	return angleAdvantge;
}

double SpeedAdvantageFunction(ByyEntityObject* selfEntObject,ByyEntityObject* tgtEntObject)
{
	double Dmax=45000; // 雷达探测距离
	double DTmax=20000;// 最大不可逃逸距离
	double  vm; double vt;//定义交战飞机的速度
	ByyRemoteEntity *selfRemoteEnt=dynamic_cast<ByyRemoteEntity*>(selfEntObject);
	ByyRemoteEntity *tgtRemoteEnt=dynamic_cast<ByyRemoteEntity*>(tgtEntObject);
	vm = selfRemoteEnt->speed();
	vt = tgtRemoteEnt->speed();
	JGeoCal::GeoPt selfPos,tgtPos;
	selfPos.x = selfRemoteEnt->position().y();
	selfPos.y = selfRemoteEnt->position().x();
	selfPos.z = selfRemoteEnt->position().z();
	tgtPos.x = tgtRemoteEnt->position().y();
	tgtPos.y = tgtRemoteEnt->position().x();
	tgtPos.z = tgtRemoteEnt->position().z();

	JudgeMathFun::CJudgeMathFun fun;
	JGeoCal::TopocentricPolar tcp;
	JGeoCal::GeodeticInDegreeToTopocentricPolar(tcp,tgtPos,selfPos);
	///进行速度优势裁决
	double m_bestvel = fun.getBestSpeed(Dmax,DTmax,tcp.r,vm);
	double m_bestAircombatRate=fun.SpeedAdvantageFunction(m_bestvel, vm, vt);
	return m_bestAircombatRate;
}

double HeightAdvantageFunction(ByyEntityObject* selfEntObject,ByyEntityObject* tgtEntObject)
{
	double hm;double ht;//定义交战飞机的高度
	ByyRemoteEntity *selfRemoteEnt=dynamic_cast<ByyRemoteEntity*>(selfEntObject);
	ByyRemoteEntity *tgtRemoteEnt=dynamic_cast<ByyRemoteEntity*>(tgtEntObject);

	hm = selfRemoteEnt->position().z();
	ht = tgtRemoteEnt->position().z();

	JudgeMathFun::CJudgeMathFun fun;
	///进行高度优势裁决
	double m_bestHeightDiff=10000;
	double heightAdvantge=fun.HeightAdvantageFunction(m_bestHeightDiff,hm,ht);
	return heightAdvantge;
}

double DistanceAdvantageFunction(ByyEntityObject* selfEntObject,ByyEntityObject* tgtEntObject)
{
	double Dmax=45000; // 雷达探测距离
	double DTmax=20000;// 最大不可逃逸距离
	double DTmin=5000; //  最小不可逃逸距离
	double DATmax=30000; // 导弹最大攻击距离
	double DATmin=1000; // 导弹最小攻击距离
	JGeoCal::GeoPt selfPos,tgtPos;
	ByyRemoteEntity *selfRemoteEnt=dynamic_cast<ByyRemoteEntity*>(selfEntObject);
	ByyRemoteEntity *tgtRemoteEnt=dynamic_cast<ByyRemoteEntity*>(tgtEntObject);

	selfPos.x = selfRemoteEnt->position().y();
	selfPos.y = selfRemoteEnt->position().x();
	selfPos.z = selfRemoteEnt->position().z();
	tgtPos.x = tgtRemoteEnt->position().y();
	tgtPos.y = tgtRemoteEnt->position().x();
	tgtPos.z = tgtRemoteEnt->position().z();
	JGeoCal::TopocentricPolar tcp;
	JGeoCal::GeodeticInDegreeToTopocentricPolar(tcp,tgtPos,selfPos);
	JudgeMathFun::CJudgeMathFun fun;
	///进行位置优势裁决
	double disAdvantge=fun.DistanceAdvantageFunction(tcp.r, Dmax, DATmax, DTmax, DTmin, DATmin);
	return disAdvantge;
}

double airCombatSuperiority(ByyEntityObject* selfEntObject,ByyEntityObject* tgtEntObject)
{
	double  vm; double vt;//定义交战飞机的速度
	double hm;double ht;//定义交战飞机的高度
	double Dmax=45000; // 雷达探测距离
	double DTmax=20000;// 最大不可逃逸距离
	double DTmin=5000; //  最小不可逃逸距离
	double DATmax=30000; // 导弹最大攻击距离
	double DATmin=1000; // 导弹最小攻击距离
	double agl_search_max= DegreesToRadians(45);
	double agl_lch_max = DegreesToRadians(30);
	double agl_T_max =  DegreesToRadians(20);
	JGeoCal::GeoPt selfPos,tgtPos;
	ByyRemoteEntity *selfRemoteEnt=dynamic_cast<ByyRemoteEntity*>(selfEntObject);
	ByyRemoteEntity *tgtRemoteEnt=dynamic_cast<ByyRemoteEntity*>(tgtEntObject);

	selfPos.x = selfRemoteEnt->position().y();
	selfPos.y = selfRemoteEnt->position().x();
	selfPos.z = selfRemoteEnt->position().z();
	tgtPos.x = tgtRemoteEnt->position().y();
	tgtPos.y = tgtRemoteEnt->position().x();
	tgtPos.z = tgtRemoteEnt->position().z();
	vm = selfRemoteEnt->speed();
	vt = tgtRemoteEnt->speed();
	hm = selfRemoteEnt->position().z();
	ht = tgtRemoteEnt->position().z();

	JudgeMathFun::CJudgeMathFun fun;
	JGeoCal::TopocentricPolar tcp;
	JGeoCal::GeodeticInDegreeToTopocentricPolar(tcp,tgtPos,selfPos);

	///进行速度优势裁决
	double m_bestAircombatRate=fun.getBestSpeed(Dmax,DTmax,tcp.r,vm);
	double rateAdvantge=fun.SpeedAdvantageFunction(m_bestAircombatRate, vm, vt);

	///进行高度优势裁决
	double m_bestHeightDiff=10000;
	double heightAdvantge=fun.HeightAdvantageFunction(m_bestHeightDiff,hm,ht);

	///进行位置优势裁决
	double disAdvantge=fun.DistanceAdvantageFunction(tcp.r, Dmax, DATmax, DTmax, DTmin, DATmin);

	///进行角度优势裁决
	double agl_p = JGeoCal::Math::negMaxToMax(selfRemoteEnt->orientation().x()-  JGeoCal::Math::zeroTo360(tcp.Az), 180.0) * Deg2Rad;
	double agl_q = JGeoCal::Math::negMaxToMax(tgtRemoteEnt->orientation().x()-JGeoCal::Math::zeroTo360(tcp.Az), 180.0)* Deg2Rad;
	double  angleAdvantge=fun.AngleAdvantageFunction(1.0, agl_p, agl_search_max,agl_lch_max, agl_T_max,agl_q);

	const int size=4;
	double values[size] = {rateAdvantge,heightAdvantge,disAdvantge,angleAdvantge};
	double weights[size] = {0.15,0.25,0.10,0.50};
	double result = weightedAverage(values,weights,size);

	return result;
}

AirCombatAdvantageCalculation::AirCombatAdvantageCalculation(VerdictBaseWidget* parent /*= nullptr*/)
	: VerdictBaseWidget(parent)
{
	createWidget();
	setupLayout();
	updateEntities();
	resetUi();
}

// 空战优势裁决
void AirCombatAdvantageCalculation::createButton()
{
	Button_AirCombatAdvantageCalculation = new QPushButton;
	Button_AirCombatAdvantageCalculation->setText(START_QUERY);
	//Button_AirCombatAdvantageCalculation->setEnabled(false);
	connect(Button_AirCombatAdvantageCalculation, SIGNAL(clicked()), this, SLOT(slotBtnAirCombatAdvantageCalculation()));
}

// 空战优势裁决
void AirCombatAdvantageCalculation::createChart()
{
	foreach(QString AirCombatAdvantageType, getAirCombatAdvantageTypes())
	{
		AirCombatAdvantageChart chart;
		chart.graphicsView_AirCombatAdvantageCalculation = new QChartView(this);
		chart.graphicsView_AirCombatAdvantageCalculation->setRenderHint(QPainter::Antialiasing);
		chart.graphicsView_AirCombatAdvantageCalculation->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding);

		// 创建横纵坐标轴并设置显示范围
		chart.axisX_ACAC = new QValueAxis();
		chart.axisY_ACAC = new QValueAxis();
		chart.axisX_ACAC->setTitleText(QString::fromLocal8Bit("时间"));
		chart.axisY_ACAC->setTitleText(AirCombatAdvantageType);
		chart.axisX_ACAC->setMin(0);
		chart.axisY_ACAC->setMin(0);
		chart.axisX_ACAC->setMax(AXIS_MAX_X);
		chart.axisY_ACAC->setMax(AXIS_MAX_Y);

		chart.R_lineSeries_ACAC = new QSplineSeries();                  // 创建曲线绘制对象
		chart.R_lineSeries_ACAC->setPointsVisible(true);                // 设置数据点可见
		chart.R_lineSeries_ACAC->setPen(QPen(Qt::red));
		QString rstr = ComboBox_AirCombatAdvantageCalculation_Red->itemText(ComboBox_AirCombatAdvantageCalculation_Red->currentIndex());
		chart.R_lineSeries_ACAC->setName(rstr);							// 图例名称

		chart.B_lineSeries_ACAC = new QSplineSeries();                  // 创建曲线绘制对象
		chart.B_lineSeries_ACAC->setPointsVisible(true);                // 设置数据点可见
		chart.B_lineSeries_ACAC->setPen(QPen(Qt::blue));
		QString bstr = ComboBox_AirCombatAdvantageCalculation_Blue->itemText(ComboBox_AirCombatAdvantageCalculation_Blue->currentIndex());
		chart.B_lineSeries_ACAC->setName(bstr);							// 图例名称

		chart.chart_ACAC = new QChart();								// 创建图表对象
		chart.chart_ACAC->addAxis(chart.axisX_ACAC, Qt::AlignBottom);			// 将X轴添加到图表上
		chart.chart_ACAC->addAxis(chart.axisY_ACAC, Qt::AlignLeft);           // 将Y轴添加到图表上
		chart.chart_ACAC->addSeries(chart.R_lineSeries_ACAC); 
		chart.chart_ACAC->addSeries(chart.B_lineSeries_ACAC);                    // 将曲线对象添加到图表上
		chart.chart_ACAC->setAnimationOptions(QChart::SeriesAnimations);   // 动画：能使曲线绘制显示的更平滑，过渡效果更好看
		chart.chart_ACAC->legend()->setAlignment(Qt::AlignRight);
		chart.chart_ACAC->resize(chart.graphicsView_AirCombatAdvantageCalculation->size());

		chart.R_lineSeries_ACAC->attachAxis(chart.axisX_ACAC);             // 曲线对象关联上X轴，此步骤必须在m_chart->addSeries之后
		chart.R_lineSeries_ACAC->attachAxis(chart.axisY_ACAC);             // 曲线对象关联上Y轴，此步骤必须在m_chart->addSeries之后
		chart.B_lineSeries_ACAC->attachAxis(chart.axisX_ACAC);    
		chart.B_lineSeries_ACAC->attachAxis(chart.axisY_ACAC);

		chart.graphicsView_AirCombatAdvantageCalculation->setChart(chart.chart_ACAC);					 // 将图表对象设置到graphicsView上进行显示
		chart.graphicsView_AirCombatAdvantageCalculation->setRenderHint(QPainter::Antialiasing);       // 设置渲染：抗锯齿，如果不设置那么曲线就显得不平滑
		m_AirCombatAdvantages.insert(AirCombatAdvantageType, chart);
	}
}

// 空战优势裁决
void AirCombatAdvantageCalculation::createComBox()
{
	ComboBox_AirCombatAdvantageCalculation_Red = new QComboBox;
	ComboBox_AirCombatAdvantageCalculation_Red->addItem(RED_SIDE);
	ComboBox_AirCombatAdvantageCalculation_Red->setStyleSheet("QComboBox {combobox-popup: 1;}");
	ComboBox_AirCombatAdvantageCalculation_Red->setEditable(true);
	ComboBox_AirCombatAdvantageCalculation_Red->lineEdit()->setAlignment(Qt::AlignCenter);
	ComboBox_AirCombatAdvantageCalculation_Red->lineEdit()->setReadOnly(true);
	connect(ComboBox_AirCombatAdvantageCalculation_Red,SIGNAL(currentIndexChanged(int)),this,SLOT(slotAirCombatAdvantageCalculationRed(int)));

	ComboBox_AirCombatAdvantageCalculation_Blue = new QComboBox;
	ComboBox_AirCombatAdvantageCalculation_Blue->addItem(BLUE_SIDE);
	ComboBox_AirCombatAdvantageCalculation_Blue->setStyleSheet("QComboBox {combobox-popup: 1;}");
	ComboBox_AirCombatAdvantageCalculation_Blue->setEditable(true);
	ComboBox_AirCombatAdvantageCalculation_Blue->lineEdit()->setAlignment(Qt::AlignCenter);
	ComboBox_AirCombatAdvantageCalculation_Blue->lineEdit()->setReadOnly(true);
	connect(ComboBox_AirCombatAdvantageCalculation_Blue,SIGNAL(currentIndexChanged(int)),this,SLOT(slotAirCombatAdvantageCalculationBlue(int)));

	QStringList EntityList;
	EntityList<<QString::fromLocal8Bit("速度优势")<<QString::fromLocal8Bit("高度优势")<<QString::fromLocal8Bit("位置优势")<<QString::fromLocal8Bit("角度优势")<<QString::fromLocal8Bit("综合优势");
	ComboBox_AirCombatAdvantageCalculation_Advantage = new QComboBox;
	ComboBox_AirCombatAdvantageCalculation_Advantage->setEditable(true);
	ComboBox_AirCombatAdvantageCalculation_Advantage->lineEdit()->setAlignment(Qt::AlignCenter);
	ComboBox_AirCombatAdvantageCalculation_Advantage->lineEdit()->setReadOnly(true);
	ComboBox_AirCombatAdvantageCalculation_Advantage->addItems(EntityList);
	connect(ComboBox_AirCombatAdvantageCalculation_Advantage,SIGNAL(currentIndexChanged(int)),this,SLOT(slotAirCombatAdvantageCalculationAdvantage(int)));
	ComboBox_AirCombatAdvantageCalculation_Advantage->hide();
}

void AirCombatAdvantageCalculation::createTableView()
{

}

void AirCombatAdvantageCalculation::setupLayout()
{
	Layout_AirCombatAdvantageCalculation = new QGridLayout ;
	Layout_AirCombatAdvantageCalculation->addWidget(ComboBox_AirCombatAdvantageCalculation_Red,0,0);
	Layout_AirCombatAdvantageCalculation->addWidget(ComboBox_AirCombatAdvantageCalculation_Blue,0,1);
	//Layout_AirCombatAdvantageCalculation->addWidget(ComboBox_AirCombatAdvantageCalculation_Advantage,0,2);
	Layout_AirCombatAdvantageCalculation->addWidget(Button_AirCombatAdvantageCalculation,0,2);
	int index  = 1;
	foreach(QString AirCombatAdvantageType, getAirCombatAdvantageTypes())
	{
		if(m_AirCombatAdvantages.contains(AirCombatAdvantageType))
		{
			Layout_AirCombatAdvantageCalculation->addWidget(m_AirCombatAdvantages[AirCombatAdvantageType].graphicsView_AirCombatAdvantageCalculation,index++,0,1,4);
		}
	}
	setLayout(Layout_AirCombatAdvantageCalculation);
}

void AirCombatAdvantageCalculation::resetUi()
{
	ComboBox_AirCombatAdvantageCalculation_Red->clear();
	ComboBox_AirCombatAdvantageCalculation_Blue->clear();
	QStringList airobjects;
	airobjects.append(getEntitiesByType(RED_SIDE, AirEntityObject));
	airobjects.append(getEntitiesByType(BLUE_SIDE, AirEntityObject));
	ComboBox_AirCombatAdvantageCalculation_Red->addItems(airobjects);
	Button_AirCombatAdvantageCalculation->setEnabled(true);
}

void AirCombatAdvantageCalculation::preHandleDataFrame()
{
	foreach(QString AirCombatAdvantageType, getAirCombatAdvantageTypes())
	{
		if(m_AirCombatAdvantages.contains(AirCombatAdvantageType))
		{
			m_AirCombatAdvantages[AirCombatAdvantageType].reset();
		}
	}
}

void AirCombatAdvantageCalculation::slotBtnAirCombatAdvantageCalculation()
{
	if(isDataUpdata())
	{
		stopDataHandle();
		ComboBox_AirCombatAdvantageCalculation_Red->setEnabled(true);
		ComboBox_AirCombatAdvantageCalculation_Blue->setEnabled(true);
		ComboBox_AirCombatAdvantageCalculation_Advantage->setEnabled(true);
		Button_AirCombatAdvantageCalculation->setEnabled(true);
		Button_AirCombatAdvantageCalculation->setText(START_QUERY);
	}
	else
	{
		startDataHandle(1000);
		ComboBox_AirCombatAdvantageCalculation_Red->setEnabled(false);
		ComboBox_AirCombatAdvantageCalculation_Blue->setEnabled(false);
		ComboBox_AirCombatAdvantageCalculation_Advantage->setEnabled(false);
		Button_AirCombatAdvantageCalculation->setText(STOP_QUERY);
	}
}

void AirCombatAdvantageCalculation::handleDataFrame()
{
	//生成数据
	ByyEntityObject* redEntity = getEntityByName(ComboBox_AirCombatAdvantageCalculation_Red->currentText());
	ByyEntityObject* blueEntity = getEntityByName(ComboBox_AirCombatAdvantageCalculation_Blue->currentText());
	if(redEntity && blueEntity)
	{
		for (int index = 0; index < 5; index++)
		{
			AirCombatAdvantageData data;
			m_currentAdvantage = index;
			data.redAdvantage = switchAirCombatAdvantage(redEntity, blueEntity);
			data.blueAdvantage = switchAirCombatAdvantage(blueEntity, redEntity);
			//更新数据
			updataAirCombatAdvantageData(getAirCombatAdvantageTypes().at(index), data);
		}
	}
}

void AirCombatAdvantageCalculation::updataAirCombatAdvantageData(const QString& AirCombatAdvantageType, const AirCombatAdvantageData& data)
{
	if(m_AirCombatAdvantages.contains(AirCombatAdvantageType))
	{
		m_AirCombatAdvantages[AirCombatAdvantageType].updataAirCombatAdvantageData(data);
	}
}

void AirCombatAdvantageCalculation::stopHandle()
{
	slotBtnAirCombatAdvantageCalculation();
}

void AirCombatAdvantageCalculation::slotAirCombatAdvantageCalculationRed(int index)
{
	ComboBox_AirCombatAdvantageCalculation_Blue->clear();
	ComboBox_AirCombatAdvantageCalculation_Blue->addItems(getEntitiesByType(getAntiPartyName(getEntityByName(ComboBox_AirCombatAdvantageCalculation_Red->currentText())), AirEntityObject));
}

void AirCombatAdvantageCalculation::slotAirCombatAdvantageCalculationBlue(int index)
{

}

void AirCombatAdvantageCalculation::slotAirCombatAdvantageCalculationAdvantage(int index)
{
	m_currentAdvantage = index;
}

double AirCombatAdvantageCalculation::switchAirCombatAdvantage(ByyEntityObject* selfEntObject,ByyEntityObject* tgtEntObject)
{
	double Advantage = 0;
	if(selfEntObject && tgtEntObject)
	{
		switch (m_currentAdvantage)
		{
			//速度优势
		case 0:
			{
				Advantage = SpeedAdvantageFunction(selfEntObject,tgtEntObject);
				break;
			}
			//高度优势
		case 1:
			{
				Advantage = HeightAdvantageFunction(selfEntObject,tgtEntObject);
				break;
			}
			//位置优势
		case 2:
			{
				Advantage = DistanceAdvantageFunction(selfEntObject,tgtEntObject);
				break;
			}
			//角度优势
		case 3:
			{
				Advantage = AngleAdvantageFunction(selfEntObject,tgtEntObject);
				break;
			}
			//综合优势
		case 4:
			{
				Advantage = airCombatSuperiority(selfEntObject,tgtEntObject) ;
				break;
			}
		default:
			break;
		}
	}
	return Advantage;
}

void AirCombatAdvantageChart::reset()
{
	R_lineSeries_ACAC->clear();
	B_lineSeries_ACAC->clear();

	pointCount = 0;
	localYMin = 0.0;
	localYMax = 1.0;
}

void AirCombatAdvantageChart::updataAirCombatAdvantageData(const AirCombatAdvantageData& data)
{
	if(pointCount > AXIS_MAX_X)
	{
		R_lineSeries_ACAC->remove(0);
		B_lineSeries_ACAC->remove(0);
		chart_ACAC->axisX()->setMin(pointCount - AXIS_MAX_X);
		chart_ACAC->axisX()->setMax(pointCount);                    // 更新X轴范围
	}
	//
	double yMax = GET_MAX(data.redAdvantage, data.blueAdvantage);
	double yMin = GET_MIN(data.redAdvantage, data.blueAdvantage);

	int recordcount = 0;
	double yRMax = 1.0, yRMin = 0.0;
	for(int index = R_lineSeries_ACAC->count() - 1;index >= 0; index--)
	{
		recordcount++;
		if(recordcount>=AXIS_MAX_X) //
		{
			break;
		}
		else if(1 == recordcount)
		{
			yRMax = R_lineSeries_ACAC->at(index).y();
			yRMin = R_lineSeries_ACAC->at(index).y();
		}
		else
		{
			yRMax = GET_MAX(yRMax, R_lineSeries_ACAC->at(index).y());
			yRMin = GET_MIN(yRMin, R_lineSeries_ACAC->at(index).y());
		}
	}
	recordcount = 0;
	double yBMax = 1.0, yBMin = 0.0;
	for(int index = B_lineSeries_ACAC->count() - 1;index >= 0; index--)
	{
		recordcount++;
		if(recordcount>=AXIS_MAX_X) //
		{
			break;
		}
		else if(1 == recordcount)
		{
			yBMax = B_lineSeries_ACAC->at(index).y();
			yBMin = B_lineSeries_ACAC->at(index).y();
		}
		else
		{
			yBMax = GET_MAX(yBMax, B_lineSeries_ACAC->at(index).y());
			yBMin = GET_MIN(yBMin, B_lineSeries_ACAC->at(index).y());
		}
	}

	double vMax,vMin;
	vMax = GET_MAX(yMax, GET_MAX(yBMax, yRMax));
	vMin = GET_MIN(yMin, GET_MIN(yBMin, yRMin));
	vMax = vMax>0? vMax*1.1 : vMax*0.9;
	vMin = vMin>0? vMin*0.9 : vMin*1.1;
	chart_ACAC->axisY()->setMax(vMax);
	chart_ACAC->axisY()->setMin(vMin);
	R_lineSeries_ACAC->append(QPointF(pointCount, data.redAdvantage)); 
	B_lineSeries_ACAC->append(QPointF(pointCount, data.blueAdvantage));  // 更新显示
	pointCount++;
}
