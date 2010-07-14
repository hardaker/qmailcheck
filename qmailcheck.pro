# -------------------------------------------------
# Project created by QtCreator 2010-05-19T19:06:51
# -------------------------------------------------
QT += network
TARGET = qmailcheck
TEMPLATE = app
SOURCES += main.cpp \
    qtincoming.cpp \
    incomingmailmodel.cpp \
    mailmsg.cpp \
    folderitem.cpp \
    foldermodel.cpp \
    MailSource.cpp \
    MailChecker.cpp
HEADERS += qtincoming.h \
    incomingmailmodel.h \
    mailmsg.h \
    folderitem.h \
    foldermodel.h \
    MailSource.h \
    MailChecker.h
FORMS += qtincoming.ui \
    prefs.ui
CONFIG += link_pkgconfig
PKGCONFIG += libnotify
