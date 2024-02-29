#ifndef _http_parser_h
#define _http_parser_h

#include <string>
#include <map>
#include <sstream>
#include <iostream>
#include <cstring>
#include <sys/types.h>

using namespace std;

class HttpParser
{
private:
    map<string, string> http;
    string format_key(string &str);
    string method;
    string url;
    string version;
    string status_code;
    string status;

public:
    HttpParser(char *buf); // 解析函数
    ~HttpParser();
    void show(char *msg); // 展示http请求报文的所有内容
    string operator[](string str);
    void get_method();  // 获取请求方法
    string get_URL();   // 获取URL
    void get_version(); // 获取协议及版本
    void get_status_code();
    void get_status();
};

// 将http请求中的Host解析为ip地址
string extract_dom_host(const string &host);

string remove_carriage_return(const string &str);

string dom_ip(string hostname);
#endif