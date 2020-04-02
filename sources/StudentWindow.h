#pragma once
#include <QWidget>
#include <QString>
#include <QTableview>
#include <QPushButton>
#include "Engine.h"
#include<QGridLayout>
#include "ui_StudentWindow.h"
#include "QEvent"
class StudentWindow : public QWidget
{
	Q_OBJECT

public:
	StudentWindow(QString user,Engine *engine,QWidget *parent = Q_NULLPTR);
	~StudentWindow();
	void show_courses(int mode,QStringList &title,vector<int> *courselist=NULL);
	void show_all_courses();
	void show_my_courses();
	void show_exams();
	void addAssistantWidgets(vector<int>& courselist);
	void closeEvent(QCloseEvent* ev);
signals:
	void logout(QWidget* widget);
private:
	Ui::StudentWindow ui;
	Engine* engine;
	QString myName;
	QPushButton *searchButton;
	QAction* myCourseAction, * allCourseAction, * examAction, * LogOutAction;
	QLineEdit* searchEdit;
	QTableView* CourseTable;
	QStandardItemModel* CourseTableModel;
	QGridLayout* Courselayout;

};
