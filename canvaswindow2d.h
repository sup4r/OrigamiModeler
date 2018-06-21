#ifndef CANVASWINDOW2D_H
#define CANVASWINDOW2D_H

#include "canvaswindowbase.h"
#include "canvas2d.h"
#include "mydialog.h"
#include "editordialog.h"
#include "mode.h"
#include <QMenu>
#include <QVector>
#include "canvasdialog2d.h"
class QStatusBar;

class CanvasWindow2d : public CanvasWindowBase
{
    Q_OBJECT
public:
    CanvasWindow2d(QWidget *parent = 0);
    Canvas2d *getCanvas2d();
    void createCanvas();
protected:
    void keyPressEvent(QKeyEvent *event);
    void keyReleaseEvent(QKeyEvent *event);
private:
    Canvas2d *canvas;
    MyDialog *dialog;
    EditORDialog *editORDialog;
    CanvasDialog2d *dialog2;

    //void createCanvas();
    void createActions();
    void createMenus();
    QMenu *fileMenu;
    QAction *loadImageAction;
    QVector<QAction*>fileActions;
    QString fileName;
    QMenu *modeMenu;
    QVector<QAction*> modeActions;
    QMenu *toolMenu;
    QVector<QAction*> toolActions;
    QMenu *debugMenu;
    QVector<QAction*> debugActions;
    QMenu *layerMenu;
    QVector<QAction*> layerActions;

signals:
    void face3dCreated(Face3d *);
    void keyPressed(QKeyEvent *);
    void keyReleased(QKeyEvent *);

private slots:
    void updateStatusBar();
    void loadImage();
    void setEditMode(bool checked);
    void setCreateMode(bool checked);
    void setGlueMode(bool checked);
    void uncheckModeActionsExceptOne(int id);
};

#endif // CANVASWINDOW2D_H
