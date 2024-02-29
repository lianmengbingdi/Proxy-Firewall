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



using namespace std;



int mode1(string keyword);
int mode2(string keyword);
int mode3(string keyword, string new_keyword);
int mode4();
bool search_user_name(string keyword);
//bool search_password(string user_name, string password);
//bool searchMode(string domain, int port, string mode);
void write2();


vector<tuple<string>> keywords;
string Filename="../keywords.txt";
//string User="Xu Yufei";





int main(int argc, char *argv[]) {
    int opt;
    string operation="add";
    string new_keyword;
    string keyword;
    //string password, default_password="password";
    ifstream inputFile(Filename);

    if (!inputFile) {
        cout << "Failed to open the file." << endl;
        return -1;
    }

    string line;
    while (getline(inputFile, keyword)) {  
        keywords.emplace_back(keyword);   
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
    while ((opt = getopt(argc, argv, "n:k:o:")) != EOF) {
        flag=true;
        switch (opt) {
            case 'n':
                new_keyword = optarg;
                break;
            case 'k':
                keyword = optarg;
                break;
            case 'o':
                operation= optarg;
                break;
            default:
                printf("Usage: %s \n-o add -k <keyword> ---add a keyword\n -o delete -k <keyword> ---delete a keyword\n -o modify -k <keyword>, -n <new_keyword> ---change keyword\n -o list ---list all keywords\n", argv[0]);
                return -1;
        }
    }

    if (!flag) {
        printf("Usage: %s \n-o add -k <keyword> ---add a keyword\n -o delete -k <keyword> ---delete a keyword\n -o modify -k <keyword>, -n <new_keyword> ---change keyword\n -o list ---list all keywords\n", argv[0]);
        return -1;
    }
    if (operation=="add"){
        mode1(keyword);
    }
    else if (operation=="delete"){
        //if (user_name == "0") user_name = default_name;
        mode2(keyword);
    }
    else if (operation=="modify"){
        //cout<<keyword<<endl;
        //if (user_name == "0") user_name = default_name;
        //if (password == "0") password = default_password;
        mode3(keyword,new_keyword);
    }

    else if (operation=="list"){
        mode4();
    }
    else {
        printf("Wrong Parameter.\n");
        printf("The parameter for \"-o\" can only be \"add\", \"delete\", \"modify\", \"list\".\n");
        return -1;
    }
    return 0;

}


//添加
int mode1(string keyword){
    if (search_user_name(keyword)){
        cout<<"keyword already exists!\n";
        return -1;
    }

    if (keyword==""){
        cout<< "Please enter a keyword!\n";
        return -1;
    }

    keywords.push_back(make_tuple(keyword));
    int fd = open(Filename.c_str(), O_WRONLY | O_CREAT, 0644);//获取fd
    ofstream file(Filename, ios::app);  
    flock(fd,LOCK_EX);
    if (file.is_open()) {
        file.seekp(0, ios::end);
        if (!keywords.empty()) {
            tuple<string>& lastElement = keywords.back();
            file <<get<0>(lastElement) << endl;
        } 
        file.close();
        cout<<"Done!\n";
    } 

    else {
        cerr << "Fail to open the file.\n";
        return -1;
    }
    flock(fd,LOCK_UN);
    return 0;

}

//删除
int mode2(string keyword){
    bool flag;
    flag=false;
    
    for (auto it = keywords.begin(); it != keywords.end(); ++it) {
        string keyword0 = get<0>(*it);
        //string password0 = get<1>(*it);
        if (keyword0 == keyword) {
            keywords.erase(it);
            it = keywords.begin();
            flag=true;
        }   
    }

    if (!flag) cout<<"Not found!\n";
    else {
        write2();
        //rule_record(User,"delete",mode,domain,address,port,"a");
        cout<<"Done!\n";

    }

    return 0;

}

//修改
int mode3(string keyword, string new_keyword){
    for (auto it = keywords.begin(); it != keywords.end(); ++it) {
        string keyword0 = get<0>(*it);
        //cout<<keyword<<endl;
        if (keyword0 == keyword) {
            if (new_keyword==keyword0){
                cout<<"No changes!\n";
                return 0;
            }
            else {
                //rule_record(User,"modify",mode,domain,get<1>(*it),port,get<3>(*it));
                get<0>(*it) = new_keyword;
                write2();
                cout<<"Done!\n";
                return 0;
            }
        }   
    }
    cout<<"keyword is not existed!\n";
    return -1;

}


//展示
int mode4(){
    bool flag = false;
    for (auto it = keywords.begin(); it != keywords.end(); ++it) {
        cout << get<0>(*it) << endl;
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



bool search_user_name(string keyword){
    for (const auto& element : keywords) {
        string keyword0 = get<0>(element);
        if (keyword0 == keyword) {
            return true;
            break;
        }
    }
    return false;
}


void write2(){
    int fd = open(Filename.c_str(), O_WRONLY | O_CREAT, 0644);//获取fd
    ofstream outputFile(Filename); 
    flock(fd,LOCK_EX);

    if (!outputFile) {
        cout << "Failed to open the file." << endl;
        return;
    }
    for (const auto& tuple : keywords) {
        outputFile << get<0>(tuple) << endl;
    }
    flock(fd,LOCK_UN);
    outputFile.close();

}