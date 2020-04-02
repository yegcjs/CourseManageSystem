#pragma once
#include<map>
#include<string>
#include<QString>
using namespace std;

class Start {
public:
	Start();
	~Start();
	//0:succeed, 1:fail
	int Login(int mode,QString username,QString password);
	// 0:succes, 1:username_illegal, 2:username_userd, 3:password!=confirm
	int SignUp(QString username, QString password, QString confirm);
private:
	map<string, string>* existing_users;
	//1:legel, 0:illegal
	bool check_username(string username);
};