#include "SearchForm.h"
#include <QMessageBox>

SearchForm::SearchForm(QWidget* parent, SearchEngine* engine) : QWidget(parent), se(engine), us(NULL) {
    // Hide results window?
    reswin = new ResultWin(NULL,se);
    reswin->hide();

    // Hide users window
    uswin = new UserWin(NULL,se,this);
    uswin->hide();

    // Allocate memory for all widgets
    searchBtn = new QPushButton("&Search");
    aboutBtn = new QPushButton("&About");
    quitBtn = new QPushButton("&Quit");
    loginoutBtn = new QPushButton("Login");
    userinfo = new QLabel("Not logged in");
    searchTerms = new QLineEdit;
    resultBox = new QListWidget;
    adBox = new QListWidget;
    pageRank = new QRadioButton("&PageRank");
    alphabet = new QRadioButton("&Alphabetical");
    searchSection = new QHBoxLayout;
    buttons = new QHBoxLayout;
    radioButtons = new QHBoxLayout;
    mainLayout = new QVBoxLayout;
    buttonBox = new QGroupBox("Sort results by...");

    // Lay out buttons
    buttons->addWidget(loginoutBtn);
    buttons->addWidget(aboutBtn);
    buttons->addWidget(quitBtn);

    // Lay out search sections
    searchSection->addWidget(searchTerms);
    searchSection->addWidget(searchBtn);

    // Lay out radio buttons, with pagerank as default
    pageRank->setChecked(true);
    radioButtons->addWidget(pageRank);
    radioButtons->addWidget(alphabet);
    buttonBox->setLayout(radioButtons);

    // Lay out everything
    mainLayout->addWidget(userinfo);
    mainLayout->addLayout(searchSection);
    mainLayout->addWidget(buttonBox);
    mainLayout->addWidget(resultBox);
    mainLayout->addWidget(adBox);
    mainLayout->addLayout(buttons);
    
    // Signals and slots
    connect(aboutBtn,SIGNAL(clicked()),this,SLOT(aboutClicked()));
    connect(quitBtn,SIGNAL(clicked()),this,SLOT(quitClicked()));
    connect(searchBtn,SIGNAL(clicked()),this,SLOT(searchClicked()));
    connect(loginoutBtn,SIGNAL(clicked()),this,SLOT(loginClicked()));
    connect(resultBox,SIGNAL(itemDoubleClicked(QListWidgetItem*)),this,SLOT(resultClicked(QListWidgetItem*)));
    connect(adBox,SIGNAL(itemDoubleClicked(QListWidgetItem*)),this,SLOT(adClicked(QListWidgetItem*)));
    connect(pageRank,SIGNAL(toggled(bool)),this,SLOT(searchClicked())); // When we switch modes, also want to search again.

    // Set layout
    setLayout(mainLayout);
}

SearchForm::~SearchForm()   {
    resultBox->clear();
    delete searchBtn;
    delete aboutBtn;
    delete quitBtn;
    delete loginoutBtn;
    delete userinfo;
    delete searchTerms;
    delete resultBox;
    delete adBox;
    delete pageRank;
    delete alphabet;
    delete searchSection;
    delete buttons;
    delete radioButtons;
    delete buttonBox;
    delete mainLayout;
}

void SearchForm::aboutClicked() {
    QMessageBox::information(this, tr("About"), tr("Search Engine Application"));
}

void SearchForm::adClicked(QListWidgetItem* item) {
    // Find which ad was clicked
    std::string companyName = item->text().toStdString();
    // Iterate through list of ads until the correct one is found
    for(std::deque<Bid>::iterator it = ads.begin(); it != ads.end(); ++it)  {
        if(companyName == it->company)  {
            // then tell search engine which was clicked
            se->adClicked(*it);
            break;
        }
    }
    companyName = "You clicked on an ad from " + companyName;
    QMessageBox::information(this, tr("Advertisement"), tr(companyName.c_str()));
    return;
}

void SearchForm::loginClicked() {
    if(us == NULL)  {
        if(uswin->isHidden()) uswin->show();
    }
    else    {
        us = NULL;
        loginoutBtn->setText(QString("Login"));
        userinfo->setText(QString("Not logged in"));
    }
}

void SearchForm::setUser(User* nus)  {
    us = nus;
    loginoutBtn->setText(QString("Logout"));
    std::string message = "Logged in as " + us->getname();
    userinfo->setText(QString(message.c_str()));
}

void SearchForm::quitClicked()  {
    close();
    reswin->close();
}

void SearchForm::searchClicked()    {
    resultBox->clear();
    adBox->clear();
    // Get results
    std::deque<WebPage*> results = se->processSearch(searchTerms->text().toStdString(),pageRank->isChecked(),us);
    ads = se->processAds(searchTerms->text().toStdString());
    // Populate box with results
    for(std::deque<WebPage*>::iterator it = results.begin(); it != results.end(); ++it)    {
        new QListWidgetItem(QString((*it)->filename().c_str()), resultBox);
    }

    // Populate ads box with results
    for(std::deque<Bid>::iterator it = ads.begin(); it != ads.end(); ++it)  {
        new QListWidgetItem(QString(it->company.c_str()),adBox);
    }
}

void SearchForm::resultClicked(QListWidgetItem* item)   {
    // If other window hidden, show it
    if(reswin->isHidden()) reswin->show();
    reswin->populate(item->text().toStdString());
    if(us != NULL) us->incrementClick(item->text().toStdString());
}
