#ifndef AUDIOMAPPINGWIDGET_H
#define AUDIOMAPPINGWIDGET_H

#include <QWidget>
#include "ui_audioMappingWidget.h"

class ByyAbstractMapping;

class ByyAudioMappingWidget : public QWidget
{
	Q_OBJECT

public:
	ByyAudioMappingWidget(QWidget *parent = 0);
	~ByyAudioMappingWidget();


	void init(ByyAbstractMapping* m,const QString& label);

private slots:
	void add();
	void remove();
	void edit();

private:
	Ui::ByyAudioMappingWidget ui;
	ByyAbstractMapping *mapping;
};

#endif // AUDIOMAPPINGWIDGET_H
