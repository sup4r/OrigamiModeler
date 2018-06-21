#-------------------------------------------------
#
# Project created by QtCreator 2016-04-14T18:41:03
#
#-------------------------------------------------

QT       += core gui opengl

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

TARGET = OrigamiLikeModeler2
TEMPLATE = app

win32:DEPENDPATH  += "C:\dev\opencv-3.0.0\build\include"
win32:INCLUDEPATH += "C:\dev\opencv-3.0.0\build\include"
win32:LIBS += -L"C:\dev\opencv-3.0.0\build\x86\vc12\lib"
win32:LIBS += -lopencv_world300d

SOURCES += main.cpp\
        mainwindow.cpp \
    canvas2d.cpp \
    canvas3d.cpp \
    face2d.cpp \
    face3d.cpp \
    halfedge2d.cpp \
    halfedge3d.cpp \
    model2d.cpp \
    model3d.cpp \
    vertex2d.cpp \
    vertex3d.cpp \
    canvaswindowbase.cpp \
    canvaswindow2d.cpp \
    canvaswindow3d.cpp \
    canvasbase.cpp \
    finddialog.cpp \
    modelbase.cpp \
    mydialog.cpp \
    createface3dbutton.cpp \
    mycamera3d.cpp \
    editordialog.cpp \
    canvasdialog3d.cpp \
    oriline.cpp \
    subface.cpp \
    orivertex.cpp \
    orihalfedge.cpp \
    oriedge.cpp \
    subfacebuilder.cpp \
    globject.cpp \
    gldrawutil.cpp \
    annotation.cpp \
    insidereversefoldannotation.cpp \
    glueedgeannotation.cpp \
    pleatfoldannotation.cpp \
    turnfoldannotation.cpp \
    segment2d.cpp \
    outsidereversefoldannotation.cpp \
    sinkfoldannotation.cpp \
    pigslegannotation.cpp \
    orikomiedgeannotation.cpp \
    mirrorplane.cpp \
    mirrorconnectannotation.cpp \
    canvasdialog2d.cpp

HEADERS  += mainwindow.h \
    canvas2d.h \
    canvas3d.h \
    face2d.h \
    face3d.h \
    halfedge2d.h \
    halfedge3d.h \
    model2d.h \
    model3d.h \
    vertex2d.h \
    vertex3d.h \
    canvaswindowbase.h \
    canvaswindow2d.h \
    canvaswindow3d.h \
    canvasbase.h \
    finddialog.h \
    modelbase.h \
    mydialog.h \
    createface3dbutton.h \
    mycamera3d.h \
    editordialog.h \
    canvasdialog3d.h \
    mode.h \
    oriline.h \
    subface.h \
    orivertex.h \
    orihalfedge.h \
    oriedge.h \
    subfacebuilder.h \
    globject.h \
    gldrawutil.h \
    annotation.h \
    insidereversefoldannotation.h \
    glueedgeannotation.h \
    pleatfoldannotation.h \
    turnfoldannotation.h \
    ../../Visual Studio 2013/Projects/OpenOrigami/OpenOrigami/Segment2D.h \
    segment2d.h \
    outsidereversefoldannotation.h \
    sinkfoldannotation.h \
    pigslegannotation.h \
    orikomiedgeannotation.h \
    annotationname.h \
    mirrorplane.h \
    mirrorconnectannotation.h \
    canvasdialog2d.h

