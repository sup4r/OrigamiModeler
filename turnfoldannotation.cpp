#include "turnfoldannotation.h"
#include "vertex2d.h"
#include "vertex3d.h"
#include "halfedge2d.h"
#include "halfedge3d.h"
#include "face2d.h"
#include "face3d.h"
#include "gldrawutil.h"
#include <QOpenGLFunctions>
#include <QVector2D>
#include <QDebug>
TurnFoldAnnotation::TurnFoldAnnotation(Halfedge2d *_he1, Halfedge2d *_he2, QObject *parent)
    :Annotation(parent)
{
    setHe1(_he1);
    setHe2(_he2);
    setColor(QColor(240,240,160));
    setSize(getHe1()->getSize());
    // ZOrder
    Face3d *f1 = he1->face->getFace3d();
    Face3d *f2 = he2->face->getFace3d();
    f1->setZOrderValue(f2->getZOrder()+1);
}

Halfedge2d *TurnFoldAnnotation::getHe1() const
{
    return he1;
}

void TurnFoldAnnotation::setHe1(Halfedge2d *value)
{
    he1 = value;    addHes(value);
}

Halfedge2d *TurnFoldAnnotation::getHe2() const
{
    return he2;
}

void TurnFoldAnnotation::setHe2(Halfedge2d *value)
{
    he2 = value;    addHes(value);
}

void TurnFoldAnnotation::draw()
{
    glLineWidth(getSize()*30);
    glColor3d((float)getColor().red()/255, (float)getColor().green()/255, (float)getColor().blue()/255);
//    GLDrawUtil::drawArrow2D(he1->getCenter(), he2->getCenter());
}
