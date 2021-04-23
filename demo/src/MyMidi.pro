QT       += core gui

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

CONFIG += c++11

# You can make your code fail to compile if it uses deprecated APIs.
# In order to do so, uncomment the following line.
#DEFINES += QT_DISABLE_DEPRECATED_BEFORE=0x060000    # disables all the APIs deprecated before Qt 6.0.0

SOURCES += \
    Midi/device.cpp \
    Midi/devicedescription.cpp \
    Midi/driver.cpp \
    Midi/framework.cpp \
    Midi/port.cpp \
    main.cpp \
    mainwindow.cpp \
    mididevicestree.cpp \
    midieventlistwidget.cpp \
    miditestsource.cpp \
    qlabeldroptarget.cpp

HEADERS += \
    Midi/device.h \
    Midi/devicedescription.h \
    Midi/driver.h \
    Midi/framework.h \
    Midi/port.h \
    mainwindow.h \
    mididevicestree.h \
    midieventlistwidget.h \
    miditestsource.h \
    qlabeldroptarget.h

FORMS += \
    mainwindow.ui \
    miditestsource.ui

win32* {
    LIBS += -lwinmm
    SOURCES += \
      Midi/OS/midiwin.cpp
    HEADERS += \
      Midi/OS/midiwin.h
}
linux* {
  LIBS += -lasound
  SOURCES += \
    Midi/OS/midilinux.cpp
  HEADERS += \
    Midi/OS/midilinux.h
}
# Default rules for deployment.
qnx: target.path = /tmp/$${TARGET}/bin
else: unix:!android: target.path = /opt/$${TARGET}/bin
!isEmpty(target.path): INSTALLS += target

RESOURCES += \
    Icons.qrc
