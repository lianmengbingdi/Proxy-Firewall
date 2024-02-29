#include <iostream>
#include <string>
#include <regex>
#include "file_transfer.h"
using namespace std;

bool Filecontrol::detect_name(string message, string filename)
{
    /*cout << "Please input the prohibited name of the file:" << endl;
    cin >> filename;*/
    filename += ".";
    // 在报文中搜索文件名
    if (message.find(filename) != string::npos)
    {
        cout << "Prohibited file name appears!" << endl;
        return false;
    }
    else
    {
        cout << "Prohibited file name does not appear." << endl;
        return true;
    }
}

// 检测文件传输方向
string Filecontrol::detect_dire(string message)
{
    string symbol1 = "Content-Type: multipart";
    string symbol2 = "Content-Length";
    string symbol3 = "Content-Disposition";
    string method1 = "GET";
    string method2 = "POST";
    // 检测的标志以及转换为正则表达式
    regex pattern1(symbol1);
    regex pattern2(symbol2);
    regex pattern3(symbol3);
    regex meth_pat1(method1);
    regex meth_pat2(method2);

    smatch result;

    if (regex_search(message, result, meth_pat1))
    {
        message = result.suffix(); // 匹配剩下的字符串
        if (regex_search(message, result, pattern3))
        {
            cout << "The method is download." << endl;
            string mode = "download";
            return mode;
        }
    }

    if (regex_search(message, result, meth_pat2))
    {
        message = result.suffix();
        if (regex_search(message, result, pattern1))
        {
            cout << "The method is upload." << endl;
            string mode = "upload";
            return mode;
        }
    }
    return "";
}

// 检测文件的长度
bool Filecontrol::detect_len(string message, float length1)
{
    // 用户输入最大的文件长度
    float max_leng;
    max_leng = length1 * 1024;

    // 找到content-length的位置，并将其后的内容存入result中
    smatch result;
    regex lengthPattern("Content-Length: (\\d+)");
    if (regex_search(message, result, lengthPattern))
    {
        // result的第二个元素即为文件长度，并进行类型转化
        string lengthStr = result[1];
        int length = stoi(lengthStr);

        if (length < max_leng)
        {
            cout << "The length of the file matches the standard." << endl;
            return true;
        }
        else
        {
            cout << "The length of the file does not match the standard." << endl;
            return false;
        }
    }
    else
    {
        cout << "Content-Length not found in the message." << endl;
        return true;
    }
}

// 检测文件的类型
bool Filecontrol::detect_type(string message, string filetype)
{
    filetype.insert(0, ".");

    if (message.find(filetype) != string::npos)
    {
        cout << "Prohibited file type appears!" << endl;
        return false;
    }
    else
    {
        cout << "Prohibited file type does not appear." << endl;
        return true;
    }
}