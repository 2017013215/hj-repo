#ifndef XDYNAMICTESTTHREAD_H
#define XDYNAMICTESTTHREAD_H

#include <QThread>
#include "XCoordinateGeo.h"

using namespace XSpace_Common;
namespace XGIS
{
	class XGISCanvas;
	class XShapePointJB;
}
using namespace XGIS;
class XDynamicTestThread : public QThread
{
	Q_OBJECT

public:
	XDynamicTestThread(QObject *parent,XGIS::XGISCanvas* _pGISCanvas);
	~XDynamicTestThread();

	virtual void run();

	void Stop();

	void Pause();

	void Continue();
private:
	XGIS::XGISCanvas* m_pGISCanvas;
	int m_nRumMark;	// 0:Õ£÷π 1£∫‘À–– 2£∫‘›Õ£

	XShapePointJB* m_pAirPlaneMark;
	XShapePointJB* m_pShipMark;

	QList<XCoordinateGeo> m_xAirPlanePoints;
	QList<XCoordinateGeo> m_xShipPoints;
};

#endif // XDYNAMICTESTTHREAD_H
