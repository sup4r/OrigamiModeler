#include <QApplication>
#include "canvaswindow2d.h"
#include "canvaswindow3d.h"

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);

    CanvasWindow2d *w2d = new CanvasWindow2d();
    CanvasWindow3d *w3d = new CanvasWindow3d();

    QObject::connect(w2d, SIGNAL(closed()), w3d, SLOT(close()));
    QObject::connect(w2d, SIGNAL(face3dCreated(Face3d*)), w3d, SIGNAL(face3dCreated(Face3d*)));
    w2d->show();
    w3d->show();
    /****** DANGER ZONE ******/
    w2d->getCanvas2d()->setCanvas3d(w3d->getCanvas3d());
    w3d->getCanvas3d()->setCanvas2d(w2d->getCanvas2d());

    w2d->getCanvas2d()->getModel()->setM3d(w3d->getCanvas3d()->getModel());
    /*******************************/
    return a.exec();
}
