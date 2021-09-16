#-------------------------------------------------
#
# Project created by QtCreator 2019-06-11T20:13:24
#
#-------------------------------------------------

QT       += core gui xml sql svg network

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

TARGET = qgis_demo
TEMPLATE = app

#DESTDIR = ../qgis_demo_app

# The following define makes your compiler emit warnings if you use
# any feature of Qt which has been marked as deprecated (the exact warnings
# depend on your compiler). Please consult the documentation of the
# deprecated API in order to know how to port your code away from it.
DEFINES += QT_DEPRECATED_WARNINGS

# You can also make your code fail to compile if you use deprecated APIs.
# In order to do so, uncomment the following line.
# You can also select to disable deprecated APIs only up to a certain version of Qt.
#DEFINES += QT_DISABLE_DEPRECATED_BEFORE=0x060000    # disables all the APIs deprecated before Qt 6.0.0


SOURCES += \
        main.cpp \
        mainwindow.cpp

HEADERS += \
        mainwindow.h

FORMS += \
        mainwindow.ui

#INCLUDEPATH +=  "D:/OSGeo4W64/apps/qgis/include"
#INCLUDEPATH +=  "D:/OSGeo4W64/include"
INCLUDEPATH += "C:/OSGeo4W64/apps/qgis/include"
INCLUDEPATH += "C:/OSGeo4W64/include"
INCLUDEPATH += "C:/OSGeo4W64/apps/proj-dev/include"
INCLUDEPATH += "C:/OSGeo4W64/apps/proj5/include"
#INCLUDEPATH += "C:/OSGeo4W64/apps/qgis-ltr-dev/plugins"

#LIBS += -L"D:/OSGeo4W64/apps/qgis/lib" -lqgis_core -lqgis_gui



DEFINES += CORE_EXPORT=__declspec(dllimport)
DEFINES += GUI_EXPORT=__declspec(dllimport)

DEFINES += QGISPLUGINDIR=$${QGISPLUGINDIR}

#LIBS +=  "C:/OSGeo4W64/apps/qgis-ltr-dev/plugins/delimitedtextprovider.dll"

win32: LIBS += -L$$PWD/../../../../OSGeo4W64/apps/qgis/lib/ -lqgis_3d

INCLUDEPATH += $$PWD/../../../../OSGeo4W64/apps/qgis/lib
DEPENDPATH += $$PWD/../../../../OSGeo4W64/apps/qgis/lib

win32: LIBS += -L$$PWD/../../../../OSGeo4W64/apps/qgis/lib/ -lqgis_analysis

INCLUDEPATH += $$PWD/../../../../OSGeo4W64/apps/qgis/lib
DEPENDPATH += $$PWD/../../../../OSGeo4W64/apps/qgis/lib

win32: LIBS += -L$$PWD/../../../../OSGeo4W64/apps/qgis/lib/ -lqgis_app

INCLUDEPATH += $$PWD/../../../../OSGeo4W64/apps/qgis/lib
DEPENDPATH += $$PWD/../../../../OSGeo4W64/apps/qgis/lib

win32: LIBS += -L$$PWD/../../../../OSGeo4W64/apps/qgis/lib/ -lqgis_core

INCLUDEPATH += $$PWD/../../../../OSGeo4W64/apps/qgis/lib
DEPENDPATH += $$PWD/../../../../OSGeo4W64/apps/qgis/lib

win32: LIBS += -L$$PWD/../../../../OSGeo4W64/apps/qgis/lib/ -lqgis_gui

INCLUDEPATH += $$PWD/../../../../OSGeo4W64/apps/qgis/lib
DEPENDPATH += $$PWD/../../../../OSGeo4W64/apps/qgis/lib

win32: LIBS += -L$$PWD/../../../../OSGeo4W64/apps/qgis/lib/ -lqgis_native

INCLUDEPATH += $$PWD/../../../../OSGeo4W64/apps/qgis/lib
DEPENDPATH += $$PWD/../../../../OSGeo4W64/apps/qgis/lib

win32: LIBS += -L$$PWD/../../../../OSGeo4W64/apps/qgis/lib/ -lqgis_server

INCLUDEPATH += $$PWD/../../../../OSGeo4W64/apps/qgis/lib
DEPENDPATH += $$PWD/../../../../OSGeo4W64/apps/qgis/lib




win32: LIBS += -L$$PWD/../../../../OSGeo4W64/apps/proj-dev/lib/ -lproj

INCLUDEPATH += $$PWD/../../../../OSGeo4W64/apps/proj-dev/lib
DEPENDPATH += $$PWD/../../../../OSGeo4W64/apps/proj-dev/lib

win32: LIBS += -L$$PWD/../../../../OSGeo4W64/apps/proj5/lib/ -lproj_5_2

INCLUDEPATH += $$PWD/../../../../OSGeo4W64/apps/proj5/lib
DEPENDPATH += $$PWD/../../../../OSGeo4W64/apps/proj5/lib

win32: LIBS += -L$$PWD/../../../../OSGeo4W64/apps/gdal-dev/lib/ -lgdal_i

INCLUDEPATH += $$PWD/../../../../OSGeo4W64/apps/gdal-dev/lib
DEPENDPATH += $$PWD/../../../../OSGeo4W64/apps/gdal-dev/lib

win32: LIBS += -L$$PWD/../../../../OSGeo4W64/apps/gdal2/lib/ -lgdal_i

INCLUDEPATH += $$PWD/../../../../OSGeo4W64/apps/gdal2/lib
DEPENDPATH += $$PWD/../../../../OSGeo4W64/apps/gdal2/lib

win32: LIBS += -L$$PWD/../../../../OSGeo4W64/lib/ -lgdal_i

INCLUDEPATH += $$PWD/../../../../OSGeo4W64/lib
DEPENDPATH += $$PWD/../../../../OSGeo4W64/lib

win32: LIBS += -L$$PWD/../../../../OSGeo4W64/lib/ -lgeos_c

INCLUDEPATH += $$PWD/../../../../OSGeo4W64/lib
DEPENDPATH += $$PWD/../../../../OSGeo4W64/lib

win32: LIBS += -L$$PWD/../../../../OSGeo4W64/lib/ -lgeotiff

INCLUDEPATH += $$PWD/../../../../OSGeo4W64/lib
DEPENDPATH += $$PWD/../../../../OSGeo4W64/lib

win32: LIBS += -L$$PWD/../../../../OSGeo4W64/lib/ -lgeotiff_i

INCLUDEPATH += $$PWD/../../../../OSGeo4W64/lib
DEPENDPATH += $$PWD/../../../../OSGeo4W64/lib
