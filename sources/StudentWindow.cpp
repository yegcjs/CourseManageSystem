#include "StudentWindow.h"
#include "StartWindow.h"
#pragma warning(disable:26444)
#include <QDebug>
#include <QStandardItemModel>
#include<QMessagebox>
#include <Qtimer>
#include<QGridLayout>
#include<QCombobox>
#include<QToolBar>
#pragma execution_character_set("utf-8")
StudentWindow::StudentWindow(QString user,Engine *ENGINE,QWidget *parent)
	: myName(user),engine(ENGINE),QWidget(parent)
{
	ui.setupUi(this);
	setFixedSize(800,500);
	QToolBar* tools = new QToolBar(this);
	
	myCourseAction = new QAction("我的课程");
	connect(myCourseAction,&QAction::triggered, this, &StudentWindow::show_my_courses);
	tools->addAction(myCourseAction);
	
	allCourseAction = new QAction("所有课程");
	connect(allCourseAction, &QAction::triggered, this, &StudentWindow::show_all_courses);
	tools->addAction(allCourseAction);

	examAction = new QAction("考试安排");
	connect(examAction, &QAction::triggered, this, &StudentWindow::show_exams);
	tools->addAction(examAction);

	LogOutAction = new QAction("登出", this); 
	connect(LogOutAction, &QAction::triggered, [=]() {
		//engine->Archive(myName.toStdString());
		emit(logout(this));
		});
	tools->addAction(LogOutAction);
	
	CourseTable = NULL; CourseTableModel = NULL; searchButton = NULL; searchEdit = NULL;
	Courselayout = NULL;


}

void StudentWindow::show_all_courses() {
	if (Courselayout) {
		delete Courselayout;
		Courselayout = NULL;
	}
	QStringList title;
	title << "课程号" << "课程名" << "老师" << "限额" << "已选" << "类型" << "操作";
	show_courses(0,title);

	if (!searchButton) {
		searchEdit = new QLineEdit(this); 
		searchButton = new QPushButton("搜索",this); 
	}
	connect(searchButton, &QPushButton::clicked, [=]() {
		if (CourseTableModel) {
			delete CourseTableModel;
			CourseTableModel = NULL;
		}
		CourseTableModel = new QStandardItemModel(CourseTable);
		CourseTable->setModel(CourseTableModel);
		CourseTableModel->setHorizontalHeaderLabels(title);
		static QPushButton* abbrButtons[1000];
		for (int i = 0; i < 1000; i++){
			if (abbrButtons[i]) delete abbrButtons[i];
			abbrButtons[i] = new QPushButton(CourseTable);
		}
		engine->print_courses(0, CourseTableModel, abbrButtons, myName, searchEdit->text().toStdString());
		int cnt = 0;
		for (int i = 0; i < 1000; i++) {
			if (!abbrButtons[i]) continue;
			CourseTable->setIndexWidget(CourseTableModel->index(cnt, CourseTableModel->columnCount() - 1), abbrButtons[i]);
			connect(abbrButtons[i], &QPushButton::clicked, [=]() {
				if (abbrButtons[i]->text() == QString("选课")) {
					if (engine->student_select_course(myName.toStdString(), i) > 0) { //>0 means success
						engine->reprint_course(CourseTableModel, i, cnt);
						QMessageBox::information(NULL, "选课", "选课成功");
						abbrButtons[i]->setText("退课");
					}
				}
				else {
					if (engine->student_quit_course(myName.toStdString(), i) > 0) {
						engine->reprint_course(CourseTableModel, i, cnt);
						QMessageBox::information(NULL, "退课", "退课成功");
						abbrButtons[i]->setText("选课");
					}
				}
				});
			cnt++;
		}
		});
	
	if (Courselayout)
		delete Courselayout;
	Courselayout = new QGridLayout(this);
	Courselayout->setVerticalSpacing(15);
	Courselayout->addItem(new QSpacerItem(0,10),0,7,1);
	Courselayout->addWidget(searchEdit, 2, 0, 1, 3);
	Courselayout->addWidget(searchButton, 2, 3, 1, 1);
	Courselayout->addWidget(CourseTable, 3, 0,10,7);
}

void StudentWindow::show_my_courses() {
	if (Courselayout) {
		delete Courselayout;
		Courselayout = NULL;
	}
	if (searchButton&&searchEdit) {
		delete searchButton;
		searchButton = NULL;
		delete searchEdit;
		searchEdit = NULL;
	}
	QStringList title;
	vector<int> courselist;

	title << "课程号" << "课程名" << "老师"  << "类型" <<"我的助教"<<"成为助教"<<"退选";
	show_courses(1,title, &courselist);
	addAssistantWidgets(courselist);
	if (Courselayout)
		delete Courselayout;
	Courselayout = new QGridLayout(this);
	Courselayout->setVerticalSpacing(15);
	Courselayout->addItem(new QSpacerItem(0, 10), 0, 7, 1);
	Courselayout->addWidget(searchEdit, 2, 0, 1, 3);
	Courselayout->addWidget(searchButton, 2, 3, 1, 1);
	Courselayout->addWidget(CourseTable, 3, 0, 10, 7);
	
	int require_code = engine->check_course_requirement(myName.toStdString());
	if (require_code == 0)
		QMessageBox::information(NULL, "选课", "您选的课程不足，请至少选择4门专业课，2门非专业课");
	else if(require_code==-1)
		QMessageBox::information(NULL, "选课", "您选的课程超过10门，请考虑退课");
		
}

void StudentWindow::addAssistantWidgets(vector<int> &courselist) {
	QPushButton* btn[1000];
	memset(btn, 0, sizeof(btn));

	for (int i = 0; i < courselist.size(); i++) {
		QString current_assistant = engine->get_assistant(courselist[i], myName);
		QStringList assistants;
		engine->get_assistants(courselist[i], assistants);
		int cnt = assistants.count();
		assistants.removeOne(myName);

		QComboBox* combo = new QComboBox(CourseTable);
		combo->addItems(assistants);
		combo->setCurrentText(current_assistant);
		if (engine->student_choose_assistant(myName.toStdString(), courselist[i], current_assistant.toStdString()) < 0) {
			QMessageBox::information(NULL, "请注意", QString::asprintf("您的课程%d的助教%s已退课，请重新选择助教", courselist[i], current_assistant));
			engine->student_choose_assistant(myName.toStdString(), courselist[i], string("Null"));
		}

		CourseTable->setIndexWidget(CourseTableModel->index(i, CourseTableModel->columnCount() - 3), combo);
		connect(combo, &QComboBox::currentTextChanged, [=](QString newAssistant) {
			if (engine->student_choose_assistant(myName.toStdString(), courselist[i], newAssistant.toStdString()) > 0)
				QMessageBox::information(NULL, "更换助教", "更换助教成功");
			else {
				QMessageBox::information(NULL, "更换助教", "更换助教失败");
				combo->setCurrentText(current_assistant);
			}
			});

		btn[i] = new QPushButton(CourseTable);
		CourseTable->setIndexWidget(CourseTableModel->index(i, CourseTableModel->columnCount() - 2), btn[i]);
		if (assistants.count() < cnt) //after removeOne(myName)
			btn[i]->setText("取消");
		else
			btn[i]->setText("成为助教");

	}
	QTimer* timer = new QTimer(CourseTable);
	timer->start(10);
	connect(timer, &QTimer::timeout, [=]() {
		for (int i = 0; i < courselist.size(); i++) {
			if (btn[i]->text() != QString("取消")) {
				if (engine->cnt_assisting(myName.toStdString()) >= 2)
					btn[i]->hide();
				else btn[i]->show();
			}
		}
		timer->stop();
		}
	);

	for (int i = 0; i < courselist.size(); i++) {
		connect(btn[i], &QPushButton::clicked, [=]() {
			timer->start(10);
			if (btn[i]->text() == QString("取消")) {
				engine->student_quit_assist(myName.toStdString(), courselist[i]);
				btn[i]->setText("成为助教");
				QMessageBox::information(NULL, "取消成为助教", QString::asprintf("取消成为课程 %03d 的助教成功", courselist[i]));
			}
			else {
				if (engine->student_become_assistant(myName.toStdString(), courselist[i]) <= 0)
					return;
				btn[i]->setText("取消");
				QMessageBox::information(NULL, "成为助教", QString::asprintf("您已成为课程 %03d 的助教", courselist[i]));
			}
			});
	}
}

void StudentWindow::show_courses(int mode,QStringList &title,vector<int> *courselist) {
	if (CourseTable) {
		delete CourseTable;
		CourseTable = NULL;
	}

	CourseTable = new QTableView(this);
	
	QPushButton* selectButtons[1000];
	for (int i = 0; i < 1000; i++)
		selectButtons[i] = new QPushButton(CourseTable);
	CourseTableModel = new QStandardItemModel(CourseTable);
	CourseTable->setModel(CourseTableModel);
	CourseTableModel->setHorizontalHeaderLabels(title);
	engine->print_courses(mode, CourseTableModel, selectButtons,myName);
	
	int cnt = 0;
	for (int i = 0; i < 1000; i++) {
		if (!selectButtons[i]) continue;
		if (courselist)
			courselist->push_back(i);
		CourseTable->setIndexWidget(CourseTableModel->index(cnt, CourseTableModel->columnCount()-1), selectButtons[i]);
		connect(selectButtons[i], &QPushButton::clicked, [=]() {
			if (selectButtons[i]->text() == QString("选课")) {
				if (engine->student_select_course(myName.toStdString(), i) > 0) { //>0 means success
					if(mode==0) engine->reprint_course(CourseTableModel, i, cnt);
					QMessageBox::information(NULL, "选课", "选课成功");
					selectButtons[i]->setText("退课");
				}
			}
			else {
				if (engine->student_quit_course(myName.toStdString(), i) > 0) {
					if (mode == 0) engine->reprint_course(CourseTableModel, i, cnt);
					QMessageBox::information(NULL, "退课", "退课成功");
					selectButtons[i]->setText("选课");	
				}
			}
			});
		cnt++;
	}
	
}

void StudentWindow::closeEvent(QCloseEvent* ev) {
	engine->Archive(myName.toStdString());
}

void StudentWindow::show_exams() {
	//QMessageBox::information(NULL, "未开放", "没有考试！");
	pair<vector<string>, vector<int>> exam = engine->get_exams(myName.toStdString());
	if (!exam.first.size()) 
		QMessageBox::information(NULL, "考试信息", "暂无考试");
	else {
		QDialog examDialog;
		QGridLayout layout(&examDialog);
		layout.addWidget(new QLabel("课程号", &examDialog), 0, 0);
		layout.addWidget(new QLabel("考试场次", &examDialog), 0, 1);
		
		for (int i = 0; i < exam.first.size(); i++) {
			layout.addWidget(new QLabel(QString::asprintf("%s", exam.first[i].c_str()), &examDialog), i + 1, 0);
			layout.addWidget(new QLabel(QString::asprintf("%02d", exam.second[i]), &examDialog), i + 1, 1);
		}
		examDialog.exec();
	}
}

StudentWindow::~StudentWindow()
{
}
