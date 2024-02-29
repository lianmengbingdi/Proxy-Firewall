#include "mainwindow.h"
#include <QPixmap>
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QString>
#include <QFile>
#include <QDebug>
#include <QDir>
#include <QProcess>
#include<QCheckBox>

void removeSubstring(QString& str, const QString& substr) {
    str.replace(substr, "");
}
void MainWindow::loadproxyrules() {

    QTcpSocket clientSocket;
    clientSocket.connectToHost(server_IP, 8888);

    if (!clientSocket.waitForConnected()) {
        qDebug() << "Error: Failed to connect to server.";
        return ;
    }

    // 构建发送消息
    QString message = "GET /load_Proxy_rules HTTP/1.1\r\n"
                      "Username:"+username+"\r\n"
                      "Host: 120.27.142.144\r\n"
                      "\r\n";

    // 发送消息
    clientSocket.write(message.toUtf8());
    if (!clientSocket.waitForBytesWritten()) {
        qDebug() << "Error: Failed to send message.";
        clientSocket.close();
        return ;
    }

    // 接收消息
    QByteArray response;
    if (!clientSocket.waitForReadyRead()) {
        qDebug() << "Error: Failed to receive response from server.";
        clientSocket.close();
        return ;
    }
    response = clientSocket.readAll();
    clientSocket.close();

    QString responseString = QString::fromUtf8(response);

    // 解析服务器返回的代理规则
    QString address, port, mode, enable;
    QStringList lines = responseString.split("\n");
    foreach (QString line, lines) {
        if (line.isEmpty()) {
            continue; // 跳过空行
        }
        QStringList fields = line.split(", ");
        if (fields.size() < 4) {
            continue; // 跳过字段数不足的行
        }
        QString condition = fields.value(0);
        QString address = fields.value(1);
        QString internet = fields.value(2);
        QString enable = fields.value(3);
        removeSubstring(condition, "Address: ");
        removeSubstring(address, "Port: ");
        removeSubstring(internet, "Mode: ");
        removeSubstring(enable, "Enable: ");
        QList<QStandardItem *> rowItems;
        rowItems.append(new QStandardItem(condition));
        rowItems.append(new QStandardItem(address));
        rowItems.append(new QStandardItem(internet));
        rowItems.append(new QStandardItem(enable));
        proxyRuleModel->appendRow(rowItems);
    }


}
void MainWindow::loadfilerules(){
    QTcpSocket clientSocket;
    clientSocket.connectToHost(server_IP, 8888);

    if (!clientSocket.waitForConnected()) {
        qDebug() << "Error: Failed to connect to server.";
        return ;
    }

    // 构建发送消息
    QString message = "GET /load_File_rules HTTP/1.1\r\n"
                      "Username:"+username+"\r\n"
                      "Host: 120.27.142.144\r\n"
                      "\r\n";

    // 发送消息
    clientSocket.write(message.toUtf8());
    if (!clientSocket.waitForBytesWritten()) {
        qDebug() << "Error: Failed to send message.";
        clientSocket.close();
        return ;
    }

    // 接收消息
    QByteArray response;
    if (!clientSocket.waitForReadyRead()) {
        qDebug() << "Error: Failed to receive response from server.";
        clientSocket.close();
        return ;
    }
    response = clientSocket.readAll();
    clientSocket.close();

    QString responseString = QString::fromUtf8(response);

    // 解析服务器返回的代理规则
    QString address, port, mode, enable;
    QStringList lines = responseString.split("\n");
    foreach (QString line, lines) {
        if (line.isEmpty()) {
            continue; // 跳过空行
        }
        QStringList fields = line.split(", ");
        if (fields.size() < 4) {
            continue; // 跳过字段数不足的行
        }
        QString condition = fields.value(0);
        QString address = fields.value(1);
        QString internet = fields.value(2);
        QString enable = fields.value(3);
        removeSubstring(condition, "Filedire: ");
        removeSubstring(address, "Filename: ");
        removeSubstring(internet, "Filetype: ");
        removeSubstring(enable, "Filelen: ");
        QList<QStandardItem *> rowItems;
        rowItems.append(new QStandardItem(condition));
        rowItems.append(new QStandardItem(address));
        rowItems.append(new QStandardItem(internet));
        rowItems.append(new QStandardItem(enable));
        FileRuleModel->appendRow(rowItems);
    }

}
void MainWindow::createConfigPage()
{
    // 创建新的配置页面
    QDialog *configDialog = new QDialog(this);
    configDialog->setWindowTitle("配置页面");
    configDialog->resize(500, 400);

    // 创建代理规则页面
    QWidget *proxyRuleWidget = new QWidget(configDialog);
    QVBoxLayout *proxyRuleLayout = new QVBoxLayout(proxyRuleWidget);

    // 创建代理规则表格
    proxyRuleTableView = new QTableView(proxyRuleWidget);
    proxyRuleModel = new QStandardItemModel(0, 4, proxyRuleTableView);
    proxyRuleModel->setHeaderData(0, Qt::Horizontal, "IP地址");
    proxyRuleModel->setHeaderData(1, Qt::Horizontal, "端口");
    proxyRuleModel->setHeaderData(2, Qt::Horizontal, "网络协议");
    proxyRuleModel->setHeaderData(3, Qt::Horizontal, "是否启用");
    proxyRuleTableView->setModel(proxyRuleModel);
    proxyRuleTableView->setEditTriggers(QAbstractItemView::DoubleClicked |
                                        QAbstractItemView::EditKeyPressed |
                                        QAbstractItemView::SelectedClicked);
    // 获取当前用户目录
    loadproxyrules();
    // 创建代理规则操作按钮
    QHBoxLayout *proxyRuleButtonLayout = new QHBoxLayout();
    addProxyRuleButton = new QPushButton("添加规则",proxyRuleWidget);
    editProxyRuleButton = new QPushButton("编辑规则",proxyRuleWidget);
    deleteProxyRuleButton = new QPushButton("删除规则",proxyRuleWidget);
    proxyRuleButtonLayout->addWidget(addProxyRuleButton);
    proxyRuleButtonLayout->addWidget(editProxyRuleButton);
    proxyRuleButtonLayout->addWidget(deleteProxyRuleButton);
    proxyRuleLayout->addWidget(proxyRuleTableView);
    proxyRuleLayout->addLayout(proxyRuleButtonLayout);

    // 创建过滤规则页面
    QWidget *FileRuleWidget = new QWidget(configDialog);
    QVBoxLayout *FileRuleLayout = new QVBoxLayout(FileRuleWidget);

    // 创建过滤规则表格
    FileRuleTableView = new QTableView(FileRuleWidget);
    FileRuleModel = new QStandardItemModel(0, 4, FileRuleTableView);
    FileRuleModel->setHeaderData(0, Qt::Horizontal, "传输方向");
    FileRuleModel->setHeaderData(1, Qt::Horizontal, "文件名");
    FileRuleModel->setHeaderData(2, Qt::Horizontal, "文件类型");
    FileRuleModel->setHeaderData(3, Qt::Horizontal, "文件大小");
    FileRuleTableView->setModel(FileRuleModel);
    FileRuleTableView->setEditTriggers(QAbstractItemView::DoubleClicked |
                                         QAbstractItemView::EditKeyPressed |
                                         QAbstractItemView::SelectedClicked);
    // 获取当前用户目录
    loadfilerules();
    // 创建过滤规则操作按钮
    QHBoxLayout *FileRuleButtonLayout = new QHBoxLayout();
    addFileRuleButton = new QPushButton("添加规则",FileRuleWidget);
    editFileRuleButton = new QPushButton("编辑规则",FileRuleWidget);
    deleteFileRuleButton = new QPushButton("删除规则",FileRuleWidget);
    FileRuleButtonLayout->addWidget(addFileRuleButton);
    FileRuleButtonLayout->addWidget(editFileRuleButton);
    FileRuleButtonLayout->addWidget(deleteFileRuleButton);
    FileRuleLayout->addWidget(FileRuleTableView);
    FileRuleLayout->addLayout(FileRuleButtonLayout);

    // 添加选项卡
    configTabWidget = new QTabWidget(configDialog);
    configTabWidget->addTab(proxyRuleWidget, "代理规则");
    configTabWidget->addTab(FileRuleWidget, "文件控制规则");

    // 显示配置对话框
    QVBoxLayout *configLayout = new QVBoxLayout(configDialog);
    configLayout->addWidget(configTabWidget);
    setupConfigConnections();
    configDialog->exec();



}
void MainWindow::setupConfigConnections()
{
    connect(addProxyRuleButton, &QPushButton::clicked, this, &MainWindow::addProxyRule);
    connect(editProxyRuleButton, &QPushButton::clicked, this, &MainWindow::editProxyRule);
    connect(deleteProxyRuleButton, &QPushButton::clicked, this, &MainWindow::deleteProxyRule);
    connect(addFileRuleButton, &QPushButton::clicked, this, &MainWindow::addFileRule);
    connect(editFileRuleButton, &QPushButton::clicked, this, &MainWindow::editFileRule);
    connect(deleteFileRuleButton, &QPushButton::clicked, this, &MainWindow::deleteFileRule);

}


//void MainWindow::addProxyRule()
//{

//    // 创建一个新的代理规则行
//    QStandardItem *addressItem = new QStandardItem("waiting");
//    QStandardItem *portItem = new QStandardItem("waiting");
//    QStandardItem *internetItem = new QStandardItem("waiting");
//    QStandardItem *enableItem = new QStandardItem("true");
//    proxyRuleModel->appendRow({addressItem, portItem, internetItem,enableItem});

//    // 打开编辑对话框
//    ProxyRuleDialog dialog(this, "添加代理规则", "", "", "","");


//    if (dialog.exec() == QDialog::Accepted) {
//        // 获取用户输入
//        QString address = dialog.getAddress();
//        QString port = dialog.getPort();
//        QString internet = dialog.getInternet();
//        bool enable = dialog.getEnabled() ;
//        QString enablestr=(enable? "true" : "false");
//        // 更新代理规则模型
//        addressItem->setData(address, Qt::DisplayRole);
//        portItem->setData(port, Qt::DisplayRole);
//        internetItem->setData(internet, Qt::DisplayRole);
//        enableItem->setData(enable, Qt::DisplayRole);

//        // 保存代理规则到文件
//        // 构建HTTP消息
//        QTcpSocket clientSocket;
//        clientSocket.connectToHost(server_IP, 8888);

//        if (!clientSocket.waitForConnected()) {
//            qDebug() << "Error: Failed to connect to server.";
//            return ;
//        }
//               QString addProxyRuleMessage = "POST /addProxyRule HTTP/1.1\r\n"
//                                      "Host: " + address + "\r\n"
//                                      "Content-Length: " + QString::number(address.length() + port.length() +internet.length()+enablestr.length()+3) + "\r\n"
//                                      "\r\n"
//                                      "address=" + address + "&port=" + port+"&mode="+internet+"&enable="+enablestr;

//               // 发送消息
//               clientSocket.write(addProxyRuleMessage.toUtf8());

//               if (!clientSocket.waitForBytesWritten()) {
//                   qDebug() << "Error: Failed to send addProxyRule message.";

//                   return;
//               }

//               QByteArray response;
//               while (clientSocket.waitForReadyRead()) {
//                   response += clientSocket.readAll();
//               }

//               QString responseString = response;
//               if (responseString.contains("OK")) {
//                   qDebug() << "addProxyRule success!";
//               }
//               else {
//                   QMessageBox::warning(this, "addProxyRule", "Failed to addProxyRule");
//                   qDebug() << responseString;
//               }




//    }
//}
void MainWindow::addProxyRule()
{
    // 创建一个新的代理规则行
    QStandardItem* addressItem = new QStandardItem("");
    QStandardItem* portItem = new QStandardItem("");
    QStandardItem* internetItem = new QStandardItem("");
    QStandardItem* enableItem = new QStandardItem("true");
    proxyRuleModel->appendRow({addressItem, portItem, internetItem, enableItem});

    // 打开编辑对话框
    ProxyRuleDialog dialog(this, "添加代理规则", "", "", "", "");
    if (dialog.exec() == QDialog::Accepted) {
        // 获取用户输入
        QString address = dialog.getAddress();
        QString port = dialog.getPort();
        QString internet = dialog.getInternet();
        bool enable = dialog.getEnabled();
        QString enablestr = (enable ? "true" : "false");
        // 更新代理规则模型
        addressItem->setData(address, Qt::DisplayRole);
        portItem->setData(port, Qt::DisplayRole);
        internetItem->setData(internet, Qt::DisplayRole);
        enableItem->setData(enable, Qt::DisplayRole);
        // 创建新线程并移动Socket操作到新线程中
        QThread* thread = new QThread(this);
        ProxyRuleWorker* worker = new ProxyRuleWorker(username,address, port, internet, enablestr);
        worker->moveToThread(thread);
        connect(thread, &QThread::started, worker, &ProxyRuleWorker::runWithSocket);
        connect(worker, &ProxyRuleWorker::finished, thread, &QThread::quit);
        connect(worker, &ProxyRuleWorker::finished, worker, &ProxyRuleWorker::deleteLater);
        connect(thread, &QThread::finished, thread, &QThread::deleteLater);
        thread->start();
    }
}


void MainWindow::editProxyRule() {
    // 获取选中的行
    QModelIndexList indexes = proxyRuleTableView->selectionModel()->selectedRows();
    if (indexes.empty()) {
        QMessageBox::warning(this, "警告", "请先选择一行代理规则");
        return;
    }
    QModelIndex index = indexes.front();
    int rowIndex = index.row();  // 获取行号

    // 获取原始代理规则
    QString oldAddress = proxyRuleModel->data(proxyRuleModel->index(rowIndex, 0)).toString();
    QString oldPort = proxyRuleModel->data(proxyRuleModel->index(rowIndex, 1)).toString();
    QString oldInternet = proxyRuleModel->data(proxyRuleModel->index(rowIndex, 2)).toString();
    bool oldEnabled = (proxyRuleModel->data(proxyRuleModel->index(rowIndex, 3)).toString() == "true");
    QString oldEnablestr = (oldEnabled ? "true" : "false");
    // 打开编辑对话框
    ProxyRuleDialog dialog(this, "编辑代理规则", oldAddress, oldPort, oldInternet, oldEnabled);
    if (dialog.exec() == QDialog::Accepted) {
        // 获取新的代理规则
        QString newAddress = dialog.getAddress();
        QString newPort = dialog.getPort();
        QString newInternet = dialog.getInternet();
        bool newEnabled = dialog.getEnabled();
        QString newEnablestr = (newEnabled ? "true" : "false");
        // 更新代理规则
        if (newAddress != oldAddress) {
            proxyRuleModel->setData(proxyRuleModel->index(rowIndex, 0), newAddress);
        }
        if (newPort != oldPort) {
            proxyRuleModel->setData(proxyRuleModel->index(rowIndex, 1), newPort);
        }
        if (newInternet != oldInternet) {
            proxyRuleModel->setData(proxyRuleModel->index(rowIndex, 2), newInternet);
        }
        if (newEnabled != oldEnabled) {
            proxyRuleModel->setData(proxyRuleModel->index(rowIndex, 3), newEnabled ? "true" : "false");
        }


        // 创建新线程并移动Socket操作到新线程中
        QThread* thread = new QThread(this);
        newAddress=oldAddress+" "+newAddress;
        newPort=oldPort+" "+newPort;
        newInternet=oldInternet+" "+newInternet;
        newEnablestr=oldEnablestr+" "+newEnablestr;

        ProxyRuleWorker* worker = new ProxyRuleWorker(username,newAddress, newPort, newInternet, newEnablestr );
        // 设置编辑模式
        worker->m_editMode = true;
        worker->moveToThread(thread);
        connect(thread, &QThread::started, worker, &ProxyRuleWorker::runWithSocket);
        connect(worker, &ProxyRuleWorker::finished, thread, &QThread::quit);
        connect(worker, &ProxyRuleWorker::finished, worker, &ProxyRuleWorker::deleteLater);
        connect(thread, &QThread::finished, thread, &QThread::deleteLater);
        thread->start();

    }


}


void MainWindow::deleteProxyRule()
{
    // 获取选中的行
    QModelIndexList indexes = proxyRuleTableView->selectionModel()->selectedRows();
    if (indexes.empty()) {
        QMessageBox::warning(this, "警告", "请先选择一行代理规则");
        return;
    }
    QModelIndex index = indexes.front();
    QString address = proxyRuleModel->data(proxyRuleModel->index(index.row(), 0)).toString();
    QString port = proxyRuleModel->data(proxyRuleModel->index(index.row(), 1)).toString();
    QString internet = proxyRuleModel->data(proxyRuleModel->index(index.row(), 2)).toString();
    QString enable = proxyRuleModel->data(proxyRuleModel->index(index.row(), 3)).toString();

    // 从模型中删除代理规则
    proxyRuleModel->removeRow(index.row());
    // 创建新线程并移动Socket操作到新线程中
    QThread* thread = new QThread(this);
    ProxyRuleWorker* worker = new ProxyRuleWorker(username,address, port, internet, enable);
    worker->m_deleteMode = true;
    worker->moveToThread(thread);
    connect(thread, &QThread::started, worker, &ProxyRuleWorker::runWithSocket);
    connect(worker, &ProxyRuleWorker::finished, thread, &QThread::quit);
    connect(worker, &ProxyRuleWorker::finished, worker, &ProxyRuleWorker::deleteLater);
    connect(thread, &QThread::finished, thread, &QThread::deleteLater);
    thread->start();
}

ProxyRuleDialog::ProxyRuleDialog(QWidget *parent, const QString &title, const QString &address, const QString &port, const QString &internet,bool enabled) : QDialog(parent)
{
    // 创建控件
    addressLabel = new QLabel("IP：", this);
    addressLineEdit = new QLineEdit(address, this);
    portLabel = new QLabel("端口：", this);
    portLineEdit = new QLineEdit(port, this);
    intenetLabel = new QLabel("网络协议：", this);
    internetComboBox = new QComboBox(this);
    internetComboBox->addItems({"http", "https", "ftp"});
    internetComboBox->setCurrentText(internet);
    enabledLabel = new QLabel("启用：", this);
    enabledCheckBox = new QCheckBox(this);
    enabledCheckBox->setChecked(enabled);
    okButton = new QPushButton("确定", this);
    cancelButton = new QPushButton("取消", this);

    // 布局控件
    QGridLayout *layout = new QGridLayout(this);
    layout->addWidget(addressLabel, 0, 0);
    layout->addWidget(addressLineEdit, 0, 1);
    layout->addWidget(portLabel, 1, 0);
    layout->addWidget(portLineEdit, 1, 1);
    layout->addWidget(intenetLabel, 2, 0);
    layout->addWidget(internetComboBox, 2, 1);
    layout->addWidget(enabledLabel, 3, 0);
    layout->addWidget(enabledCheckBox, 3, 1);
    layout->addWidget(okButton, 4, 0);
    layout->addWidget(cancelButton, 4, 1);
    layout->setColumnStretch(1, 1);
    setLayout(layout);

    // 设置窗口标题和大小
    setWindowTitle(title);
    setFixedWidth(300);

    // 连接信号和槽
    connect(okButton, &QPushButton::clicked, this, &QDialog::accept);
    connect(cancelButton, &QPushButton::clicked, this, &QDialog::reject);
}

QString ProxyRuleDialog::getAddress() const
{
    return addressLineEdit->text();
}

QString ProxyRuleDialog::getPort() const
{
    return portLineEdit->text();
}
QString ProxyRuleDialog::getInternet() const
{
    return internetComboBox->currentText();
}
bool ProxyRuleDialog::getEnabled() const
{
    return enabledCheckBox->isChecked();
}


void MainWindow::addFileRule()
{
    // 创建一个新的过滤规则行
    QStandardItem *direItem = new QStandardItem("");
    QStandardItem *nameItem = new QStandardItem("");
    QStandardItem *typeItem = new QStandardItem("");
    QStandardItem *sizeItem = new QStandardItem("-1");
    FileRuleModel->appendRow({direItem, nameItem, typeItem,sizeItem});

    // 打开编辑对话框
    FileRuleDialog dialog(this, "添加文件控制规则", "", "", "","");
    if (dialog.exec() == QDialog::Accepted) {
        // 获取用户输入
        QString dire = dialog.getDire();
        QString name = dialog.getName();
        QString type = dialog.getType();
        QString size = dialog.getSize();

        // 更新过滤规则模型
        direItem->setData(dire, Qt::DisplayRole);
        nameItem->setData(name, Qt::DisplayRole);
        typeItem->setData(type, Qt::DisplayRole);
        sizeItem->setData(size, Qt::DisplayRole);

        // 创建新线程并移动Socket操作到新线程中
        QThread* thread = new QThread(this);
        FileRuleWorker* worker = new FileRuleWorker(username,dire, name, type, size);
        worker->moveToThread(thread);
        connect(thread, &QThread::started, worker, &FileRuleWorker::runWithSocket);
        connect(worker, &FileRuleWorker::finished, thread, &QThread::quit);
        connect(worker, &FileRuleWorker::finished, worker, &FileRuleWorker::deleteLater);
        connect(thread, &QThread::finished, thread, &QThread::deleteLater);
        thread->start();
    }
}

void MainWindow::editFileRule()
{
    // 获取选中的行
    QModelIndexList indexes = FileRuleTableView->selectionModel()->selectedRows();
    if (indexes.empty()) {
        QMessageBox::warning(this, "警告", "请先选择一行文件控制规则");
        return;
    }
    QModelIndex index = indexes.front();

    // 获取原始文件控制规则
    QString oldDire = FileRuleModel->data(FileRuleModel->index(index.row(), 0)).toString();
    QString oldName = FileRuleModel->data(FileRuleModel->index(index.row(), 1)).toString();
    QString oldType = FileRuleModel->data(FileRuleModel->index(index.row(), 2)).toString();
    QString oldSize = FileRuleModel->data(FileRuleModel->index(index.row(), 3)).toString();

    // 打开编辑对话框
    FileRuleDialog dialog(this, "编辑文件控制规则", oldDire, oldName, oldType, oldSize);
    if (dialog.exec() == QDialog::Accepted) {
        // 获取新的文件控制规则
        QString newDire = dialog.getDire();
        QString newName = dialog.getName();
        QString newType = dialog.getType();
        QString newSize = dialog.getSize();

        // 更新文件控制规则
        if (newDire != oldDire) {
            FileRuleModel->setData(FileRuleModel->index(index.row(), 0), newDire);
        }
        if (newName != oldName) {
            FileRuleModel->setData(FileRuleModel->index(index.row(), 1), newName);
        }
        if (newType != oldType) {
            FileRuleModel->setData(FileRuleModel->index(index.row(), 2), newType);
        }
        if (newSize != oldSize) {
            FileRuleModel->setData(FileRuleModel->index(index.row(), 3), newSize);
        }

        // 创建新线程并移动Socket操作到新线程中
        QThread* thread = new QThread(this);
        newDire=oldDire+" "+newDire;
        newName=oldName+" "+newName;
        newSize=oldSize+" "+newSize;
        newType=oldType+" "+newType;
        FileRuleWorker* worker = new FileRuleWorker(username, newDire, newName, newType, newSize);
        worker->m_editMode = true;
        worker->moveToThread(thread);
        connect(thread, &QThread::started, worker, &FileRuleWorker::runWithSocket);
        connect(worker, &FileRuleWorker::finished, thread, &QThread::quit);
        connect(worker, &FileRuleWorker::finished, worker, &FileRuleWorker::deleteLater);
        connect(thread, &QThread::finished, thread, &QThread::deleteLater);
        thread->start();
    }
}

 void MainWindow::deleteFileRule()
{   // 获取选中的行
     QModelIndexList indexes = FileRuleTableView->selectionModel()->selectedRows();
     if (indexes.empty()) {
         QMessageBox::warning(this, "警告", "请先选择一行文件控制规则");
         return;
     }
     QModelIndex index = indexes.front();
     QString dire = FileRuleModel->data(FileRuleModel->index(index.row(), 0)).toString();
     QString name = FileRuleModel->data(FileRuleModel->index(index.row(), 1)).toString();
     QString type = FileRuleModel->data(FileRuleModel->index(index.row(), 2)).toString();
     QString size = FileRuleModel->data(FileRuleModel->index(index.row(), 3)).toString();
     // 从模型中删除代理规则
     FileRuleModel->removeRow(index.row());
     // 创建新线程并移动Socket操作到新线程中
     QThread* thread = new QThread(this);
     FileRuleWorker* worker = new FileRuleWorker(username,dire, name, type, size);
     worker->m_deleteMode = true;
     worker->moveToThread(thread);
     connect(thread, &QThread::started, worker, &FileRuleWorker::runWithSocket);
     connect(worker, &FileRuleWorker::finished, thread, &QThread::quit);
     connect(worker, &FileRuleWorker::finished, worker, &FileRuleWorker::deleteLater);
     connect(thread, &QThread::finished, thread, &QThread::deleteLater);
     thread->start();




}
 FileRuleDialog::FileRuleDialog(QWidget *parent, const QString &title, const QString &dire, const QString &name, const QString &type,const QString &size) : QDialog(parent)
 {
     // 创建控件
     direLabel = new QLabel("传输方向：", this);
     direComboBox = new QComboBox(this);
     direComboBox->addItems({"上传", "下载"});
     direComboBox->setCurrentText(dire);

     nameLabel = new QLabel("文件名：", this);
     nameLineEdit = new QLineEdit(name, this);

     typeLabel = new QLabel("文件类型：", this);
     typeLineEdit = new QLineEdit(type, this);

     sizeLabel = new QLabel("文件大小：", this);
     sizeLineEdit = new QLineEdit(size, this);

     okButton = new QPushButton("确定", this);
     cancelButton = new QPushButton("取消", this);

     // 布局控件
     QGridLayout *layout = new QGridLayout(this);
     layout->addWidget(direLabel, 0, 0);
     layout->addWidget(direComboBox, 0, 1);
     layout->addWidget(nameLabel, 1, 0);
     layout->addWidget(nameLineEdit, 1, 1);
     layout->addWidget(typeLabel, 2, 0);
     layout->addWidget(typeLineEdit, 2, 1);
     layout->addWidget(sizeLabel, 3, 0);
     layout->addWidget(sizeLineEdit, 3, 1);
     layout->addWidget(okButton, 4, 0);
     layout->addWidget(cancelButton, 4, 1);
     layout->setColumnStretch(1, 1);
     setLayout(layout);

     // 设置窗口标题和大小
     setWindowTitle(title);
     setFixedWidth(300);

     // 连接信号和槽
     connect(okButton, &QPushButton::clicked, this, &QDialog::accept);
     connect(cancelButton, &QPushButton::clicked, this, &QDialog::reject);
 }

 QString FileRuleDialog::getDire() const
 {
     return direComboBox->currentText();
 }

 QString FileRuleDialog::getName() const
 {
     return nameLineEdit->text();
 }

 QString FileRuleDialog::getType() const
 {
     return typeLineEdit->text();
 }

 QString FileRuleDialog::getSize() const
 {
     return sizeLineEdit->text();
 }
