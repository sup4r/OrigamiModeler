#include "Pleatfoldannotation.h"
#include "halfedge2d.h"
#include "halfedge3d.h"
#include "face2d.h"
#include "face3d.h"
#include "gldrawutil.h"
#include <QVector2D>
#include <QOpenGLFunctions>
#include <QtMath>
PleatFoldAnnotation::PleatFoldAnnotation(Halfedge2d *_he1, Halfedge2d *_he2, QObject *parent)
    :Annotation(parent)
{
    setHe1(_he1);
    setHe2(_he2);
    setColor(QColor(40,120,240));
    setSize(he1->getSize()+1);
    setAnnotationName(PLEAT);
//    // ZOrder
//    Face3d *f1 = he1->face->getFace3d();
//    Face3d *f2 = he2->face->getFace3d();
//    int zOrder = (f1->getZOrder()>f2->getZOrder()+1)?f1->getZOrder():f2->getZOrder()+1;
//    f1->setZOrderValue(zOrder);

}

PleatFoldAnnotation::~PleatFoldAnnotation()
{
    emit(deleted(he1, he2));
}

Halfedge2d *PleatFoldAnnotation::getHe1() const
{
    return he1;
}

void PleatFoldAnnotation::setHe1(Halfedge2d *value)
{
    he1 = value;    addHes(value);
}

Halfedge2d *PleatFoldAnnotation::getHe2() const
{
    return he2;
}

void PleatFoldAnnotation::setHe2(Halfedge2d *value)
{
    he2 = value;    addHes(value);
}

void PleatFoldAnnotation::draw()
{

    glLineWidth(getSize());
    glColor3d((float)getColor().red()/255, (float)getColor().green()/255, (float)getColor().blue()/255);
    QVector2D sv, ev;
    // edge
    sv = he1->getSV2D();
    ev = he1->getEV2D();
    GLDrawUtil::drawLine2D(sv, ev);
    sv = he2->getSV2D();
    ev = he2->getEV2D();
    GLDrawUtil::drawLine2D(sv, ev);
    // arrow
    QVector2D A,B,C,D,E;
    static const double r = 5;
    sv = (he1->getCenter()+he1->face->getG())/2;
    ev = (he2->getCenter()+he2->face->getG())/2;
    A = QVector2D(0, r);
    E = QVector2D((ev-sv).length(), -r);
    C = (A+E)/2;
    B = C +QVector2D(r, r);
    D  = C - QVector2D(r, r);
    glPushMatrix();
    glTranslated(sv.x(), sv.y(), 0);
    double angle = atan2((ev-sv).y(),(ev-sv).x())*360/M_PI/2;
    glRotated(angle, 0, 0, 1);
    GLDrawUtil::drawLine2D(A, B);
    GLDrawUtil::drawLine2D(B, D);
    GLDrawUtil::drawArrow2D(D, E);
    glPopMatrix();

}
