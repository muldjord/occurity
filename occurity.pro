TEMPLATE = app
TARGET = Occurity
DEPENDPATH += .
INCLUDEPATH += .
QT += widgets xml svg multimediawidgets multimedia
CONFIG += release
TRANSLATIONS = occurity_da_DK.ts
QMAKE_CXXFLAGS += -std=c++11

include(./VERSION)
DEFINES+=VERSION=\\\"$$VERSION\\\"

# Input
HEADERS += src/mainwindow.h \
           src/videoplayer.h \
           src/pindialog.h \
           src/preferences.h \
           src/mainsettings.h \
           src/about.h \
           src/slider.h \
           src/abstractchart.h \
           src/svgchart.h \
           src/optotypechart.h \
           src/jobrunner.h \
           src/messagebox.h

SOURCES += src/main.cpp \
           src/mainwindow.cpp \
           src/videoplayer.cpp \
           src/pindialog.cpp \
           src/preferences.cpp \
           src/about.cpp \
           src/slider.cpp \
           src/abstractchart.cpp \
           src/svgchart.cpp \
           src/optotypechart.cpp \
           src/jobrunner.cpp \
           src/messagebox.cpp

RESOURCES += occurity.qrc
RC_FILE = occurity.rc
ICON = icon.icns
