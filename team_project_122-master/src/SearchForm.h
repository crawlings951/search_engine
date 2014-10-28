#ifndef SEARCHFORM_H
#define SEARCHFORM_H
#include <QPushButton>
#include <QMainWindow>
#include <QWidget>
#include <QLabel>
#include <QLineEdit>
#include <QListWidget>
#include <QHBoxLayout>
#include <QVBoxLayout>
#include <QListWidgetItem>
#include <QRadioButton>
#include <QGroupBox>
#include <deque>
#include "SearchEngine.h"
#include "ResultWin.h"
#include "UserWin.h"
#include "User.h"

class UserWin;

class SearchForm : public QWidget   {
    Q_OBJECT

    public:
        SearchForm(QWidget* parent = NULL, SearchEngine* engine = NULL);
        ~SearchForm();

        //Help define initial size
        QSize sizeHint() const  {return QSize(800,600);}
        QSize minimumSizeHint() const   {return QSize(300,200);}

        // Interface w/ Userwin
        void setUser(User* nus);

    private slots:
        void aboutClicked();
        void adClicked(QListWidgetItem* item);
        void loginClicked();
        void quitClicked();
        void searchClicked();
        void resultClicked(QListWidgetItem* item);

    private:
        QPushButton*    searchBtn;
        QPushButton*    aboutBtn;
        QPushButton*    quitBtn;
        QPushButton*    loginoutBtn;
        QLabel*         userinfo;
        QLineEdit*      searchTerms;
        QListWidget*    resultBox;
        QListWidget*    adBox;
        QRadioButton*   pageRank;
        QRadioButton*   alphabet;
        QHBoxLayout*    searchSection;
        QHBoxLayout*    buttons;
        QHBoxLayout*    radioButtons;
        QVBoxLayout*    mainLayout;
        QGroupBox*      buttonBox;

        // Pointer to search engine
        SearchEngine* se;

        // Pointer to Result Window
        ResultWin* reswin;
        UserWin* uswin;
        User* us;

        // List of dynamic result windows
        std::deque<Bid> ads;
};

#endif
