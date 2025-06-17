// LogDialog.h
#ifndef LOGDIALOG_H
#define LOGDIALOG_H

#include <QDialog>
#include <QTextEdit>

class LogDialog : public QDialog
{
    Q_OBJECT

public:
    explicit LogDialog(QWidget *parent = nullptr);
	~LogDialog();  // 析构函数
    void appendLog(const QString &message);

private:
    QTextEdit *logTextEdit;

signals:
    void windowClosed();  // 窗口关闭信号
};

#endif // LOGDIALOG_H
