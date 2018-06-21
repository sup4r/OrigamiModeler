#include "pigslegannotation.h"
#include "halfedge2d.h"
#include "face2d.h"
#include "face3d.h"
#include "gldrawutil.h"
#include <QtMath>
#include <QDebug>
#include <QOpenGLFunctions>
PigsLegAnnotation::PigsLegAnnotation(Halfedge2d *_he1, Halfedge2d *_he2, Halfedge2d *_he3, QObject *parent)
    :Annotation(parent)
{
    setHe1(_he1);
    setHe2(_he2);
    setHe3(_he3);
//    setColor(Qt::gray);
    setColor(QColor(120,240, 40));
//    setSize(he1->getSize()+1);
    // ZOrder
    Face3d *f1U = he1->face->getFace3d();
    Face3d *f2 = he2->face->getFace3d();
    Face3d *f3 = he3->face->getFace3d();
    if(f1U->getFace2d()->getMirrorTimes()!=2){
        qDebug()<<"PigsLegAnnotation::PigsLegAnnotation: f1U->getFace2d()->getMirrorTimes()!=2";
    }
    Face3d *f1B = f1U->getMirrorNext();
    f1B->setZOrderValue(((f2->getZOrder()>f3->getZOrder())?f2->getZOrder():f3->getZOrder())+1);
    f1U->setZOrderValue(f1B->getZOrder()+1);
    setAnnotationName(PIG);
}

PigsLegAnnotation::~PigsLegAnnotation()
{

}

void PigsLegAnnotation::draw()
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
    sv = he3->getSV2D();
    ev = he3->getEV2D();
//    GLDrawUtil::drawLine2D(sv, ev);
    // arrow
    GLDrawUtil::drawArrow2D(he2->getCenter(), he1->face->getG());
    GLDrawUtil::drawArrow2D(he3->getCenter(), he1->face->getG());
//    GLDrawUtil::drawCircle2D(he1->face->getG(),5);
}

Halfedge2d *PigsLegAnnotation::getHe1() const
{
    return he1;
}

void PigsLegAnnotation::setHe1(Halfedge2d *value)
{
    he1 = value;    addHes(value);
}

Halfedge2d *PigsLegAnnotation::getHe2() const
{
    return he2;
}

void PigsLegAnnotation::setHe2(Halfedge2d *value)
{
    he2 = value;    addHes(value);
}

Halfedge2d *PigsLegAnnotation::getHe3() const
{
    return he3;
}

void PigsLegAnnotation::setHe3(Halfedge2d *value)
{
    he3 = value;    addHes(value);
}

void PigsLegAnnotation::dragEvent(const QVector2D &mouseMove)
{

}
