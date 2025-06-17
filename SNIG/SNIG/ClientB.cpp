#include "StdAfx.h"
#include "ClientB.h"
#include <QDebug>

ClientB::ClientB(QObject *parent) 
    : QObject(parent)
{
    identify = false;
	socket = new QTcpSocket(this);

    connect(socket, SIGNAL(connected()), this, SLOT(onConnected()));
    connect(socket, SIGNAL(readyRead()), this, SLOT(onReadyRead()));
    connect(socket, SIGNAL(disconnected()), this, SLOT(onDisconnected()));
    connect(socket, SIGNAL(error(QAbstractSocket::SocketError)),
            this, SLOT(onError(QAbstractSocket::SocketError)));
}

ClientB::~ClientB()
{
	if(socket){
		socket->disconnectFromHost();
		if (socket->state() != QAbstractSocket::UnconnectedState) {
            socket->waitForDisconnected(1000);
        }
        socket->deleteLater(); 
        socket = nullptr;	
	}
}

void ClientB::connectToServer(const QString &host, quint16 port)
{
    qDebug() << "蓝方正在连接服务器..." << host << ":" << port;
    socket->connectToHost(host, port);
}

void ClientB::sendReady()
{
    if (socket->state() == QTcpSocket::ConnectedState && identify) {
		// 发送想定文件
		if(m_scenarioPath.isEmpty()) {
			qDebug() << "未找到想定文件！";
			return;
		}
		// 先发送文件头
        sendFileHeader();
    }
}

void ClientB::sendFileHeader()
{
    QFile file(m_scenarioPath);
    if (!file.open(QIODevice::ReadOnly)) {
        qDebug() << "Cannot open file:" << m_scenarioPath;
        return;
    }

    fileData_ = file.readAll();
    file.close();

    // 发送文件大小信息
    QByteArray sizeInfo = QString("FILE_SIZE:%1\n").arg(fileData_.size()).toUtf8();
    socket->write(sizeInfo);
	waitingForReady_ = true;  // 标记为等待服务器确认状态
}

void ClientB::onConnected()
{
    qDebug() << "已连接到服务器，正在标识为CLIENT_B";
    socket->write("CLIENT_B\n");  // 注意: 保持与服务器一致
    emit connected();
}

void ClientB::onReadyRead()
{
    QByteArray data = socket->readAll().trimmed();
    QString message = QString::fromUtf8(data).trimmed();
    qDebug() << "从服务器收到消息:" << message;

    if (message == "IDENTIFIED_B" && !identify) {
        identify = true;
        qDebug() << "身份标识完成";
        emit identified();
        
        sendReady();
    }else if (waitingForReady_ && message == "READY_TO_RECEIVE_FILE") {
        qDebug() << "Server is ready, sending file content...";
        waitingForReady_ = false;
        sendFileContent();  // 收到确认后发送文件内容
    }
    else if (message == "FILE_RECEIVED_B") {
        qDebug() << "Server received file, sending READY...";
        socket->write("READY\n");
        emit readySent();
    }
}

void ClientB::sendFileContent()
{
    // 发送文件内容
    socket->write(fileData_);
    qDebug() << "File content sent:" << m_scenarioPath;
}

//上传想定文件至服务器
void ClientB::uploadScn()
{
	
}
void ClientB::onDisconnected()
{
    qDebug() << "与服务器断开连接";
    identify = false;
    emit disconnected();
    
    reconnectTimer->start();
}

void ClientB::onError(QAbstractSocket::SocketError error)
{
    QString errorStr = socket->errorString();
    qDebug() << "套接字错误:" << errorStr;
    emit errorOccurred(errorStr);
    
    reconnectTimer->start();
}