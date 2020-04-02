#include "Start.h"
#include "SHA256.h"
#pragma execution_character_set("utf-8")
#include<fstream>
#include<QMessagebox>
#include<cctype>
Start::Start() {
	existing_users = new map<string, string>;
	fstream read;
	read.open("data/student.txt", ios::in);
	if (!read) {
		QMessageBox::information(NULL, "打开文件失败", "找不到student.txt");
		return;
	}
	while (read.peek() != EOF) {
		char username[256], password[256];
		read.getline(username, 255, ',');
		read.getline(password, 255);
		existing_users->insert(pair<string, string>(string(username), string(password)));
	}
	read.close();
}

Start::~Start() {
	delete existing_users;
}

int Start::Login(int mode, QString username, QString password) {
	string user = username.toStdString(), psw = password.toStdString();
	if (mode == 1) 
		return (user == string("Admin") && psw == string("admin"));
	if (existing_users->count(user)) 
		return(sha256(psw) == existing_users->find(user)->second);
	return 0;
}

int Start::SignUp(QString username, QString password, QString confirm) {
	string user = username.toStdString(), psw = password.toStdString(), cfm = confirm.toStdString();
	if (!check_username(user)) return 1;
	if (existing_users->count(user)) return 2;
	if (psw != cfm) return 3;
	
	fstream print("data/student.txt", ios::app);
	if (!print) {
		QMessageBox::information(NULL, "打开文件失败", "找不到student.txt");
		return -1;
	}
	psw = sha256(psw);
	print << user << ',' << psw << endl;
	print.close();
	
	existing_users->insert(pair<string, string>(user, psw));
	return 0;
}

bool Start::check_username(string username) {
	for(int i=0;i<username.size();i++)
		if(!isdigit(username[i]) && !islower(username[i]) && !isupper(username[i]))
				return 0;
		return 1;
}