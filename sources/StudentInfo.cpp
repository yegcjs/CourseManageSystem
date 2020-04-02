#pragma warning(disable:4996)
#include"StudentInfo.h"
#pragma execution_character_set("utf-8")
#include<cstdlib>
StudentInfo::StudentInfo(char *file,int &flag) {
	FILE* input = fopen(file, "r");
	if (!input) {
		printf("打开文件%s失败\n", file);
		flag = 0;
		return;
	}
	while (!feof(input)) {
		int num; char tmp_assistant[300];
		if(fscanf(input, "%d\t%s", &num, tmp_assistant)<=0)
			break;
		string assistant=string(tmp_assistant);
		courses.insert(pair<int, string>(num, assistant));
	}
	assisting_courses[0] = assisting_courses[1] = NaN;
	flag = 1;
	fclose(input);
}

bool StudentInfo::selected(int course_number) {
	return courses.find(course_number) != courses.end();
}

bool StudentInfo::assist(int course_number) {
	if (assisting_courses[0] == NaN) {
		assisting_courses[0] = course_number;
		return true;
	}
	else if (assisting_courses[1] == NaN) {
		assisting_courses[1] = course_number;
		return true;
	}
	return false;
}

bool StudentInfo::quit_assist(int course_number) {
	if (assisting_courses[0] == course_number) {
		assisting_courses[0] = NaN;
		return true;
	}
	else if (assisting_courses[1] == course_number) {
		assisting_courses[1] = NaN;
		return true;
	}
	return false;
}

int StudentInfo::cnt_assisting() {
	if ((assisting_courses[0] != NaN) && (assisting_courses[1] != NaN))
		return 2;
	if ((assisting_courses[0] != NaN) || (assisting_courses[1] != NaN))
		return 1;
	return 0;
}

bool StudentInfo::isAssistant(int course_number) {
	return (assisting_courses[0] == course_number || assisting_courses[1] == course_number);
}

void StudentInfo::selectCourse(int course_number,string assistant) {
	courses[course_number] = assistant;
}

void StudentInfo::quitCourse(int course_number) {
	quit_assist(course_number);
	courses.erase(course_number);
}

string StudentInfo::assistant(int course_number) {
	return courses.find(course_number)->second;
}

int StudentInfo::get_courseList(vector<int>& course_list) {
	map<int, string>::iterator it;
	course_list.clear();
	for (it = courses.begin(); it != courses.end(); it++)
		course_list.push_back(it->first);
	return courses.size();
}