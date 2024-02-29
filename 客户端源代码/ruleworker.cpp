#include "mainwindow.h"

ProxyRuleWorker::ProxyRuleWorker(const QString& username,const QString& address, const QString& port, const QString& internet, const QString& enable)
    : m_username(username),m_address(address), m_port(port), m_internet(internet), m_enable(enable)
{
}

void ProxyRuleWorker::runWithSocket()
{
    if (m_editMode) {
        // 修改规则
        editProxyRule(m_username,m_address, m_port, m_internet, m_enable);
    } else if (m_deleteMode) {
        // 删除规则
        deleteProxyRule(m_username,m_address, m_port, m_internet, m_enable);
    } else {
        // 添加规则
        addProxyRule(m_username,m_address, m_port, m_internet, m_enable);
    }

    emit finished();
}


void ProxyRuleWorker::addProxyRule(const QString& username,const QString& address, const QString& port, const QString& internet, const QString& enable)
{

    // 创建一个 QTcpSocket 对象
        QTcpSocket clientSocket;
        clientSocket.connectToHost(server_IP, 8888);

        if (!clientSocket.waitForConnected()) {
           qDebug()<<"Error: Failed to connect to server.";
            return;
        }


        QString addProxyRuleMessage = "POST /addProxyRule HTTP/1.1\r\n"
                                      "Username:"+username+"\r\n"
                                      "Host: " + address + "\r\n"
                                      "Content-Length: " + QString::number(address.length() + port.length() + internet.length() + enable.length() + 3) + "\r\n"

                                      "address=" + address + "&port=" + port + "&mode=" + internet + "&enable=" + enable;

        // 发送消息
        clientSocket.write(addProxyRuleMessage.toUtf8());

        if (!clientSocket.waitForBytesWritten()) {
            qDebug()<<"Error: Failed to send addProxyRule message.";
            return;
        }

        QByteArray response;
        while (clientSocket.waitForReadyRead()) {
            response += clientSocket.readAll();
        }

        QString responseString = QString::fromUtf8(response);
        if (responseString.contains("OK")) {
            qDebug() << "addProxyRule success!";
        }
        else {
            qDebug()<<responseString;
        }


    qDebug() << "Added proxy rule: " << address << port << internet << enable;
}

void ProxyRuleWorker::editProxyRule(const QString& username,const QString& address, const QString& port, const QString& internet, const QString& enable)
{
    // 创建一个 QTcpSocket 对象
    QTcpSocket clientSocket;
    clientSocket.connectToHost(server_IP, 8888);

    if (!clientSocket.waitForConnected()) {
        qDebug() << "Error: Failed to connect to server.";
        return;
    }
    QStringList splittedaddress = address.split(" ");
    QString oldaddress = splittedaddress.at(0);
    QString newaddress = splittedaddress.at(1);
    QStringList splittedport = port.split(" ");
    QString oldport = splittedport.at(0);
    QString newport = splittedport.at(1);
    QStringList splittedinternet = internet.split(" ");
    QString oldinternet = splittedinternet.at(0);
    QString newinternet = splittedinternet.at(1);
    QStringList splittedenable = enable.split(" ");
    QString oldenable = splittedenable.at(0);
    QString newenable = splittedenable.at(1);

    QString editProxyRuleMessage = "POST /editProxyRule HTTP/1.1\r\n"
                                    "Username:"+username+"\r\n"
                                   "Host: " + address + "\r\n"
                                   "Content-Length: " + QString::number(address.length() + port.length() + internet.length() + enable.length() - 1) + "\r\n"

                                   "oldaddress=" + oldaddress + "&newaddress=" + newaddress + "&oldport=" + oldport + "&newport=" + newport
                                    +"&oldinternet=" + oldinternet + "&newinternet=" + newinternet + "&oldenable=" + oldenable + "&newenable=" + newenable;

    // 发送消息
    clientSocket.write(editProxyRuleMessage.toUtf8());

    if (!clientSocket.waitForBytesWritten()) {
        qDebug() << "Error: Failed to send editProxyRule message.";
        return;
    }

    QByteArray response;
    while (clientSocket.waitForReadyRead()) {
        response += clientSocket.readAll();
    }

    QString responseString = QString::fromUtf8(response);
    if (responseString.contains("OK")) {
        qDebug() << "editProxyRule success!";
    }
    else {
        qDebug() << responseString;
    }

    qDebug() << "Modified proxy rule: " << address << port << internet << enable;
}

void ProxyRuleWorker::deleteProxyRule(const QString& username,const QString& address, const QString& port, const QString& internet, const QString& enable)
{
    // 创建一个 QTcpSocket 对象
    QTcpSocket clientSocket;
    clientSocket.connectToHost(server_IP, 8888);

    if (!clientSocket.waitForConnected()) {
        qDebug() << "Error: Failed to connect to server.";
        return;
    }

    QString deleteProxyRuleMessage = "POST /deleteProxyRule HTTP/1.1\r\n"
                                     "Username:"+username+"\r\n"
                                     "Host: " + address + "\r\n"
                                     "Content-Length: " + QString::number(address.length() + port.length() + internet.length() + enable.length() + 3) + "\r\n"

                                     "address=" + address + "&port=" + port + "&mode=" + internet + "&enable=" + enable;

    // 发送消息
    clientSocket.write(deleteProxyRuleMessage.toUtf8());

    if (!clientSocket.waitForBytesWritten()) {
        qDebug() << "Error: Failed to send deleteProxyRule message.";
        return;
    }

    QByteArray response;
    while (clientSocket.waitForReadyRead()) {
        response += clientSocket.readAll();
    }

    QString responseString = QString::fromUtf8(response);
    if (responseString.contains("OK")) {
        qDebug() << "deleteProxyRule success!";
    }
    else {
        qDebug() << responseString;
    }

    qDebug() << "Deleted proxy rule: " << address << port << internet << enable;
}


FileRuleWorker::FileRuleWorker(const QString& username, const QString& dire, const QString& name, const QString& type, const QString& size)
    : m_username(username), m_dire(dire), m_name(name), m_type(type), m_size(size)
{
}

void FileRuleWorker::runWithSocket()
{
    if (m_editMode)
    {
        editFileRule(m_username, m_dire, m_name, m_type, m_size);
    }
    else if (m_deleteMode)
    {
        deleteFileRule(m_username, m_dire, m_name, m_type, m_size);
    }
    else
    {
        addFileRule(m_username, m_dire, m_name, m_type, m_size);
    }

    emit finished();
}

void FileRuleWorker::addFileRule(const QString& username, const QString& dire, const QString& name, const QString& type, const QString& size)
{
    // 创建一个 QTcpSocket 对象
        QTcpSocket clientSocket;
        clientSocket.connectToHost(server_IP, 8888);

        if (!clientSocket.waitForConnected()) {
            qDebug() << "Error: Failed to connect to server.";
            return;
        }

        QString addFileRuleMessage = "POST /addFileRule HTTP/1.1\r\n"
                                      "Username:" + username + "\r\n"
                                      "Content-Length: " + QString::number(dire.length() + name.length() + type.length() + size.length() + 3) + "\r\n"

                                      "Filedire="+dire+"&Filename=" + name + "&Filetype=" + type + "&Filelen=" + size;

        // 发送消息
        clientSocket.write(addFileRuleMessage.toUtf8());

        if (!clientSocket.waitForBytesWritten()) {
            qDebug() << "Error: Failed to send addFileRule message.";
            return;
        }

        QByteArray response;
        while (clientSocket.waitForReadyRead()) {
            response += clientSocket.readAll();
        }

        QString responseString = QString::fromUtf8(response);
        if (responseString.contains("OK")) {
            qDebug() << "addFileRule success!";
        }
        else {
            qDebug() << responseString;
        }

        qDebug() << "Added file rule: " << name << type << size;
}

void FileRuleWorker::editFileRule(const QString& username, const QString& dire, const QString& name, const QString& type, const QString& size)
{
    // 创建一个 QTcpSocket 对象
    QTcpSocket clientSocket;
    clientSocket.connectToHost(server_IP, 8888);

    if (!clientSocket.waitForConnected()) {
        qDebug() << "Error: Failed to connect to server.";
        return;
    }

    QStringList splitteddire = dire.split(" ");
    QString olddire = splitteddire.at(0);
    QString newdire = splitteddire.at(1);
    QStringList splittedname = name.split(" ");
    QString oldname = splittedname.at(0);
    QString newname = splittedname.at(1);
    QStringList splittedtype = type.split(" ");
    QString oldtype = splittedtype.at(0);
    QString newtype = splittedtype.at(1);
    QStringList splittedsize = size.split(" ");
    QString oldsize = splittedsize.at(0);
    QString newsize = splittedsize.at(1);

    QString editFileRuleMessage = "POST /editFileRule HTTP/1.1\r\n"

                                  "Content-Length: " + QString::number(dire.length() + name.length() + type.length() + size.length() - 1) + "\r\n"

                                  "Username:"+username+"\r\n"
                                  "old_file_dire=" + olddire + "&new_file_dire=" + newdire + "&old_file_name=" + oldname + "&new_file_name=" + newname
                                  + "&old_file_type=" + oldtype + "&new_file_type=" + newtype + "&old_file_len=" + oldsize + "&new_file_len=" + newsize;

    // 发送消息
    clientSocket.write(editFileRuleMessage.toUtf8());

    if (!clientSocket.waitForBytesWritten()) {
        qDebug() << "Error: Failed to send editFileRule message.";
        return;
    }

    QByteArray response;
    while (clientSocket.waitForReadyRead()) {
        response += clientSocket.readAll();
    }

    QString responseString = QString::fromUtf8(response);
    if (responseString.contains("OK")) {
        qDebug() << "editFileRule success!";
    }
    else {
        qDebug() << responseString;
    }

    qDebug() << "Modified file rule: " << dire << name << type << size;
}


void FileRuleWorker::deleteFileRule(const QString& username, const QString& dire, const QString& name, const QString& type, const QString& size)
{
    // 创建一个 QTcpSocket 对象
    QTcpSocket clientSocket;
    clientSocket.connectToHost(server_IP, 8888);

    if (!clientSocket.waitForConnected()) {
        qDebug() << "Error: Failed to connect to server.";
        return;
    }

    QString deleteFileRuleMessage = "POST /deleteFileRule HTTP/1.1\r\n"

                                    "Content-Length: " + QString::number(dire.length() + name.length() + type.length() + size.length() + 3) + "\r\n"

                                    "Username:"+username+"\r\n"
                                    "Filedire=" + dire + "&Filename=" + name + "&Filetype=" + type + "&Filelen=" + size;

    // 发送消息
    clientSocket.write(deleteFileRuleMessage.toUtf8());

    if (!clientSocket.waitForBytesWritten()) {
        qDebug() << "Error: Failed to send deleteFileRule message.";
        return;
    }

    QByteArray response;
    while (clientSocket.waitForReadyRead()) {
        response += clientSocket.readAll();
    }

    QString responseString = QString::fromUtf8(response);
    if (responseString.contains("OK")) {
        qDebug() << "deleteFileRule success!";
    }
    else {
        qDebug() << responseString;
    }

    qDebug() << "Deleted file rule: " << dire << name << type << size;
}


