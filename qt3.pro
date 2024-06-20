QT       += core gui opengl svg widgets
greaterThan(QT_MAJOR_VERSION, 4): QT += core gui widgets


INCLUDEPATH += $$PWD/freeglut-2.8.1.tar/freeglut-2.8.1/freeglut-2.8.1/include
INCLUDEPATH += $$PWD/eigen-3.4.0/eigen-3.4.0

LIBS += -lopengl32
LIBS += -lglu32
# LIBS += -L"D:/OneDrive_2/OneDrive/CGProgramings/qt3/freeglut-2.8.1.tar/freeglut-2.8.1/freeglut-2.8.1/lib/x86" -lfreeglut
LIBS += -L"$$PWD/freeglut-2.8.1.tar/freeglut-2.8.1/freeglut-2.8.1/lib/x86" -lfreeglut

CONFIG += c++17

# You can make your code fail to compile if it uses deprecated APIs.
# In order to do so, uncomment the following line.
#DEFINES += QT_DISABLE_DEPRECATED_BEFORE=0x060000    # disables all the APIs deprecated before Qt 6.0.0

SOURCES += \
    CustomScene.cpp \
    camera.cpp \
    drawObject.cpp \
    main.cpp \
    mainwindow.cpp \
    myopenglwidget.cpp \
    myopenglwidget_camera.cpp

HEADERS += \
    CustomScene.h \
    camera.h \
    drawObject.h \
    mainwindow.h \
    myopenglwidget.h \
    myopenglwidget_camera.h

FORMS += \
    mainwindow.ui

# Default rules for deployment.
qnx: target.path = /tmp/$${TARGET}/bin
else: unix:!android: target.path = /opt/$${TARGET}/bin
!isEmpty(target.path): INSTALLS += target

RESOURCES += \
    resorce.qrc
