#ifndef CANVASWINDOW3D_H
#define CANVASWINDOW3D_H
#include "canvaswindowbase.h"
#include "canvas3d.h"
#include "canvasdialog3d.h"
class QMenu;
class QAction;

class CanvasWindow3d : public CanvasWindowBase
{
    Q_OBJECT
public:
    CanvasWindow3d(QWidget *parent  = 0);
    Canvas3d *getCanvas3d();
protected:
    void keyPressEvent(QKeyEvent *event);
private:
    Canvas3d *canvas;
    CanvasDialog3d *dialog;

    /* --- File Menu ---*/
    QMenu *fileMenu;
    QAction *exportOBJAction;
    QAction *exportOBJAction2;
    QAction *loadImageAction;
    /* --- View Menu --- */
    QMenu *viewMenu;
    QAction *drawAxisAction;
    QAction *drawGridAction;
    QAction *drawNormalAction;
    QAction *drawEdgeConnectionAction;
    QAction *drawVertexAction;
    QAction *drawEdgeAction;
    QAction *drawFaceAction;

    /* --- Edit Menu --- */
    QMenu *editMenu;
    QAction *deleteFacesSelectedAction;
    QAction *flipFacesSelectedAction;
    QAction *bridgeAutoAction;
    void createCanvas();
    void createActions();
    void createMenus();
    void createDialog();

signals:
    void face3dCreated(Face3d *);
    void keyPressed(QKeyEvent *);
private slots:
    void exportOBJ();
    void exportOBJ2();
public slots:
    void loadImage();

};

#endif // CANVASWINDOW3D_H
