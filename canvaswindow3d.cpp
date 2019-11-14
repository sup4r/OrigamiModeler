#include "canvaswindow3d.h"
#include <QMenuBar>
#include <QAction>
#include <QFileDialog>

CanvasWindow3d::CanvasWindow3d(QWidget *parent)
    :CanvasWindowBase(parent)
{
    setGeometry(900, 100, 800, 600);
    setWindowTitle("CanvasWindow3D");

    createCanvas();
    createActions();
    createMenus();
    createDialog();

    // this<->canvas
    connect(this, SIGNAL(face3dCreated(Face3d*)), canvas, SLOT(addFaceFromC2d(Face3d*)));
    connect(canvas, SIGNAL(faceAdded()), this, SLOT(update()));
    connect(canvas, SIGNAL(updated()), this, SLOT(update()));
    connect(this, SIGNAL(keyPressed(QKeyEvent*)), canvas, SLOT(slotKeyPressEvent(QKeyEvent*)));
    // dialog->canvas
    connect(dialog, SIGNAL(allChecked()),canvas, SLOT(allSelectMode()));
    connect(dialog, SIGNAL(vertexChecked()), canvas, SLOT(vertexSelectMode()));
    connect(dialog, SIGNAL(halfedgeChecked()), canvas, SLOT(halfedgeSelectMode()));
    connect(dialog, SIGNAL(faceChecked()), canvas, SLOT(faceSelectMode()));

    // init
    dialog->init();
}

Canvas3d *CanvasWindow3d::getCanvas3d()
{
    return canvas;
}

void CanvasWindow3d::keyPressEvent(QKeyEvent *event)
{
    //canvas->keyPressEvent(event);// protected]
    emit(keyPressed(event));
}

void CanvasWindow3d::createCanvas()
{
    canvas = new Canvas3d(this);
    setCentralWidget(canvas);

}

void CanvasWindow3d::createActions()
{
    /* --- File Menu Actions --- */
    exportOBJAction = new QAction("Export OBJ", this);
    connect(exportOBJAction, SIGNAL(triggered(bool)), this, SLOT(exportOBJ()));
    exportOBJAction2 = new QAction("Export OBJ 2", this);
    connect(exportOBJAction2, SIGNAL(triggered(bool)), this, SLOT(exportOBJ2()));
    loadImageAction = new QAction("Load Texture", this);
    connect(loadImageAction, SIGNAL(triggered(bool)), this, SLOT(loadImage()));

    /* --- View Menu Actions---*/
    drawAxisAction = new QAction("&Axis", this);
    drawAxisAction->setCheckable(true);
    connect(drawAxisAction, SIGNAL(triggered(bool)), canvas, SLOT(setDrawAxisFlag(bool)));
    drawAxisAction->trigger();drawAxisAction->trigger();
    drawGridAction = new QAction("&Grid", this);
    drawGridAction->setCheckable(true);
    connect(drawGridAction, SIGNAL(triggered(bool)), canvas, SLOT(setDrawGridFlag(bool)));
    drawGridAction->trigger();drawGridAction->trigger();
    drawNormalAction = new QAction("&Normal",this);
    drawNormalAction->setCheckable(true);
    connect(drawNormalAction, SIGNAL(triggered(bool)), canvas, SLOT(setDrawNormalFlag(bool)));
    drawNormalAction->trigger();drawNormalAction->trigger();
    drawEdgeConnectionAction = new QAction("Edge&Connection",this);
    drawEdgeConnectionAction->setCheckable(true);
    connect(drawEdgeConnectionAction, SIGNAL(triggered(bool)), canvas, SLOT(setDrawEdgeConnectionFlag(bool)));
    drawEdgeConnectionAction->trigger();drawEdgeConnectionAction->trigger();
    drawVertexAction = new QAction("&Vertex", this);
    drawVertexAction->setCheckable(true);
    connect(drawVertexAction, SIGNAL(triggered(bool)), canvas, SLOT(setDrawVertexFlag(bool)));
//    drawVertexAction->trigger();//20170323
    drawEdgeAction = new QAction("&Edge", this);
    drawEdgeAction->setCheckable(true);
    connect(drawEdgeAction, SIGNAL(triggered(bool)), canvas, SLOT(setDrawEdgeFlag(bool)));
    drawEdgeAction->trigger();
    drawFaceAction = new QAction("&Face", this);
    drawFaceAction->setCheckable(true);
    connect(drawFaceAction, SIGNAL(triggered(bool)), canvas, SLOT(setDrawFaceFlag(bool)));
    drawFaceAction->trigger();
    /* --- Edit Menu --- */
    deleteFacesSelectedAction = new QAction("Delete Selected Faces", this);
    connect(deleteFacesSelectedAction, SIGNAL(triggered(bool)), canvas, SLOT(deleteFacesSelected()));
    flipFacesSelectedAction = new QAction("Flip Selected Faces", this);
    connect(flipFacesSelectedAction, SIGNAL(triggered(bool)), canvas, SLOT(flipFacesSelected()));
    bridgeAutoAction = new QAction("Auto Bridge", this);
    connect(bridgeAutoAction, SIGNAL(triggered(bool)), canvas, SLOT(bridgeAllOverlapEdges()));
}

void CanvasWindow3d::createMenus()
{
    /* --- File Menu --- */
    fileMenu = menuBar()->addMenu("&File");
    fileMenu->addAction(exportOBJAction);
    fileMenu->addAction(loadImageAction);
    fileMenu->addAction(exportOBJAction2);
    /* --- View Menu --- */
    viewMenu = menuBar()->addMenu("&View");
    viewMenu->addAction(drawAxisAction);
    viewMenu->addAction(drawGridAction);
    viewMenu->addAction(drawNormalAction);
    viewMenu->addAction(drawEdgeConnectionAction);
    viewMenu->addAction(drawVertexAction);
    viewMenu->addAction(drawEdgeAction);
    viewMenu->addAction(drawFaceAction);
    /* --- Edit Menu --- */
    editMenu = menuBar()->addMenu("&Edit");
    editMenu->addAction(deleteFacesSelectedAction);
    //editMenu->addAction(flipFacesSelectedAction);
    editMenu->addAction(bridgeAutoAction);
}

void CanvasWindow3d::createDialog()
{
    dialog = new CanvasDialog3d(this);
    dialog->setGeometry(920,750, 200, 100);
}

void CanvasWindow3d::exportOBJ()
{
    QString filename = QFileDialog::getSaveFileName(this, "Export OBJ File", "", "OBJ File (*.obj);;All Files (*)");
    canvas->exportOBJ(filename);
}
void CanvasWindow3d::exportOBJ2()
{
    QString filename = QFileDialog::getSaveFileName(this, "Export OBJ File", "", "OBJ File (*.obj);;All Files (*)");
    canvas->exportOBJ170223(filename);
}

void CanvasWindow3d::loadImage()
{
    QString filename = QFileDialog::getOpenFileName(this, "Select Your Favorite Image File(*.png)", ".", "*.png *.jpg *.bmp *.gif");
    if(!filename.isEmpty()){
        canvas->setTexture(canvas->loadImage(filename));
        qDebug()<<"CanvasWindow3d::loadImage: fileName = "<<filename;
    }
}
