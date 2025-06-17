#include "stdafx.h"
#include "ByyPlot.h"
#include "curvedata.h"
#include "signaldata.h"
#include <qwt_plot_grid.h>
#include <qwt_plot_layout.h>
#include <qwt_plot_canvas.h>
#include <qwt_plot_marker.h>
#include <qwt_plot_curve.h>
#include <qwt_plot_directpainter.h>
#include <qwt_curve_fitter.h>
#include <qwt_painter.h>
#include <qwt_plot_picker.h>
#include <qwt_picker_machine.h>
#include <qwt_plot_panner.h>
#include <qevent.h>

class Canvas: public QwtPlotCanvas
{
public:
    Canvas( QwtPlot *plot = NULL ):
        QwtPlotCanvas( plot )
    {
        // The backing store is important, when working with widget
        // overlays ( f.e rubberbands for zooming ).
        // Here we don't have them and the internal
        // backing store of QWidget is good enough.

        setPaintAttribute( QwtPlotCanvas::BackingStore, false );
        setBorderRadius( 10 );

        if ( QwtPainter::isX11GraphicsSystem() )
        {
#if QT_VERSION < 0x050000
            // Even if not liked by the Qt development, Qt::WA_PaintOutsidePaintEvent
            // works on X11. This has a nice effect on the performance.

            setAttribute( Qt::WA_PaintOutsidePaintEvent, true );
#endif

            // Disabling the backing store of Qt improves the performance
            // for the direct painter even more, but the canvas becomes
            // a native window of the window system, receiving paint events
            // for resize and expose operations. Those might be expensive
            // when there are many points and the backing store of
            // the canvas is disabled. So in this application
            // we better don't both backing stores.

            if ( testPaintAttribute( QwtPlotCanvas::BackingStore ) )
            {
                setAttribute( Qt::WA_PaintOnScreen, true );
                setAttribute( Qt::WA_NoSystemBackground, true );
            }
        }

        setupPalette();
    }

private:
    void setupPalette()
    {
        QPalette pal = palette();

#if QT_VERSION >= 0x040400
        QLinearGradient gradient;
        gradient.setCoordinateMode( QGradient::StretchToDeviceMode );
        gradient.setColorAt( 0.0, QColor( 0, 49, 110 ) );
        gradient.setColorAt( 1.0, QColor( 0, 87, 174 ) );

        pal.setBrush( QPalette::Window, QBrush( gradient ) );
#else
        pal.setBrush( QPalette::Window, QBrush( color ) );
#endif

        // QPalette::WindowText is used for the curve color
        pal.setColor( QPalette::WindowText, Qt::green );

        setPalette( pal );
    }
};

ByyPlot::ByyPlot( QWidget *parent )
	: QwtPlot( parent ),
    d_interval( 0.0, 10.0 )
	,myAutoIncrement(false)
{
    d_directPainter = new QwtPlotDirectPainter();

    setAutoReplot( false );
    setCanvas( new Canvas() );

    plotLayout()->setAlignCanvasToScales( true );

    //setAxisTitle( QwtPlot::xBottom, "Time [s]" );
    setAxisScale( QwtPlot::xBottom, d_interval.minValue(), d_interval.maxValue() );
    setAxisScale( QwtPlot::yLeft, -20.0, 20.0 );

    QwtPlotGrid *grid = new QwtPlotGrid();
    grid->setPen( Qt::gray, 0.0, Qt::DotLine );
    grid->enableX( true );
    grid->enableXMin( true );
    grid->enableY( true );
    grid->enableYMin( false );
    grid->attach( this );

    /*d_origin = new QwtPlotMarker();
    d_origin->setLineStyle( QwtPlotMarker::Cross );
    d_origin->setValue( d_interval.minValue() + d_interval.width() / 2.0, 0.0 );
    d_origin->setLinePen( Qt::gray, 0.0, Qt::DashLine );
    d_origin->attach( this );*/

	//QPen pen=canvas()->palette().color( QPalette::WindowText );
}

ByyPlot::~ByyPlot()
{
    delete d_directPainter;
}

QwtPlotCurve* ByyPlot::addCurve( const QString& name,SignalData& data,const QPen & pen )
{
	QwtPlotCurve *curve = new QwtPlotCurve();
	curve->setTitle(name);
	curve->setStyle( QwtPlotCurve::Lines );
	curve->setPen(pen);
	curve->setRenderHint( QwtPlotItem::RenderAntialiased, true );
	curve->setPaintAttribute( QwtPlotCurve::ClipPolygons, false );
	curve->setData( new CurveData(data) );
	curve->attach( this );

	myCurveList.insert(curve,0);

	return curve;
}

void ByyPlot::replot()
{
	CurveMap::iterator it=myCurveList.begin();

	for (;it!=myCurveList.end();++it)
	{
		CurveData *data = static_cast<CurveData*>(it.key()->data());
		data->values().lock();
	}

	QwtPlot::replot();

	it=myCurveList.begin();

	for (;it!=myCurveList.end();++it)
	{
		CurveData *data = static_cast<CurveData *>(it.key()->data());

		it.value() = data->size();

		data->values().unlock();
	}
}

void ByyPlot::setIntervalLength( double interval )
{
    if ( interval > 0.0 && interval != d_interval.width() )
    {
        d_interval.setMaxValue( d_interval.minValue() + interval );
        setAxisScale( QwtPlot::xBottom,
            d_interval.minValue(), d_interval.maxValue() );

        replot();
    }
}

QRectF ByyPlot::updateCurve( QwtPlotCurve* curve )
{
	QRectF br;

	CurveData *data = static_cast<CurveData *>( curve->data() );
    data->values().lock();

    const int numPoints = data->size();

	int &paintedPoints=myCurveList[curve];

    if ( numPoints > paintedPoints )
    {
        const bool doClip = !canvas()->testAttribute( Qt::WA_PaintOnScreen );
        if ( doClip )
        {
            /*
                Depending on the platform setting a clip might be an important
                performance issue. F.e. for Qt Embedded this reduces the
                part of the backing store that has to be copied out - maybe
                to an unaccelerated frame buffer device.
            */

            const QwtScaleMap xMap = canvasMap( curve->xAxis() );
            const QwtScaleMap yMap = canvasMap( curve->yAxis() );

            br = qwtBoundingRect( *data,
                paintedPoints - 1, numPoints - 1 );

            const QRect clipRect = QwtScaleMap::transform( xMap, yMap, br.adjusted(-1,-1,1,1) ).toRect();
            d_directPainter->setClipRegion( clipRect );
        }

        d_directPainter->drawSeries( curve,
            paintedPoints - 1, numPoints - 1 );
        paintedPoints = numPoints;
    }

    data->values().unlock();

	return br;
}

void ByyPlot::incrementInterval()
{
    d_interval = QwtInterval( d_interval.maxValue(),
        d_interval.maxValue() + d_interval.width() );

	CurveMap::iterator it=myCurveList.begin();

	for (;it!=myCurveList.end();++it)
	{
		CurveData *data = static_cast<CurveData *>(it.key()->data() );
		data->values().clearStaleValues( d_interval.minValue() );
		it.value()=0;
	}

    // To avoid, that the grid is jumping, we disable
    // the autocalculation of the ticks and shift them
    // manually instead.

    QwtScaleDiv scaleDiv = axisScaleDiv( QwtPlot::xBottom );
    scaleDiv.setInterval( d_interval );

    for ( int i = 0; i < QwtScaleDiv::NTickTypes; i++ )
    {
        QList<double> ticks = scaleDiv.ticks( i );
        for ( int j = 0; j < ticks.size(); j++ )
            ticks[j] += d_interval.width();
        scaleDiv.setTicks( i, ticks );
    }
    setAxisScaleDiv( QwtPlot::xBottom, scaleDiv );

//    d_origin->setValue( d_interval.minValue() + d_interval.width() / 2.0, 0.0 );

    replot();
}

void ByyPlot::tick( double elapsed )
{
	CurveMap::iterator it=myCurveList.begin();

	QRectF rect;

	for (;it!=myCurveList.end();++it)
	{
		if (it.key()->isVisible())
		{
			rect|=updateCurve(it.key());
		}
	}

	{
		QwtInterval d_interval=axisInterval(QwtPlot::xBottom);

		double _width=d_interval.width();

		bool xChanged=false;

		if (rect.right()>d_interval.maxValue())
		{
			d_interval.setMaxValue(d_interval.maxValue()+_width/2);
			xChanged=true;
		}

		if (rect.left()<d_interval.minValue())
		{
			d_interval.setMinValue(d_interval.minValue()-_width/2);
			xChanged=true;
		}

		if (xChanged)
		{
			setAxisScale(QwtPlot::xBottom,d_interval.minValue(),d_interval.maxValue());
			replot();
		}
	}

	{
		QwtInterval d_interval=axisInterval(QwtPlot::yLeft);

		double _width=d_interval.width();

		bool yChanged=false;

		double rr=rect.top();
		double aa=d_interval.maxValue();

		if (rect.top()>d_interval.maxValue())
		{
			d_interval.setMaxValue(d_interval.maxValue()+_width/2);
			yChanged=true;
		}

		if (rect.bottom()<d_interval.minValue())
		{
			d_interval.setMinValue(d_interval.minValue()-_width/2);
			yChanged=true;
		}

		if (yChanged)
		{
			setAxisScale(QwtPlot::yLeft,d_interval.minValue(),d_interval.maxValue());
			replot();
		}
	}

	if (myAutoIncrement)
	{
		if ( elapsed> d_interval.maxValue() )
			incrementInterval();
	}
}

void ByyPlot::resizeEvent( QResizeEvent *event )
{
    d_directPainter->reset();
    QwtPlot::resizeEvent( event );
}

void ByyPlot::showEvent( QShowEvent * )
{
    replot();
}

bool ByyPlot::eventFilter( QObject *object, QEvent *event )
{
    if ( object == canvas() && 
        event->type() == QEvent::PaletteChange )
    {
        //d_PsiCurve->setPen( canvas()->palette().color( QPalette::WindowText ) );
    }

    return QwtPlot::eventFilter( object, event );
}

void ByyPlot::setCurveVisible( QwtPlotCurve* curve,bool v )
{
	curve->setVisible(v);

	replot();
}

void ByyPlot::fullView()
{
	setAxisAutoScale(yLeft);
	setAxisAutoScale(xBottom);
	replot();
	setAxisAutoScale(yLeft,false);
	setAxisAutoScale(xBottom,false);
}

void ByyPlot::enablePicker( bool enabled )
{
	if (enabled)
	{
		d_picker = new QwtPlotPicker( QwtPlot::xBottom, QwtPlot::yLeft,
			QwtPlotPicker::CrossRubberBand, QwtPicker::AlwaysOn,canvas() );
		d_picker->setStateMachine( new QwtPickerDragPointMachine() );
		d_picker->setRubberBandPen( QColor( Qt::green ) );
		d_picker->setRubberBand( QwtPicker::CrossRubberBand );
		d_picker->setTrackerPen( QColor( Qt::white ) );
	}
	else
	{
		delete d_picker;
		d_picker=0;
	}
}

void ByyPlot::enablePanner( bool enabled )
{
	if (enabled)
	{
		d_panner=new QwtPlotPanner(canvas());
	}
	else
	{
		delete d_panner;
		d_panner=0;
	}
}
