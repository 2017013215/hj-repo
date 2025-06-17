#include "StdAfx.h"
#include "tcpServer.h"
#include <QByteArray>
#include <QDebug>

tcpServer::tcpServer(QObject* parent)
    : QObject(parent), server_(nullptr), socketA_(nullptr), socketB_(nullptr)
{
}

tcpServer::~tcpServer()
{
	//断开客户端连接（但不需手动delete）
    if (socketA_) {
        socketA_->disconnectFromHost();
        if (socketA_->state() != QAbstractSocket::UnconnectedState) {
            socketA_->waitForDisconnected(1000);
        }
        // 不需要 socketA_->deleteLater(); 
        socketA_ = nullptr;  // 依然需要置空指针
    }

    // socketB_同理...
	if (socketB_) {
		socketB_->disconnectFromHost();
	if (socketB_->state() != QAbstractSocket::UnconnectedState) {
		socketB_->waitForDisconnected(1000);
	}
		socketB_ = nullptr;  // 依然需要置空指针
	}

    // 2. 关闭服务器
    if (server_) {
        server_->close();
		server_->deleteLater();
        server_ = nullptr;
    }

    // 3. 清理映射表
    clientRoles_.clear();
}

void tcpServer::startListening(quint16 port)
{
    server_ = new QTcpServer(this);
    connect(server_, SIGNAL(newConnection()), this, SLOT(onNewConnection()));
    if (!server_->listen(QHostAddress::Any, port))
        qDebug() << "Failed to listen on port" << port;
    else
        qDebug() << "Listening on port" << port;
}

void tcpServer::onNewConnection()
{
    QTcpSocket* clientSocket = server_->nextPendingConnection();
    connect(clientSocket, SIGNAL(readyRead()), this, SLOT(onClientData()));
    qDebug() << "New client connected.";
}

void tcpServer::onClientData()
{
    QTcpSocket* senderSocket = qobject_cast<QTcpSocket*>(sender());
    if (!senderSocket) return;

    QByteArray data = senderSocket->readAll();
	// 检查是否在接收文件中
    if (expectedFileSizes_.contains(senderSocket)) {
        receiveFileData(senderSocket, data);
        return;
    }

    QString message = QString::fromUtf8(data).trimmed();
    qDebug() << "Received from client:" << message;

    if (!clientRoles_.contains(senderSocket)) {
        // First message is identity
        if (message == "CLIENT_A") {
            socketA_ = senderSocket;
            clientRoles_[senderSocket] = "A";
            qDebug() << "Identified CLIENT_A";
			// 向客户端发送确认消息
            senderSocket->write("IDENTIFIED_A\n");
        } else if (message == "CLIENT_B") {
            socketB_ = senderSocket;
            clientRoles_[senderSocket] = "B";
            qDebug() << "Identified CLIENT_B";
			// 向客户端发送确认消息
            senderSocket->write("IDENTIFIED_B\n");
        }
        return;
    }

	// 处理文件传输开始指令
    if (message.startsWith("FILE_SIZE:")) {
        startFileTransfer(senderSocket, message);
        return;
    }

    // Process READY message
    QString role = clientRoles_[senderSocket];
    if (message == "READY") {
        if (role == "A"){
			emit clientAReady();
		}
		else if (role == "B"){
			emit clientBReady();
		}
    }
}

void tcpServer::startFileTransfer(QTcpSocket* socket, const QString& message)
{
    // 解析文件大小信息 (格式: "FILE_SIZE:12345")
    qint64 fileSize = message.mid(10).toLongLong();
    
    // 确保ScnData目录存在
    QDir currentDir;
    if (!currentDir.exists("ScnData")) {
        currentDir.mkdir("ScnData");
    }
    
    // 设置保存路径为当前目录下的ScnData文件夹
	QString role = clientRoles_[socket]; 
	QString scnPath;
	if (role == "A") {
		scnPath = currentDir.absoluteFilePath("ScnData/redScn.scn");
	} else if (role == "B") {
		scnPath = currentDir.absoluteFilePath("ScnData/blueScn.scn");
	}
    
    expectedFileSizes_[socket] = fileSize;
    fileBuffers_[socket] = QByteArray();
    filePaths_[socket] = scnPath;
    
    qDebug() << "Preparing to receive file (" << fileSize << "bytes) to" << scnPath;
    socket->write("READY_TO_RECEIVE_FILE\n");
}

void tcpServer::receiveFileData(QTcpSocket* socket, const QByteArray& data)
{
    fileBuffers_[socket].append(data);
    
    // 检查是否接收完成
    if (fileBuffers_[socket].size() >= expectedFileSizes_[socket]) {
        // 保存文件
        QFile file(filePaths_[socket]);
        if (file.open(QIODevice::WriteOnly)) {
            file.write(fileBuffers_[socket]);
            file.close();
            qDebug() << "File saved to:" << filePaths_[socket];
            
            // 根据客户端类型发送不同的确认消息
            if (clientRoles_.contains(socket)) {
                QString role = clientRoles_[socket];
                
                if (role == "A") {
                    socket->write("FILE_RECEIVED_A\n");
                    qDebug() << "Sent FILE_RECEIVED_A to ClientA";
                } else if (role == "B") {
                    socket->write("FILE_RECEIVED_B\n");
                    qDebug() << "Sent FILE_RECEIVED_B to ClientB";
                }
            }
            
            // 清理状态
            expectedFileSizes_.remove(socket);
            fileBuffers_.remove(socket);
            filePaths_.remove(socket);
        } else {
            qDebug() << "Failed to save file to:" << filePaths_[socket];
            socket->write("FILE_SAVE_ERROR\n");
        }
    }
}
