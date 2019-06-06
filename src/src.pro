#-------------------------------------------------
#
# Project created by QtCreator 2019-05-15T22:20:56
#
#-------------------------------------------------

QT       += core gui concurrent

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

TARGET = src
TEMPLATE = app

# The following define makes your compiler emit warnings if you use
# any feature of Qt which has been marked as deprecated (the exact warnings
# depend on your compiler). Please consult the documentation of the
# deprecated API in order to know how to port your code away from it.
DEFINES += QT_DEPRECATED_WARNINGS

# You can also make your code fail to compile if you use deprecated APIs.
# In order to do so, uncomment the following line.
# You can also select to disable deprecated APIs only up to a certain version of Qt.
#DEFINES += QT_DISABLE_DEPRECATED_BEFORE=0x060000    # disables all the APIs deprecated before Qt 6.0.0

#CONFIG += c++11
QMAKE_CXXFLAGS += -std=c++11

SOURCES += \
    burn.cpp \
        dir.cpp \
        file.cpp \
        isofs.cpp \
        main.cpp \
        mainwindow.cpp \
        node.cpp \
    createimage.cpp \
    recordform.cpp

HEADERS += \
    burn.h \
    burnconst.h \
        constants.h \
        dir.h \
        file.h \
        isofs.h \
        mainwindow.h \
        node.h \
    fs.h \
    createimage.h \
    recordform.h \
    senddata.h

# Default rules for deployment.
qnx: target.path = /tmp/$${TARGET}/bin
else: unix:!android: target.path = /opt/$${TARGET}/bin
!isEmpty(target.path): INSTALLS += target

unix:!macx: LIBS += -L$$PWD/../lib/libisofs/ -lisofs

INCLUDEPATH += $$PWD/../lib/libisofs
DEPENDPATH += $$PWD/../lib/libisofs

FORMS +=

unix:!macx: LIBS += -L$$PWD/../lib/libburn/ -lburn

INCLUDEPATH += $$PWD/../lib/libburn
DEPENDPATH += $$PWD/../lib/libburn

RESOURCES += \
    icons.qrc
