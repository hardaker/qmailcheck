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
    foldermodel.cpp
HEADERS += qtincoming.h \
    incomingmailmodel.h \
    mailmsg.h \
    foldermodel.h
FORMS += qtincoming.ui \
    prefs.ui
CONFIG += link_pkgconfig
PKGCONFIG += libnotify

