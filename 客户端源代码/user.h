#ifndef USER_H
#define USER_H

#include <QMainWindow>
#include <QPushButton>
#include <QLabel>
#include <QLineEdit>
#include <QMessageBox>
#include <QtNetwork/QNetworkAccessManager>
#include <QtNetwork/QNetworkRequest>
#include <QtNetwork/QNetworkReply>
#include <QtCore/QEventLoop>
#include <QtCore>
#include <QCryptographicHash>

#include<QUrlQuery>

class UserWindow : public QMainWindow
{
    Q_OBJECT

public:
    UserWindow(QWidget *parent = nullptr);
    ~UserWindow();
private slots:
    void handleLogin();
    void handleRegister();
    void handleUNregister();
private:
    QLabel *backgroundLabel;
    QLabel *usernameLabel;
    QLabel *passwordLabel;
    QLineEdit *usernameLineEdit;
    QLineEdit *passwordLineEdit;
    QPushButton *loginButton;
    QPushButton *registerButton;
    QPushButton *unregisterButton;

    void setupUI();
    void setupConnections();
    bool validateLogin(const QString& username, const QString& password);
    bool registerUser(const QString& username, const QString& password);
    bool unregisterUser(const QString& username, const QString& password);
   // bool storeUserCredentials(const QString& username, const QString& encryptedPassword);
};

#endif //
