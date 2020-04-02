#pragma once
#include "StudentInfo.h"
#include "Course.h"
#include<map>
#include<QTableView>
#include<QPushButton>
#include<QStandardItemModel>
using namespace std;
class Engine{
public:
    Engine();
    ~Engine();
	bool Archive(string student=string("Null"));

    int add_course(int course_number,string course_name,
        string teacher,int st_lim,string c_type);

    int delete_Course(int course_number);

    //option = 0: teacher, 1: size, 2:time, 3:type
	int change_course_teacher(int course_number, string teacher);
	int change_course_size(int course_number, int newSize);
	int change_course_type(int course_number, string newType);

    //search_course with abbreviation, return the number of courses found
    int search_course(string abbr);

    //student choose assistant in the course
	//0: no this course, -1 assistant is not the assistant of the course
    int student_choose_assistant(string student, int course_number, string assisant);
	
    //student become the assistant of the course
	//0: no this course, -1 assisting more than 2
    int student_become_assistant(string student, int course_number);

	//0: no this course, -1 not assisting
	int student_quit_assist(string student, int course_number);

	//return how many courses student is assisting
	int cnt_assisting(string student);

    //student selects a course
	//0: no this course, -1 course is max
    int student_select_course(string student, int course_number);

    //student quits a course
	//0 no this course or not selected
    int student_quit_course(string strudent, int course_number);

    //show the students all courses
    void show_student_courses(string student);

    // at most 10 courses a group, cur is where to start counting
    void show_ten_courses(int &cur);

    //show all the students who has select the course
	//0:not found this course
    int show_students(int course_number,QStandardItemModel *model);

    //show all assistants of the course
    int get_assistants(int course_number,QStringList &list);
	
	//get student's assistant of course_number;
	QString get_assistant(int course_number, QString student_name);

    //check that if the student have selected enough courses
	//0 not enough, 1 OK, -1 too many
    int check_course_requirement(string student);
	//mode 0:allCourse, 1:myCourse,
    void print_courses(int mode,QStandardItemModel *model,QPushButton *buttons[],QString name,string abbr=string(""));

	//return 0: no this course, 1:success
	int reprint_course(QStandardItemModel* model, int course_number,int row);

	//return the number of exam sections
	int arrange_exams();
	pair<vector<string>, vector<int>> get_exams(string student);
	bool read_courses(const char* courses_file);
	bool read_students();
	bool read_assistants();
	bool write_assistant();
	bool write_courses();
	bool write_students(string student_name);
private:
    Course *courses[1000];
    //map<string, StudentInfo*> *students;//student_name,StudentInfo
	map<string, StudentInfo*>* students;
	
};