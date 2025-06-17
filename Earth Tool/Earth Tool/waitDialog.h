#ifndef WAITDIALOG_H
#define WAITDIALOG_H

#include <QDialog>

class ByyWaitDialog : public QDialog
{
public:
	ByyWaitDialog(const QString &message, QWidget *parent = 0);
	~ByyWaitDialog();

private:
	
};

#endif // WAITDIALOG_H
