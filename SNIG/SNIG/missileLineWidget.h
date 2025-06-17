#ifndef MISSILELINEWIDGET_H
#define MISSILELINEWIDGET_H

#include <QWidget>
#include <QObject>
#include <QTimer>
#include "ui_missileLineWidget.h"

class ByyApp;
class MissileLineWidget : public QWidget
{
	Q_OBJECT
public:
	struct MissileInfo
	{
		int id;
		QString name;
		double height;
		double time;
		double v;
		int type;
	};

public:
	MissileLineWidget(QWidget *parent = 0);
	~MissileLineWidget();
	
	void setApp(ByyApp* app);

	void clearUi();

public slots:
	void slt_misslieInfo(int id, double height, double time, double v, QString name, int type);
	
protected:

	void paintEvent( QPaintEvent *event);
private slots:
	void onTick();

private:
	Ui::MissileLineWidget ui;
	ByyApp* myApp;

	QMap<int, QList<MissileInfo> > m_missileMap;

	QTimer timer;
	double m_maxTime;
};

#endif // MISSILELINEWIDGET_H
