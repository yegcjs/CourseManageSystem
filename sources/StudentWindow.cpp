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
	
	myCourseAction = new QAction("�ҵĿγ�");
	connect(myCourseAction,&QAction::triggered, this, &StudentWindow::show_my_courses);
	tools->addAction(myCourseAction);
	
	allCourseAction = new QAction("���пγ�");
	connect(allCourseAction, &QAction::triggered, this, &StudentWindow::show_all_courses);
	tools->addAction(allCourseAction);

	examAction = new QAction("���԰���");
	connect(examAction, &QAction::triggered, this, &StudentWindow::show_exams);
	tools->addAction(examAction);

	LogOutAction = new QAction("�ǳ�", this); 
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
	title << "�γ̺�" << "�γ���" << "��ʦ" << "�޶�" << "��ѡ" << "����" << "����";
	show_courses(0,title);

	if (!searchButton) {
		searchEdit = new QLineEdit(this); 
		searchButton = new QPushButton("����",this); 
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
				if (abbrButtons[i]->text() == QString("ѡ��")) {
					if (engine->student_select_course(myName.toStdString(), i) > 0) { //>0 means success
						engine->reprint_course(CourseTableModel, i, cnt);
						QMessageBox::information(NULL, "ѡ��", "ѡ�γɹ�");
						abbrButtons[i]->setText("�˿�");
					}
				}
				else {
					if (engine->student_quit_course(myName.toStdString(), i) > 0) {
						engine->reprint_course(CourseTableModel, i, cnt);
						QMessageBox::information(NULL, "�˿�", "�˿γɹ�");
						abbrButtons[i]->setText("ѡ��");
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

	title << "�γ̺�" << "�γ���" << "��ʦ"  << "����" <<"�ҵ�����"<<"��Ϊ����"<<"��ѡ";
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
		QMessageBox::information(NULL, "ѡ��", "��ѡ�Ŀγ̲��㣬������ѡ��4��רҵ�Σ�2�ŷ�רҵ��");
	else if(require_code==-1)
		QMessageBox::information(NULL, "ѡ��", "��ѡ�Ŀγ̳���10�ţ��뿼���˿�");
		
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
			QMessageBox::information(NULL, "��ע��", QString::asprintf("���Ŀγ�%d������%s���˿Σ�������ѡ������", courselist[i], current_assistant));
			engine->student_choose_assistant(myName.toStdString(), courselist[i], string("Null"));
		}

		CourseTable->setIndexWidget(CourseTableModel->index(i, CourseTableModel->columnCount() - 3), combo);
		connect(combo, &QComboBox::currentTextChanged, [=](QString newAssistant) {
			if (engine->student_choose_assistant(myName.toStdString(), courselist[i], newAssistant.toStdString()) > 0)
				QMessageBox::information(NULL, "��������", "�������̳ɹ�");
			else {
				QMessageBox::information(NULL, "��������", "��������ʧ��");
				combo->setCurrentText(current_assistant);
			}
			});

		btn[i] = new QPushButton(CourseTable);
		CourseTable->setIndexWidget(CourseTableModel->index(i, CourseTableModel->columnCount() - 2), btn[i]);
		if (assistants.count() < cnt) //after removeOne(myName)
			btn[i]->setText("ȡ��");
		else
			btn[i]->setText("��Ϊ����");

	}
	QTimer* timer = new QTimer(CourseTable);
	timer->start(10);
	connect(timer, &QTimer::timeout, [=]() {
		for (int i = 0; i < courselist.size(); i++) {
			if (btn[i]->text() != QString("ȡ��")) {
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
			if (btn[i]->text() == QString("ȡ��")) {
				engine->student_quit_assist(myName.toStdString(), courselist[i]);
				btn[i]->setText("��Ϊ����");
				QMessageBox::information(NULL, "ȡ����Ϊ����", QString::asprintf("ȡ����Ϊ�γ� %03d �����̳ɹ�", courselist[i]));
			}
			else {
				if (engine->student_become_assistant(myName.toStdString(), courselist[i]) <= 0)
					return;
				btn[i]->setText("ȡ��");
				QMessageBox::information(NULL, "��Ϊ����", QString::asprintf("���ѳ�Ϊ�γ� %03d ������", courselist[i]));
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
			if (selectButtons[i]->text() == QString("ѡ��")) {
				if (engine->student_select_course(myName.toStdString(), i) > 0) { //>0 means success
					if(mode==0) engine->reprint_course(CourseTableModel, i, cnt);
					QMessageBox::information(NULL, "ѡ��", "ѡ�γɹ�");
					selectButtons[i]->setText("�˿�");
				}
			}
			else {
				if (engine->student_quit_course(myName.toStdString(), i) > 0) {
					if (mode == 0) engine->reprint_course(CourseTableModel, i, cnt);
					QMessageBox::information(NULL, "�˿�", "�˿γɹ�");
					selectButtons[i]->setText("ѡ��");	
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
	//QMessageBox::information(NULL, "δ����", "û�п��ԣ�");
	pair<vector<string>, vector<int>> exam = engine->get_exams(myName.toStdString());
	if (!exam.first.size()) 
		QMessageBox::information(NULL, "������Ϣ", "���޿���");
	else {
		QDialog examDialog;
		QGridLayout layout(&examDialog);
		layout.addWidget(new QLabel("�γ̺�", &examDialog), 0, 0);
		layout.addWidget(new QLabel("���Գ���", &examDialog), 0, 1);
		
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
