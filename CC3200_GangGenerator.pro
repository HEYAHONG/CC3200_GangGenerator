QT       += core gui xml

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

CONFIG += c++11

# The following define makes your compiler emit warnings if you use
# any Qt feature that has been marked deprecated (the exact warnings
# depend on your compiler). Please consult the documentation of the
# deprecated API in order to know how to port your code away from it.
DEFINES += QT_DEPRECATED_WARNINGS

# You can also make your code fail to compile if it uses deprecated APIs.
# In order to do so, uncomment the following line.
# You can also select to disable deprecated APIs only up to a certain version of Qt.
#DEFINES += QT_DISABLE_DEPRECATED_BEFORE=0x060000    # disables all the APIs deprecated before Qt 6.0.0

SOURCES += \
    buildthread.cpp \
    cmdsettings.cpp \
    main.cpp \
    mainwindow.cpp \
    setflashsize.cpp

HEADERS += \
    buildthread.h \
    cmdsettings.h \
    mainwindow.h \
    setflashsize.h

FORMS += \
    cmdsettings.ui \
    mainwindow.ui \
    setflashsize.ui
#windows下的rc文件
RC_FILE+=window_resources.rc

# Default rules for deployment.
qnx: target.path = /tmp/$${TARGET}/bin
else: unix:!android: target.path = /opt/$${TARGET}/bin
!isEmpty(target.path): INSTALLS += target

RESOURCES += \
    qrc.qrc

DISTFILES += \
    window_resources.ico \
    window_resources.manifest \
    window_resources.rc
