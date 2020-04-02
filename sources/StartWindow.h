#pragma once

#include <QtWidgets/QMainWindow>
#include "Start.h"
#include <QLabel>
#include <QPushButton>
#include <QLineEdit>
#include <QDialog>
#include<QEvent>
#include "Engine.h"

class StartWindow : public QMainWindow
{
	Q_OBJECT

public:
	StartWindow(QWidget *parent = Q_NULLPTR);
	void Login();
	void SignUp();
	void closeEvent(QCloseEvent* ev);
private:
	void Layout(QWidget *cWidget);
	Start starter;
	QLabel* usernameLabel, *passwordLabel, *confirmLabel;
	QLineEdit* usernameEdit, *passwordEdit, *confirmEdit;
	QPushButton* identityButton, * modeButton, * submitButton;
	Engine* engine;
	QLabel* open_eyes, * close_eyes;
};
