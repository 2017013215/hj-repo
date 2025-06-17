#ifndef OBSERVERSETTINGS_H
#define OBSERVERSETTINGS_H

#include <QObject>
#include "scene_global.h"

class SCENE_EXPORT ByyObserverSettings : public QObject
{
	Q_OBJECT

public:
	~ByyObserverSettings();

	static ByyObserverSettings& instance();

	bool oceanVisible()const;
	bool ctrlObjVisible()const;
	bool ctrlObjLableVisible() const;
	bool entityVisible()const;
	bool graticuleVisible()const;
	bool is2D()const;
	bool radioCommVisible()const;
	bool subjectionVisible()const;
	bool trackVisible()const;
	bool guidanceVisible()const;
	bool routePlanVisible()const;

	std::string viewPointInfo(){return _viewPoint;}

	int	openScale(){return myOpenScale;}
	bool	gridOpen(){return myGridOpen;}
	double	lonStart(){return myLonStart;}
	double	lonEnd(){return myLonEnd;}
	double	latStart(){return myLatStart;}
	double	latEnd(){return myLatEnd;}
	int		resolution(){return myResolution;}
	bool	clampToTerrain(){return myClampToTerrain;}
	void	setGridSettings(bool GridOpen,int openScale,double LonStart,double LonEnd,double LatStart,double LatEnd,int Resolution,bool clampToTerrain);

public slots:
	void setOceanVisible(bool b);
	void setCtrlObjVisible(bool b);
	void setCtrlObjLableVisible(bool b);
	void setEntityVisible(bool b);
	void setGraticuleVisible(bool b);
	void set2D(bool b);
	void setRadioCommVisible(bool b);
	void setSubjectionVisible(bool b);
	void setTrackVisible(bool b);
	void setGuidanceVisible(bool b);
	void setRoutePlanVisible(bool b);

signals:
	void oceanVisibleChanged(bool);
	void ctrlObjVisibleChanged(bool);
	void ctrlObjLableVisibleChanged(bool);
	void entityVisibleChanged(bool);
	void graticuleVisibleChanged(bool);
	void projectionChanged(bool);
	void radioCommVisibleChanged(bool);
	void subjectionVisibleChanged(bool);
	void trackVisibleChanged(bool);
	void guidanceVisibleChanged(bool);
	void routePlanVisibleChanged(bool);

protected:
	ByyObserverSettings();
	void readSettings();
	void writeSettings();

protected:
	bool myOceanVisible;
	bool myCtrlObjVisible;
	bool myCtrlObjLableVisible;
	bool myEntityVisible;
	bool myGraticuleVisible;
	bool my2DProjection;
	bool myRadioCommVisible;
	bool mySubjectionVisible;
	bool myTrackVisible;
	bool myGuidanceVisible;
	bool myRoutePlanVisible;
	std::string	_viewPoint;

	bool	myGridOpen;
	int		myOpenScale;
	double	myLonStart;
	double	myLonEnd;
	double	myLatStart;
	double	myLatEnd;
	int		myResolution;
	bool	myClampToTerrain;
};

#endif // OBSERVERSETTINGS_H
