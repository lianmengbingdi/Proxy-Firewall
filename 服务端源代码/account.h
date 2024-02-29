#ifndef _Accont_h
#define _Account_h

#include <string>
#include <iostream>
#include <tuple>
#include <vector>
#include <unordered_map>

using namespace std;

bool userExists(const string &username);

void writeLog(const string &path, const string &response);

string getCurrentTimestamp();

string formatLogMessage(const string &timestamp, const string &level, const string &message, const string &user);
string createRegistrationLog(const string &user);
string createLoginLog(const string &user);
string createLogoutLog(const string &user);
string createExitLog(const string &user);

bool userRight(const string &username, const string &password);

void handleRegisterRequest(int clientSocket, char *requestBuffer);
void handleExitRequest(int clientSocket, char *requestBuffer, string clientIP);

void removeUser(const string &username, const string &password);

void handleLogoutRequest(int clientSocket, char *requestBuffer);
void handleLoginResponse(int clientSocket, char *requestBuffer, string clientIP);
void handleloadRequest(int clientSocket, char *requestBuffer);
void handleLogRequest(int clientSocket, char *requestBuffer);

void LogClear(int clientSocket, char *requestBuffer);



#endif