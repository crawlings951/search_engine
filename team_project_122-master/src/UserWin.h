#ifndef USERWIN_H
#define USERWIN_H
#include <QPushButton>
#include <QLineEdit>
#include <QFormLayout>
#include <QHBoxLayout>
#include <QVBoxLayout>
#include "SearchEngine.h"
#include "SearchForm.h"

class SearchForm;

class UserWin : public QWidget    {
    Q_OBJECT

    public:
        UserWin(QWidget* parent = NULL, SearchEngine* eng = NULL, SearchForm* form  = NULL);
        ~UserWin();

        //Help define initial size
        QSize sizeHint() const  {return QSize(400,300);}
        QSize minimumSizeHint() const   {return QSize(300,200);}

    private slots:
        void createClicked();   // Login and create buttons clicked
        void loginClicked();

    private:
        QPushButton*    createButton;
        QPushButton*    loginButton;
        QLineEdit*      usernameField;
        QLineEdit*      passwordField;
        QFormLayout*    flayout;
        QHBoxLayout*    buttons;
        QVBoxLayout*    mainLayout;

        SearchEngine* se;
        SearchForm* sf;

};

#endif
