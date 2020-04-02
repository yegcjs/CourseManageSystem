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

	identityButton = new QPushButton(QString("切换到管理员"));
	modeButton = new QPushButton("切换到注册");
	
	usernameLabel = new QLabel("用户名");
	passwordLabel = new QLabel("密码");
	usernameEdit = new QLineEdit();
	passwordEdit = new QLineEdit();
	passwordEdit->setEchoMode(QLineEdit::Password);

	submitButton = new QPushButton("Submit" ); 
	
	confirmLabel = new QLabel("确认密码"); confirmLabel->hide();
	confirmEdit = new QLineEdit(); confirmEdit->hide();
	confirmEdit->setEchoMode(QLineEdit::Password);

	connect(identityButton, &QPushButton::clicked, [=]() {
		if (identityButton->text() == QString("切换到管理员")) {
			identityButton->setText("切换到学生");
			usernameEdit->setText("Admin");
			if(modeButton->text()==QString("切换到登录"))
				modeButton->click();
		}
		else {
			identityButton->setText("切换到管理员");
			usernameEdit->clear();
		}
		}
		);

	connect(modeButton, &QPushButton::clicked, [=]() {
		if (modeButton->text() == QString("切换到注册")) {
			confirmLabel->show(); confirmEdit->show();
			modeButton->setText("切换到登录");
		}
		else {
			modeButton->setText("切换到注册");
			confirmLabel->hide(); confirmEdit->hide();
		}
		});
	
	connect(submitButton, &QPushButton::clicked, [=]() {
		if (modeButton->text() == QString("切换到注册"))
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
	int flag = starter.Login(identityButton->text() == QString("切换到管理员") ? 0 : 1,
		usernameEdit->text(), passwordEdit->text());
	if (flag == 0)
		QMessageBox::information(this, "登录失败", "用户名或密码错误");
	else {
		QMessageBox::information(this, "登录成功", "Welcome!");
		
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
		QMessageBox::information(this, "注册失败", "用户名不合法，仅限大小写字母和数字");
	else if (flag == 2)
		QMessageBox::information(this, "注册失败", "用户名已被注册");
	else if (flag == 3)
		QMessageBox::information(this, "注册失败", "两次密码不同");
	else if(flag==0){
		QMessageBox::information(this, "注册成功", "成功");
		modeButton->setText("切换到注册");
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