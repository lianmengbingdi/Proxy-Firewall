#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string>
#include <iostream>
#include <fstream>
#include <sstream>
#include <tuple>
#include <vector>
#include <ctime>
#include <fcntl.h>
#include <sys/file.h>
#include <functional>
#include <cstring>
#include "openssl/sha.h"
#include <sys/stat.h>  
#include <sys/types.h>



using namespace std;



int mode1(string user_name, string password);
int mode2(string user_name);
int mode3(string user_name, string password);
int mode4(string user_name, string password);
string pas_sha256(const string str);
bool search_user_name(string user_name);
//bool search_password(string user_name, string password);
//bool searchMode(string domain, int port, string mode);
void write();


vector<tuple<string, string>> account;
string Filename="../account.txt";
string User="Xu Yufei";





int main(int argc, char *argv[]) {
    int opt;
    string operation="add";
    string user_name ="null", default_name="User";
    string password, default_password="password";
    ifstream inputFile(Filename);

    if (!inputFile) {
        cout << "Failed to open the file." << endl;
        return -1;
    }

    string line;
    while (getline(inputFile, line)) {
        istringstream iss(line);
        string user_name,password;
        string U_name, Pas_wrd;
        if (iss >> U_name >> user_name >> Pas_wrd >> password) {
            if (U_name == "Username:" && Pas_wrd=="Password:"){
            user_name=user_name.substr(0, user_name.size() - 1);
            account.emplace_back(user_name, password);
            }
            else cout<<"Wrong File Content!"<<endl;
        }
    }

    inputFile.close();

/* 
需要的参数：
-m 选择模式 HTTP FTP
-o 操作
-a 地址
-p 端口号
*/ 
    bool flag=false;
    while ((opt = getopt(argc, argv, "u:p:o:")) != EOF) {
        flag=true;
        switch (opt) {
            case 'p':
                password = optarg;
                password = pas_sha256(password);
                break;
            case 'u':
                user_name = optarg;
                break;
            case 'o':
                operation= optarg;
                break;
            default:
                printf("Usage: %s \n-o add -u <user_name>, -p <password> ---add a account\n -o delete -u <user_name> ---delete a account\n -o modify -u <user_name>, -p <new_password> ---change password\n -o list ---list all accounts\n", argv[0]);
                return -1;
        }
    }

    if (!flag) {
        printf("Usage: %s \n -o add -u <user_name>, -p <password> ---add a account\n -o delete -u <user_name> ---delete a account\n -o modify -u <user_name>, -p <new_password> ---change password\n -o list ---list all accounts\n", argv[0]);
        return -1;
    }
    if (operation=="add"){
        //if (user_name == "") user_name = default_name;
        if (password == "") password = default_password;
        mode1(user_name, password);
    }
    else if (operation=="delete"){
        if (user_name == "0") user_name = default_name;
        mode2(user_name);
    }
    else if (operation=="modify"){
        if (user_name == "0") user_name = default_name;
        if (password == "0") password = default_password;
        mode3(user_name, password);
    }

    else if (operation=="list"){
        mode4(user_name, password);
    }
    else {
        printf("Wrong Parameter.\n");
        printf("The parameter for \"-o\" can only be \"add\", \"delete\", \"modify\", \"list\".\n");
        return -1;
    }
    return 0;

}


//增加
int mode1(string user_name, string password){
    if (search_user_name(user_name)){
        cout<<"Account already exists!\n";
        return -1;
    }

    if (user_name=="null"){
        cout<< "Please enter a Username!\n";
        return -1;
    }

    account.push_back(make_tuple(user_name, password));
    int fd = open(Filename.c_str(), O_WRONLY | O_CREAT, 0644);//获取fd
    ofstream file(Filename, ios::app);  
    flock(fd,LOCK_EX);
    if (file.is_open()) {
        file.seekp(0, ios::end);
        if (!account.empty()) {
            tuple<string, string>& lastElement = account.back();
            file << "Username: " <<get<0>(lastElement) << ", Password: " << get<1>(lastElement) << endl;
        } 
        file.close();
        cout<<"Done!\n";
    } 

    else {
        cerr << "Fail to open the file.\n";
        return -1;
    }
    flock(fd,LOCK_UN);

    string path = "../"+user_name;
    string path1 = "../"+user_name+"/file_rules.txt";
    string path2 = "../"+user_name+"/proxy_rules.txt";
    string path3 = "../"+user_name+"/proxylog.txt";
    mkdir(path.c_str(),0777);
    ofstream file1(path1);
    ofstream file2(path2);
    ofstream file3(path3);
    if (!file1.is_open() || !file2.is_open() || !file3.is_open()) 
    {
        cerr<<"fail to create files!"<<endl;
        return 0;
    }
    file1.close();  
    file2.close();  
    file3.close();

    return 0;

}

//删除
int mode2(string user_name){
    bool flag;
    flag=false;
    
    for (auto it = account.begin(); it != account.end(); ++it) {
        string user_name0 = get<0>(*it);
        //string password0 = get<1>(*it);
        if (user_name0 == user_name) {
            account.erase(it);
            it = account.begin();
            flag=true;
        }   
    }

    if (!flag) cout<<"Not found!\n";
    else {
        write();
        //rule_record(User,"delete",mode,domain,address,port,"a");
        string command="rm -rf ../" + user_name;
        system(command.c_str());
        cout<<"Done!\n";

    }

    

    return 0;

}

//修改
int mode3(string user_name, string password){
    for (auto it = account.begin(); it != account.end(); ++it) {
        string user_name0 = get<0>(*it);
        if (user_name0 == user_name) {
            if (password==get<1>(*it)){
                cout<<"No changes!\n";
                return 0;
            }
            else {
                //rule_record(User,"modify",mode,domain,get<1>(*it),port,get<3>(*it));
                get<1>(*it) = password;
                write();
                cout<<"Done!\n";
                return 0;
            }
        }   
    }
    cout<<"Username is not existed!\n";
    return -1;

}


//展示
int mode4(string user_name, string password){
    bool flag = false;
    for (auto it = account.begin(); it != account.end(); ++it) {
        cout << "Username: " << get<0>(*it) << ", password: " << get<1>(*it) << endl;
        /*string user_name0 = get<0>(*it);
        if (user_name0 == user_name) {
            flag = true;    
        }   */
    }

    /*if (!flag) {
        cout<<"Not found!\n";
        return -1;
    }
    else {
        //rule_record(User,"list",mode,domain,address,port,"a");
        cout<<"Done!\n";
        return 0;
    }*/
    return 0;
}



bool search_user_name(string user_name){
    for (const auto& element : account) {
        string user_name0 = get<0>(element);
        if (user_name0 == user_name) {
            return true;
            break;
        }
    }
    return false;
}






//生成SHA-256哈希密码
string pas_sha256(const string str)
{
    char buf[2];
    unsigned char hash[SHA256_DIGEST_LENGTH];
    SHA256_CTX sha256;
    SHA256_Init(&sha256);
    SHA256_Update(&sha256, str.c_str(), str.size());
    SHA256_Final(hash, &sha256);
    std::string NewString = "";
    for(int i = 0; i < SHA256_DIGEST_LENGTH; i++)
    {
        sprintf(buf,"%02x",hash[i]);
        NewString = NewString + buf;
    }
        return NewString;
}



void write(){
    int fd = open(Filename.c_str(), O_WRONLY | O_CREAT, 0644);//获取fd
    ofstream outputFile(Filename); 
    flock(fd,LOCK_EX);

    if (!outputFile) {
        cout << "Failed to open the file." << endl;
        return;
    }
    for (const auto& tuple : account) {
        outputFile << "Username: " << get<0>(tuple) << ", Password: " << get<1>(tuple)<< endl;
    }
    flock(fd,LOCK_UN);
    outputFile.close();

}