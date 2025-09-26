QT       += core gui

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

TARGET = RecruitmentHelper
TEMPLATE = app

SOURCES += \
    main.cpp \
    mainwindow.cpp \
    snippetmanager.cpp \
    inputsimulator.cpp

HEADERS += \
    mainwindow.h \
    snippetmanager.h \
    inputsimulator.h

RESOURCES += \
    resources.qrc

# 平台特定代码
win32 {
    QT += winextras
    LIBS += -luser32
}

macx {
    LIBS += -framework Carbon -framework ApplicationServices
    QMAKE_CXXFLAGS += -x objective-c++
}

# 程序图标
RC_ICONS = resources/icon.ico
ICON = resources/icon.icns