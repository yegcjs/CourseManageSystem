#pragma warning(disable: 26812)
#pragma warning(disable: 26444)
#include "AdminWindow.h"
#include<QDebug>
#include<QInputDialog>
#include<QDialogButtonBox>
#include<QMessageBox>
#include<QDialog>
#include<QFileDialog>
#include<QFormLayout>
#include<QComboBox>
#include<QLabel>
#include<QSpinbox>
#include<QToolbar>
#pragma execution_character_set("utf-8")

AdminWindow::AdminWindow(Engine* ENGINE,QWidget *parent)
	: engine(ENGINE),QWidget(parent)
{
	ui.setupUi(this);
	QToolBar *tools = new QToolBar(this);
	setFixedSize(1010, 400);
	
	coursesAction = new QAction("课程/刷新", this);
	connect(coursesAction, &QAction::triggered,this,&AdminWindow::show_courses);
	tools->addAction(coursesAction);

	fromFileAction = new QAction("从文件读入",this);
	connect(fromFileAction, &QAction::triggered, [=]() {
		QString file_name = QFileDialog::getOpenFileName(this, "OpenFile", "/data", ("Text(*.txt)"));
		engine->read_courses(file_name.toStdString().c_str());
		});
	tools->addAction(fromFileAction);

	examAction = new QAction("安排考试", this);
	connect(examAction, &QAction::triggered, [this]() {
		int n_exams = engine->arrange_exams();
		if (!n_exams)
			QMessageBox::information(NULL, "安排考试", "安排失败");
		else QMessageBox::information(NULL, "安排考试", QString::asprintf("安排成功，最少%d个场次", n_exams));
		});
	tools->addAction(examAction);

	LogOutAction = new QAction("登出", this);
	connect(LogOutAction, &QAction::triggered, [=]() {
		emit(logout(this));
		});
	tools->addAction(LogOutAction);

	courseTable = NULL; CourseTableModel = NULL; currentModel = NULL;
	searchButton = NULL; searchEdit = NULL; groupTreeModel = NULL;
	groupTree = NULL; idxTree = NULL; courseLayout = NULL;
}

void AdminWindow::show_courses() {
	
	if (searchButton&&searchEdit) {
		delete searchButton;
		delete searchEdit;
		searchButton = NULL;
		searchEdit = NULL;
	}
	searchEdit = new QLineEdit(this);
	searchEdit->show(); searchEdit->move(100, 0);
	searchButton = new QPushButton("搜索", this);
	searchButton->show(); searchButton->move(200, 0);
	
	
	connect(searchButton, &QPushButton::clicked, [=]() {
		set_up_Courses(searchEdit->text().toStdString());
		});
	set_up_Courses();

	
	
}

void AdminWindow::addCourse() {
	QDialog newCourseDialog(courseTable);
	QFormLayout form(&newCourseDialog);
	form.addRow(new QLabel("新课程信息"));
	// Format
	QLineEdit* courseNameEdit = new QLineEdit(&newCourseDialog);
	form.addRow(QString("课程名称"), courseNameEdit);
	QLineEdit* teacherEdit = new QLineEdit(&newCourseDialog);
	form.addRow(QString("授课教师"), teacherEdit);
	QSpinBox* sizeSpin = new QSpinBox(&newCourseDialog);
	sizeSpin->setMinimum(1); sizeSpin->setMaximum(999);
	form.addRow(QString("限额人数"), sizeSpin);
	QComboBox* typeBox = new QComboBox(&newCourseDialog);
	typeBox->addItem(QString("专业课"));
	typeBox->addItem(QString("非专业课"));
	form.addRow(QString("课程类型"), typeBox);
	// Add Cancel and OK button
	QDialogButtonBox buttonBox(QDialogButtonBox::Ok | QDialogButtonBox::Cancel,
		Qt::Horizontal, &newCourseDialog);
	form.addRow(&buttonBox);
	QObject::connect(&buttonBox, SIGNAL(accepted()), &newCourseDialog, SLOT(accept()));
	// Process when OK button is clicked
	if (newCourseDialog.exec() == QDialog::Accepted) {
		int newCourseNum;
		if (newCourseNum = engine->add_course(0, courseNameEdit->text().toStdString(), teacherEdit->text().toStdString(),
			sizeSpin->value(), typeBox->currentText().toStdString()))
			QMessageBox::information(NULL, "添加课程", QString::asprintf("添加成功，新课程编号%03d", newCourseNum));
		else QMessageBox::information(NULL, "添加课程", "添加失败");
		return;
	}
}


void AdminWindow::set_up_Courses(string abbr) {	
	if (courseTable) {
		delete courseTable;
		delete idxTree;
		courseTable = NULL;
		idxTree = NULL;
	}
	courseTable = new QTableView(this);
	CourseTableModel = new QStandardItemModel(courseTable);
	idxTree = new Tree;
	
	QPushButton* addCourseButton = new QPushButton("新课程",this);
	addCourseButton->move(500, 0); addCourseButton->show();
	connect(addCourseButton, &QPushButton::clicked, this,&AdminWindow::addCourse);


	QPushButton* deleteButtons[1000];
	for (int i = 0; i < 1000; i++)
		deleteButtons[i] = new QPushButton(courseTable);

	QStringList title;
	title << "课程号" << "课程名" << "老师" << "限额" << "已选" << "类型" << "删除";
	engine->print_courses(0, CourseTableModel, deleteButtons, QString("Admin"),abbr);
	CourseTableModel->setHorizontalHeaderLabels(title);
	courseTable->setModel(CourseTableModel);
	courseTable->setSelectionMode(QAbstractItemView::SingleSelection);
	courseTable->setSelectionBehavior(QAbstractItemView::SelectItems);

	
	QList<int>* courselist = new QList<int>;
	//deleteButtons
	int cnt = 0;
	for (int i = 0; i < 1000; i++) {
		if (!deleteButtons[i]) continue;
		courselist->append(i);
		deleteButtons[i]->setText("删除");
		idxTree->addOne(i);
		courseTable->setIndexWidget(CourseTableModel->index(cnt, CourseTableModel->columnCount() - 1), deleteButtons[i]);
		connect(deleteButtons[i], &QPushButton::clicked, [=]() {
			if (!engine->delete_Course(i)) return;
			int idx = idxTree->getVal(i) - 1;
			CourseTableModel->removeRow(idx);
			courselist->erase(courselist->begin() + idx);
			idxTree->minusOne(i);
			});
		cnt++;
	}

	//Layout
	if (courseLayout)
		delete courseLayout;
	courseLayout = new QGridLayout(this);
	courseLayout->addItem(new QSpacerItem(1, 10), 0, 0);
	courseLayout->addWidget(searchEdit, 1, 0, 1, 3);
	courseLayout->addWidget(searchButton, 1, 3, 1, 1);
	courseLayout->addWidget(courseTable, 2, 0, 10, 7);
	courseLayout->addWidget(addCourseButton, 1, 6, 1, 1);

	//AssistantList
	connect(courseTable, &QAbstractItemView::clicked, [=](const QModelIndex& idx) {
		if (groupTree) {
			delete groupTree;
		}
		groupTree = new QTreeView(this);
		courseLayout->addWidget(groupTree, 2, 7, 7, 2);
		groupTreeModel = new QStandardItemModel(groupTree);
		groupTree->setModel(groupTreeModel);
		engine->show_students(courselist->at(idx.row()), groupTreeModel);
		groupTree->show();
		groupTree->move(1000, 30);
		}
	);

	//makeChanges
	connect(courseTable, &QAbstractItemView::doubleClicked, [=](const QModelIndex& idx) {
		switch (idx.column()) {
		case 2: {//teacher
			QString newTeacher = QInputDialog::getText(this, QString::asprintf("课程%03d_修改教师", courselist->at(idx.row())), "请输入新老师的姓名");
			engine->change_course_teacher(courselist->at(idx.row()), newTeacher.toStdString());
			engine->reprint_course(CourseTableModel, courselist->at(idx.row()), idxTree->getVal(courselist->at(idx.row()))-1);
			break;
		}
		case 3: {//size
			int newSize = QInputDialog::getInt(this, QString::asprintf("课程%03d_修改限额", courselist->at(idx.row())), "请选择新的课程大小");
			if (engine->change_course_size(courselist->at(idx.row()), newSize))
				engine->reprint_course(CourseTableModel, courselist->at(idx.row()), idxTree->getVal(courselist->at(idx.row()))-1);
			else QMessageBox::warning(NULL, "修改失败", "新的班级限额不应该低于现有选课人数");
			break;
		}
		case 5: {//course_type
			QString newType = QInputDialog::getText(this, QString::asprintf("课程%03d_修改类型", courselist->at(idx.row())), "请输入类型");
			engine->change_course_type(courselist->at(idx.row()), newType.toStdString());
			engine->reprint_course(CourseTableModel, courselist->at(idx.row()), idxTree->getVal(courselist->at(idx.row()))-1);
			break;
		}
		default: break;
		}
		});
}

AdminWindow::~AdminWindow()
{
	engine->Archive();
}


void AdminWindow::closeEvent(QCloseEvent* ev) {
	engine->Archive();
}
