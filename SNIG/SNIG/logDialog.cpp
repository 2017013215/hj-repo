#include "StdAfx.h"
#include "logDialog.h"


LogDialog::LogDialog(QWidget *parent)
    : QDialog(parent), logTextEdit(new QTextEdit(this))
{
    setAttribute(Qt::WA_DeleteOnClose);  // 关键设置：关闭时自动删除
	setWindowTitle(QString::fromLocal8Bit("联机模式"));
    logTextEdit->setReadOnly(true);  // 只读，不允许编辑
    logTextEdit->setMinimumSize(400, 300);
    QVBoxLayout *layout = new QVBoxLayout;
    layout->addWidget(logTextEdit);
    setLayout(layout);
}

LogDialog::~LogDialog()
{
    emit windowClosed();  // 发送窗口关闭信号
}

void LogDialog::appendLog(const QString &message)
{
    logTextEdit->append(message);  // 在 QTextEdit 中追加日志
    logTextEdit->verticalScrollBar()->setValue(logTextEdit->verticalScrollBar()->maximum());  // 自动滚动到底部
}