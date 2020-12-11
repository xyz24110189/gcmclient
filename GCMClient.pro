#-------------------------------------------------
#
# Project created by QtCreator 2020-04-20T09:30:57
#
#-------------------------------------------------

QT       += core gui network
DEFINES  -= UNICODE

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

TARGET = GCMClient
TEMPLATE = app

QMAKE_CXXFLAGS += -std=c++0x

INCLUDEPATH += include/pkiAgent
INCLUDEPATH += include/cppHttplib
INCLUDEPATH += include/openssl
INCLUDEPATH += include/plog
INCLUDEPATH += include/libusb
INCLUDEPATH += include/sqlite3

include(src/src.pri)

unix {
    LIBS += -L$$PWD/lib/pkiAgent/x64 -lpkiAgent4c -lparson -lthrift-0.11.0
    LIBS += -L$$PWD/lib/openssl/x64 -lcrypto -lssl
    LIBS += -L$$PWD/lib/libusb/x64 -lusb-1.0
    LIBS += -L$$PWD/lib/sqlite3/x64 -lsqlite3
    QMAKE_LFLAGS += -Wl,-rpath=. 
    QMAKE_LFLAGS += -Wl,-rpath=./Qtlib/lib
    QMAKE_LFLAGS += -Wl,-rpath=/opt/koal/GCM/Qtlib/lib
    QMAKE_LFLAGS += -Wl,-rpath=/opt/koal/GCM
}

win32 {
    LIBS += -L$$PWD/lib/pkiAgent/x86 -lpkiAgent4c
    LIBS += -L$$PWD/lib/openssl/x86 -llibcrypto -llibssl
    LIBS += -L$$PWD/lib/libusb/x86 -llibusb-1.0
    LIBS += -L$$PWD/lib/sqlite3/x86 -lsqlite3
}

#DEFINES += CPPHTTPLIB_OPENSSL_SUPPORT  ///开启https模式

RESOURCES += \
    gcmclient.qrc

TRANSLATIONS = GCMClient_zh_CN.ts



