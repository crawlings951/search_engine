######################################################################
# Automatically generated by qmake (2.01a) Wed Apr 2 23:25:44 2014
######################################################################

TEMPLATE = app
TARGET = 
DEPENDPATH += . lib src
INCLUDEPATH += . src lib
DESTDIR = bin                                                                  
OBJECTS_DIR = obj                                                              
QMAKE_CXX = g++-4.8                                                            
QMAKE_LINK = g++-4.8                                                           
QMAKE_CXXFLAGS += -std=c++11 
QMAKE_CXXFLAGS -= -stdlib=libc++
QMAKE_LFLAGS -= -stdlib=libc++

# Input
HEADERS += lib/Set.h \
           lib/msort.h \
           src/comps.h \
           src/ResultWin.h \
           src/SearchEngine.h \
           src/SearchForm.h \
           src/WebPage.h \
           src/User.h \
           src/UserWin.h
SOURCES += src/comps.cpp \
           src/ResultWin.cpp \
           src/SearchEngine.cpp \
           src/SearchForm.cpp \
           src/main.cpp \
           src/WebPage.cpp \
           src/User.cpp \
           src/UserWin.cpp
