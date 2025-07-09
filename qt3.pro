QT       += core gui opengl svg widgets
greaterThan(QT_MAJOR_VERSION, 4): QT += core gui widgets


INCLUDEPATH += $$PWD/freeglut-2.8.1.tar/freeglut-2.8.1/freeglut-2.8.1/include
INCLUDEPATH += $$PWD/eigen-3.4.0/eigen-3.4.0
INCLUDEPATH += $$PWD/sources
INCLUDEPATH += $$PWD/sources/scene
INCLUDEPATH += $$PWD/sources/animation
INCLUDEPATH += $$PWD/sources/object
INCLUDEPATH += $$PWD/sources/camera
INCLUDEPATH += $$PWD/sources/pass_tracing
INCLUDEPATH += $$PWD/sources/pass_tracing/film
INCLUDEPATH += $$PWD/sources/pass_tracing/core
INCLUDEPATH += $$PWD/sources/pass_tracing/intersectors
INCLUDEPATH += $$PWD/sources/core
INCLUDEPATH += $$PWD/sources/geometry
INCLUDEPATH += $$PWD/jpeg-9c/jpeg-9c

LIBS += -lopengl32
LIBS += -lglu32
# LIBS += -L"D:/OneDrive_2/OneDrive/CGProgramings/qt3/freeglut-2.8.1.tar/freeglut-2.8.1/freeglut-2.8.1/lib/x86" -lfreeglut
LIBS += -L"$$PWD/freeglut-2.8.1.tar/freeglut-2.8.1/freeglut-2.8.1/lib/x86" -lfreeglut
LIBS += -L$$PWD/jpeg-9c/jpeg-9c/Release
LIBS += -ljpeg

CONFIG += c++17

# You can make your code fail to compile if it uses deprecated APIs.
# In order to do so, uncomment the following line.
#DEFINES += QT_DISABLE_DEPRECATED_BEFORE=0x060000    # disables all the APIs deprecated before Qt 6.0.0

SOURCES += \
    sources/animation/interpolator.cpp \
    sources/animation/keyframemanager.cpp \
    sources/camera/camera.cpp \
    sources/core/Jpeg.cpp \
    sources/geometry/Light.cpp \
    sources/geometry/TriMesh.cpp \
    sources/main.cpp \
    sources/mainwindow.cpp \
    sources/object/drawObject.cpp \
    sources/pass_tracing/film/drawfilm.cpp \
    sources/pass_tracing/film/film_buffer.cpp \
    sources/pass_tracing/intersectors/Intersection.cpp \
    sources/scene/CustomScene.cpp \
    sources/scene/GLPreview.cpp \
    sources/scene/myopenglwidget.cpp \
    sources/scene/myopenglwidget_camera.cpp


HEADERS += \
    sources/animation/CameraKeyframe.h \
    sources/animation/DualQuaternion.h \
    sources/animation/interpolator.h \
    sources/animation/keyframemanager.h \
    sources/camera/camera.h \
    sources/conversion_utils.h \
    sources/core/Jpeg.h \
    sources/geometry/Light.h \
    sources/geometry/TriMesh.h \
    sources/mainwindow.h \
    sources/object/drawObject.h \
    sources/pass_tracing/core/Ray.h \
    sources/pass_tracing/core/RayHit.h \
    sources/pass_tracing/film/drawfilm.h \
    sources/pass_tracing/film/film_buffer.h \
    sources/pass_tracing/intersectors/Intersection.h \
    sources/scene/CustomScene.h \
    sources/scene/GLPreview.h \
    sources/scene/myopenglwidget.h \
    sources/scene/myopenglwidget_camera.h


FORMS += \
    mainwindow.ui

# Default rules for deployment.
qnx: target.path = /tmp/$${TARGET}/bin
else: unix:!android: target.path = /opt/$${TARGET}/bin
!isEmpty(target.path): INSTALLS += target

RESOURCES += \
    resorce.qrc

DISTFILES += \
    sources/Doxygenmemo.md \
    sources/toQtmemo.md \
    sources/updatesandmemo.md \
    sources/移植メモ.md \
    移植メモ.md
