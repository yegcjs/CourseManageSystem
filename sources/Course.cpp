#include"Course.h"
#include<iostream>
#pragma execution_character_set("utf-8")

Course::Course(int course_number, string course_name,
               string teacher_name, int st_lim, int cur_st, course_type c_type)
    :number(course_number), title(course_name),
    teacher(teacher_name), stu_lim(st_lim), stu_cur(cur_st),type(c_type){
    set<string> *tmp=new set<string>;
    groups.insert(pair<string,set<string>*>(string("Null"), tmp));
}
//cur_st::bug
void Course::printCourse(bool show_info,bool show_assistants,bool show_group_members){
	if (show_info) {
		printf("%03d", number);
		printf("\t%s\t%s\t%d\t%d", title.c_str(), teacher.c_str(), stu_lim, stu_cur);	
		//cout << '\t' << title << '\t' << teacher << '\t' << stu_lim << '\t' << stu_cur;
		//printf("%5d\t%20s\t%5s\t%5d\t%5d", number, title, teacher, stu_lim, stu_cur)
		if (type == compulsory)
			printf("\t专业课\n");
		else
			printf("\t非专业课\n");
	}

    if(show_assistants){
        map<string,set<string>*>::iterator it;
		int i = 0;
        for (it = groups.begin(); it != groups.end();it++){
			i++;
			if (!show_group_members && it->first == string("Null")) continue;
			if (!show_group_members && i>2) putchar(',');
			printf("%s",it->first.c_str());
            if(show_group_members){
				putchar('\n');
                set<string>::iterator iter;
                set<string> *members = it->second;
				for (iter = members->begin(); iter != members->end(); iter++)
					printf("   %s\n", (*iter).c_str());
            }
        }
    }
    putchar('\n');
}

const int Course::get_number() const {
	return number;
}

string Course::get_title()const {
	return title;
}

string Course::get_teacher() const {
	return teacher;
}


int Course::get_size()const {
	return stu_lim;
}

int Course::get_student_num() const {
	return stu_cur;
}

QStringList Course::get_assistants() {
	QStringList assistants;
	map<string, set<string>*>::iterator it;
	for (it = groups.begin(); it != groups.end(); it++)
		assistants<<QString::fromStdString(it->first);
	return assistants;
}

QStringList Course::get_group(string assistant) {
	QStringList memberList;
	if(groups.find(assistant)==groups.end()) return memberList;
	set<string>* members = groups.find(assistant)->second;
	set<string>::iterator it;
	for (it = members->begin(); it != members->end(); it++)
		memberList << QString::fromStdString(*it);
	return memberList;
}

Course::course_type Course::get_type() const {
	return type;
}

bool Course::isStudent(string name) {
	map<string, set<string>*>::iterator it;
	for (it = groups.begin(); it != groups.end(); it++) {
		if (it->second->count(name))
			return true;
	}
	return false;
}

bool Course::isAssistant(string name) {
	int x = groups.count(name);
	return (groups.count(name));
}

bool Course::abbreviate(string abbr) {
	int k = 0;
	for (int i = 0; i < title.length(); i++) {
		if (title[i] == abbr[k])
			if (++k == abbr.length()) return true;
	}
	return false;
}

void Course::set_teacher(string newTeacher) {
	teacher = newTeacher;
}


int Course::set_size(int newSize) {
	if (newSize < stu_cur)
		return -1;
	stu_lim = newSize;
	return 0;
}

void Course::set_type(Course::course_type newType) {
	type = newType;
}

void Course::add_assistant(string assistant_name) {
	if (groups.count(assistant_name)) return;
	set<string> *tmp=new set<string>;
	groups.insert(pair<string, set<string>*>(assistant_name, tmp));
}


void Course::join(string student, string assistant_name) {
	//cout << "check!" << endl;
	set<string>* members = groups.find(assistant_name)->second;
	members->insert(student);
	stu_cur++;
}

void Course::quit(int mode,string student, string assistant_name) {
	set<string>* members = groups.find(assistant_name)->second;
	members->erase(student);
	if (isAssistant(student)&&mode==1) {//0:quit group, 1:quit course
		delete groups.find(student)->second;
		groups.erase(student);
	}
	stu_cur--;
}

void Course::del_assistant(string assistant) {
	if (isAssistant(assistant)) {
		set<string>* members = groups.find(assistant)->second;
		delete members;
		groups.erase(assistant);
	}
}