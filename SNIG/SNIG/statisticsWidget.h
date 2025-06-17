#ifndef STATISTICSWIDGET_H
#define STATISTICSWIDGET_H

#include <QWidget>
#include <QObject>
#include <QDialog>
#include <QTimer>
#include "ui_statisticsWidget.h"


class ByyApp;
class Statisticswidget : public QDialog
{
	Q_OBJECT

public:
	Statisticswidget(QWidget *parent = 0);
	~Statisticswidget();

	static Statisticswidget* instance();

	void setApp(ByyApp* app);

public slots:
	void slt_missileTJ(int type);
	
	void slt_entityDm(int type, QString name);

	void slt_started();

private slots:
	void onTick();
	
private:
	void updataInfo();

private:
	Ui::Statisticswidget ui;

	ByyApp* myApp;


	int m_missileNumFJ;  //反舰导弹数量
	int m_missileNumLJ;//拦截弹数量
	int m_missileNumMZ;//命中数量
	int m_missileNumBL;//被拦截数量
	int m_missileNumTF;//突防数量
	int m_missileNumGR;//被干扰数量
	double m_tfgl;//突防概率
	double m_ljgl;//拦截成功率
	QList<QString> m_dmNamList;
	QTimer timer;

};

#endif // STATISTICSWIDGET_H
