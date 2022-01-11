TEMPLATE = app
TARGET = VisuTest
DEPENDPATH += .
INCLUDEPATH += .
QT += widgets xml svg
CONFIG += release
TRANSLATIONS = visutest_da_DK.ts
QMAKE_CXXFLAGS += -std=c++11

include(./VERSION)
DEFINES+=VERSION=\\\"$$VERSION\\\"

# Input
HEADERS += src/mainwindow.h \
           src/preferences.h \
           src/mainsettings.h \
           src/about.h \
           src/slider.h \
           src/abstractchart.h \
           src/fontchart.h \
           src/svgchart.h \
           src/optotypechart.h \
           src/lettersize.h \
           src/letterrow.h \
           src/letter.h


SOURCES += src/main.cpp \
           src/mainwindow.cpp \
           src/preferences.cpp \
           src/about.cpp \
           src/slider.cpp \
           src/abstractchart.cpp \
           src/fontchart.cpp \
           src/svgchart.cpp \
           src/optotypechart.cpp \
           src/lettersize.cpp \
           src/letterrow.cpp \
           src/letter.cpp

RESOURCES += visutest.qrc
RC_FILE = visutest.rc
ICON = icon.icns
