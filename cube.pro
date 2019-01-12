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
    src/geometry/octreeterrain.cpp \
    src/camera.cpp \
    src/postprocessing.cpp \
    src/geometry/sphere.cpp \
    src/noise/SimplexNoise.cpp

HEADERS += \
    src/mainwidget.h \
    src/scene.h \
    src/geometry/terrain.h \
    src/geometry/cube.h \
    src/geometry/geometry.h \
    src/geometry/octreeterrain.h \
    src/camera.h \
    src/postprocessing.h \
    src/geometry/sphere.h \
    src/noise/SimplexNoise.h \
    src/light.h \
    src/geometry/boundingbox.h


RESOURCES += \
    shaders.qrc \
    textures.qrc \
    heightmaps.qrc \
    geometries.qrc


# remove possible other optimization flags
QMAKE_CXXFLAGS_RELEASE -= -O
QMAKE_CXXFLAGS_RELEASE -= -O1
QMAKE_CXXFLAGS_RELEASE -= -O2

# add the desired -O3 if not present
QMAKE_CXXFLAGS_RELEASE *= -O3

# install
target.path = ..
INSTALLS += target
