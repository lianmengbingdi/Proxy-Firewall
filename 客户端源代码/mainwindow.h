#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QPushButton>
#include <QLabel>
#include <QTableView>
#include <QLineEdit>
#include <QMessageBox>
#include <QStandardItemModel>
#include <QSpinBox>
#include <QComboBox>
#include <QTextEdit>
#include <QtNetwork/QNetworkAccessManager>
#include <QtNetwork/QNetworkRequest>
#include <QtNetwork/QNetworkReply>
#include <QtCore/QEventLoop>
#include<QUrlQuery>
#include<QThread>
#include <QDebug>
extern  std::string user_id;
#define server_IP "120.27.142.144"
class MainWindow : public QMainWindow
{
 Q_OBJECT

public:
    MainWindow(QWidget *parent = nullptr);
    ~MainWindow();
    void setupname(QString name);
    QString user();
protected:
    void closeEvent(QCloseEvent *event) override;

private slots:
    void createConfigPage();
    void createLogPage();
    void createMainPage();
    void toggleProxyStatus();
    //handle configgage click
    void addProxyRule();
    void editProxyRule();
    void deleteProxyRule();
    void addFileRule();
    void editFileRule();
    void deleteFileRule();
     void logout();

private:
    void loadproxyrules();
    void loadfilerules();
    void setupConnections();
    void setupConfigConnections();
    void saveProxyRulesToFile();
    void saveFileRulesToFile();
    void writeLogToFile(const QString& logMessage);
    QString username;
    //状态按钮
    QLabel* statusIcon;
    QPixmap activeIcon;
    QPixmap inactiveIcon;
    bool proxyActive=1;

    QTableView *proxyRuleTableView;   // 代理规则表格
    QTableView *FileRuleTableView;  // 过滤规则表格
    QStandardItemModel *proxyRuleModel;   // 代理规则模型
    QStandardItemModel *FileRuleModel;  // 过滤规则模型
    QLabel *backgroundLabel;
    QLabel *statusLabel;
    QLabel *connectionLabel;
    QPushButton *startStopButton;
    QPushButton *logoutButton;
    QPushButton *logButton;
    QTextEdit *logTextEdit = nullptr;

    //配置按钮
    QPushButton *configButton ;
    QPushButton *addProxyRuleButton ;
    QPushButton *editProxyRuleButton ;
    QPushButton *deleteProxyRuleButton ;
    QPushButton *addFileRuleButton ;
    QPushButton *editFileRuleButton ;
    QPushButton *deleteFileRuleButton ;

    QTabWidget *configTabWidget;

};

class ProxyRuleDialog : public QDialog
{
    Q_OBJECT

public:
    explicit ProxyRuleDialog(QWidget *parent = nullptr,
                             const QString &title = "添加代理规则",
                             const QString &address = "",
                             const QString &port = "",
                             const QString &internet = "",
                             bool enabled = true);
    QString getAddress() const;
    QString getPort() const;
    QString getInternet() const;
    bool getEnabled() const;

private:
    QLabel *addressLabel;
    QLineEdit *addressLineEdit;
    QLabel *portLabel;
    QLineEdit *portLineEdit;
    QLabel *enabledLabel;
    QCheckBox *enabledCheckBox;
    QPushButton *okButton;
    QPushButton *cancelButton;
    QLabel *intenetLabel ;
    QComboBox *internetComboBox ;
};

class ProxyRuleWorker : public QObject
{
    Q_OBJECT

public:
    ProxyRuleWorker(const QString& username,const QString& address, const QString& port, const QString& internet, const QString& enable);
    bool m_editMode = false; // 标记是否为修改规则模式
    bool m_deleteMode=false;  //标记是否为删除规则模式

public slots:
    void runWithSocket();

private:
    void addProxyRule(const QString& username,const QString& address, const QString& port, const QString& internet, const QString& enable);
    void editProxyRule(const QString& username,const QString& address, const QString& port, const QString& internet, const QString& enable);
    void deleteProxyRule(const QString& username,const QString& address, const QString& port, const QString& internet, const QString& enable);

signals:
    void finished();

private:
    QString m_username;
    QString m_address;
    QString m_port;
    QString m_internet;
    QString m_enable;

};


class FileRuleDialog : public QDialog
{
    Q_OBJECT

public:
    explicit FileRuleDialog(QWidget *parent = nullptr,
                              const QString &title = "添加文件控制规则",
                              const QString &dire="",
                              const QString &name="",
                              const QString &type="",
                              const QString &size="");
    QString getDire() const;
    QString getName() const;
    QString getType() const;
    QString getSize() const;

private:
    QLabel *nameLabel;
    QLineEdit *nameLineEdit;
    QLabel *direLabel;
    QComboBox *direComboBox;
    QLabel *typeLabel;
    QLineEdit *typeLineEdit;
    QLabel *sizeLabel;
    QLineEdit *sizeLineEdit;
    QPushButton *okButton;
    QPushButton *cancelButton;
};
class FileRuleWorker : public QObject
{
    Q_OBJECT

public:
    FileRuleWorker(const QString& username,const QString& dire, const QString& name, const QString& type, const QString& size);
    bool m_editMode = false; // 标记是否为修改规则模式
    bool m_deleteMode=false;  //标记是否为删除规则模式

public slots:
    void runWithSocket();

private:
    void addFileRule(const QString& username,const QString& dire, const QString& name, const QString& type, const QString& size);
    void editFileRule(const QString& username,const QString& dire, const QString& name, const QString& type, const QString& size);
    void deleteFileRule(const QString& username,const QString& dire, const QString& name, const QString& type, const QString& size);

signals:
    void finished();

private:
    QString m_username;
    QString m_dire;
    QString m_name;
    QString m_type;
    QString m_size;

};
#endif // MAINWINDOW_H
