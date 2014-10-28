#include "UserWin.h"
#include <QMessageBox>

UserWin::UserWin(QWidget* parent, SearchEngine* eng, SearchForm* form) : QWidget(parent), se(eng), sf(form) {
    // Allocate widget memory
    createButton = new QPushButton("Create");
    loginButton = new QPushButton("Login");
    usernameField = new QLineEdit;
    passwordField = new QLineEdit;
    flayout = new QFormLayout;
    buttons = new QHBoxLayout;
    mainLayout = new QVBoxLayout;

    // Layout fields
    flayout->addRow("Username:",usernameField);
    passwordField->setEchoMode(QLineEdit::Password);
    flayout->addRow("Password:",passwordField);

    // Layout buttons
    buttons->addWidget(createButton);
    buttons->addWidget(loginButton);

    // Layout entire thing
    mainLayout->addLayout(flayout);
    mainLayout->addLayout(buttons);

    // Signals and slots
    connect(createButton,SIGNAL(clicked()),this,SLOT(createClicked()));
    connect(loginButton,SIGNAL(clicked()),this,SLOT(loginClicked()));

    // Set layout
    setLayout(mainLayout);
}

UserWin::~UserWin() {
    delete createButton;
    delete loginButton;
    delete usernameField;
    delete passwordField;
    delete flayout;
    delete buttons;
    delete mainLayout;
}

void UserWin::createClicked()   {
    if(se->createUser(usernameField->text().toStdString(),passwordField->text().toStdString())) loginClicked();
    else    {
        QMessageBox::information(this, tr("Create User"), tr("Username already taken"));
        passwordField->clear();
    }
}

void UserWin::loginClicked()    {
    User* us;
    us = se->userLogin(usernameField->text().toStdString(),passwordField->text().toStdString());
    if(us == NULL)  {
        QMessageBox::information(this, tr("Login"), tr("Username/password combination not found"));
    }
    else    {
        sf->setUser(us);
        usernameField->clear();
        passwordField->clear();
        this->hide();
    }
}
