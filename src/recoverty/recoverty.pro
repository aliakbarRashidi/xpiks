TEMPLATE = app

macx {
    travis-ci {
        CONFIG += sdk_no_version_check
    } else {
        QMAKE_MAC_SDK = macosx10.13
    }
}

#CONFIG -= app_bundle

QT += qml quick svg
CONFIG += c++11
CONFIG += qtquickcompiler
TARGET = Recoverty

VERSION = 0.0.1.0
QMAKE_TARGET_PRODUCT = Recoverty
QMAKE_TARGET_DESCRIPTION = "Crash recovery application"
QMAKE_TARGET_COPYRIGHT = "Copyright (C) 2018 Taras Kushnir"

DEFINES += QT_NO_CAST_TO_ASCII \
           QT_RESTRICTED_CAST_FROM_ASCII \
           QT_NO_CAST_FROM_BYTEARRAY

unix {
    DESTDIR = ../xpiks-qt/deps/recoverty/
}

ICON = recoverty.icns
RC_ICONS = recoverty.ico

SOURCES += main.cpp \
    restartmodel.cpp

HEADERS += \
    restartmodel.h

RESOURCES += qml.qrc

# Additional import path used to resolve QML modules in Qt Creator's code model
QML_IMPORT_PATH =

# Default rules for deployment.
include(deployment.pri)
