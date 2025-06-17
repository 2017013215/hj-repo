#ifndef SIGNALSINGLETON_H
#define SIGNALSINGLETON_H

#include <QObject>

class signalSingleton : public QObject
{
    Q_OBJECT

public:
    static signalSingleton* instance();
	// 提供一个公有方法来触发信号
    void triggerLoadSuccess();

signals:
    void loadSuccess();

private:
    explicit signalSingleton(QObject* parent = 0);
    static signalSingleton* m_instance;
};

#endif // SIGNALSINGLETON_H
