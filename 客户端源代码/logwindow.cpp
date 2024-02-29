#include "mainwindow.h"
#include <QPixmap>
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QString>
#include <QFile>
#include <QDebug>
#include <QDir>
#include <QProcess>
#include<QDateTime>
#include<QTextEdit>
#include <QtGlobal>


void MainWindow::createLogPage()
{
    // 创建新的日志页面
    QDialog *logDialog = new QDialog(this);
    logDialog->setWindowTitle("日志页面");
    logDialog->resize(600, 400);

    // 创建日志文本框
    QTextEdit *logTextEdit = new QTextEdit(logDialog);
    logTextEdit->setReadOnly(true);

    // 从日志文件中读取日志信息
    // 创建一个 QTcpSocket 对象
    QTcpSocket clientSocket;
    clientSocket.connectToHost(server_IP, 8888);

    if (!clientSocket.waitForConnected()) {
        qDebug() << "Error: Failed to connect to server.";
        return;
    }

    QString loadLogMessage = "POST /load log HTTP/1.1\r\n"
                                     "Username:"+username+"\r\n"
                                     ;

    // 发送消息
    clientSocket.write(loadLogMessage.toUtf8());

    if (!clientSocket.waitForBytesWritten()) {
        qDebug() << "Error: Failed to send deleteProxyRule message.";
        return;
    }

    
    QByteArray response;
    if (!clientSocket.waitForReadyRead()) {
        qDebug() << "Error: Failed to receive response from server.";
        clientSocket.close();
        return ;
    }
    response = clientSocket.readAll();
    clientSocket.close();

    QString responseString = QString::fromUtf8(response);
    logTextEdit->setPlainText(responseString);
    // 创建清空按钮
    QPushButton *clearLogButton = new QPushButton("清空日志", logDialog);

    // 创建布局管理器
    QVBoxLayout *layout = new QVBoxLayout(logDialog);
    layout->addWidget(logTextEdit);
    layout->addWidget(clearLogButton);

    // 连接槽函数
    connect(clearLogButton, &QPushButton::clicked, [=]() {
            // 清空日志文本框
            logTextEdit->clear();

            // 清空日志文件
            // 创建一个 QTcpSocket 对象
            QTcpSocket clientSocket;
            clientSocket.connectToHost(server_IP, 8888);
        
            if (!clientSocket.waitForConnected()) {
                qDebug() << "Error: Failed to connect to server.";
                return;
            }
        
            QString deleteLogMessage = "POST /clear log HTTP/1.1\r\n"
                                             "Username:"+username+"\r\n"
                                             ;
        
            // 发送消息
            clientSocket.write(deleteLogMessage.toUtf8());
        
            if (!clientSocket.waitForBytesWritten()) {
                qDebug() << "Error: Failed to send deleteProxyRule message.";
                return;
            }
        });

    // 显示日志页面
    logDialog->exec();
}




// 日志记录
//   QString message = QString("[%1] %2 %3 %4").arg(QDateTime::currentDateTime().toString(),
//                                                  clientSocket->peerAddress().toString(),
//                                                  request.method(),
//                                                  request.url().toString());
//   qDebug() << message;


//void MyHttpProxy::handleHttpRequest(QTcpSocket *clientSocket, QByteArray requestData)
//{
//    // 解析 HTTP 请求
//    QHttpRequestHeader requestHeader(QString(requestData));

//    // 记录请求信息
//    QString logMessage = QString("Received HTTP request from %1:%2:\n")
//            .arg(clientSocket->peerAddress().toString())
//            .arg(clientSocket->peerPort());
//    logMessage += QString("%1 %2\n").arg(requestHeader.methodString()).arg(requestHeader.path());
//    logMessage += QString("Headers: %1\n").arg(requestHeader.toString());

//    // 发送 HTTP 请求到目标服务器
//    QTcpSocket *serverSocket = new QTcpSocket(this);
//    serverSocket->connectToHost(requestHeader.host(), requestHeader.port(80));
//    serverSocket->waitForConnected();
//    serverSocket->write(requestData);
//    serverSocket->flush();

//    // 等待 HTTP 响应
//    serverSocket->waitForReadyRead();
//    QByteArray responseData = serverSocket->readAll();

//    // 解析 HTTP 响应
//    QHttpResponseHeader responseHeader(QString(responseData));

//    // 记录响应信息
//    logMessage += QString("Received HTTP response from %1:%2:\n")
//            .arg(requestHeader.host())
//            .arg(requestHeader.port(80));
//    logMessage += QString("%1 %2\n").arg(responseHeader.statusCode()).arg(responseHeader.reasonPhrase());
//    logMessage += QString("Headers: %1\n").arg(responseHeader.toString());

//    // 发送 HTTP 响应到客户端
//    clientSocket->write(responseData);
//    clientSocket->flush();

//    // 记录日志信息到文件
//    QFile logFile("proxy.log");
//    if (logFile.open(QIODevice::WriteOnly | QIODevice::Append)) {
//        QTextStream out(&logFile);
//        out << logMessage << endl;
//        logFile.close();
//    }

//    // 记录日志信息到日志文本框
//    logMessage += "------------------------------------------------------------\n";
//    logTextEdit->append(logMessage);
//}
