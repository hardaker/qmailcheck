#############################################################################
# Makefile for building: qmailcheck
# Generated by qmake (2.01a) (Qt 4.6.2) on: Mon May 24 14:08:33 2010
# Project:  qmailcheck.pro
# Template: app
# Command: /usr/bin/qmake-qt4 -spec /usr/lib/qt4/mkspecs/linux-g++ -unix CONFIG+=debug -o Makefile qmailcheck.pro
#############################################################################

####### Compiler, tools and options

CC            = gcc
CXX           = g++
DEFINES       = -DQT_GUI_LIB -DQT_NETWORK_LIB -DQT_CORE_LIB
CFLAGS        = -pipe -g -Wall -W -D_REENTRANT $(DEFINES)
CXXFLAGS      = -pipe -g -Wall -W -D_REENTRANT $(DEFINES)
INCPATH       = -I/usr/lib/qt4/mkspecs/linux-g++ -I. -I/usr/include/QtCore -I/usr/include/QtNetwork -I/usr/include/QtGui -I/usr/include -I. -I.
LINK          = g++
LFLAGS        = 
LIBS          = $(SUBLIBS)   -lQtGui -lQtNetwork -lQtCore -lpthread 
AR            = ar cqs
RANLIB        = 
QMAKE         = /usr/bin/qmake-qt4
TAR           = tar -cf
COMPRESS      = gzip -9f
COPY          = cp -f
SED           = sed
COPY_FILE     = $(COPY)
COPY_DIR      = $(COPY) -r
STRIP         = 
INSTALL_FILE  = install -m 644 -p
INSTALL_DIR   = $(COPY_DIR)
INSTALL_PROGRAM = install -m 755 -p
DEL_FILE      = rm -f
SYMLINK       = ln -f -s
DEL_DIR       = rmdir
MOVE          = mv -f
CHK_DIR_EXISTS= test -d
MKDIR         = mkdir -p

####### Output directory

OBJECTS_DIR   = ./

####### Files

SOURCES       = main.cpp \
		qtincoming.cpp \
		incomingmailmodel.cpp \
		mailmsg.cpp 
OBJECTS       = main.o \
		qtincoming.o \
		incomingmailmodel.o \
		mailmsg.o
DIST          = /usr/lib/qt4/mkspecs/common/g++-multilib.conf \
		/usr/lib/qt4/mkspecs/common/unix.conf \
		/usr/lib/qt4/mkspecs/common/linux.conf \
		/usr/lib/qt4/mkspecs/qconfig.pri \
		/usr/lib/qt4/mkspecs/features/qt_functions.prf \
		/usr/lib/qt4/mkspecs/features/qt_config.prf \
		/usr/lib/qt4/mkspecs/features/exclusive_builds.prf \
		/usr/lib/qt4/mkspecs/features/default_pre.prf \
		/usr/lib/qt4/mkspecs/features/debug.prf \
		/usr/lib/qt4/mkspecs/features/default_post.prf \
		/usr/lib/qt4/mkspecs/features/warn_on.prf \
		/usr/lib/qt4/mkspecs/features/qt.prf \
		/usr/lib/qt4/mkspecs/features/unix/thread.prf \
		/usr/lib/qt4/mkspecs/features/moc.prf \
		/usr/lib/qt4/mkspecs/features/resources.prf \
		/usr/lib/qt4/mkspecs/features/uic.prf \
		/usr/lib/qt4/mkspecs/features/yacc.prf \
		/usr/lib/qt4/mkspecs/features/lex.prf \
		/usr/lib/qt4/mkspecs/features/include_source_dir.prf \
		qmailcheck.pro
QMAKE_TARGET  = qmailcheck
DESTDIR       = 
TARGET        = qmailcheck

first: all
####### Implicit rules

.SUFFIXES: .o .c .cpp .cc .cxx .C

.cpp.o:
	$(CXX) -c $(CXXFLAGS) $(INCPATH) -o "$@" "$<"

.cc.o:
	$(CXX) -c $(CXXFLAGS) $(INCPATH) -o "$@" "$<"

.cxx.o:
	$(CXX) -c $(CXXFLAGS) $(INCPATH) -o "$@" "$<"

.C.o:
	$(CXX) -c $(CXXFLAGS) $(INCPATH) -o "$@" "$<"

.c.o:
	$(CC) -c $(CFLAGS) $(INCPATH) -o "$@" "$<"

####### Build rules

all: Makefile $(TARGET)

$(TARGET): ui_qtincoming.h $(OBJECTS)  
	$(LINK) $(LFLAGS) -o $(TARGET) $(OBJECTS) $(OBJCOMP) $(LIBS)

Makefile: qmailcheck.pro  /usr/lib/qt4/mkspecs/linux-g++/qmake.conf /usr/lib/qt4/mkspecs/common/g++-multilib.conf \
		/usr/lib/qt4/mkspecs/common/unix.conf \
		/usr/lib/qt4/mkspecs/common/linux.conf \
		/usr/lib/qt4/mkspecs/qconfig.pri \
		/usr/lib/qt4/mkspecs/features/qt_functions.prf \
		/usr/lib/qt4/mkspecs/features/qt_config.prf \
		/usr/lib/qt4/mkspecs/features/exclusive_builds.prf \
		/usr/lib/qt4/mkspecs/features/default_pre.prf \
		/usr/lib/qt4/mkspecs/features/debug.prf \
		/usr/lib/qt4/mkspecs/features/default_post.prf \
		/usr/lib/qt4/mkspecs/features/warn_on.prf \
		/usr/lib/qt4/mkspecs/features/qt.prf \
		/usr/lib/qt4/mkspecs/features/unix/thread.prf \
		/usr/lib/qt4/mkspecs/features/moc.prf \
		/usr/lib/qt4/mkspecs/features/resources.prf \
		/usr/lib/qt4/mkspecs/features/uic.prf \
		/usr/lib/qt4/mkspecs/features/yacc.prf \
		/usr/lib/qt4/mkspecs/features/lex.prf \
		/usr/lib/qt4/mkspecs/features/include_source_dir.prf
	$(QMAKE) -spec /usr/lib/qt4/mkspecs/linux-g++ -unix CONFIG+=debug -o Makefile qmailcheck.pro
/usr/lib/qt4/mkspecs/common/g++-multilib.conf:
/usr/lib/qt4/mkspecs/common/unix.conf:
/usr/lib/qt4/mkspecs/common/linux.conf:
/usr/lib/qt4/mkspecs/qconfig.pri:
/usr/lib/qt4/mkspecs/features/qt_functions.prf:
/usr/lib/qt4/mkspecs/features/qt_config.prf:
/usr/lib/qt4/mkspecs/features/exclusive_builds.prf:
/usr/lib/qt4/mkspecs/features/default_pre.prf:
/usr/lib/qt4/mkspecs/features/debug.prf:
/usr/lib/qt4/mkspecs/features/default_post.prf:
/usr/lib/qt4/mkspecs/features/warn_on.prf:
/usr/lib/qt4/mkspecs/features/qt.prf:
/usr/lib/qt4/mkspecs/features/unix/thread.prf:
/usr/lib/qt4/mkspecs/features/moc.prf:
/usr/lib/qt4/mkspecs/features/resources.prf:
/usr/lib/qt4/mkspecs/features/uic.prf:
/usr/lib/qt4/mkspecs/features/yacc.prf:
/usr/lib/qt4/mkspecs/features/lex.prf:
/usr/lib/qt4/mkspecs/features/include_source_dir.prf:
qmake:  FORCE
	@$(QMAKE) -spec /usr/lib/qt4/mkspecs/linux-g++ -unix CONFIG+=debug -o Makefile qmailcheck.pro

dist: 
	@$(CHK_DIR_EXISTS) .tmp/qmailcheck1.0.0 || $(MKDIR) .tmp/qmailcheck1.0.0 
	$(COPY_FILE) --parents $(SOURCES) $(DIST) .tmp/qmailcheck1.0.0/ && $(COPY_FILE) --parents qtincoming.h incomingmailmodel.h mailmsg.h .tmp/qmailcheck1.0.0/ && $(COPY_FILE) --parents main.cpp qtincoming.cpp incomingmailmodel.cpp mailmsg.cpp .tmp/qmailcheck1.0.0/ && $(COPY_FILE) --parents qtincoming.ui .tmp/qmailcheck1.0.0/ && (cd `dirname .tmp/qmailcheck1.0.0` && $(TAR) qmailcheck1.0.0.tar qmailcheck1.0.0 && $(COMPRESS) qmailcheck1.0.0.tar) && $(MOVE) `dirname .tmp/qmailcheck1.0.0`/qmailcheck1.0.0.tar.gz . && $(DEL_FILE) -r .tmp/qmailcheck1.0.0


clean:compiler_clean 
	-$(DEL_FILE) $(OBJECTS)
	-$(DEL_FILE) *~ core *.core


####### Sub-libraries

distclean: clean
	-$(DEL_FILE) $(TARGET) 
	-$(DEL_FILE) Makefile


mocclean: compiler_moc_header_clean compiler_moc_source_clean

mocables: compiler_moc_header_make_all compiler_moc_source_make_all

compiler_moc_header_make_all: moc_qtincoming.cpp moc_incomingmailmodel.cpp
compiler_moc_header_clean:
	-$(DEL_FILE) moc_qtincoming.cpp moc_incomingmailmodel.cpp
moc_qtincoming.cpp: qtincoming.h
	/usr/lib/qt4/bin/moc $(DEFINES) $(INCPATH) qtincoming.h -o moc_qtincoming.cpp

moc_incomingmailmodel.cpp: mailmsg.h \
		incomingmailmodel.h
	/usr/lib/qt4/bin/moc $(DEFINES) $(INCPATH) incomingmailmodel.h -o moc_incomingmailmodel.cpp

compiler_rcc_make_all:
compiler_rcc_clean:
compiler_image_collection_make_all: qmake_image_collection.cpp
compiler_image_collection_clean:
	-$(DEL_FILE) qmake_image_collection.cpp
compiler_moc_source_make_all:
compiler_moc_source_clean:
compiler_uic_make_all: ui_qtincoming.h
compiler_uic_clean:
	-$(DEL_FILE) ui_qtincoming.h
ui_qtincoming.h: qtincoming.ui
	/usr/lib/qt4/bin/uic qtincoming.ui -o ui_qtincoming.h

compiler_yacc_decl_make_all:
compiler_yacc_decl_clean:
compiler_yacc_impl_make_all:
compiler_yacc_impl_clean:
compiler_lex_make_all:
compiler_lex_clean:
compiler_clean: compiler_moc_header_clean compiler_uic_clean 

####### Compile

main.o: main.cpp qtincoming.h
	$(CXX) -c $(CXXFLAGS) $(INCPATH) -o main.o main.cpp

qtincoming.o: qtincoming.cpp qtincoming.h \
		ui_qtincoming.h \
		incomingmailmodel.h \
		mailmsg.h \
		moc_qtincoming.cpp
	$(CXX) -c $(CXXFLAGS) $(INCPATH) -o qtincoming.o qtincoming.cpp

incomingmailmodel.o: incomingmailmodel.cpp incomingmailmodel.h \
		mailmsg.h \
		moc_incomingmailmodel.cpp
	$(CXX) -c $(CXXFLAGS) $(INCPATH) -o incomingmailmodel.o incomingmailmodel.cpp

mailmsg.o: mailmsg.cpp mailmsg.h
	$(CXX) -c $(CXXFLAGS) $(INCPATH) -o mailmsg.o mailmsg.cpp

####### Install

install:   FORCE

uninstall:   FORCE

FORCE:

