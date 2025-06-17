#pragma once

#include <qwt_plot.h>
#include <qwt_interval.h>
#include <qwt_system_clock.h>

class QwtPlotCurve;
class QwtPlotMarker;
class QwtPlotDirectPainter;
class ByyEntityStorage;
class SignalData;
class QwtPlotPicker;
class QwtPlotPanner;

class ByyPlot: public QwtPlot
{
    Q_OBJECT

public:
    ByyPlot(QWidget * = NULL );
    virtual ~ByyPlot();

    virtual void replot();

    virtual bool eventFilter( QObject *, QEvent * );

	void setCurveVisible(QwtPlotCurve* curve,bool v);

	void tick(double elapsed);

	QwtPlotCurve* addCurve(const QString& name,SignalData& data,const QPen & pen);

	void fullView();

public Q_SLOTS:
    void setIntervalLength( double );
	void enablePicker(bool enabled);
	void enablePanner(bool enabled);

protected:
    virtual void showEvent( QShowEvent * );
    virtual void resizeEvent( QResizeEvent * );

private:
	QRectF updateCurve(QwtPlotCurve* curve);
    void incrementInterval();

    //QwtPlotMarker *d_origin;

    QwtPlotDirectPainter *d_directPainter;
	QwtPlotPicker *d_picker;
	QwtPlotPanner *d_panner;

    QwtInterval d_interval;

	typedef QHash<QwtPlotCurve*,int> CurveMap;

	CurveMap myCurveList;

	bool myAutoIncrement;
};
