QT       += core gui widgets

TARGET = cube
TEMPLATE = app

CONFIG += c++1z

SOURCES += \
    src/main.cpp \
    src/scene.cpp \
    src/mainwidget.cpp \
    src/geometry/terrain.cpp \
    src/geometry/cube.cpp \
    src/geometry/geometry.cpp \
    src/geometry/octreeterrain.cpp

HEADERS += \
    src/mainwidget.h \
    src/scene.h \
    src/geometry/terrain.h \
    src/geometry/cube.h \
    src/geometry/geometry.h \
    src/geometry/octreeterrain.h

RESOURCES += \
    shaders.qrc \
    textures.qrc \
    heightmaps.qrc

# install
target.path = ..
INSTALLS += target
