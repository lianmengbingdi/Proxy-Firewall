#include <iostream>
#include <unistd.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <cstring>
#include <tuple>
#include <vector>
#include <sys/stat.h>
#include <algorithm>
#include <cstdlib>
#include <fstream>
#include <sstream>
#include <fcntl.h>
#include <sys/select.h>
#include "rule.h"
#include "file_control.h"
#include "account.h"
#include <unordered_map>

using namespace std;

unordered_map<string, string> cip;
extern unordered_map<string, bool> status;
extern unordered_map<string, vector<tuple<string, int, string, string>>> rule;
extern unordered_map<string, vector<tuple<string, string, string, float>>> rule2;

bool userExists(const string &username)
{
    ifstream inputFile("account.txt");
    if (!inputFile.is_open())
    {
        cerr << "Failed to open account.txt" << endl;
        return false;
    }
    string line;
    while (getline(inputFile, line))
    {
        size_t pos = line.find("Username: ");
        if (pos != string::npos)
        {
            string existingUsername = line.substr(pos + 10, line.find(",") - (pos + 10));
            if (existingUsername == username)
            {
                return true;
            }
        }
    }
    inputFile.close();
    return false;
}

void writeLog(const string &path, const string &response)
{
    ofstream outputFile(path);
    if (!outputFile.is_open())
    {
        cerr << "Failed to open account.txt" << endl;
        return;
    }

    outputFile << response << endl;
    outputFile.close();
}

string getCurrentTimestamp()
{
    time_t now = time(nullptr);
    char timestamp[25];
    strftime(timestamp, sizeof(timestamp), "%Y-%m-%dT%H:%M:%S.000Z", gmtime(&now));
    return string(timestamp);
}

string formatLogMessage(const string &timestamp, const string &level, const string &message, const string &user)
{
    return "{\"timestamp\": \"" + timestamp + "\", \"level\": \"" + level + "\", \"message\": \"" + message + "\", \"user_id\": \"" + user + "\"}";
}

string createRegistrationLog(const string &user)
{
    string timestamp = getCurrentTimestamp();
    string level = "INFO";
    string message = "Register Success";
    return formatLogMessage(timestamp, level, message, user);
}

string createLoginLog(const string &user)
{
    string timestamp = getCurrentTimestamp();
    string level = "INFO";
    string message = "Login Success";
    return formatLogMessage(timestamp, level, message, user);
}

string createLogoutLog(const string &user)
{
    string timestamp = getCurrentTimestamp();
    string level = "INFO";
    string message = "Delete Success";
    return formatLogMessage(timestamp, level, message, user);
}

string createExitLog(const string &user) // changed
{
    string timestamp = getCurrentTimestamp();
    string level = "INFO";
    string message = "Exit Success";
    return formatLogMessage(timestamp, level, message, user);
}

bool userRight(const string &username, const string &password)
{
    ifstream inputFile("account.txt");
    if (!inputFile.is_open())
    {
        cerr << "Failed to open account.txt" << endl;
        return false;
    }

    string line;
    while (getline(inputFile, line))
    {
        size_t usernamePos = line.find("Username: ");
        size_t passwordPos = line.find(", Password: ");
        if (usernamePos != string::npos && passwordPos != string::npos)
        {
            string existingUsername = line.substr(usernamePos + 10, passwordPos - (usernamePos + 10));
            string existingPassword = line.substr(passwordPos + 12);
            if (existingUsername == username && existingPassword == password)
            {
                inputFile.close();
                return true;
            }
        }
    }

    inputFile.close();
    return false;
}

void handleRegisterRequest(int clientSocket, char *requestBuffer)
{
    // 从请求数据中解析出用户名和密码
    string requestData(requestBuffer);
    string username, password;
    size_t usernamePos = requestData.find("username=");
    size_t passwordPos = requestData.find("&password=");
    if (usernamePos != string::npos && passwordPos != string::npos)
    {
        username = requestData.substr(usernamePos + 9, passwordPos - (usernamePos + 9));
        password = requestData.substr(passwordPos + 10);
    }

    // 检查用户名和密码是否为空
    if (username.empty() || password.empty())
    {
        string response = "HTTP/1.1 400 Bad Request\r\n"
                          "Content-Length: 16\r\n"
                          "\r\n"
                          "Invalid request";
        send(clientSocket, response.c_str(), response.length(), 0);
        return;
    }

    // 打开 account.txt 文件
    ofstream outputFile("account.txt", ios::app);
    if (!outputFile.is_open())
    {
        cerr << "Failed to open account.txt" << endl;
        string response = "HTTP/1.1 500 Internal Server Error\r\n"
                          "Content-Length: 21\r\n"
                          "\r\n"
                          "Failed to store data";
        send(clientSocket, response.c_str(), response.length(), 0);
        return;
    }
    // 检查用户名是否已存在
    if (userExists(username))
    {
        string response = "HTTP/1.1 400 Bad Request\r\n"
                          "Content-Length: 19\r\n"
                          "\r\n"
                          "Username already exists";
        send(clientSocket, response.c_str(), response.length(), 0);
        return;
    }
    else
    {
        // 将用户名和密码写入文件
        outputFile << "Username: " << username << ", Password: " << password << endl;
        outputFile.close();
        string path = "./" + username;
        int status = mkdir(path.c_str(), 0777);
        string filePath_1 = path + "/proxy_rules.txt";

        // 打开文件并写入内容
        ofstream outputFile_1(filePath_1);
        if (outputFile_1.is_open())
        {

            outputFile_1.close();
            cerr << "File created successfully: " << filePath_1 << endl;
        }
        else
        {
            cerr << "Failed to create file: " << filePath_1 << endl;
        }
        string filePath_2 = path + "/proxylog.txt";

        // 打开文件并写入内容
        ofstream outputFile_2(filePath_2);
        if (outputFile_2.is_open())
        {
            string content = "Proxy log file has been created.";
            outputFile_2 << content << endl;
            outputFile_2.close();
            cerr << "File created successfully: " << filePath_2 << endl;
        }
        else
        {
            cerr << "Failed to create file: " << filePath_2 << endl;
        }
        string response = "HTTP/1.1 200 OK\r\n"
                          "Content-Length: 2\r\n"
                          "\r\n"
                          "OK";
        writeLog("account_log.txt", createRegistrationLog(username));
        string filePath_3 = path + "/file_rules.txt";

        // 打开文件并写入内容
        ofstream outputFile_3(filePath_3);
        if (outputFile_3.is_open())
        {

            outputFile_3.close();
            cerr << "File created successfully: " << filePath_3 << endl;
        }
        else
        {
            cerr << "Failed to create file: " << filePath_3 << endl;
        }
        send(clientSocket, response.c_str(), response.length(), 0);
    }
}

void handleExitRequest(int clientSocket, char *requestBuffer, string clientIP) // changed
{
    // 从请求数据中解析出用户名
    string requestData(requestBuffer);
    string user;
    size_t userPos = requestData.find("Username:");
    size_t newlinePos = requestData.find("\r\n", userPos);
    if (userPos != string::npos)
    {
        user = requestData.substr(userPos + 9, newlinePos - (userPos + 9));
    }
    cerr << user << endl;
    // 检查用户名是否为空
    if (user.empty())
    {
        string response = "HTTP/1.1 400 Bad Request\r\n"
                          "Content-Length: 16\r\n"
                          "\r\n"
                          "Invalid request";
        send(clientSocket, response.c_str(), response.length(), 0);
        return;
    }
    // begin your modification
    status.erase(user);
    rule.erase(user);
    rule2.erase(user);
    cip.erase(clientIP);
    string response = "HTTP/1.1 200 OK\r\n"
                      "Content-Length: 2\r\n"
                      "\r\n"
                      "OK";
    writeLog("account_log.txt", createExitLog(user));
    send(clientSocket, response.c_str(), response.length(), 0);
}

void removeUser(const string &username, const string &password)
{
    // 打开 account.txt 文件
    ifstream inputFile("account.txt");
    if (!inputFile.is_open())
    {
        cerr << "Failed to open account.txt" << endl;
        return;
    }

    // 读取文件内容
    vector<string> lines;
    string line;
    while (getline(inputFile, line))
    {
        lines.push_back(line);
    }
    inputFile.close();

    // 在内存中删除指定的用户名和密码
    vector<string>::iterator it = find_if(lines.begin(), lines.end(), [&](const string &line)
                                          {
        size_t pos = line.find("Username: ");
        if (pos != string::npos) {
            string existingUsername = line.substr(pos + 10, line.find(", Password: ") - (pos + 10));
            string existingPassword = line.substr(line.find(", Password: ") + 12);
            return (existingUsername == username && existingPassword == password);
        }
        return false; });

    if (it != lines.end())
    {
        lines.erase(it);
    }
    else
    {
        cerr << "error" << endl;
        return;
    }

    // 重新写回 account.txt 文件
    ofstream outputFile("account.txt");
    if (!outputFile.is_open())
    {
        cerr << "Failed to open account.txt" << endl;
        return;
    }

    for (const string &line : lines)
    {
        outputFile << line << endl;
    }

    outputFile.close();
    cerr << "User removed" << endl;
    string command = "rm -rf ./" + username;
    int status = system(command.c_str());
}

void handleLogoutRequest(int clientSocket, char *requestBuffer)
{
    // 从请求数据中解析出用户名和密码
    string requestData(requestBuffer);
    string username, password;
    size_t usernamePos = requestData.find("username=");
    size_t passwordPos = requestData.find("&password=");
    if (usernamePos != string::npos && passwordPos != string::npos)
    {
        username = requestData.substr(usernamePos + 9, passwordPos - (usernamePos + 9));
        password = requestData.substr(passwordPos + 10);
    }

    // 检查用户名和密码是否为空
    if (username.empty() || password.empty())
    {
        string response = "HTTP/1.1 400 Bad Request\r\n"
                          "Content-Length: 16\r\n"
                          "\r\n"
                          "Invalid request";
        send(clientSocket, response.c_str(), response.length(), 0);
        return;
    }

    // 检查用户名和密码是否匹配
    if (!userRight(username, password))
    {
        string response = "HTTP/1.1 401 Unauthorized\r\n"
                          "Content-Length: 21\r\n"
                          "\r\n"
                          "Invalid username or password";
        send(clientSocket, response.c_str(), response.length(), 0);
        return;
    }

    // 执行注销操作，例如从活动会话中删除用户
    removeUser(username, password);

    string response = "HTTP/1.1 200 OK\r\n"
                      "Content-Length: 2\r\n"
                      "\r\n"
                      "OK";
    writeLog("account_log.txt", response);
    send(clientSocket, response.c_str(), response.length(), 0);
}

void handleLoginResponse(int clientSocket, char *requestBuffer, string clientIP)
{
    // 从请求数据中解析出用户名和密码
    string requestData(requestBuffer);
    string user, username, password;
    size_t usernamePos = requestData.find("username=");
    size_t passwordPos = requestData.find("&password=");
    if (usernamePos != string::npos && passwordPos != string::npos)
    {
        username = requestData.substr(usernamePos + 9, passwordPos - (usernamePos + 9));
        password = requestData.substr(passwordPos + 10);
    }
    // 检查用户名和密码是否为空
    if (username.empty() || password.empty())
    {
        string response = "HTTP/1.1 400 Bad Request\r\n"
                          "Content-Length: 16\r\n"
                          "\r\n"
                          "Invalid request";
        send(clientSocket, response.c_str(), response.length(), 0);
        return;
    }

    // 检查用户名和密码是否匹配
    if (!userRight(username, password))
    {
        string response = "HTTP/1.1 401 Unauthorized\r\n"
                          "Content-Length: 21\r\n"
                          "\r\n"
                          "Invalid username or password";
        send(clientSocket, response.c_str(), response.length(), 0);
        return;
    }

    // 执行登录操作，例如将用户添加到活动会话中
    user = username;
    string response = "HTTP/1.1 200 OK\r\n"
                      "Content-Length: 2\r\n"
                      "\r\n"
                      "OK";
    writeLog("account_log.txt", response);
    send(clientSocket, response.c_str(), response.length(), 0);
    cip[clientIP] = user;

    string proxyFile = "./" + user + "/proxy_rules.txt";
    cerr << proxyFile << endl;
    ifstream file(proxyFile);
    if (!file)
    {
        cerr << "Failed to open file: " << proxyFile << endl;
        return;
    }

    stringstream buffer;
    buffer << file.rdbuf();
    response = buffer.str();
    send(clientSocket, response.c_str(), response.length(), 0);
    read(proxyFile, rule[user]);
    status[user] = true;

    string myFile = "./" + user + "/file_rules.txt";
    cerr << myFile << endl;
    ifstream file2(myFile);
    if (!file2)
    {
        cerr << "Failed to open file: " << myFile << endl;
        return;
    }

    buffer << file2.rdbuf();
    response = buffer.str();
    // cerr<<response;
    send(clientSocket, response.c_str(), response.length(), 0);
    file_read(myFile, rule2[user]);
}

void handleloadRequest(int clientSocket, char *requestBuffer) // 读取指定用户的代理规则文件，将其中的内容作为HTTP响应发送回客户端，并将代理规则文件的内容解析到rule[user]中
{
    string requestData(requestBuffer);
    size_t userPos = requestData.find("Username:");
    size_t newlinePos = requestData.find("\r\n", userPos);
    string user;
    user = requestData.substr(userPos + 9, newlinePos - (userPos + 9));
    string proxyFile = "./" + user + "/proxy_rules.txt";
    cerr << proxyFile << endl;
    ifstream file(proxyFile);
    if (!file)
    {
        cerr << "Failed to open file: " << proxyFile << endl;
        return;
    }

    stringstream buffer;
    buffer << file.rdbuf();
    string response = buffer.str();
    // cerr<<response;
    send(clientSocket, response.c_str(), response.length(), 0);
    read(proxyFile, rule[user]);
}

void handleLogRequest(int clientSocket, char *requestBuffer)
{
    string requestData(requestBuffer);
    size_t userPos = requestData.find("Username:");
    size_t newlinePos = requestData.find("\r\n", userPos);
    string user;
    user = requestData.substr(userPos + 9, newlinePos - (userPos + 9));
    string proxyFile = "./" + user + "/proxylog.txt";
    cerr << proxyFile << endl;
    ifstream file(proxyFile);
    if (!file)
    {
        cerr << "Failed to open file: " << proxyFile << endl;
        return;
    }

    stringstream buffer;
    buffer << file.rdbuf();
    string response = buffer.str();
    // cerr<<response;
    send(clientSocket, response.c_str(), response.length(), 0);
    // read(proxyFile, rule[user]);
}

void LogClear(int clientSocket, char *requestBuffer)
{
    string requestData(requestBuffer);
    size_t userPos = requestData.find("Username:");
    size_t newlinePos = requestData.find("\r\n", userPos);
    string user;
    user = requestData.substr(userPos + 9, newlinePos - (userPos + 9));
    string proxyFile = "./" + user + "/proxylog.txt";
    cerr << proxyFile << endl;

    ofstream file(proxyFile, ios::trunc); // 打开文件，并设置打开模式为清空文件内容

    if (file.is_open())
    {
        file.close(); // 关闭文件
    }
    else
    {
        cerr << "无法打开文件" << endl;
        return;
    }

    // 重新打开文件以进行后续操作
    ofstream file2(proxyFile, ios::app); // 打开文件，并设置打开模式为追加内容

    if (file2.is_open())
    {
        string content = "Proxy log file has been created.";
        file2 << content << endl;
        file2.close(); // 关闭文件
    }
    else
    {
        cerr << "无法打开文件" << endl;
        return;
    }
}
