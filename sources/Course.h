#pragma once
#include<set>
#include<map>
#include<QStringList>
#include<string>
#define NaN -1
using namespace std; 

class Course {
public:
    enum course_type
    {
        compulsory,
        optional
    };
public:
    //build course and init with the first class
    Course(int course_number, string course_name,
           string teacher_name, int st_lim, int cur_st, course_type c_type);

    //if show_assistants=true, show all the assistants of the class
    //if show_group_menbers=true, show all the members in every group
	void printCourse(bool show_info = true, bool show_assistants = false, bool show_group_members = false);
    
    const int get_number() const;

	string get_title()const;

	string get_teacher()const;

    int get_size() const;

    int get_student_num() const;

    course_type get_type() const;

	QStringList get_assistants();

	QStringList get_group(string assistant);

	//judge whether "name" is the student of this course
    bool isStudent(string name);

    //judge whether "name" is the assistant of this course
    bool isAssistant(string name);

    //judge whether abbr is the abbreviate of the course title
    bool abbreviate(string abbr);

    //reset course teacher
    void set_teacher(string newTeacher);
    
    //reset course size, 
	//if cur_stu>newSize, return -1
    int set_size(int newSize);
    
    //reset type
    void set_type(course_type newType);

    //add a group, whose assistant is 'assistant_name'
    void add_assistant(string assistant_name);

    //student join group whose assistant is assistant_name
    void join(string student, string assistant_name = "Null");
	
	//student quit group whose assistant is assistant_name
	//mode:0 quit group, 1:quit course
	void quit(int mode,string student, string assistant_name = "Null");

	//0:not found this assistant, 1:success
	void del_assistant(string assistant_name);
private:
    const int number; // the number for the coures(unique);
    
    string title;
    string teacher;
    course_type type;//compulsory course or optional course
    
    int stu_lim,stu_cur; // the limit of students selecting this class, and the current number of student selecting
    map<string, set<string>*> groups; // <Assistant, group members>
};
