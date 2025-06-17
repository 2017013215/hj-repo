#ifndef CLIENT_A_H
#define CLIENT_A_H

#include <QTcpSocket>
#include <QObject>
#include <QTimer>

class ClientA : public QObject
{
    Q_OBJECT
public:
    explicit ClientA(QObject *parent = 0);
    ~ClientA();

    void connectToServer(const QString &host = "192.168.100.10", quint16 port = 8888);
    void sendReady();
	void uploadScn();
	void setScenarioPath(const QString &path) { m_scenarioPath = path; }

signals:
    void connected();
    void identified();
    void readySent();
    void disconnected();
    void errorOccurred(const QString &error);
	void fileSent();  // 文件发送完成信号

private slots:
    void onConnected();
    void onReadyRead();
    void onDisconnected();
    void onError(QAbstractSocket::SocketError error);
	void sendFileContent();  // 实际发送文件内容的函数

private:
    QTcpSocket *socket;
    bool identify;
    QTimer *reconnectTimer;
	QString m_scenarioPath;
	void sendFileHeader();  // 发送文件头
	bool waitingForReady_;  // 标记是否在等待服务器确认
	QByteArray fileData_;   // 存储文件数据
};

#endif // CLIENT_A_H