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
	
	coursesAction = new QAction("�γ�/ˢ��", this);
	connect(coursesAction, &QAction::triggered,this,&AdminWindow::show_courses);
	tools->addAction(coursesAction);

	fromFileAction = new QAction("���ļ�����",this);
	connect(fromFileAction, &QAction::triggered, [=]() {
		QString file_name = QFileDialog::getOpenFileName(this, "OpenFile", "/data", ("Text(*.txt)"));
		engine->read_courses(file_name.toStdString().c_str());
		});
	tools->addAction(fromFileAction);

	examAction = new QAction("���ſ���", this);
	connect(examAction, &QAction::triggered, [this]() {
		int n_exams = engine->arrange_exams();
		if (!n_exams)
			QMessageBox::information(NULL, "���ſ���", "����ʧ��");
		else QMessageBox::information(NULL, "���ſ���", QString::asprintf("���ųɹ�������%d������", n_exams));
		});
	tools->addAction(examAction);

	LogOutAction = new QAction("�ǳ�", this);
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
	searchButton = new QPushButton("����", this);
	searchButton->show(); searchButton->move(200, 0);
	
	
	connect(searchButton, &QPushButton::clicked, [=]() {
		set_up_Courses(searchEdit->text().toStdString());
		});
	set_up_Courses();

	
	
}

void AdminWindow::addCourse() {
	QDialog newCourseDialog(courseTable);
	QFormLayout form(&newCourseDialog);
	form.addRow(new QLabel("�¿γ���Ϣ"));
	// Format
	QLineEdit* courseNameEdit = new QLineEdit(&newCourseDialog);
	form.addRow(QString("�γ�����"), courseNameEdit);
	QLineEdit* teacherEdit = new QLineEdit(&newCourseDialog);
	form.addRow(QString("�ڿν�ʦ"), teacherEdit);
	QSpinBox* sizeSpin = new QSpinBox(&newCourseDialog);
	sizeSpin->setMinimum(1); sizeSpin->setMaximum(999);
	form.addRow(QString("�޶�����"), sizeSpin);
	QComboBox* typeBox = new QComboBox(&newCourseDialog);
	typeBox->addItem(QString("רҵ��"));
	typeBox->addItem(QString("��רҵ��"));
	form.addRow(QString("�γ�����"), typeBox);
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
			QMessageBox::information(NULL, "��ӿγ�", QString::asprintf("��ӳɹ����¿γ̱��%03d", newCourseNum));
		else QMessageBox::information(NULL, "��ӿγ�", "���ʧ��");
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
	
	QPushButton* addCourseButton = new QPushButton("�¿γ�",this);
	addCourseButton->move(500, 0); addCourseButton->show();
	connect(addCourseButton, &QPushButton::clicked, this,&AdminWindow::addCourse);


	QPushButton* deleteButtons[1000];
	for (int i = 0; i < 1000; i++)
		deleteButtons[i] = new QPushButton(courseTable);

	QStringList title;
	title << "�γ̺�" << "�γ���" << "��ʦ" << "�޶�" << "��ѡ" << "����" << "ɾ��";
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
		deleteButtons[i]->setText("ɾ��");
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
			QString newTeacher = QInputDialog::getText(this, QString::asprintf("�γ�%03d_�޸Ľ�ʦ", courselist->at(idx.row())), "����������ʦ������");
			engine->change_course_teacher(courselist->at(idx.row()), newTeacher.toStdString());
			engine->reprint_course(CourseTableModel, courselist->at(idx.row()), idxTree->getVal(courselist->at(idx.row()))-1);
			break;
		}
		case 3: {//size
			int newSize = QInputDialog::getInt(this, QString::asprintf("�γ�%03d_�޸��޶�", courselist->at(idx.row())), "��ѡ���µĿγ̴�С");
			if (engine->change_course_size(courselist->at(idx.row()), newSize))
				engine->reprint_course(CourseTableModel, courselist->at(idx.row()), idxTree->getVal(courselist->at(idx.row()))-1);
			else QMessageBox::warning(NULL, "�޸�ʧ��", "�µİ༶�޶Ӧ�õ�������ѡ������");
			break;
		}
		case 5: {//course_type
			QString newType = QInputDialog::getText(this, QString::asprintf("�γ�%03d_�޸�����", courselist->at(idx.row())), "����������");
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
