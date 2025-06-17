#ifndef AUTOHIDEDOCK_H
#define AUTOHIDEDOCK_H

#include <QDockWidget>

class ByyAutoHideDock : public QDockWidget
{
	Q_OBJECT

public:
	ByyAutoHideDock(const QString & title, QWidget * parent = 0, Qt::WindowFlags flags = 0 );
	~ByyAutoHideDock();

public slots:
	void hidden();

protected:
	void enterEvent(QEvent *event);
	void leaveEvent (QEvent * event);
	void closeEvent(QCloseEvent *event);

signals:
	void closed();

protected:
	QTimer myTimer;
};

#endif // AUTOHIDEDOCK_H
