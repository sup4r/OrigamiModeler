#include "canvaswindow2d.h"
#include <QFileDialog>
#include <QDebug>
#include <QAction>
#include <QMenu>
#include <QMenuBar>
#include <QStatusBar>
#include <opencv2/opencv.hpp>
#include <opencv2/highgui/highgui.hpp>
#include <opencv2/imgproc/imgproc.hpp>
#include <vector>
#include <iostream>

using namespace cv;

CanvasWindow2d::CanvasWindow2d(QWidget *parent)
    :CanvasWindowBase(parent)
{
    createCanvas();
    createActions();
    createMenus();

    dialog = new MyDialog(this);
    //dialog->show();

    dialog2 = new CanvasDialog2d(canvas, this);
    //dialog2->show();

    editORDialog = new EditORDialog(this);
    //editORDialog->show();

    connect(dialog, SIGNAL(face3dCreated(Face3d*)), this , SIGNAL(face3dCreated(Face3d*)));
    connect(canvas, SIGNAL(face2dCreated(Face2d*)), dialog, SLOT(createCreateFace3dButton(Face2d*)));
    connect(canvas, SIGNAL(face2dCreated(Face2d*)), editORDialog, SLOT(createHBox(Face2d*)));
    connect(editORDialog, SIGNAL(face3dCreated(Face3d*)), this, SIGNAL(face3dCreated(Face3d*)));

    /* --- key event --- */
    connect(this, SIGNAL(keyPressed(QKeyEvent*)), canvas, SLOT(slotKeyPressEvent(QKeyEvent*)));
    connect(this, SIGNAL(keyReleased(QKeyEvent*)), canvas, SLOT(slotKeyReleaseEvent(QKeyEvent*)));

    setGeometry(100,100,800,600);
    setWindowTitle("CanvasWindow2D");

    statusBar();
}

Canvas2d *CanvasWindow2d::getCanvas2d()
{
    return canvas;
}

void CanvasWindow2d::keyPressEvent(QKeyEvent *event)
{
    emit(keyPressed(event));
}

void CanvasWindow2d::keyReleaseEvent(QKeyEvent *event)
{
    emit(keyReleased(event));
}

void CanvasWindow2d::updateStatusBar()
{

}

void CanvasWindow2d::createCanvas(){
    canvas = new Canvas2d(this);
    setCentralWidget(canvas);
    connect(canvas, SIGNAL(updated()), this, SLOT(update()));
}

void CanvasWindow2d::createActions()
{
    /* tuika @ 2018 */
    //loadImageAction = new QAction("Auto", this);
   //loadImageAction->setShortcut(tr("Ctrl+L"));
    //loadImageAction->setStatusTip("Auto kettei vertex");
    //connect(loadImageAction, SIGNAL(triggered(bool)), canvas, SLOT(autoVertex()));
    //fileActions.push_back(loadImageAction);
    /* tuika @ 2018 fin*/

    // File
    loadImageAction = new QAction("Load Image", this);
    loadImageAction->setShortcut(tr("Ctrl+I"));
    loadImageAction->setStatusTip("Load Image File");
    connect(loadImageAction, SIGNAL(triggered(bool)), this, SLOT(loadImage()));
    fileActions.push_back(loadImageAction);

    QAction *fileAction;
    fileAction = new QAction("Save File", this);
    fileAction->setShortcut(tr("Ctrl+S"));
    connect(fileAction, SIGNAL(triggered(bool)), canvas, SLOT(saveToFile()));
    fileActions.push_back(fileAction);

    QAction *debugAction;
    debugAction = new QAction("Debug", this);
    debugAction->setShortcut(tr("Ctrl+D"));
    connect(debugAction, SIGNAL(triggered(bool)), canvas, SLOT(loadFromFile3()));
    debugActions.push_back(debugAction);

    fileAction = new QAction("Load File", this);
    fileAction->setShortcut(tr("Ctrl+O"));
    connect(fileAction, SIGNAL(triggered(bool)), canvas, SLOT(loadFromFile()));
    fileActions.push_back(fileAction);

    // Mode
    QAction *modeAction;
    modeAction = new QAction("Polygon", this);
    modeAction->setStatusTip("Polygon Mode");
    modeAction->setShortcut(tr("Q"));
//    modeAction->setCheckable(true);
    modeActions.push_back(modeAction);
    connect(modeAction, SIGNAL(triggered(bool)), this, SLOT(setCreateMode(bool)));
    modeAction->trigger(); //  Face Mode <- active

    modeAction = new QAction("Annotation", this);
    modeAction->setStatusTip("Annotation Mode");
    modeAction->setShortcut(tr("W"));
//    modeAction->setCheckable(true);
    modeActions.push_back(modeAction);
    connect(modeAction, SIGNAL(triggered(bool)), this, SLOT(setGlueMode(bool)));

    modeAction = new QAction("Edit", this);
    modeAction->setStatusTip("Edit Mode");
    modeAction->setShortcut(tr("E"));
//    modeAction->setCheckable(true);
    modeActions.push_back(modeAction);
    connect(modeAction, SIGNAL(triggered(bool)), this, SLOT(setEditMode(bool)));


    // Tool
    QAction *toolAction;
//    toolAction = new QAction(" Mirror Bridge", this);
//    toolAction->setStatusTip(" bridge between selected edge and the mirror");
//    toolActions.push_back(toolAction);
//    connect(toolAction, SIGNAL(triggered(bool)), canvas, SLOT(addSelectEdgesToGlueEdges()));

//    toolAction = new QAction("Delete Mirror Bridge", this);
//    toolAction->setStatusTip("delete bridge between selected edge and the mirror");
//    toolActions.push_back(toolAction);
//    connect(toolAction, SIGNAL(triggered(bool)), canvas, SLOT(removeSelectEdgesFromGlueEdges()));

    toolAction = new QAction("Half Fold", this);
    toolActions.push_back(toolAction);
    connect(toolAction, SIGNAL(triggered(bool)), canvas, SLOT(addSelectEdgesToGlueEdges2()));

    toolAction = new QAction("Tucking", this);
    toolActions.push_back(toolAction);
    connect(toolAction, SIGNAL(triggered(bool)), canvas, SLOT(createOrikomiEdgeAnnotationOnSelectedEdge()));

    toolAction = new QAction("Sink Fold", this);
    toolActions.push_back(toolAction);
    connect(toolAction, SIGNAL(triggered(bool)), canvas, SLOT(createSinkFoldWithSelectedEdge()));

    toolAction = new QAction("Inside Reverse Fold", this);
    toolActions.push_back(toolAction);
    connect(toolAction, SIGNAL(triggered(bool)), canvas, SLOT(createInsideReverseFoldWithSelectEdges()));

    toolAction = new QAction("Outside Reverse Fold", this);
    toolActions.push_back(toolAction);
    connect(toolAction, SIGNAL(triggered(bool)), canvas, SLOT(createOutsideReverseFoldWithSelectEdges()));

    toolAction = new QAction("Pleat Fold", this);
    toolActions.push_back(toolAction);
    connect(toolAction, SIGNAL(triggered(bool)), canvas, SLOT(createPleatFoldWithSelectEdges()));

    toolAction = new QAction("Pigs Leg", this);
    toolActions.push_back(toolAction);
    connect(toolAction, SIGNAL(triggered(bool)), canvas, SLOT(createPigsLegAnnotationWithSelectedEdges()));

    toolAction = new QAction("Mirror", this);
    toolActions.push_back(toolAction);
    connect(toolAction, SIGNAL(triggered(bool)), canvas, SLOT(createMirrorConnectionBetweenSelectedEdges()));
//    toolAction = new QAction(" Pleate Fold with Face", this);
//    toolAction->setStatusTip(" pleat fold structure from 2 selected faces");
//    toolActions.push_back(toolAction);
//    connect(toolAction, SIGNAL(triggered(bool)), canvas, SLOT(createPleatFoldWithSelectFaces()));

//    toolAction = new QAction(" Turn Fold", this);
//    toolAction->setStatusTip("status");
//    toolActions.push_back(toolAction);
//    connect(toolAction, SIGNAL(triggered(bool)), canvas, SLOT(createTurnFoldWithSelectEdges()));


    // ------------------------------------------------------------------------------------------------------------------------ //
    //toolAction = new QAction("Delete", this);
    //toolActions.push_back(toolAction);
    //connect(toolAction, SIGNAL(triggered(bool)), canvas, SLOT(deleteSelectAnnotations()));

    /* layer */
    QAction *layerAction = new QAction("Connect Layers", this);
    layerActions.push_back(layerAction);
    connect(layerAction, SIGNAL(triggered(bool)), canvas, SLOT(createMirrorConnectionBetweenSelectedEdges()));

    layerAction = new QAction("Add Layer", this);
    layerAction->setShortcut(tr("Ctrl+N"));
    layerActions.push_back(layerAction);
    connect(layerAction, SIGNAL(triggered(bool)), canvas, SLOT(createPairModelsSlot()));

    layerAction  = new QAction("Trash Layer", this);
    layerAction->setShortcut(tr("Delete"));
    layerActions.push_back(layerAction);
    connect(layerAction, SIGNAL(triggered(bool)), canvas, SLOT(trashCurrentModel()));

}

void CanvasWindow2d::createMenus()
{
    fileMenu = menuBar()->addMenu("&File");
    fileMenu->addActions(fileActions.toList());

    modeMenu = menuBar()->addMenu("&Mode");
    modeMenu->addActions(modeActions.toList());

    toolMenu = menuBar()->addMenu("&Tool");
    toolMenu->addActions(toolActions.toList());

    layerMenu = menuBar()->addMenu("&Layer");
    layerMenu->addActions(layerActions.toList());

    debugMenu = menuBar()->addMenu("&Debug");
    debugMenu->addActions(debugActions.toList());
}

/* --- private slots --- */
void CanvasWindow2d::setEditMode(bool checked)
{
//    if(!checked)return;
//    uncheckModeActionsExceptOne(EDIT);
    canvas->setMode(EDIT);
    statusBar()->showMessage("Edit Mode");
}

void CanvasWindow2d::setCreateMode(bool checked)
{
//    if(!checked)return;
//    uncheckModeActionsExceptOne(CREATE);
    canvas->setMode(CREATE);
    statusBar()->showMessage(" Polygon Mode");
}

void CanvasWindow2d::setGlueMode(bool checked)
{
//    if(!checked)return;
//    uncheckModeActionsExceptOne(GLUE);
    canvas->setMode(GLUE);
    statusBar()->showMessage(" Annotation Mode");
}

/* --- public slots --- */
void CanvasWindow2d::loadImage(){
    QString filename = QFileDialog::getOpenFileName(this, "Select Your Favorite Image File(*.png)", ".", "*.png *.jpg *.bmp *.gif");
    if(!filename.isEmpty()){
        canvas->loadImage(filename);
        //qDebug()<<"CanvasWindow2d::loadImage: fileName = "<<fileName;
    }
    //canvas->autoVertex();
}

void CanvasWindow2d::uncheckModeActionsExceptOne(int id)
{
    for(int i =0; i<modeActions.size(); ++i){
        if(i == id){
            continue;
        }
        modeActions.at(i)->setChecked(false);
    }
}
