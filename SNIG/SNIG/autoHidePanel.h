#ifndef AUTOHIDEPANEL_H
#define AUTOHIDEPANEL_H

#include <QObject>

class QWidget;
class ByyAutoHidePanelItem;
class ByyAutoHideDock;

class ByyAutoHidePanel : public QObject
{
	Q_OBJECT

public:
	ByyAutoHidePanel(QWidget* centralWidget,QObject *parent);
	~ByyAutoHidePanel();

	void addDockWidget(ByyAutoHideDock* dockWidget,const QString& text,const QIcon& icon);

protected:
	bool eventFilter(QObject *obj, QEvent *ev);
	void refreshPos();

protected:
	QWidget* myCentralWidget;
	QList<ByyAutoHidePanelItem*> myPannelItems;
	QList<ByyAutoHideDock*> myDockWidgets;
};

#endif // AUTOHIDEPANEL_H
