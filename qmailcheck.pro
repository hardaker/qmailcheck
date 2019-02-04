# -------------------------------------------------
# Project created by QtCreator 2010-05-19T19:06:51
# -------------------------------------------------
QT += network core gui multimedia
TARGET = qmailcheck
TEMPLATE = app
SOURCES += main.cpp \
    qtincoming.cpp \
    incomingmailmodel.cpp \
    mailmsg.cpp \
    folderitem.cpp \
    foldermodel.cpp \
    MailSource.cpp \
    MailChecker.cpp \
    TextPainter.cpp
HEADERS += qtincoming.h \
    incomingmailmodel.h \
    mailmsg.h \
    folderitem.h \
    foldermodel.h \
    MailSource.h \
    MailChecker.h \
    TextPainter.h
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
    TARGET.CAPABILITY += NetworkServices
}

OTHER_FILES += \
    debian/changelog \
    debian/compat \
    debian/control \
    debian/copyright \
    debian/README \
    debian/rules \
    qmailcheck.desktop \
    qtc_packaging/debian_fremantle/rules \
    qtc_packaging/debian_fremantle/README \
    qtc_packaging/debian_fremantle/copyright \
    qtc_packaging/debian_fremantle/control \
    qtc_packaging/debian_fremantle/compat \
    qtc_packaging/debian_fremantle/changelog \
    qtc_packaging/debian_harmattan/rules \
    qtc_packaging/debian_harmattan/README \
    qtc_packaging/debian_harmattan/copyright \
    qtc_packaging/debian_harmattan/control \
    qtc_packaging/debian_harmattan/compat \
    qtc_packaging/debian_harmattan/changelog

unix:!symbian {
    maemo5 {
        target.path = /opt/usr/bin
    } else {
        target.path = /usr/local/bin
    }
    INSTALLS += target

    CONFIG += link_pkgconfig
    PKGCONFIG += libnotify glib-2.0 dbus-1 gmodule-2.0 gdk-2.0 atk gtk+-2.0
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
