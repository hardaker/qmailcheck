# -------------------------------------------------
# Project created by QtCreator 2010-05-19T19:06:51
# -------------------------------------------------
QT += network core gui
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
RESOURCES = qmailcheck.qrc

CONFIG += mobility
MOBILITY = 

symbian {
    TARGET.UID3 = 0xe0df144b
    # TARGET.CAPABILITY += 
    TARGET.EPOCSTACKSIZE = 0x14000
    TARGET.EPOCHEAPSIZE = 0x020000 0x800000
}

OTHER_FILES += \
    debian/changelog \
    debian/compat \
    debian/control \
    debian/copyright \
    debian/README \
    debian/rules \
    qmailcheck.desktop

unix:!symbian {
    maemo5 {
        target.path = /opt/usr/bin
    } else {
        target.path = /usr/local/bin
    }
    INSTALLS += target
}

unix:!symbian {
    desktopfile.files = $${TARGET}.desktop
    maemo5 {
        desktopfile.path = /usr/share/applications/hildon
    } else {
        desktopfile.path = /usr/share/applications
    }
    INSTALLS += desktopfile
}
