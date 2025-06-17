#ifndef TASKEDITORWIDGET_H
#define TASKEDITORWIDGET_H

#include <QWidget>

class ByyTask;
class ByyIG;

class ByyTaskEditorWidget : public QWidget
{
	Q_OBJECT
public:
	ByyTaskEditorWidget(ByyIG& ig,QWidget *parent);
	virtual ~ByyTaskEditorWidget();

	virtual	void	createWidget(){}
	virtual void init(ByyTask* task)=0;
	virtual ByyTask* task()=0;

signals:
	void accepted();
	void rejected();

protected:
	ByyIG& myIG;	
};

class ByyTaskEditorWidgetCreator
{
public:
	virtual ByyTaskEditorWidget* create(ByyIG& ig,QWidget* parent)=0;
	virtual QString type()=0;
};

template<class T,class E>
class ByyTaskEditorWidgetCreatorTemplate : public ByyTaskEditorWidgetCreator
{
public:
	virtual ByyTaskEditorWidget* create(ByyIG& ig,QWidget* parent)
	{
		return new T(ig,parent);
	}
	virtual QString type()
	{
		return E::theTypeInfo().type;
	}
};

#endif // TASKEDITORWIDGET_H
