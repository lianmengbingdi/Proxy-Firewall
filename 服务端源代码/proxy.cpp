#include <iostream>
#include <pthread.h>
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
#include <unordered_map>
#include <cctype>
#include "account.h"
#include "rule.h"
#include "file_control.h"
#include "file_transfer.h"
#include "HttpParser.h"

using namespace std;

pthread_mutex_t mutex = PTHREAD_MUTEX_INITIALIZER;

struct buff
{
    vector<string> buffer;
    bool flag = false;
    int frequence = 0;
};

unordered_map<string, buff> cache;
extern unordered_map<string, string> cip;
extern unordered_map<string, bool> status;
extern unordered_map<string, vector<tuple<string, int, string, string>>> rule;
extern unordered_map<string, vector<tuple<string, string, string, float>>> rule2;
time_t fileModifiedTime = time(nullptr);

void LFU();
void *handleProxyRequest(void *arg);
void *zero(void *arg);

int main()
{
    printf("\033[34mooooooooo.                                                 oooooooooooo  o8o                                                oooo  oooo\033[0m\n");
    printf("\033[34m`888   `Y88.                                               `888'     `8  `\"'                                                `888  `888\033[0m\n");
    printf("\033[34m 888   .d88' oooo d8b  .ooooo.  oooo    ooo oooo    ooo     888         oooo  oooo d8b  .ooooo.  oooo oooo    ooo  .oooo.    888   888\033[0m\n");
    printf("\033[34m 888ooo88P'  `888\"\"8P d88' `88b  `88b..8P'   `88.  .8'      888oooo8    `888  `888\"\"8P d88' `88b  `88. `88.  .8'  `P  )88b   888   888\033[0m\n");
    printf("\033[34m 888          888     888   888    Y888'      `88..8'       888    \"     888   888     888ooo888   `88..]88..8'    .oP\"888   888   888\033[0m\n");
    printf("\033[34m 888          888     888   888  .o8\"'88b      `888'        888          888   888     888    .o    `888'`888'    d8(  888   888   888\033[0m\n");
    printf("\033[34mo888o        d888b    `Y8bod8P' o88'   888o     .8'        o888o        o888o d888b    `Y8bod8P'     `8'  `8'     `Y888\"\"8o o888o o888o\033[0m\n");
    printf("\033[34m                                            .o..P' \033[0m\n");
    printf("\033[34m                                           `Y8P'    \033[0m\n");
    cerr << "Server started." << endl;

    int listenSocket = socket(AF_INET, SOCK_STREAM, 0);
    if (listenSocket == -1)
    {
        cerr << "Error: Failed to create listen socket." << endl;
        return 1;
    }

    // 设置监听的地址和端口
    struct sockaddr_in serverAddress
    {
    };
    serverAddress.sin_family = AF_INET;
    serverAddress.sin_port = htons(8888); // 设置监听的端口号
    serverAddress.sin_addr.s_addr = INADDR_ANY;

    // 绑定监听套接字
    if (bind(listenSocket, (struct sockaddr *)&serverAddress, sizeof(serverAddress)) == -1)
    {
        cerr << "Error: Failed to bind socket." << endl;
        close(listenSocket);
        return 1;
    }

    // 开始监听
    if (listen(listenSocket, 10) == -1)
    {
        cerr << "Error: Failed to listen on socket." << endl;
        close(listenSocket);
        return 1;
    }

    pthread_t thread0;
    if (pthread_create(&thread0, nullptr, zero, NULL) != 0)
    {
        cerr << "Error: Failed to create thread." << endl;
    }

    while (true)
    {
        struct sockaddr_in clientAddress
        {
        };
        socklen_t clientAddressLength = sizeof(clientAddress);
        int clientSocket = accept(listenSocket, (struct sockaddr *)&clientAddress, &clientAddressLength);
        if (clientSocket == -1)
        {
            cerr << "Error: Failed to accept connection." << endl;
            continue;
        }

        pthread_t threadId;
        if (pthread_create(&threadId, nullptr, handleProxyRequest, &clientSocket) != 0)
        {
            cerr << "Error: Failed to create thread." << endl;
            close(clientSocket);
            continue;
        }
    }

    // 关闭监听套接字
    close(listenSocket);

    return 0;
}

void LFU()
{
    while (cache.size() > 2)
    {
        unordered_map<string, buff>::iterator minElement;
        minElement = cache.begin();
        for (auto it = cache.begin(); it != cache.end(); ++it)
        {
            if (it->second.frequence < minElement->second.frequence)
            {
                minElement = it;
            }
        }
        if (minElement != cache.end())
            cache.erase(minElement);
    }
}

// 处理代理请求的子线程
void *handleProxyRequest(void *arg)
{
    int clientSocket = *(reinterpret_cast<int *>(arg));
    struct sockaddr_in clientAddr;
    socklen_t clientAddrLen = sizeof(clientAddr);
    getpeername(clientSocket, (struct sockaddr *)&clientAddr, &clientAddrLen);
    string clientIP = inet_ntoa(clientAddr.sin_addr);

    // 读取客户端发来的代理请求具体内容
    char buffer[4096];
    bzero(buffer, sizeof(buffer));
    ssize_t bytesRead = recv(clientSocket, buffer, sizeof(buffer), 0);
    cerr << buffer << endl;
    if (bytesRead == -1)
    {
        cerr << "Error: Failed to read client request." << endl;
        close(clientSocket);
        pthread_exit(nullptr);
    }
    // 判断请求类型
    if (strstr(buffer, "username=") != nullptr)
    {
        // 处理用户注册请求
        if (strstr(buffer, "register") != nullptr)
            handleRegisterRequest(clientSocket, buffer);
        if (strstr(buffer, "logout") != nullptr)
            handleLogoutRequest(clientSocket, buffer);
        if (strstr(buffer, "login") != nullptr)
        {
            handleLoginResponse(clientSocket, buffer, clientIP);
        }

        close(clientSocket);
        pthread_exit(nullptr);
    }
    else if (cip.find(clientIP) != cip.end() && strstr(buffer, "Proxy") != nullptr)
    {
        if (strstr(buffer, "addProxyRule") != nullptr)
            handleAddRuleRequest(clientSocket, buffer);
        if (strstr(buffer, "load_Proxy_rules") != nullptr)
            handleloadRequest(clientSocket, buffer);
        if (strstr(buffer, "editProxyRule") != nullptr)
            handleEditRuleRequest(clientSocket, buffer);
        if (strstr(buffer, "deleteProxyRule") != nullptr)
            handleDeleteRuleRequest(clientSocket, buffer);
        if (strstr(buffer, "toggle") != nullptr)
            handleToggleRequest(clientSocket, buffer);
        close(clientSocket);
        pthread_exit(nullptr);
    }
    else if (cip.find(clientIP) != cip.end() && strstr(buffer, "File") != nullptr)
    {
        if (strstr(buffer, "addFileRule") != nullptr)
            add_file_rule(clientSocket, buffer);
        if (strstr(buffer, "load_File_rules") != nullptr)
            load_file_rule(clientSocket, buffer);
        if (strstr(buffer, "editFileRule") != nullptr)
            edit_file_rule(clientSocket, buffer);
        if (strstr(buffer, "deleteFileRule") != nullptr)
            delete_file_rule(clientSocket, buffer);
        close(clientSocket);
        pthread_exit(nullptr);
    }
    else if (cip.find(clientIP) != cip.end() && strstr(buffer, "load log") != nullptr)
    {
        handleLogRequest(clientSocket, buffer);
        close(clientSocket);
        pthread_exit(nullptr);
    }
    else if (cip.find(clientIP) != cip.end() && strstr(buffer, "clear log") != nullptr)
    {
        LogClear(clientSocket, buffer);
        close(clientSocket);
        pthread_exit(nullptr);
    }
    else if (cip.find(clientIP) != cip.end() && strstr(buffer, "exit") != nullptr)
    {
        handleExitRequest(clientSocket, buffer, clientIP);
        close(clientSocket);
        pthread_exit(nullptr);
    }
    else if (cip.find(clientIP) != cip.end())
    {

        // 解析客户端请求中的Host字段
        char *hostStart = strstr(buffer, "Host: ");
        if (hostStart == nullptr)
        {
            cerr << "Error: Failed to parse Host field in client request." << endl;
            close(clientSocket);
            pthread_exit(nullptr);
        }
        hostStart += 6; // Skip "Host: "
        char *hostEnd = strchr(hostStart, '\r');
        if (hostEnd == nullptr)
        {
            cerr << "Error: Failed to parse Host field in client request." << endl;
            close(clientSocket);
            pthread_exit(nullptr);
        }
        *hostEnd = '\0'; // Null-terminate the host string

        // 比对请求的目标服务器的IP地址
        string domain;
        string ip;
        string enable;
        int port;
        domain = "";
        ip = "";
        port = 80;
        string file_dire;
        string file_name;
        string file_type;
        float file_len;
        Filecontrol con;
        int size = sizeof(buffer) / sizeof(char); // 计算数组长度
        string m(buffer, size);                   // 使用构造函数将char数组转换为string类型

        if (!status[cip[clientIP]])  // 判断当前用户是否使用代理
        {
            close(clientSocket);
            pthread_exit(nullptr);
        }

        for (const auto &tuple : rule[cip[clientIP]])  
        {
            cerr << get<0>(tuple) << endl;
            cerr << dom_ip(hostStart) << endl;
            if (get<0>(tuple).compare(dom_ip(hostStart)) == 0)
            {
                domain = hostStart;
                ip = get<0>(tuple);
                port = get<1>(tuple);
                enable = get<3>(tuple);
                break;
            }
        }

        for (const auto &tuple : rule2[cip[clientIP]])
        {
            file_dire = get<0>(tuple);
            file_name = get<1>(tuple);
            file_type = get<2>(tuple);
            file_len = get<3>(tuple);
            if (file_name != "")
            {
                if (!con.detect_name(m, file_name))
                {
                    cerr << "Error: Unexpected file name." << endl;
                    close(clientSocket);
                    pthread_exit(nullptr);
                }
            }
            // 检测传输的文件方向是否符合要求
            if (file_dire != "")
            {
                if (file_dire == con.detect_dire(m))
                {
                    cerr << "Error: Unexpected file direction." << endl;
                    close(clientSocket);
                    pthread_exit(nullptr);
                }
            }
            // 检测传输的文件类型是否符合要求
            if (file_type != "")
            {
                if (!con.detect_type(m, file_type))
                {
                    cerr << "Error: Unexpected file type." << endl;
                    close(clientSocket);
                    pthread_exit(nullptr);
                }
            }
            // 检测传输的文件大小是否符合要求
            if (file_len != -1)
            {
                if (!con.detect_len(m, file_len))
                {
                    cerr << "Error: Unexpected file size." << endl;
                    close(clientSocket);
                    pthread_exit(nullptr);
                }
            }
        }
        
        if (domain == "")
        {
            cerr << hostStart << endl;
            cerr << "Error: Unexpected server domain name." << endl;
            close(clientSocket);
            pthread_exit(nullptr);
        }

        if (enable == "false")
        {
            cerr << "Error: Please enable." << endl;
            close(clientSocket);
            pthread_exit(nullptr);
        }

        *hostEnd = '\r';

        

        // 判断缓存是否存在
        pthread_mutex_lock(&mutex);

        bool if_keywords1 = false;

        if (cache.find(buffer) != cache.end())
        {
            cache[buffer].frequence++;
            if (cache[buffer].flag == 1)
            {
               
                for (size_t i = 0; i < cache[buffer].buffer.size(); i++)
                {
                    string cachedResponse = cache[buffer].buffer[i];
                    size_t position1;

                    // 从配置文件中读取关键词列表
                    ifstream configFile("keywords.txt");
                    if (configFile.is_open())
                    {
                        string keyword1;
                        while (getline(configFile, keyword1))
                        {
                            // cerr<<keyword1<<endl;
                            position1 = cachedResponse.find(keyword1);
                            // 检查关键词是否存在于服务器响应中
                            if (position1 != string::npos)
                            {
                                char nextChar = cachedResponse[position1 + keyword1.length()];
                                char lastChar = cachedResponse[position1 - 1];
                                if (!isalpha(nextChar) && !isalpha(lastChar))
                                {
                                    cerr << "Error: Found keyword '" << keyword1 << "' in server response." << endl;
                                    if_keywords1 = true;
                                }
                                break;
                            }
                        }
                        configFile.close();
                    }
                    else
                    {
                        cerr << "Error: Failed to open configuration file." << endl;
                        break;
                    }

                    if (if_keywords1)
                        break;

                    if (send(clientSocket, cachedResponse.c_str(), cachedResponse.size(), 0) == -1)
                    {
                        cerr << "Error: Failed to send cached response to client." << endl;
                        pthread_mutex_unlock(&mutex);
                        close(clientSocket);
                        pthread_exit(nullptr);
                    }
                    cerr << "Cache used!" << endl;
                }
                pthread_mutex_unlock(&mutex);
                close(clientSocket);
                pthread_exit(nullptr);
            }
        }
        if (pthread_mutex_trylock(&mutex))
            pthread_mutex_unlock(&mutex);
        // 与远程HTTP服务器建立连接
        int serverSocket = socket(AF_INET, SOCK_STREAM, 0);
        if (serverSocket == -1)
        {
            cerr << "Error: Failed to create server socket." << endl;
            close(clientSocket);
            pthread_exit(nullptr);
        }

        // 设置远程HTTP服务器的地址
        struct sockaddr_in serverAddress
        {
        };
        serverAddress.sin_family = AF_INET;
        serverAddress.sin_port = htons(port); // 假设HTTP服务器使用80端口
        serverAddress.sin_addr.s_addr = inet_addr(ip.c_str());

        // 连接远程HTTP服务器
        if (connect(serverSocket, (struct sockaddr *)&serverAddress, sizeof(serverAddress)) == -1)
        {
            cerr << "Error: Failed to connect to server." << endl;
            close(clientSocket);
            close(serverSocket);
            pthread_exit(nullptr);
        }

        // 将客户端请求内容发送给远程HTTP服务器
        if (send(serverSocket, buffer, bytesRead, 0) == -1)
        {
            cerr << "Error: Failed to send request to server." << endl;
            close(clientSocket);
            close(serverSocket);
            pthread_exit(nullptr);
        }

        // 从远程HTTP服务器接收响应，并转发给客户端
        bool flag0 = false;
        pthread_mutex_lock(&mutex);
        if (cache.find(buffer) == cache.end())
            flag0 = true;
        cerr << cache.size() << endl;
        if (flag0)
            LFU();
        pthread_mutex_unlock(&mutex);

        fd_set readSet;
        FD_ZERO(&readSet);
        FD_SET(serverSocket, &readSet);
        timeval timeout;
        timeout.tv_sec = 1;
        timeout.tv_usec = 0;

        char cbuffer[4096];
        bool if_keywords = false;

        while (1)
        {
            int ready = select(serverSocket + 1, &readSet, nullptr, nullptr, &timeout);
            int bytesRead = 0;
            if (ready > 0)
            {
                if (FD_ISSET(serverSocket, &readSet))
                {
                    bytesRead = read(serverSocket, cbuffer, sizeof(cbuffer));
                    if (bytesRead <= 0)
                        // cerr<<"bytesRead <=0"<<endl;
                        break;
                }
                string response(cbuffer, bytesRead);

                if (flag0)
                {
                    pthread_mutex_lock(&mutex);
                    cache[buffer].buffer.push_back(response);
                    pthread_mutex_unlock(&mutex);
                }

                size_t position;

                // cerr<<"here is read!"<<endl;
                // cerr<<response0<<endl;
                // cerr<<"response end!"<<endl;

                // 从配置文件中读取关键词列表
                ifstream configFile("keywords.txt");
                if (configFile.is_open())
                {
                    string keyword;
                    while (getline(configFile, keyword))
                    {
                        // cerr<<keyword<<endl;
                        position = response.find(keyword);
                        // 检查关键词是否存在于服务器响应中
                        if (position != string::npos)
                        {
                            char nextChar = response[position + keyword.length()];
                            char lastChar = response[position - 1];
                            if (!isalpha(nextChar) && !isalpha(lastChar))
                            {
                                cerr << "Error: Found keyword '" << keyword << "' in server response." << endl;
                                if_keywords = true;
                            }
                            break;
                        }
                    }
                    configFile.close();
                }
                else
                {
                    cerr << "Error: Failed to open configuration file." << endl;
                    break;
                }

                if (if_keywords)
                    break;

                send(clientSocket, cbuffer, bytesRead, MSG_NOSIGNAL);
            }
            else
                break;
        }

        pthread_mutex_lock(&mutex);
        if (flag0)
            cache[buffer].flag = 1;
        pthread_mutex_unlock(&mutex);

        // 关闭套接字
        close(clientSocket);
        close(serverSocket);
        pthread_exit(nullptr);
    }
    else 
    {
        close(clientSocket);
        pthread_exit(nullptr);
    }
}

void *zero(void *arg)
{
    while (1)
    {
        sleep(60);
        pthread_mutex_lock(&mutex);
        for (auto it = cache.begin(); it != cache.end(); ++it)
        {
            it->second.frequence = 0;
        }
        pthread_mutex_unlock(&mutex);
    }
    pthread_exit(nullptr);
}
