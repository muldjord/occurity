TEMPLATE = app
TARGET = Occurity
DEPENDPATH += .
INCLUDEPATH += .
QT += widgets xml svg multimediawidgets multimedia network
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
           src/combobox.h \
           src/checkbox.h \
           src/abstractchart.h \
           src/svgchart.h \
           src/optotypechart.h \
           src/optotyperow.h \
           src/optosymbol.h \
           src/touchcontrolitem.h \
           src/touchcontrols.h \
           src/jobrunner.h \
           src/messagebox.h

SOURCES += src/main.cpp \
           src/mainwindow.cpp \
           src/videoplayer.cpp \
           src/pindialog.cpp \
           src/preferences.cpp \
           src/about.cpp \
           src/slider.cpp \
           src/combobox.cpp \
           src/checkbox.cpp \
           src/abstractchart.cpp \
           src/svgchart.cpp \
           src/optotypechart.cpp \
           src/optotyperow.cpp \
           src/optosymbol.cpp \
           src/touchcontrolitem.cpp \
           src/touchcontrols.cpp \
           src/jobrunner.cpp \
           src/messagebox.cpp

RESOURCES += occurity.qrc
RC_FILE = occurity.rc
ICON = icon.icns
