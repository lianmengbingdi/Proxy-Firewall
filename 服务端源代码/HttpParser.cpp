#include <string>
#include <map>
#include <sstream>
#include <iostream>
#include <netdb.h>
#include <cstring>
#include <arpa/inet.h>
#include <sys/types.h>
#include <sys/socket.h>
#include "HttpParser.h"

using namespace std;

HttpParser::HttpParser(char *msg)
{
    string buf(msg);
    istringstream buf_stream(buf);
    enum parts
    {
        start_line,
        headers,
        body
    };
    parts part = start_line;
    string line;
    string body_string;
    while (getline(buf_stream, line))
    {
        switch (part)
        {
        case start_line: // 请求行
        {
            istringstream line_stream(line);
            string tmp;
            line_stream >> tmp;
            if (tmp.find("HTTP") == string::npos)
            {
                http.insert(make_pair("method", tmp));
                method = tmp;
                line_stream >> tmp;

                http.insert(make_pair("url", tmp));
                url = tmp;
                line_stream >> tmp;

                http.insert(make_pair("pro_version", tmp));
                version = tmp;
            }
            else
            {
                http.insert(make_pair("version", tmp));
                version = tmp;
                line_stream >> tmp;

                http.insert(make_pair("status", tmp));
                status_code = tmp;
                line_stream >> tmp;

                http.insert(make_pair("status_text", tmp));
                status = tmp;
            }
            part = headers;
            break;
        }
        case headers: // 请求头部
        {
            if (line.size() == 1)
            {
                part = body;
                break;
            }
            auto pos = line.find(":");
            if (pos == string::npos)
                continue;
            string tmp1(line, 0, pos);
            string tmp2(line, pos + 2);
            http.insert(make_pair(format_key(tmp1), tmp2));
            break;
        }
        case body: // 主体部分
        {
            body_string.append(line);
            body_string.push_back('\n');
            break;
        }
        default:
            break;
        }
    }
    http.insert(make_pair("body", body_string));
}

HttpParser::~HttpParser() {}

void HttpParser::show(char *msg)
{ // 展示http请求报文的所有内容
    cout << msg << endl;
    /*for(auto it = http.cbegin(); it != http.cend(); ++it){
        cout << it->first << ": " << it->second << endl;
    }*/
}

string HttpParser::operator[](string str)
{
    auto it = http.find(format_key(str));

    return it != http.end() ? it->second : "";
}

string HttpParser::format_key(string &str)
{
    if (str[0] >= 'a' && str[0] <= 'z')
    {
        str[0] = str[0] + 'A' - 'a';
    }
    int position = 0;
    while ((position = str.find("-", position)) != string::npos)
    {
        if (str[position + 1] >= 'a' && str[position + 1] <= 'z')
        {
            str[position + 1] = str[position + 1] + 'A' - 'a';
        }
        position++;
    }
    return str;
}

void HttpParser::get_method()
{
    cout << "method:" << method << endl;
}

string HttpParser::get_URL()
{
    return url;
}

void HttpParser::get_version()
{
    cout << "version:" << version << endl;
}

void HttpParser::get_status_code()
{
    cout << "status code:" << status_code << endl;
}

void HttpParser::get_status()
{
    cout << "status:" << status << endl;
}

// 将http请求中的Host解析为ip地址
string extract_dom_host(const string &host) // 从Host中解析出域名并返回一个string类型的变量，例如"www.example.com"解析为"example.com"
{
    size_t pos = host.find('.');
    if (pos == std::string::npos || pos == 0)
    {
        cout << "invalid hostname!" << endl;
        return ""; // 主机名不合法
    }
    return host.substr(pos + 1);
}

string remove_carriage_return(const string &str) // 去掉解析出的string类型Host中的'\r'，返回不包含'\r'的string类型的变量
{
    string result = str;
    size_t pos = result.find('\r');
    while (pos != string::npos)
    {
        result.erase(pos, 1);
        pos = result.find('\r', pos);
    }
    return result;
}

// 将获得的域名解析出ipv4地址，返回一个string类型的变量
// HttpParser http(buf);
// 则函数参数hostname即为http["host"]
string dom_ip(string hostname)
{
    struct addrinfo hints, *res;
    int status;
    char ipstr[INET_ADDRSTRLEN];

    memset(&hints, 0, sizeof(hints));
    hints.ai_family = AF_INET; // 只获取IPv4地址
    hints.ai_socktype = SOCK_STREAM;

    if ((status = getaddrinfo(hostname.c_str(), nullptr, &hints, &res)) != 0)
    {
        std::cerr << "getaddrinfo: " << gai_strerror(status) << std::endl;
        return "";
    }

    struct sockaddr_in *ipv4 = reinterpret_cast<struct sockaddr_in *>(res->ai_addr);
    inet_ntop(res->ai_family, &(ipv4->sin_addr), ipstr, sizeof(ipstr));

    freeaddrinfo(res);
    return std::string(ipstr);
}
// 域名解析示例：HttpParser http(buf);cout << dom_ip(http["host"]) << endl;
