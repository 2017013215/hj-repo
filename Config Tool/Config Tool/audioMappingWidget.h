#ifndef AUDIOMAPPINGWIDGET_H
#define AUDIOMAPPINGWIDGET_H

#include <QWidget>
#include "ui_audioMappingWidget.h"

class ByyAudioMappingWidget : public QWidget
{
	Q_OBJECT

public:
	ByyAudioMappingWidget(QWidget *parent = 0);
	~ByyAudioMappingWidget();


	void init(const QString& table,const QString& label);

private slots:
	void add();
	void remove();
	void edit();

signals:
	void modified();

private:
	Ui::ByyAudioMappingWidget ui;
	QString d_table;
};

#endif // AUDIOMAPPINGWIDGET_H
