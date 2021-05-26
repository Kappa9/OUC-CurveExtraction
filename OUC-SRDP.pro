QT       += core gui

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

CONFIG += c++11

# You can make your code fail to compile if it uses deprecated APIs.
# In order to do so, uncomment the following line.
#DEFINES += QT_DISABLE_DEPRECATED_BEFORE=0x060000    # disables all the APIs deprecated before Qt 6.0.0

SOURCES += \
    clickablelabel.cpp \
    colorpicker.cpp \
    getcolor.cpp \
    getcurve.cpp \
    main.cpp \
    mainwindow.cpp \
    pic.cpp

HEADERS += \
    clickablelabel.h \
    colorpicker.h \
    getcolor.h \
    getcurve.h \
    mainwindow.h \
    pic.h

FORMS += \
    mainwindow.ui

TRANSLATIONS += \
    OUC-SRDP_zh_CN.ts

# Default rules for deployment.
qnx: target.path = /tmp/$${TARGET}/bin
else: unix:!android: target.path = /opt/$${TARGET}/bin
!isEmpty(target.path): INSTALLS += target

INCLUDEPATH += "D:\Program Files\opencv310\build\include"
               "D:\Program Files\opencv310\build\include\opencv"
               "D:\Program Files\opencv310\build\include\opencv2"

LIBS += "D:\Program Files\opencv310\opencv-build\lib\libopencv_*.a"

DISTFILES += \
    qt/etc/qt.conf \
    src/colorpicker.png

RESOURCES += \
    image.qrc \
    platform.qrc
