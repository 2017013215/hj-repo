#include "stdafx.h"
#include "curvedata.h"
#include "signaldata.h"

CurveData::CurveData( SignalData& signalData )
	:mySignalData(signalData)
{

}

const SignalData &CurveData::values() const
{
    return mySignalData;
}

SignalData &CurveData::values()
{
    return mySignalData;
}

QPointF CurveData::sample( size_t i ) const
{
    return mySignalData.value( i );
}

size_t CurveData::size() const
{
    return mySignalData.size();
}

QRectF CurveData::boundingRect() const
{
    return mySignalData.boundingRect();
}
