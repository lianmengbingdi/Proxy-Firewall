#include "mainwindow.h"
#include "user.h"
#include <QPixmap>
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QString>
#include <QFile>
#include <QDebug>
#include <QDir>
#include <QProcess>
#include <QBitmap>
#include <QTextStream>

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
{
//    this->setWindowTitle("用户界面设计示例");
//    this->resize(800, 600);
    //username=name;
    // 创建主页面


    createMainPage();
    setupConnections();



}
void MainWindow::setupname(QString name)
{
    username=name;
    //确定用户名后初始化日志
//    QString  logPath = "./"+username + "/proxylog.txt";
//    QFile file(logPath);

//    // 打开文件，以写入和文本模式打开
//    if(!file.exists()){
//    if (file.open(QIODevice::ReadWrite | QIODevice::Text)) {
//        // 关联文件和文本流
//        QString command = "chmod 777 \"" + logPath + "\"";
//        QProcess::execute(command);
//        QTextStream stream(&file);

//        // 写入示例日志信息
//        stream << "Proxy log file has been created." << endl;

//        // 关闭文件
//        file.close();
//    } else {
//        // 输出错误信息
//        qDebug() << "Unable to create log file:" << file.errorString();
//    }
//    }
}
QString MainWindow::user()
{
    return username;
}
MainWindow::~MainWindow()
{
    // 析构函数
}


void MainWindow::createMainPage()
{       QString title="HTTP-HTTPS-FTP 应用代理防火墙";
        this->setWindowTitle(title);
        this->resize(1000, 600);
        // 设置背景图
        QPixmap backgroundImage(":/firewall.jpg");
        backgroundLabel = new QLabel(this);
        backgroundLabel->setPixmap(backgroundImage.scaled(size()));
        // 设置中心部件为背景图
        setCentralWidget(backgroundLabel);
        // 设置背景图的大小策略，使其随着窗口大小自动调整
        backgroundLabel->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding);
        backgroundLabel->setScaledContents(true);
        // 将背景图置底
        backgroundLabel->lower();
       // QWidget *backgroundLabel = new QWidget(this);
        setCentralWidget(backgroundLabel);

        QVBoxLayout *mainLayout = new QVBoxLayout(backgroundLabel);

        // 创建顶部导航栏
        QWidget *navigationWidget = new QWidget(backgroundLabel);
        QHBoxLayout *navigationLayout = new QHBoxLayout(navigationWidget);
        //navigationLayout->setSpacing(10);
        navigationLayout->setContentsMargins(10, 10, 10, 10);

        QLabel *statusLabel = new QLabel("当前代理状态:", navigationWidget);
        QLabel *connectionLabel = new QLabel("", navigationWidget);
        // 设置字体大小
        QFont statusFont("Arial", 32, QFont::Bold);
        //QFont connectionFont("Arial", 16,QFont::Bold);

        statusLabel->setFont(statusFont);
        //connectionLabel->setFont(connectionFont);

        // 设置字体颜色
        QPalette palette;
        palette.setColor(QPalette::WindowText, Qt::white); // 设置字体颜色为蓝色
        statusLabel->setPalette(palette);
        connectionLabel->setPalette(palette);

        // 创建状态栏布局
        QHBoxLayout *statusLayout = new QHBoxLayout();
        statusIcon = new QLabel(this);
        activeIcon = QPixmap(":/active.png");
        inactiveIcon = QPixmap(":/inactive.png");

        statusIcon->setPixmap(activeIcon.scaled(100, 100));
        statusLayout->addStretch();
        statusLayout->addStretch();
        statusLayout->addWidget(statusLabel);
        statusLayout->addStretch();
        statusLayout->addWidget(statusIcon);
        statusLayout->addStretch();
        statusLayout->addStretch();
        //QLabel *connectionCountLabel = new QLabel(navigationWidget);
        //connectionCountLabel->setFont(connectionFont);
        //connectionCountLabel->setPalette(palette);
        //QString countText = "<span style='font-size: 80px;'>3</span>"; // 调整字体大小为24px
        //connectionCountLabel->setText(countText);
        //QLabel* spacingLabel = new QLabel(navigationWidget);
        //spacingLabel->setFixedWidth(10); // 设置间距的宽度
        //statusLayout->addWidget(connectionLabel);
        //statusLayout->addWidget(spacingLabel);
        //statusLayout->addWidget(connectionCountLabel);
        //statusLayout->addStretch();
        //statusLayout->addStretch();

        navigationLayout->addLayout(statusLayout);

        mainLayout->addWidget(navigationWidget);

        // 创建按钮布局
        QVBoxLayout *buttonLayout = new QVBoxLayout();
        startStopButton = new QPushButton("启动/停止代理", backgroundLabel);
        logButton = new QPushButton("查看日志", backgroundLabel);
         configButton = new QPushButton("打开配置文件", backgroundLabel);
         logoutButton = new QPushButton("退出登录", backgroundLabel);


           // 设置按钮样式
           QString startStopbuttonStyle = "QPushButton {"
                                 "background-color: #4CAF50;"
                                 "color: white;"
                                 "border: none;"
                                 "padding: 10px 16px;"
                                 "font-size: 16px;"
                                 "border-radius: 5px;"
                                 "}"
                                 "QPushButton:hover {"
                                 "background-color: #45a049;"
                                 "}"
                                 "QPushButton:pressed {"
                                 "background-color: #3e8e41;"
                                 "}";
           QString logbuttonStyle = "QPushButton {"
                                 "background-color: #008CBA;"
                                 "color: white;"
                                 "border: none;"
                                 "padding: 10px 16px;"
                                 "font-size: 16px;"
                                 "border-radius: 5px;"
                                 "}"
                                 "QPushButton:hover {"
                                 "background-color: #45a049;"
                                 "}"
                                 "QPushButton:pressed {"
                                 "background-color: #3e8e41;"
                                 "}";
           QString configbuttonStyle = "QPushButton {"
                                 "background-color: #6a33f7;"
                                 "color: white;"
                                 "border: none;"
                                 "padding: 10px 16px;"
                                 "font-size: 16px;"
                                 "border-radius: 5px;"
                                 "}"
                                 "QPushButton:hover {"
                                 "background-color: #45a049;"
                                 "}"
                                 "QPushButton:pressed {"
                                 "background-color: #3e8e41;"
                                 "}";
            QString logoutbuttonStyle = "QPushButton {"
                                     "background-color: #f44336;"
                                     "color: white;"
                                     "border: none;"
                                     "padding: 10px 16px;"
                                     "font-size: 16px;"
                                     "border-radius: 5px;"
                                     "}"
                                     "QPushButton:hover {"
                                     "background-color: #45a049;"
                                     "}"
                                     "QPushButton:pressed {"
                                     "background-color: #3e8e41;"
                                     "}";
           startStopButton->setStyleSheet(startStopbuttonStyle);
           logButton->setStyleSheet(logbuttonStyle);
           configButton->setStyleSheet(configbuttonStyle);
            logoutButton->setStyleSheet(logoutbuttonStyle);
           buttonLayout->addWidget(startStopButton);
           buttonLayout->addWidget(logButton);
           buttonLayout->addWidget(configButton);
           buttonLayout->addWidget(logoutButton);

           mainLayout->addLayout(buttonLayout);
           connect(startStopButton, &QPushButton::clicked, this, &MainWindow::toggleProxyStatus);
           connect(logoutButton, &QPushButton::clicked, this, &MainWindow::logout);

}


void MainWindow::logout()
{
    // 关闭主界面
    close();


}
void MainWindow::closeEvent(QCloseEvent *event) {

    QTcpSocket clientSocket;
    clientSocket.connectToHost(server_IP, 8888);

    if (!clientSocket.waitForConnected()) {
        qDebug() << "Error: Failed to connect to server.";
        return;
    }

    QString deleteProxyRuleMessage = "POST /exit HTTP/1.1\r\n"
                                     "Username:"+username+"\r\n"

                                     "Content-Length: "  + "\r\n"

    ;
    // 发送消息
    clientSocket.write(deleteProxyRuleMessage.toUtf8());

    if (!clientSocket.waitForBytesWritten()) {
        qDebug() << "Error: Failed to send deleteProxyRule message.";
        return;
    }
    QWidget::closeEvent(event);
    UserWindow *w=new UserWindow();
    w->show();

}
void MainWindow::toggleProxyStatus()
{
    QTcpSocket clientSocket;
    clientSocket.connectToHost(server_IP, 8888);

    if (!clientSocket.waitForConnected()) {
        qDebug() << "Error: Failed to connect to server.";
        return ;
    }

    // 构建发送消息
    QString message = "GET /toggle Proxy HTTP/1.1\r\n"
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
    // 切换代理状态并更新图标
    if (responseString.contains("OK")) {

        proxyActive = !proxyActive;

        if (proxyActive) {
            statusIcon->setPixmap(activeIcon.scaled(100, 100));
        } else {
            statusIcon->setPixmap(inactiveIcon.scaled(100, 100));
        }
        qDebug() << "success!";
        clientSocket.close();

    }
     else {
        QMessageBox::warning(this, "Error", "Failed to toggle the agent status");
        qDebug() << responseString;

    }


}



void MainWindow::setupConnections()
{

    connect(configButton, &QPushButton::clicked, this, &MainWindow::createConfigPage);

    connect(logButton, &QPushButton::clicked, this, &MainWindow::createLogPage);
}
