#pragma once
#define NaN -1
#include<string>
#include<map>
#include<vector>
using namespace std;
class StudentInfo{
public:
    //input info and show messages
    StudentInfo(char *file,int &flag);

    //judge whether I have selected the course
    bool selected(int course_number);

	//assist,
	bool assist(int course_number);

	bool quit_assist(int course_number);

    //judge whether I am the assiatant
    bool isAssistant(int course_number);

	//return how many courses are the student assisting
	int StudentInfo::cnt_assisting();

    //select a course
    void selectCourse(int course_number,string assisstant=string("Null"));

	void quitCourse(int coures_number);

    //my assistant in course = course_number
    string assistant(int course_number);

    //get my courses, return the number of the courses 
    int get_courseList(vector<int> &courseList);
	
private:
    map<int, string> courses; // <course_number,assistant_name>
    int assisting_courses[2];//the courses where I am an assistant
};