#pragma warning(disable:4996)
#pragma execution_character_set("utf-8")
#include"Engine.h"
#include<cstdlib>
#include<iostream>
#include<algorithm>
#include<cstdio>
#include<QMessageBox>
#include<cstring>
#include<QString>
#include<QDebug>
#include<QTextCodec>

bool Engine::read_courses(const char *courses_file) {
	//memset(courses, 0, sizeof(courses));
	FILE* course_file = fopen(courses_file, "r");
	if (!course_file) {
		//QMessageBox::warning(NULL, "打开文件失败", QString::asprintf("打开文件%s失败"), courses_file);
		return false;
	}
	while (!feof(course_file)) {
		int num, size, cur, start = NaN, end = NaN;
		char title[100], teacher[100], type[20];

		if (fscanf(course_file, "%d\t%s\t%s\t%d\t%d\t%s", &num, &title, &teacher, &size, &cur, &type) <= 0)
			break;

		if (fgetc(course_file) != '\n')
			fscanf(course_file, "%d\t%d", &start, &end);

		if (!courses[num])
			courses[num] = new Course(num, string(title), string(teacher), size, 0, // current number of students = 0 beacause it will be loaded later;
				strcmp(type, "专业课") ? Course::optional : Course::compulsory);
		else {
			printf("发现重复课程，无法录入：\n");
			printf("\t%d\t%s\t%s\n", num, title, teacher);
		}
	}
	fclose(course_file);
	

	return true;
}

bool Engine::read_students() {
	FILE *studentlist=fopen("data/student.txt", "r");
	if (!studentlist) {
		//printf("打开文件%s失败\n", "student.txt");
		QMessageBox::warning(NULL, "打开文件失败", QString::asprintf("打开文件%s失败"), "student.txt");
		return false;
	}

	//print_all_courses();
	students = new map<string, StudentInfo*>;
	while (!feof(studentlist)) {
		char tmp[300],tmp2[300]; string student_name;
		if (fscanf(studentlist, "%[^,],%[^\n]", tmp, tmp2) <= 0)
			break;
		student_name = string(tmp);
		
		strcpy(tmp2, "data/students/");strcat(tmp2, tmp);strcat(tmp2, ".txt");
		//tmp2 为文件名

		int flag;
		StudentInfo *stu_info=new StudentInfo(tmp2, flag);
		if (!flag) {
			QMessageBox::information(NULL, "学生文件", QString::asprintf("文件%s不存在", tmp2));
			continue;
		}
		
		students->insert(pair<string,StudentInfo*>(student_name, stu_info));
		if (fgetc(studentlist) <= 0) break;
	}
	fclose(studentlist);
	
	return true;
}

bool Engine::read_assistants() {
	FILE* assistants_file = fopen("data/assistant.txt", "r");
	if (!assistants_file) {
		//printf("打开文件%s失败\n", "assistant.txt");
		QMessageBox::warning(NULL, "打开文件失败", QString::asprintf("打开文件%s失败"), "assistant.txt");
		return false;
	}

	while (!feof(assistants_file)) {
		int number, i = 0; char nxt = 0;
		char name[300];
		if (fscanf(assistants_file, "%d%c", &number, &nxt) <= 0)
			break;
		while (fscanf(assistants_file, "%[^,\n]", name) >0) {
			if (nxt == '\n')break;
			StudentInfo* stu_info = students->find(string(name))->second;
			stu_info->assist(number);
			if (courses[number])
				courses[number]->add_assistant(string(name));
			if (fscanf(assistants_file, "%c", &nxt) <= 0)break;
		}
	}
	fclose(assistants_file);
	
	map<string, StudentInfo*>::iterator it;
	for (it = students->begin(); it != students->end(); it++) {
		StudentInfo* stu_info = it->second;
		vector<int> course_vec;
		int n = stu_info->get_courseList(course_vec);
		for (int i = 0; i < n; i++) {
			string assistant = stu_info->assistant(course_vec[i]);
			if (courses[course_vec[i]]->isAssistant(assistant))
				courses[course_vec[i]]->join(it->first, assistant);
		}
	}
	return true;
}

bool Engine::write_courses() {
	FILE* course_file =  freopen("data/currentcourse.txt", "w",stdout);
	if (!course_file) {
		printf("打开文件%s失败\n", "currentcourse.txt");
		return false;
	}
	for (int i = 0; i < 1000; i++) {
		if (!courses[i]) continue;
		courses[i]->printCourse();
	}
	fclose(course_file);
	//freopen("CON", "w", stdout);
	return true;
}

bool Engine::write_students(string student_name) {
	string file_name = "data/students/" + student_name + ".txt";
	FILE* studentfile = freopen(file_name.c_str(), "w", stdout);
	StudentInfo* info = students->find(student_name)->second;
	
	vector<int> courselist;
	info->get_courseList(courselist);
	for (int i = 0; i < courselist.size(); i++) 
		printf("%03d\t%s\n", courselist[i], info->assistant(courselist[i]).c_str());
	
	fclose(studentfile);
	//freopen("CON", "w", stdout);
	return true;
}

bool Engine::write_assistant() {
	FILE* assistantfile = freopen("data/assistant.txt", "w", stdout);
	if (!assistantfile) {
		printf("打开文件%s失败\n", "assistant.txt");
		return false;
	}
	for (int i = 0; i < 1000; i++) {
		if (!courses[i]) continue;
		printf("%03d\t", i);
		courses[i]->printCourse(false, true, false);
	}

	fclose(assistantfile);
	//freopen("CON", "w", stdout);
	return true;
}

Engine::Engine(){
	memset(courses, 0, sizeof(courses));
	read_courses("data/currentcourse.txt");
	read_students();
	read_assistants();
	//courses[1]->isStudent("student2");
	//qDebug() << "cc";
}

bool Engine::Archive(string student) {
	bool flag = 1;
	if (!write_courses())return false;
	if (!write_assistant())return false;
	if (student == string("Null")) {
		return true;
		//if (!students->size()) return false;
		/*<string, StudentInfo*>::iterator it;
		for (it = students->begin(); it != students->end(); it++) {
			if (!write_students(it->first)) {
				flag = 0;
				continue;
			}
		}*/
	}
	else if (!write_students(student))return false;
	return flag;
}

Engine::~Engine(){
	Archive();
    for (int i = 0; i < 1000;i++)
        if(courses[i])
            delete courses[i];
	for (auto it = students->begin(); it != students->end(); it++)
		delete it->second;
	delete students;
}

void Engine::print_courses(int mode, QStandardItemModel* model,QPushButton *buttons[],QString name,string abbr){
	string student = name.toStdString();
	StudentInfo* stu;
	if(name!=QString("Admin"))
		stu = students->find(student)->second;

	int cnt = 0;
	for (int i = 0; i < 1000; i++) {
		if (!courses[i]) {
			delete buttons[i];
			buttons[i] = NULL;
			continue;
		}
		else if (mode == 1 && !stu->selected(i)) {
			delete buttons[i];
			buttons[i] = NULL;
			continue;
		}
		else if (abbr != string("") && !courses[i]->abbreviate(abbr)) {
			delete buttons[i];
			buttons[i] = NULL;
			continue;
		}
		else{
			int col = 0;
			model->setItem(cnt, col++, new QStandardItem(QString::asprintf("%03d", courses[i]->get_number())));
			model->setItem(cnt, col++, new QStandardItem(QString::fromStdString(courses[i]->get_title())));
			model->setItem(cnt, col++, new QStandardItem(QString::fromStdString(courses[i]->get_teacher())));
			if (mode == 0) {
				model->setItem(cnt, col++, new QStandardItem(QString::number(courses[i]->get_size())));
				model->setItem(cnt, col++, new QStandardItem(QString::number(courses[i]->get_student_num())));
			}
			model->setItem(cnt, col++, new QStandardItem(QString(courses[i]->get_type() == Course::optional ? "非专业课" : "专业课")));
			if (name != QString("Admin")) {
				if (stu->selected(i)) buttons[i]->setText("退课");
				else buttons[i]->setText("选课");
			}
			else buttons[i]->setText("删除");
			cnt++;
		}
	}
}

int Engine::reprint_course(QStandardItemModel* model, int course_number, int row) {
	if (row >= model->rowCount()||!courses[course_number]) return 0;
	for (int i = 0; i < 6; i++) {
		QStandardItem *item;
		if(item = model->takeItem(row,i))
		delete item;
	}
	model->setItem(row, 0, new QStandardItem(QString::asprintf("%03d",courses[course_number]->get_number())));
	model->setItem(row, 1, new QStandardItem(QString::fromStdString(courses[course_number]->get_title())));
	model->setItem(row, 2, new QStandardItem(QString::fromStdString(courses[course_number]->get_teacher())));
	model->setItem(row, 3, new QStandardItem(QString::number(courses[course_number]->get_size())));
	model->setItem(row, 4, new QStandardItem(QString::number(courses[course_number]->get_student_num())));
	model->setItem(row, 5, new QStandardItem(QString(courses[course_number]->get_type() == Course::optional ? "非专业课" : "专业课")));
	return 1;
}

int Engine::add_course(int course_number, string course_name,
	string teacher, int st_lim,string c_type) {
	Course::course_type type = (c_type == string("专业课") ? Course::compulsory : Course::optional);
	if (course_number == 0)
		while (courses[++course_number])
			if (course_number > 999) return 0;
	if (courses[course_number]) return 0;
	courses[course_number] = new Course(course_number, course_name, teacher, st_lim , 0,type);
	return course_number;
}

int Engine::delete_Course(int course_number) {
	if (!courses[course_number] || courses[course_number]->get_student_num()) return 0;
	delete courses[course_number];
	courses[course_number] = NULL;
	return 1;
}

int Engine::change_course_teacher(int course_number, string teacher) {
	Course* course = courses[course_number];
	course->set_teacher(teacher);
	return 1;
}

int Engine::change_course_size(int course_number, int newSize) {
	Course* course = courses[course_number];
	if (course->get_student_num() > newSize)
		return 0;
	else course->set_size(newSize);
	return 1;
}

int Engine::change_course_type(int course_number, string newType) {
	Course* course = courses[course_number];
	course->set_type(newType == string("optional?") ? Course::optional : Course::compulsory);
	return 1;
}

int Engine::student_choose_assistant(string student, int course_number, string assistant) {
	Course* course = courses[course_number];
	StudentInfo* stu_info = students->find(student)->second;
	if (!course || !stu_info->selected(course_number)) return 0;
	if (!course->isAssistant(assistant)) return -1;
	
	if(course->isAssistant(stu_info->assistant(course_number)))
		course->quit(0,student, stu_info->assistant(course_number));
	course->join(student, assistant);
	stu_info->selectCourse(course_number, assistant);
	
	return 1;//success
}

int Engine::student_become_assistant(string student, int course_number) {
	Course* course = courses[course_number];
	StudentInfo* stu_info = students->find(student)->second;
	if (!course || !stu_info->selected(course_number)) return 0;
	if (!stu_info->assist(course_number)) return -1;
	course->add_assistant(student);
	return 1;
}

int Engine::student_quit_assist(string student, int course_number) {
	Course* course = courses[course_number];
	StudentInfo* stu_info = students->find(student)->second;
	if (!course || !stu_info->selected(course_number)) return 0;
	if(!stu_info->quit_assist(course_number)) return -1;
	course->del_assistant(student);
	return 1;
}

int Engine::cnt_assisting(string student) {
	StudentInfo* info = students->find(student)->second;
	return info->cnt_assisting();
}

int Engine::student_select_course(string student, int course_number) {
	Course* course = courses[course_number];
	StudentInfo* stu_info = students->find(student)->second;
	if (!course) return 0;
	if (course->get_size() == course->get_student_num()) return -1;
	course->join(student);
	stu_info->selectCourse(course_number);
	return 1;
}

int Engine::student_quit_course(string student, int course_number) {
	Course* course = courses[course_number];
	StudentInfo* stu_info = students->find(student)->second;
	if (!course || !stu_info->selected(course_number)) return 0;
	course->quit(1,student, stu_info->assistant(course_number));
	stu_info->quitCourse(course_number);
	return 1;
}

void Engine::show_student_courses(string student) {
	vector<int> courselist;
	StudentInfo* stu_info = students->find(student)->second;
	stu_info->get_courseList(courselist);
	for (int i = 0; i < courselist.size(); i++) {
		courses[courselist[i]]->printCourse();
		cout<<"助教:"<<stu_info->assistant(courselist[i])<<endl;
		putchar('\n');
	}
}

void Engine::show_ten_courses(int &cur) {
	int cnt = 0;
	for (int i = cur; i < 1000; i++,cur++) {
		if (!courses[i]) continue;
		courses[i]->printCourse();
		if (++cnt == 10) return;
	}
	return;
}

int Engine::show_students(int course_number,QStandardItemModel *model) {
	Course* course = courses[course_number];
	if (!course) return 0;
	model->setHorizontalHeaderLabels(QStringList() <<"小组成员");
	QStringList assistants = course->get_assistants();
	QList<QString>::iterator it;

	for (it = assistants.begin(); it != assistants.end(); it++) {
		QList<QStandardItem*> items_assistant;
		QStandardItem* item1 = new QStandardItem(*it);
		items_assistant.append(item1);
		model->appendRow(items_assistant);

		QStringList members = course->get_group(it->toStdString());
		if (members.isEmpty()) continue;
		QList<QString>::iterator it2;
		int j = 0;
		for (it2 = members.begin(); it2 != members.end(); it2++) {
			QList<QStandardItem*> items_member;
			items_member.append(new QStandardItem(QString("├")+(*it2)));
			//qDebug() << QString("├") + (*it2) << endl;
			item1->appendRow(items_member);
		}
	}
	//course->printCourse(true, true, true);
	return 1;
}

int Engine::get_assistants(int course_number,QStringList &list) {
	Course* course = courses[course_number];
	if (!course) return 0;
	list = course->get_assistants();
	return 1;
}

QString Engine::get_assistant(int course_number, QString student_name) {
	StudentInfo* student = students->find(student_name.toStdString())->second;
	return QString::fromStdString(student->assistant(course_number));
}

int Engine::check_course_requirement(string student) {
	vector<int> courselist;
	StudentInfo* info = students->find(student)->second;
	info->get_courseList(courselist);
	
	int cnt_comp = 0, cnt_opt = 0;
	for (int i = 0; i < courselist.size(); i++) {
		if (courses[courselist[i]]->get_type() == Course::optional)
			cnt_opt++;
		else cnt_comp++;
	}
	/*to be check!!!*/
	if (cnt_comp + cnt_opt > 10) return -1;
	if (cnt_opt > 2 && cnt_comp > 4) return 1;
	return 0;
}

int Engine::arrange_exams() {
	FILE* exam = fopen("data/exam.txt", "w");
	if (!exam)
		return 0;

	int cnt = 0,n=0;//n colors
	int *head = new int[1000];
	for (int i = 0; i < 1000; i++)
		head[i] = 0;
	
	struct Node {
		int degree;
		int number;
	}node[1000];
	for (int i = 0; i < 1000; i++) {
		node[i].number = i;
		node[i].degree = 0;
	}
	
	struct Edge {
		int v, nxt;
	}*edge;
	edge = new Edge[20000010] ;
	
	//memset(head, 0, sizeof(head));

	map<string, StudentInfo*>::iterator it;
	for (it = students->begin(); it != students->end(); it++) {
		StudentInfo *student = it->second;
		vector<int> courseList;
		student->get_courseList(courseList);
		for(int i=0;i<courseList.size();i++)
			for (int j = 0; j < i; j++) {
				int u = courseList[i], v = courseList[j];
				edge[++cnt].nxt = head[u];
				edge[cnt].v = v; head[u] = cnt;
				edge[++cnt].nxt = head[v];
				edge[cnt].v = u; head[v] = cnt;
				node[u].degree++; node[v].degree++;
			}
	}

	sort(node, node + 1000, [](Node a, Node b) {return a.degree > b.degree; });
	//int max_color = node[0].degree+1; 
	bool vis[1000];
	int color[1000];
	memset(color, 0, sizeof(color));
	for (int c = 1; c <= node[0].degree + 1; c++) {
		memset(vis, 0, sizeof(vis));
		for (int i = 0; i < 1000; i++) {
			int u = node[i].number;
			if (!courses[u] || vis[u] || color[u]) continue;
			vis[u] = true; color[u] = c;
			for (int ei = head[u]; ei; ei = edge[ei].nxt)
				vis[edge[ei].v] = true;
			n = c > n ? c : n;
		}
	}
	delete []edge;

	//write to file

	for(int i=0;i<1000;i++)
		if (courses[i]) {
			fprintf(exam, "%d\t%d\n", i, color[i]);
		}
	fclose(exam);

	return n;
}

pair<vector<string>, vector<int>> Engine::get_exams(string student) {
	vector<int> examlist;
	vector<string> courselist;
	FILE* exam_file = fopen("data/exam.txt", "r");
	if (!exam_file)
		return pair<vector<string>, vector<int>>(courselist,examlist);
	StudentInfo* stu = students->find(student)->second;

	while (!feof(exam_file)) {
		int course, time;
		if (fscanf(exam_file, "%d\t\%d\n", &course, &time) <= 0)
			break;
		if (stu->selected(course))
			courselist.push_back(courses[course]->get_title()),
			examlist.push_back(time);
	}
	return  pair<vector<string>, vector<int>>(courselist, examlist);
}