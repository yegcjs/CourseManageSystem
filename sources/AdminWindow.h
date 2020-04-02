#pragma once

#include <QWidget>
#include<QPushButton>
#include<QTreeview>
#include<QTableview>
#include<QGridLayout>
#include<QLineEdit>
#include<QStandardItemModel>
#include<QItemSelectionModel>
#include"Engine.h"
#include "ui_AdminWindow.h"
class Tree;
class AdminWindow : public QWidget
{
	Q_OBJECT

public:
	AdminWindow(Engine *ENGINE,QWidget *parent = Q_NULLPTR);
	~AdminWindow();
	void show_courses();
	void show_students(QStandardItemModel *model,int course_number);
	void set_up_Courses(string abbr = string(""));
	void addCourse();
	void closeEvent(QCloseEvent* ev);
signals:
	void logout(QWidget* widget);
private:
	Ui::AdminWindow ui;
	Engine* engine;
	QAction* coursesAction, * fromFileAction, * LogOutAction, * examAction;
	QPushButton* searchButton;
	QLineEdit* searchEdit;
	QStandardItemModel* CourseTableModel,*groupTreeModel;
	QItemSelectionModel* currentModel;
	QTableView* courseTable;
	QTreeView* groupTree;
	QGridLayout* courseLayout;
	Tree* idxTree;
};


class Tree {
public:
	Tree() {
		memset(val, 0, sizeof(val));
	}
	void addOne(int x) {
		for (int i = x; i < 1001; i += (x & (-x)))
			val[i]++;
	}
	void minusOne(int x) {
		for (int i = x; i < 1001; i += (x & (-x)))
			val[i]--;
	}
	int getVal(int x) {
		int s = 0;
		for (int i = x; i; i -= (x & (-x)))
			s += val[i];
		return s;
	}
private:
	int val[1001];
};