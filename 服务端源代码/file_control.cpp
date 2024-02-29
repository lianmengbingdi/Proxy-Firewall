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
#include "file_control.h"
#include <unordered_map>

using namespace std;

unordered_map<string, vector<tuple<string, string, string, float>>> rule2;


//修改规则后写入日志文档
void file_change(string user, string file_dire, string file_name, string file_type, float file_len, string op){
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
    { outFile << dt << " " << user << " add the file_rule." << "Filedire: " << file_dire << ", Filename: " << file_name << ", Filetype: " << file_type << ", Filelength: " << file_len << endl;}
    if (op == "edit_old")
    { outFile << dt << " " << user << " edit the file_rule." << "Filedire: " << file_dire << ", Filename: " << file_name << ", Filetype: " << file_type << ", Filelength: " << file_len << endl;}
    if (op == "edit_new")
    { outFile << "The file_rule change to " << "Filedire: " << file_dire << ", Filename: " << file_name << ", Filetype: " << file_type << ", Filelength: " << file_len << endl;}
    if (op == "delete")
    { outFile << dt << " " << user << " delete the file_rule." << "Filedire: " << file_dire << ", Filename: " << file_name << ", Filetype: " << file_type << ", Filelength: " << file_len << endl;}
}

void file_read(string Filename, vector<tuple<string, string, string, float>> &rule)
{
    ifstream inputFile(Filename);
    string file_dire, file_name, file_type, file_len;
    if (!inputFile)
    {
        cout << "Failed to open the file_rule file." << endl;
        return;
    }
    rule.clear();
    string line;
    while (getline(inputFile, line))
    {

        size_t direPos = line.find("Filedire: ");
        size_t namePos = line.find(", Filename: ");
        size_t typePos = line.find(", Filetype: ");
        size_t sizePos = line.find(", Filelen: ");

        if (direPos != string::npos && namePos != string::npos && typePos != string::npos && sizePos != string::npos)
        {
            file_dire = line.substr(direPos + 10, namePos - (direPos + 10));
            file_name = line.substr(namePos + 12, typePos - (namePos + 12));
            file_type = line.substr(typePos + 12, sizePos - (typePos + 12));
            file_len = line.substr(sizePos + 11);
        }
        rule.emplace_back(file_dire, file_name, file_type, stof(file_len));
    }

    inputFile.close();
    int i = 0;
    for (const auto &tuple : rule)
    {
        cout << i++;
        cout << "Filedire: " << file_dire << ", Filename: " << file_name << ", Filetype: " << file_type << ", Filelen: " << file_len << endl;
    }
}

void add_file_rule(int clientSocket, char *requestBuffer)
{
    // 从请求数据中解析出规则信息
    string requestData(requestBuffer);
    string user, file_dire, file_name, file_type, file_len;

    size_t userPos = requestData.find("Username:");
    size_t direPos = requestData.find("Filedire=");
    size_t namePos = requestData.find("&Filename=");
    size_t typePos = requestData.find("&Filetype=");
    size_t sizePos = requestData.find("&Filelen=");
    size_t newlinePos = requestData.find("\r\n", userPos);

    if (userPos != string::npos && direPos != string::npos && namePos != string::npos && typePos != string::npos && sizePos != string::npos)
    {
        user = requestData.substr(userPos + 9, newlinePos - (userPos + 9));
        file_dire = requestData.substr(direPos + 9, namePos - (direPos + 9));
        file_name = requestData.substr(namePos + 10, typePos - (namePos + 10));
        file_type = requestData.substr(typePos + 10, sizePos - (typePos + 10));
        file_len = requestData.substr(sizePos + 9);
    }

    rule2[user].emplace_back(file_dire, file_name, file_type, stof(file_len));
    file_change(user,file_dire,file_name,file_type, stof(file_len),"add");

    // 执行添加规则操作，例如将规则保存到数据库或配置文件中
    // 假设添加规则操作成功
    // 将规则保存到文件
    ofstream file;
    string proxyFile = "./" + user + "/file_rules.txt";
    file.open(proxyFile, ios::app);
    if (file.is_open())
    {
        file << "Filedire: " << file_dire << ", Filename: " << file_name << ", Filetype: " << file_type << ", Filelen: " << file_len << endl;
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

void edit_file_rule(int clientSocket, char *requestBuffer)
{
    // 从请求数据中解析出规则信息
    string requestData(requestBuffer);
    string user, old_file_dire, new_file_dire, old_file_name, new_file_name, old_file_type, new_file_type, old_file_len, new_file_len;

    size_t userPos = requestData.find("Username:");
    size_t old_file_dire_pos = requestData.find("&old_file_dire=");
    size_t new_file_dire_pos = requestData.find("&new_file_dire=");
    size_t old_file_name_pos = requestData.find("&old_file_name=");
    size_t new_file_name_pos = requestData.find("&new_file_name=");
    size_t old_file_type_pos = requestData.find("&old_file_type=");
    size_t new_file_type_pos = requestData.find("&new_file_type=");
    size_t old_file_len_pos = requestData.find("&old_file_len=");
    size_t new_file_len_pos = requestData.find("&new_file_len=");
    size_t newlinePos = requestData.find("\r\n", userPos);

    if (userPos != string::npos && old_file_dire_pos != string::npos && new_file_dire_pos != string::npos && old_file_name_pos != string::npos && new_file_name_pos != string::npos && old_file_type_pos != string::npos && new_file_type_pos != string::npos && old_file_len_pos != string::npos && new_file_len_pos != string::npos)
    {
        user = requestData.substr(userPos + 9, old_file_dire_pos - (userPos + 9));
        old_file_dire = requestData.substr(old_file_dire_pos + 15, new_file_dire_pos - (old_file_dire_pos + 15));
        new_file_dire = requestData.substr(new_file_dire_pos + 15, old_file_name_pos - (new_file_dire_pos + 15));
        old_file_name = requestData.substr(old_file_name_pos + 15, new_file_name_pos - (old_file_name_pos + 15));
        new_file_name = requestData.substr(new_file_name_pos + 15, old_file_type_pos - (new_file_name_pos + 15));
        old_file_type = requestData.substr(old_file_type_pos + 15, new_file_type_pos - (old_file_type_pos + 15));
        new_file_type = requestData.substr(new_file_type_pos + 15, old_file_len_pos - (new_file_type_pos + 15));
        old_file_len = requestData.substr(old_file_len_pos + 14, new_file_len_pos - (old_file_len_pos + 14));
        new_file_len = requestData.substr(new_file_len_pos + 14);
    }
    cerr << new_file_len << endl;
    // 修改代理规则文件中的规则
    string fileName = "./" + user + "/file_rules.txt";
    cout << fileName << endl;
    ifstream inFile(fileName);
    ofstream outFile("./" + user + "/temp_file_rules.txt");

    if (inFile.is_open() && outFile.is_open())
    {
        string line;
        while (getline(inFile, line))
        {
            size_t direPos = line.find("Filedire: ");
            size_t namePos = line.find(", Filename: ");
            size_t typePos = line.find(", Filetype: ");
            size_t sizePos = line.find(", Filelen: ");

            if (direPos != string::npos && namePos != string::npos && typePos != string::npos && sizePos != string::npos)
            {

                string file_Dire = line.substr(direPos + 10, namePos - (direPos + 10));
                string file_Name = line.substr(namePos + 12, typePos - (namePos + 12));
                string file_Type = line.substr(typePos + 12, sizePos - (typePos + 12));
                string file_Len = line.substr(sizePos + 11);
                // cerr<<fileAddress<<filePort<<fileInternet<<fileEnable<<endl;
                // cerr<<oldaddress<<oldport<<oldinternet<<oldenable<<endl;
                if (file_Dire == old_file_dire && file_Name == old_file_name && file_Type == old_file_type && file_Len == old_file_len)
                {
                    // 修改规则行
                    line = "Filedire: " + new_file_dire + ", Filename: " + new_file_name + ", Filetype: " + new_file_type + ", Filelen: " + new_file_len;
                    //   cerr << "helo" << endl;
                    //将修改的规则写入日志
                    file_change(user,old_file_dire,old_file_name,old_file_type, stof(old_file_len),"edit_old");
                    file_change(user,new_file_dire,new_file_name,new_file_type, stof(new_file_len),"edit_new");
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
        if (rename(("./" + user + "/temp_file_rules.txt").c_str(), fileName.c_str()) != 0)
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
    file_read(fileName, rule2[user]);
}

void delete_file_rule(int clientSocket, char *requestBuffer)
{
    // 从请求数据中解析出规则信息
    string requestData(requestBuffer);
    string user, file_dire, file_name, file_type, file_len;

    size_t userPos = requestData.find("Username:");
    size_t direPos = requestData.find("&Filedire=");
    size_t namePos = requestData.find("&Filename=");
    size_t typePos = requestData.find("&Filetype=");
    size_t sizePos = requestData.find("&Filelen=");

    if (userPos != string::npos && direPos != string::npos && namePos != string::npos && typePos != string::npos && sizePos != string::npos)
    {
        user = requestData.substr(userPos + 9, direPos - (userPos + 9));
        file_dire = requestData.substr(direPos + 10, namePos - (direPos + 10));
        file_name = requestData.substr(namePos + 10, typePos - (namePos + 10));
        file_type = requestData.substr(typePos + 10, sizePos - (typePos + 10));
        file_len = requestData.substr(sizePos + 9);
        // cerr << "hello" << endl;
    }
    cerr << file_dire << file_name << file_type << file_len << endl;
    string fileName = "./" + user + "/file_rules.txt";
    ifstream inFile(fileName);
    ofstream outFile("./" + user + "/temp_file_rules.txt");

    if (inFile.is_open() && outFile.is_open())
    {
        string line;
        while (getline(inFile, line))
        {
            size_t direPos = line.find("Filedire: ");
            size_t namePos = line.find(", Filename: ");
            size_t typePos = line.find(", Filetype: ");
            size_t sizePos = line.find(", Filelen: ");

            if (direPos != string::npos && namePos != string::npos && typePos != string::npos && sizePos != string::npos)
            {
                string file_Dire = line.substr(direPos + 10, namePos - (direPos + 10));
                string file_Name = line.substr(namePos + 12, typePos - (namePos + 12));
                string file_Type = line.substr(typePos + 12, sizePos - (typePos + 12));
                string file_Size = line.substr(sizePos + 11);
                cerr << file_Dire << file_Name << file_Type << file_Size << endl;
                // cerr<<fileAddress<<filePort<<fileInternet<<fileEnable<<endl;
                // cerr<<oldaddress<<oldport<<oldinternet<<oldenable<<endl;
                if (file_Dire == file_dire && file_Name == file_name && file_Type == file_type && file_Size == file_len)
                {
                    // 修改规则行
                    line = "";
                    cerr << "cjcdsb" << endl;
                    // cerr << "helo" << endl;
                    //将删除的规则写入日志
                    file_change(user,file_dire,file_name,file_type,  stof(file_len),"delete");
                }
            }
            if (!line.empty())
                outFile << line << endl;
        }

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
        if (rename(("./" + user + "/temp_file_rules.txt").c_str(), fileName.c_str()) != 0)
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
    file_read(fileName, rule2[user]);
}

void load_file_rule(int clientSocket, char *requestBuffer)
{
    string requestData(requestBuffer);
    size_t userPos = requestData.find("Username:");
    size_t newlinePos = requestData.find("\r\n", userPos);
    string user;
    user = requestData.substr(userPos + 9, newlinePos - (userPos + 9));
    string myFile = "./" + user + "/file_rules.txt";
    cerr << myFile << endl;
    ifstream file(myFile);
    if (!file)
    {
        cerr << "Failed to open file: " << myFile << endl;
        return;
    }

    stringstream buffer;
    buffer << file.rdbuf();
    string response = buffer.str();
    // cerr<<response;
    send(clientSocket, response.c_str(), response.length(), 0);
    file_read(myFile, rule2[user]);
}