#ifndef _Rule_h
#define _Rule_h

#include <string>
#include <iostream>
#include <tuple>
#include <vector>
#include <unordered_map>

using namespace std;

void read(string Filename, vector<tuple<string, int, string, string>> &rule);
void handleAddRuleRequest(int clientSocket, char *requestBuffer);
void handleEditRuleRequest(int clientSocket, char *requestBuffer);
void handleDeleteRuleRequest(int clientSocket, char *requestBuffer);
void handleToggleRequest(int clientSocket, char *requestBuffer);


#endif