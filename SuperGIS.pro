#-------------------------------------------------
#
# Project created by QtCreator 2019-12-12T18:54:36
#
#-------------------------------------------------

QT       += core gui opengl
LIBS += -lOpenGL32
greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

TARGET = SuperGIS
TEMPLATE = app

# The following define makes your compiler emit warnings if you use
# any feature of Qt which has been marked as deprecated (the exact warnings
# depend on your compiler). Please consult the documentation of the
# deprecated API in order to know how to port your code away from it.
DEFINES += QT_DEPRECATED_WARNINGS

# You can also make your code fail to compile if you use deprecated APIs.
# In order to do so, uncomment the following line.
# You can also select to disable deprecated APIs only up to a certain version of Qt.
#DEFINES += QT_DISABLE_DEPRECATED_BEFORE=0x060000    # disables all the APIs deprecated before Qt 6.0.0

CONFIG += c++11

SOURCES += \
        main.cpp \
        mainwindow.cpp \
    geopostgislinker.cpp \
    geopoint.cpp \
    geopolyline.cpp \
    geopolygon.cpp \
    geolayer.cpp \
    geomap.cpp \
    geooglwidget.cpp \
    layerview.cpp \
    geoobject.cpp \
    geomultipolygon.cpp \
    OpenGLConfig/glcall.cpp \
    OpenGLConfig/shader.cpp \
    OpenGLConfig/renderer.cpp \
    OpenGLConfig/vertexarray.cpp \
    OpenGLConfig/vertexbuffer.cpp \
    OpenGLConfig/indexbuffer.cpp \
    OpenGLConfig/vertexbufferlayout.cpp \
    geomath.cpp \
    grid.cpp \
    gridindex.cpp \
    Dialog/layerattributetabledialog.cpp \
    Dialog/globalsearchresult.cpp \
    Widget/globalsearchwidget.cpp \
    Dialog/kernelcaculatedialog.cpp \
    Widget/kerneldensitytoolwidget.cpp \
    Dialog/globalsearchresult.cpp \
    Dialog/kernelcaculatedialog.cpp \
    Dialog/layerattributetabledialog.cpp \
    OpenGLConfig/glcall.cpp \
    OpenGLConfig/indexbuffer.cpp \
    OpenGLConfig/renderer.cpp \
    OpenGLConfig/shader.cpp \
    OpenGLConfig/vertexarray.cpp \
    OpenGLConfig/vertexbuffer.cpp \
    OpenGLConfig/vertexbufferlayout.cpp \
    Widget/globalsearchwidget.cpp \
    Widget/kerneldensitytoolwidget.cpp \
    geolayer.cpp \
    geomap.cpp \
    geomath.cpp \
    geomultipolygon.cpp \
    geoobject.cpp \
    geooglwidget.cpp \
    geopoint.cpp \
    geopolygon.cpp \
    geopolyline.cpp \
    geopostgislinker.cpp \
    grid.cpp \
    gridindex.cpp \
    layerview.cpp \
    main.cpp \
    mainwindow.cpp \
    geomultilinestring.cpp

HEADERS += \
        mainwindow.h \
    geopostgislinker.h \
    geopoint.h \
    geopolyline.h \
    geopolygon.h \
    geolayer.h \
    geomap.h \
    geooglwidget.h \
    layerview.h \
    geoobject.h \
    geomultipolygon.h \
    OpenGLConfig/glcall.h \
    OpenGLConfig/shader.h \
    OpenGLConfig/renderer.h \
    OpenGLConfig/vertexarray.h \
    OpenGLConfig/vertexbuffer.h \
    OpenGLConfig/indexbuffer.h \
    OpenGLConfig/vertexbufferlayout.h \
    OpenGLConfig/earcut.hpp \
    geomath.h \
    geobase.hpp \
    grid.h \
    gridindex.h \
    Dialog/layerattributetabledialog.h \
    Dialog/globalsearchresult.h \
    Widget/globalsearchwidget.h \
    geofielddefn.h \
    Dialog/kernelcaculatedialog.h \
    Widget/kerneldensitytoolwidget.h \
    Dialog/globalsearchresult.h \
    Dialog/kernelcaculatedialog.h \
    Dialog/layerattributetabledialog.h \
    OpenGLConfig/earcut.hpp \
    OpenGLConfig/glcall.h \
    OpenGLConfig/indexbuffer.h \
    OpenGLConfig/renderer.h \
    OpenGLConfig/shader.h \
    OpenGLConfig/vertexarray.h \
    OpenGLConfig/vertexbuffer.h \
    OpenGLConfig/vertexbufferlayout.h \
    Widget/globalsearchwidget.h \
    Widget/kerneldensitytoolwidget.h \
    geobase.hpp \
    geofielddefn.h \
    geolayer.h \
    geomap.h \
    geomath.h \
    geomultipolygon.h \
    geoobject.h \
    geooglwidget.h \
    geopoint.h \
    geopolygon.h \
    geopolyline.h \
    geopostgislinker.h \
    grid.h \
    gridindex.h \
    layerview.h \
    mainwindow.h \
    geomultilinestring.h

FORMS += \
        mainwindow.ui \
    geopostgislinker.ui \
    layerview.ui

DEFINES += QT_NO_WARNING_OUTPUT

# Default rules for deployment.
qnx: target.path = /tmp/$${TARGET}/bin
else: unix:!android: target.path = /opt/$${TARGET}/bin
!isEmpty(target.path): INSTALLS += target

win32: LIBS += -L$$PWD/../../DATA/gdal221_64_pgsql/gdal221_64_pgsql/lib/ -lgdal_i

INCLUDEPATH += $$PWD/../../DATA/gdal221_64_pgsql/gdal221_64_pgsql/include
DEPENDPATH += $$PWD/../../DATA/gdal221_64_pgsql/gdal221_64_pgsql/include

win32: LIBS += -L$$PWD/../../../../GisDev/GisDev/GisDev/lib/ -lglew32s

INCLUDEPATH += $$PWD/../../../../GisDev/GisDev/GisDev/include
DEPENDPATH += $$PWD/../../../../GisDev/GisDev/GisDev/include

DISTFILES += \
    OpenGLConfig/fragmentshader.glsl \
    OpenGLConfig/colortriangle.vert \
    OpenGLConfig/colortriangle.frag \
    OpenGLConfig/vertexshader.glsl \
    Icons/clear-text-hover.png \
    Icons/search-hover.png \
    Icons/search-press.png \
    Icons/clear-text.ico \
    Icons/search.png \
    Icons/camera-status.png \
    Icons/check-hov.png \
    Icons/check-nor.png \
    Icons/check-part.png \
    Icons/check-sel.png \
    Icons/clear-text-hover.png \
    Icons/clear-text.png \
    Icons/search-hover.png \
    Icons/search-press.png \
    Icons/search.png \
    Icons/app_32x32.ico \
    Icons/app_64x64.ico \
    Icons/camera-2.ico \
    Icons/camera.ico \
    Icons/clear-text.ico \
    Icons/connect-db.ico \
    Icons/geojson.ico \
    Icons/layer.ico \
    Icons/log.ico \
    Icons/map-2.ico \
    Icons/map.ico \
    Icons/new.ico \
    Icons/open.ico \
    Icons/postgresql.ico \
    Icons/remove.ico \
    Icons/rename.ico \
    Icons/search-focus-in.ico \
    Icons/search-focus-out.ico \
    Icons/shapefile.ico \
    Icons/shapefilex.ico \
    Icons/start-editing.ico \
    Icons/stop-editing.ico \
    Icons/table.ico \
    Icons/tool.ico \
    Icons/toolbox.ico \
    Icons/zoom-to-layer.ico

# disable C4819 warning
QMAKE_CXXFLAGS_WARN_ON += -wd4819
QMAKE_CXXFLAGS_WARN_ON += -wd4267
QMAKE_CXXFLAGS_WARN_ON += -wd4430


#win32: LIBS += -L$$PWD/'../../../../Visual Leak Detector/lib/Win64/' -lvld

#INCLUDEPATH += $$PWD/'../../../../Visual Leak Detector/lib/Win64'
#DEPENDPATH += $$PWD/'../../../../Visual Leak Detector/lib/Win64'

#win32{
#
#    CONFIG(debug, debug|release) {
#
#   INCLUDEPATH += D:/Visual Leak Detector/include
#
#    DEPENDPATH += D:/Visual Leak Detector/include
#
#    LIBS += -LD:/Visual Leak Detector/lib/Win64 -lvld
#
#   }
#
#}

win32: LIBS += -L$$PWD/'../../../../Visual Leak Detector/lib/Win64/' -lvld

INCLUDEPATH += $$PWD/'../../../../Visual Leak Detector/lib/Win64'
INCLUDEPATH += $$PWD/'../../../../Visual Leak Detector/include'
DEPENDPATH += $$PWD/'../../../../Visual Leak Detector/lib/Win64'

RESOURCES +=
