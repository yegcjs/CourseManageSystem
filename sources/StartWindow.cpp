#include "StartWindow.h"
#include "StudentWindow.h"
#include "AdminWindow.h"
#pragma execution_character_set("utf-8")
#pragma warning(disable:26812)
#include<QString>
#include<QPixmap>
#include<QTextCodec>
#include<QDebug>
#include<QGridLayout>
#include<QMessagebox>
StartWindow::StartWindow(QWidget *parent)
	: QMainWindow(parent)
{
	QWidget* centralWidget = new QWidget(this);
	setCentralWidget(centralWidget);

	engine = new Engine();

	identityButton = new QPushButton(QString("�л�������Ա"));
	modeButton = new QPushButton("�л���ע��");
	
	usernameLabel = new QLabel("�û���");
	passwordLabel = new QLabel("����");
	usernameEdit = new QLineEdit();
	passwordEdit = new QLineEdit();
	passwordEdit->setEchoMode(QLineEdit::Password);

	submitButton = new QPushButton("Submit" ); 
	
	confirmLabel = new QLabel("ȷ������"); confirmLabel->hide();
	confirmEdit = new QLineEdit(); confirmEdit->hide();
	confirmEdit->setEchoMode(QLineEdit::Password);

	connect(identityButton, &QPushButton::clicked, [=]() {
		if (identityButton->text() == QString("�л�������Ա")) {
			identityButton->setText("�л���ѧ��");
			usernameEdit->setText("Admin");
			if(modeButton->text()==QString("�л�����¼"))
				modeButton->click();
		}
		else {
			identityButton->setText("�л�������Ա");
			usernameEdit->clear();
		}
		}
		);

	connect(modeButton, &QPushButton::clicked, [=]() {
		if (modeButton->text() == QString("�л���ע��")) {
			confirmLabel->show(); confirmEdit->show();
			modeButton->setText("�л�����¼");
		}
		else {
			modeButton->setText("�л���ע��");
			confirmLabel->hide(); confirmEdit->hide();
		}
		});
	
	connect(submitButton, &QPushButton::clicked, [=]() {
		if (modeButton->text() == QString("�л���ע��"))
			Login();
		else SignUp();
		});

	open_eyes = new QLabel; close_eyes = new QLabel;
	open_eyes->setPixmap(QPixmap("pic/open_eyes.jpg"));
	close_eyes->setPixmap(QPixmap("pic/close_eyes.jpg")); 
	
	Layout(centralWidget);
}

void StartWindow::closeEvent(QCloseEvent* ev) {
	delete engine;//including Archive()
}

void StartWindow::Login() {
	int flag = starter.Login(identityButton->text() == QString("�л�������Ա") ? 0 : 1,
		usernameEdit->text(), passwordEdit->text());
	if (flag == 0)
		QMessageBox::information(this, "��¼ʧ��", "�û������������");
	else {
		QMessageBox::information(this, "��¼�ɹ�", "Welcome!");
		
		if (usernameEdit->text() != QString("Admin")) {
			StudentWindow* studentWin = new StudentWindow(usernameEdit->text(), engine);
			connect(studentWin, &StudentWindow::logout, [=](QWidget* window) {
				//window->close();
				delete window;
				this->show();
				});
			studentWin->show();
		}
		else {
			AdminWindow* AdminWin = new AdminWindow(engine);
			connect(AdminWin, &AdminWindow::logout, [=](QWidget* window) {
				//window->close();
				delete window;
				this->show();
				});
			AdminWin->show();
		}
		
		this->hide();
	}
	usernameEdit->clear(); passwordEdit->clear(); confirmEdit->clear();
}

void StartWindow::SignUp() {
	int flag = starter.SignUp(usernameEdit->text(),
		passwordEdit->text(), confirmEdit->text());
	if (flag == 1)
		QMessageBox::information(this, "ע��ʧ��", "�û������Ϸ������޴�Сд��ĸ������");
	else if (flag == 2)
		QMessageBox::information(this, "ע��ʧ��", "�û����ѱ�ע��");
	else if (flag == 3)
		QMessageBox::information(this, "ע��ʧ��", "�������벻ͬ");
	else if(flag==0){
		QMessageBox::information(this, "ע��ɹ�", "�ɹ�");
		modeButton->setText("�л���ע��");
		confirmLabel->hide(); confirmEdit->hide();
	}
	usernameEdit->clear(); passwordEdit->clear(); confirmEdit->clear();
}

void StartWindow::Layout(QWidget* cWidget) {
	QGridLayout* layout = new QGridLayout(cWidget);
	layout->addWidget(identityButton, 0, 2, 1, 2);
	layout->addWidget(modeButton, 0, 4, 1, 2);
	layout->addWidget(usernameLabel, 1, 1);	layout->addWidget(usernameEdit, 1, 2, 1, 4);
	layout->addWidget(passwordLabel, 2, 1); layout->addWidget(passwordEdit, 2, 2, 1, 4);
	layout->addWidget(confirmLabel, 3, 1); layout->addWidget(confirmEdit, 3, 2, 1, 4);
	layout->addWidget(submitButton, 4, 3, 1, 2);
	layout->addWidget(close_eyes, 0, 0, 5, 1);
	layout->addWidget(open_eyes, 0, 0, 5, 1);
	close_eyes->hide();
	connect(passwordEdit, &QLineEdit::textEdited, [this]() {
		open_eyes->hide();
		close_eyes->show();
		});
	connect(passwordEdit, &QLineEdit::editingFinished, [this]() {
		open_eyes->show();
		close_eyes->hide();
		});
	layout->setHorizontalSpacing(40);
	//qDebug() << width() <<","<< height();
}