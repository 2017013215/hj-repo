#pragma once

#include <QObject>
#include <QTcpServer>
#include <QTcpSocket>
#include <QMap>
class tcpServer : public QObject
{
    Q_OBJECT

	public:
		tcpServer(QObject* parent = nullptr);
		~tcpServer();
		void startListening(quint16 port);

	signals:
		void clientAReady();
		void clientBReady();

	private slots:
		void onNewConnection();
		void onClientData();

	private:
		QTcpServer* server_;
		QTcpSocket* socketA_;
		QTcpSocket* socketB_ ;
		QMap<QTcpSocket*, QString> clientRoles_; // Maps sockets to roles ("A" or "B")
		//接收文件
		QHash<QTcpSocket*, qint64> expectedFileSizes_;  // 存储预期的文件大小
		QHash<QTcpSocket*, QByteArray> fileBuffers_;    // 存储接收中的文件数据
		QHash<QTcpSocket*, QString> filePaths_;         // 存储文件保存路径
		void startFileTransfer(QTcpSocket* socket, const QString& message);
		void receiveFileData(QTcpSocket* socket, const QByteArray& data);
};

