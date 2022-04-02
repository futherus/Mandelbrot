QT       += core gui

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

CONFIG += c++11

# You can make your code fail to compile if it uses deprecated APIs.
# In order to do so, uncomment the following line.
#DEFINES += QT_DISABLE_DEPRECATED_BEFORE=0x060000    # disables all the APIs deprecated before Qt 6.0.0

SOURCES += \
    compute.cpp \
    get_fps.cpp \
    main.cpp \
    mandelbrot_GUI.cpp

HEADERS += \
    compute.h \
    get_fps.h \
    mandelbrot_GUI.h

QMAKE_CXXFLAGS = -mavx -mavx2 -O3

# Default rules for deployment.
qnx: target.path = /tmp/$${TARGET}/bin
else: unix:!android: target.path = /opt/$${TARGET}/bin
!isEmpty(target.path): INSTALLS += target

FORMS +=
