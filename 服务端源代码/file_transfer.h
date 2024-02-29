#ifndef _transfer_h
#define _transfer_h

#include <iostream>
#include <string>
#include <regex>

using namespace std;

class Filecontrol
{
public:
    bool detect_name(string message, string filename);
    string detect_dire(string message);
    bool detect_len(string message, float length1);
    bool detect_type(string message, string filetype);
    Filecontrol(){};
    ~Filecontrol(){};
};

#endif