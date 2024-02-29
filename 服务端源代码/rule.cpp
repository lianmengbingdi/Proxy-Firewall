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
#include <unordered_map>

using namespace std;

unordered_map<string, bool> status;
unordered_map<string, vector<tuple<string, int, string, string>>> rule;


//规则修改时将修改内容写入日志
void proxy_change(string user, string address, int port, string mode, string enable, string op){
    string Filename = "./" + user + "/proxylog.txt";
    ofstream outFile(Filename,ios::app);
    time_t now=time(0);
    char *dt=ctime(&now);

    if (!outFile) {
        cout << "Failed to open the change file." << endl;
        return;
    }

    outFile.seekp(0, ios::end);

    if (op == "add") 
    { outFile << dt << " " << user << " add the proxy_rule." << " Address: " << address << " Port: " << port << " Mode: " << mode <<" Enable: " << enable << endl; }
    if (op == "delete")
    { outFile << dt << " " << user << " delete the proxy_rule." << " Address: " << address << " Port: " << port << " Mode: " << mode <<" Enable: " << enable << endl; }
    if (op == "edit_old")
    { outFile << dt << " " << user << " edit the proxy_rule." << " Address: " << address << " Port: " << port << " Mode: " << mode <<" Enable: " << enable << endl; }
    if (op == "edit_new")
    { outFile << "The proxy_rule change to" << " Address: " << address << " Port: " << port << " Mode: " << mode <<" Enable: " << enable << endl;}
}

void read(string Filename, vector<tuple<string, int, string, string>> &rule)
{
    ifstream inputFile(Filename);

    if (!inputFile)
    {
        cout << "Failed to open the file." << endl;
        return;
    }
    rule.clear();
    string line;
    while (getline(inputFile, line))
    {
        string address, port, mode, enable;
        size_t addressPos = line.find("Address:");
        size_t portPos = line.find(", Port:");
        size_t modePos = line.find(", Mode:");
        size_t enablePos = line.find(", Enable:");

        if (addressPos != std::string::npos && portPos != std::string::npos &&
            modePos != std::string::npos && enablePos != std::string::npos)
        {
            address = line.substr(addressPos + 9, portPos - (addressPos + 9));
            port = line.substr(portPos + 8, modePos - (portPos + 8));
            mode = line.substr(modePos + 8, enablePos - (modePos + 8));
            enable = line.substr(enablePos + 10);
        }
        rule.emplace_back(address, stoi(port), mode, enable);
    }

    inputFile.close();
    int i = 0;
    for (const auto &tuple : rule)
    {
        cout << i++;
        cout << "Address: " << get<0>(tuple) << ", Port: " << get<1>(tuple) << ", Mode: " << get<2>(tuple) << ", enable: " << get<3>(tuple) << endl;
    }
}

void handleAddRuleRequest(int clientSocket, char *requestBuffer)
{
    // 从请求数据中解析出规则信息
    string requestData(requestBuffer);
    string user, address, port, internet, enable;

    size_t userPos = requestData.find("Username:");
    size_t addressPos = requestData.find("address=");
    size_t portPos = requestData.find("&port=");
    size_t internetPos = requestData.find("&mode=");
    size_t enablePos = requestData.find("&enable=");
    size_t newlinePos = requestData.find("\r\n", userPos);
    if (userPos != string::npos && addressPos != string::npos && portPos != string::npos && internetPos != string::npos && enablePos != string::npos)
    {
        user = requestData.substr(userPos + 9, newlinePos - (userPos + 9));
        address = requestData.substr(addressPos + 8, portPos - (addressPos + 8));
        port = requestData.substr(portPos + 6, internetPos - (portPos + 6));
        internet = requestData.substr(internetPos + 6, enablePos - (internetPos + 6));
        enable = requestData.substr(enablePos + 8);
    }
    rule[user].emplace_back(address, stoi(port), internet, enable);
    proxy_change(user, address, stoi(port), internet, enable, "add");

    // 执行添加规则操作，例如将规则保存到数据库或配置文件中
    // 假设添加规则操作成功
    // 将规则保存到文件
    ofstream file;
    string proxyFile = "./" + user + "/proxy_rules.txt";
    file.open(proxyFile, ios::app);
    if (file.is_open())
    {
        file << "Address: " << address << ", Port: " << port
             << ", Mode: " << internet << ", Enable: " << enable << endl;
        file.close();
    }
    else
    {
        string response = "HTTP/1.1 500 Internal Server Error\r\n"
                          "Content-Length: 21\r\n"
                          "\r\n"
                          "Error saving rule";
        send(clientSocket, response.c_str(), response.length(), 0);
        return;
    }

    // 返回成功响应
    string response = "HTTP/1.1 200 OK\r\n"
                      "Content-Length: 2\r\n"
                      "\r\n"
                      "OK";
    send(clientSocket, response.c_str(), response.length(), 0);
}

void handleEditRuleRequest(int clientSocket, char *requestBuffer)
{
    // 从请求数据中解析出规则信息
    string requestData(requestBuffer);
    string user, oldaddress, newaddress, oldport, newport, oldinternet, newinternet, oldenable, newenable;

    size_t userPos = requestData.find("Username:");
    size_t oldaddressPos = requestData.find("oldaddress=");
    size_t newaddressPos = requestData.find("&newaddress=");
    size_t oldportPos = requestData.find("&oldport=");
    size_t newportPos = requestData.find("&newport=");
    size_t oldinternetPos = requestData.find("&oldinternet=");
    size_t newinternetPos = requestData.find("&newinternet=");
    size_t oldenablePos = requestData.find("&oldenable=");
    size_t newenablePos = requestData.find("&newenable=");
    size_t newlinePos = requestData.find("\r\n", userPos);

    if (userPos != string::npos && oldaddressPos != string::npos && newaddressPos != string::npos && oldportPos != string::npos && newportPos != string::npos && oldinternetPos != string::npos && newinternetPos != string::npos && oldenablePos != string::npos && newenablePos != string::npos)
    {

        user = requestData.substr(userPos + 9, newlinePos - (userPos + 9));
        oldaddress = requestData.substr(oldaddressPos + 11, newaddressPos - (oldaddressPos + 11));
        newaddress = requestData.substr(newaddressPos + 12, oldportPos - (newaddressPos + 12));
        oldport = requestData.substr(oldportPos + 9, newportPos - (oldportPos + 9));
        newport = requestData.substr(newportPos + 9, oldinternetPos - (newportPos + 9));
        oldinternet = requestData.substr(oldinternetPos + 13, newinternetPos - (oldinternetPos + 13));
        newinternet = requestData.substr(newinternetPos + 13, oldenablePos - (newinternetPos + 13));
        oldenable = requestData.substr(oldenablePos + 11, newenablePos - (oldenablePos + 11));
        newenable = requestData.substr(newenablePos + 11);
    }

    // 修改代理规则文件中的规则
    string fileName = "./" + user + "/proxy_rules.txt";
    cout << fileName << endl;
    ifstream inFile(fileName);
    ofstream outFile("./" + user + "/temp_proxy_rules.txt");

    if (inFile.is_open() && outFile.is_open())
    {
        string line;
        while (getline(inFile, line))
        {
            size_t addressPos = line.find("Address: ");
            size_t portPos = line.find(", Port: ");
            size_t internetPos = line.find(", Mode: ");
            size_t enablePos = line.find(", Enable: ");

            if (addressPos != string::npos && portPos != string::npos && internetPos != string::npos && enablePos != string::npos)
            {

                string fileAddress = line.substr(addressPos + 9, portPos - (addressPos + 9));
                string filePort = line.substr(portPos + 8, internetPos - (portPos + 8));
                string fileInternet = line.substr(internetPos + 8, enablePos - (internetPos + 8));
                string fileEnable = line.substr(enablePos + 10);
                // cerr<<fileAddress<<filePort<<fileInternet<<fileEnable<<endl;
                // cerr<<oldaddress<<oldport<<oldinternet<<oldenable<<endl;
                if (fileAddress == oldaddress && filePort == oldport && fileInternet == oldinternet && fileEnable == oldenable)
                {
                    // 修改规则行
                    line = "Address: " + newaddress + ", Port: " + newport + ", Mode: " + newinternet + ", Enable: " + newenable;
                    //   cerr << "helo" << endl;
                    //   修改规则后写入日志文档
                    proxy_change(user, oldaddress, stoi(oldport), oldinternet, oldenable, "edit_old");
                    proxy_change(user, newaddress, stoi(newport), newinternet, newenable, "edit_new");
                }
            }

            outFile << line << endl;
        }

        inFile.close();
        outFile.close();

        // 删除原始文件，并重命名临时文件为原始文件名
        if (remove(fileName.c_str()) != 0)
        {
            string response = "HTTP/1.1 500 Internal Server Error\r\n"
                              "Content-Length: 24\r\n"
                              "\r\n"
                              "Error deleting rule file";
            send(clientSocket, response.c_str(), response.length(), 0);
            return;
        }
        // else {cerr << "xixi" << endl;}
        if (rename(("./" + user + "/temp_proxy_rules.txt").c_str(), fileName.c_str()) != 0)
        {
            string response = "HTTP/1.1 500 Internal Server Error\r\n"
                              "Content-Length: 21\r\n"
                              "\r\n"
                              "Error renaming rule file";
            send(clientSocket, response.c_str(), response.length(), 0);
            return;
        }
        // else {cerr << "xihu" << endl;}
    }
    else
    {
        string response = "HTTP/1.1 500 Internal Server Error\r\n"
                          "Content-Length: 21\r\n"
                          "\r\n"
                          "Error opening rule file";
        send(clientSocket, response.c_str(), response.length(), 0);
        return;
    }

    // 返回成功响应
    string response = "HTTP/1.1 200 OK\r\n"
                      "Content-Length: 2\r\n"
                      "\r\n"
                      "OK";
    send(clientSocket, response.c_str(), response.length(), 0);
    read(fileName, rule[user]);
}

void handleDeleteRuleRequest(int clientSocket, char *requestBuffer)
{
    // 从请求数据中解析出规则信息
    string requestData(requestBuffer);
    string user, address, port, internet, enable;

    size_t userPos = requestData.find("Username:");
    size_t addressPos = requestData.find("address=");
    size_t portPos = requestData.find("&port=");
    size_t internetPos = requestData.find("&mode=");
    size_t enablePos = requestData.find("&enable=");
    size_t newlinePos = requestData.find("\r\n", userPos);
    if (userPos != string::npos && addressPos != string::npos && portPos != string::npos && internetPos != string::npos && enablePos != string::npos)
    {
        user = requestData.substr(userPos + 9, newlinePos - (userPos + 9));
        address = requestData.substr(addressPos + 8, portPos - (addressPos + 8));
        port = requestData.substr(portPos + 6, internetPos - (portPos + 6));
        internet = requestData.substr(internetPos + 6, enablePos - (internetPos + 6));
        enable = requestData.substr(enablePos + 8);
        // cerr << "hello" << endl;
    }

    string fileName = "./" + user + "/proxy_rules.txt";
    ifstream inFile(fileName);
    ofstream outFile("./" + user + "/temp_proxy_rules.txt");

    if (inFile.is_open() && outFile.is_open())
    {
        string line;
        while (getline(inFile, line))
        {
            size_t addressPos = line.find("Address: ");
            size_t portPos = line.find(", Port: ");
            size_t internetPos = line.find(", Mode: ");
            size_t enablePos = line.find(", Enable: ");

            if (addressPos != string::npos && portPos != string::npos && internetPos != string::npos && enablePos != string::npos)
            {

                string fileAddress = line.substr(addressPos + 9, portPos - (addressPos + 9));
                string filePort = line.substr(portPos + 8, internetPos - (portPos + 8));
                string fileInternet = line.substr(internetPos + 8, enablePos - (internetPos + 8));
                string fileEnable = line.substr(enablePos + 10);
                // cerr<<fileAddress<<filePort<<fileInternet<<fileEnable<<endl;
                // cerr<<oldaddress<<oldport<<oldinternet<<oldenable<<endl;
                if (fileAddress == address && filePort == port && fileInternet == internet && fileEnable == enable)
                {
                    // 修改规则行
                    line = "";
                    // cerr << "helo" << endl;
                    //   修改规则后写入日志文档
                    proxy_change(user, address, stoi(port), internet, enable, "delete");
                }
            }
            if (!line.empty())
                outFile << line << endl;
        }

        inFile.close();
        outFile.close();
        // 删除原始文件，并重命名临时文件为原始文件名
        if (remove(fileName.c_str()) != 0)
        {
            string response = "HTTP/1.1 500 Internal Server Error\r\n"
                              "Content-Length: 24\r\n"
                              "\r\n"
                              "Error deleting rule file";
            send(clientSocket, response.c_str(), response.length(), 0);
            return;
        }
        // else {cerr << "xixi" << endl;}
        if (rename(("./" + user + "/temp_proxy_rules.txt").c_str(), fileName.c_str()) != 0)
        {
            string response = "HTTP/1.1 500 Internal Server Error\r\n"
                              "Content-Length: 21\r\n"
                              "\r\n"
                              "Error renaming rule file";
            send(clientSocket, response.c_str(), response.length(), 0);
            return;
        }
    }
    else
    {
        string response = "HTTP/1.1 500 Internal Server Error\r\n"
                          "Content-Length: 21\r\n"
                          "\r\n"
                          "Error opening rule file";
        send(clientSocket, response.c_str(), response.length(), 0);
        return;
    }
    // 返回成功响应
    string response = "HTTP/1.1 200 OK\r\n"
                      "Content-Length: 2\r\n"
                      "\r\n"
                      "OK";
    send(clientSocket, response.c_str(), response.length(), 0);
    read(fileName, rule[user]);
}

void handleToggleRequest(int clientSocket, char *requestBuffer)
{
    string requestData(requestBuffer);
    string user;
    size_t userPos = requestData.find("Username:");
    size_t newlinePos = requestData.find("\r\n", userPos);
    if (userPos != string::npos)
    {
        user = requestData.substr(userPos + 9, newlinePos - (userPos + 9));
        // cerr << "hello" << endl;
    }
    status[user] = !status[user];
    string response = "HTTP/1.1 200 OK\r\n"
                      "Content-Length: 2\r\n"
                      "\r\n"
                      "OK";
    send(clientSocket, response.c_str(), response.length(), 0);
}