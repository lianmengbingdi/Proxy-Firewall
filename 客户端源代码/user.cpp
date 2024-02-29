#include "user.h"
#include "mainwindow.h"
#include <QPixmap>
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QString>
#include <QFile>
#include <QDebug>
#include <QDir>
#include <QProcess>

UserWindow::UserWindow(QWidget *parent)
    : QMainWindow(parent)
{
    setupUI();
    setupConnections();
}

UserWindow::~UserWindow()
{
}


void UserWindow::setupUI()
{
    // 设置窗口标题
    setWindowTitle("Login Page");

    // 设置背景图
    QPixmap backgroundImage(":/login.jpg");
    backgroundLabel = new QLabel(this);
    backgroundLabel->setPixmap(backgroundImage.scaled(size()));

    // 设置中心部件为背景图
    setCentralWidget(backgroundLabel);

    // 创建垂直布局管理器
    QVBoxLayout *vLayout = new QVBoxLayout(backgroundLabel);
    vLayout->addStretch(); // 添加一个弹簧，使标签和输入框居中

    // 创建水平布局管理器1，用于用户名
    QHBoxLayout *hLayout1 = new QHBoxLayout();

    // 设置用户名标签和输入框
    usernameLabel = new QLabel("用户:", backgroundLabel);
    usernameLineEdit = new QLineEdit(backgroundLabel);

    // 将用户名标签和输入框添加到水平布局管理器1中
    hLayout1->addStretch();
    hLayout1->addWidget(usernameLabel);
    hLayout1->addWidget(usernameLineEdit);
    hLayout1->addStretch();

    // 将水平布局管理器1添加到垂直布局管理器中
    vLayout->addLayout(hLayout1);

    // 创建水平布局管理器2，用于密码
    QHBoxLayout *hLayout2 = new QHBoxLayout();

    // 设置密码标签和输入框
    passwordLabel = new QLabel("密码:", backgroundLabel);
    passwordLineEdit = new QLineEdit(backgroundLabel);
    passwordLineEdit->setEchoMode(QLineEdit::Password);
    // 将密码标签和输入框添加到水平布局管理器2中
    hLayout2->addStretch();
    hLayout2->addWidget(passwordLabel);
    hLayout2->addWidget(passwordLineEdit);
    hLayout2->addStretch();
    // 将水平布局管理器2添加到垂直布局管理器中
    vLayout->addLayout(hLayout2);

   //vLayout->addStretch(); // 再添加一个弹簧，使登录按钮居中

    // 设置密码输入框的最大宽度为合适的长度
    int maxWidth = width() / 3; // 计算合适的长度，此处假设为界面宽度的1/3
    usernameLineEdit->setMaximumWidth(maxWidth);
    passwordLineEdit->setMaximumWidth(maxWidth);

    // 设置登录按钮
    loginButton = new QPushButton("登录", backgroundLabel);
    loginButton->setFixedSize(100, 30);

    // 设置注册按钮
    registerButton = new QPushButton("注册", backgroundLabel);
    registerButton->setFixedSize(100, 30);

    // 设置注销按钮
        unregisterButton = new QPushButton("注销", backgroundLabel);
        unregisterButton->setFixedSize(100, 30);

        // 将登录按钮、注册按钮和注销按钮添加到水平布局管理器3中
        QHBoxLayout *hLayout3 = new QHBoxLayout();
        hLayout3->addStretch();
        hLayout3->addWidget(loginButton, 0, Qt::AlignCenter);
        hLayout3->addWidget(registerButton, 0, Qt::AlignCenter);
        hLayout3->addWidget(unregisterButton, 0, Qt::AlignCenter);
        hLayout3->addStretch();

        // 将布局管理器设置为主窗口的布局
        vLayout->addLayout(hLayout3);
        vLayout->addStretch();

        // 设置背景图的大小策略，使其随着窗口大小自动调整
        backgroundLabel->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding);
        backgroundLabel->setScaledContents(true);
    }

void UserWindow::setupConnections()
{
    connect(loginButton, &QPushButton::clicked, this, &UserWindow::handleLogin);
    connect(registerButton, &QPushButton::clicked, this, &UserWindow::handleRegister);
    connect(unregisterButton, &QPushButton::clicked, this, &UserWindow::handleUNregister);
}
QString sha256Hash(const QString &password) {
   QByteArray passwordBytes = password.toUtf8();
   QByteArray hashBytes = QCryptographicHash::hash(passwordBytes, QCryptographicHash::Sha256);
   QString hash = QString(hashBytes.toHex());
   return hash;
}
bool UserWindow::validateLogin(const QString& username, const QString& password)
{
    QTcpSocket clientSocket;
    clientSocket.connectToHost(server_IP, 8888);

    if (!clientSocket.waitForConnected()) {

        QMessageBox::warning(this, "login", "Failed to connect to server.");
        return false;
    }

    // 构建登录消息
    QString loginMessage = "POST /login HTTP/1.1\r\n"
                           "Host: 120.27.142.144\r\n"
                           "Content-Length: " + QString::number(username.length() + password.length() + 2) + "\r\n"
                           "\r\n"
                           "username=" + username + "&password=" + password;

    // 发送登录消息
    clientSocket.write(loginMessage.toUtf8());
    if (!clientSocket.waitForBytesWritten()) {

        QMessageBox::warning(this, "login", "Failed to send login message.");
        clientSocket.close();
        return false;
    }

    QByteArray response;
    while (clientSocket.waitForReadyRead()) {
        response += clientSocket.readAll();
    }
    QString responseString = response;



    if (responseString.contains("OK")) {
        qDebug() << "Login success!";
        clientSocket.close();
        return true;
    } else if (responseString.contains("Invalid username or password")) {
        QMessageBox::warning(this, "login", "Invalid username or password");
        clientSocket.close();
        return false;
    } else {
        QMessageBox::warning(this, "login", "Failed to login");
        qDebug() << responseString;
        return false;
    }
}


bool UserWindow::registerUser(const QString& username, const QString& password)
{
    QTcpSocket clientSocket;
    clientSocket.connectToHost(server_IP, 8888);

    if (!clientSocket.waitForConnected()) {

        QMessageBox::warning(this, "register", "Failed to connect to server.");
        return false;
    }

    // 构建注册消息
    QString registerMessage = "POST /register HTTP/1.1\r\n"
                              "Host: 120.27.142.144\r\n"
                              "Content-Length: " + QString::number(username.length() + password.length() + 2) + "\r\n"
                              "\r\n"
                              "username=" + username + "&password=" + password;

    // 发送注册消息
    clientSocket.write(registerMessage.toUtf8());
    if (!clientSocket.waitForBytesWritten()) {
        QMessageBox::warning(this, "register", "Failed to send register message.");
        clientSocket.close();
        return false;
    }


    QByteArray response;
    while (clientSocket.waitForReadyRead()) {
        response += clientSocket.readAll();
    }
    QString responseString = response;



    if (responseString.contains("OK")) {
        qDebug() << "Register success!";
        clientSocket.close();
        return true;
    } else if (responseString.contains("Username already exists")) {
        QMessageBox::warning(this, "register", "Username already exists");
        clientSocket.close();
        return false;
    } else {
        QMessageBox::warning(this, "register", "Failed to register user");
        qDebug() << responseString;
        return false;
    }

}




bool UserWindow::unregisterUser(const QString& username, const QString& password)
{  QTcpSocket clientSocket;
    clientSocket.connectToHost(server_IP, 8888);

    if (!clientSocket.waitForConnected()) {

        QMessageBox::warning(this, "Unregister", "Failed to connect to server.");
        return false;
    }

    // 构建注销消息
    QString logoutMessage = "POST /logout HTTP/1.1\r\n"
                            "Host: 120.27.142.144\r\n"
                            "Content-Length: " + QString::number(username.length() + password.length() + 2) + "\r\n"
                            "\r\n"
                            "username=" + username + "&password=" + password;

    // 发送注销消息
    clientSocket.write(logoutMessage.toUtf8());
    if (!clientSocket.waitForBytesWritten()) {
        QMessageBox::warning(this, "Unregister", "Failed to send logout message.");
        clientSocket.close();
        return false;
    }
    QByteArray response;
    while (clientSocket.waitForReadyRead()) {
        response += clientSocket.readAll();
    }
    QString responseString = response;

    if (responseString.contains("OK")) {
        qDebug() << "Unregister success!";
        clientSocket.close();
        return true;
    } else if (responseString.contains("Invalid username or password")) {
        QMessageBox::warning(this, "Unregister", "Invalid username or password");
        clientSocket.close();
        return false;
    } else {
        QMessageBox::warning(this, "register", "Failed to unregister user");
        qDebug() << responseString;
        return false;
    }
    clientSocket.close();

    return true;
}

void UserWindow::handleRegister()
{
    QString username = usernameLineEdit->text();
    QString password = sha256Hash(passwordLineEdit->text());

    if (registerUser(username, password)) {
        QMessageBox::information(this, "Register", "Registration Successful");
    }
}

void UserWindow::handleUNregister()
{
    QString username = usernameLineEdit->text();
    QString password = sha256Hash(passwordLineEdit->text());

    if (unregisterUser(username, password)) {
        QMessageBox::information(this, "Unregister", "Unregistration Successful");
    }
}

void UserWindow::handleLogin()
{
    QString username = usernameLineEdit->text();
    QString password = sha256Hash(passwordLineEdit->text());

    if (validateLogin(username, password)) {
        QMessageBox::information(this, "Login", "Login Successful");

        close();
        //QString directory = "/home/code/Desktop/nis/nis/login";
       // QString executablePath = directory + "/rule";
        // 启动新的进程，并传递命令行参数
       // QProcess::startDetached(executablePath, QStringList() << username, directory);
        MainWindow *s=new MainWindow();
        s->setupname(username);
        s->show();
    }
}


