#ifndef _file_control_h
#define _file_control_h

#include <string>
#include <iostream>
#include <tuple>
#include <vector>
#include <unordered_map>
using namespace std;

void file_read(string Filename, vector<tuple<string, string, string, float>> &rule);

void add_file_rule(int clientSocket, char *requestBuffer);

void edit_file_rule(int clientSocket, char *requestBuffer);

void delete_file_rule(int clientSocket, char *requestBuffer);

void load_file_rule(int clientSocket, char *requestBuffer);

#endif