#ifndef INDEXSUBWIDGET_H
#define INDEXSUBWIDGET_H

#include <QWidget>
#include <QMap>
#include "ui_indexsubwidget.h"

#include "indexNode.h"

struct Node_Info
{
	int x;
	int y;
	QString name;
	QString tag;
	double max;
	double min;
	double weigth;
	double value;
	IndexNode* nodeWidget;
	QList<Node_Info> list;
};



class IndexSubWidget : public QWidget
{
	Q_OBJECT

public:
	IndexSubWidget(QWidget *parent = 0);
	~IndexSubWidget();

	void calcAndShow();

protected:
	void paintEvent( QPaintEvent *event );
	
private:
	bool readIndexXml();

	QString readResultFileName();
	
	void readResultData(QString fileName);

	void calcValue();

private:
	Ui::IndexSubWidget ui;
	
	//QList<Node_Info> m_nodeList;

	Node_Info m_nodeInfo;

	QMap<QString, QString> m_resultMap;

	bool m_isUpdata;
};

#endif // INDEXSUBWIDGET_H
