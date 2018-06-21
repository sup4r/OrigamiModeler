#include "glueedgeannotation.h"
#include "halfedge2d.h"
#include "face2d.h"
#include <QVector2D>
#include <QOpenGLFunctions>
#include <QDebug>
GlueEdgeAnnotation::GlueEdgeAnnotation(Halfedge2d *_he, QObject *parent)
    :Annotation(parent)
{
    setHe(_he);
    setD(0.9);
//    setSize(3);
//    he->face->fixNormal();
    setAnnotationName(GLUE_E);
}

GlueEdgeAnnotation::~GlueEdgeAnnotation()
{
    emit(deleted(he));
}

Halfedge2d *GlueEdgeAnnotation::getHe() const
{
    return he;
}

void GlueEdgeAnnotation::setHe(Halfedge2d *value)
{
    he = value;
    setColor(QColor(240,120,120));
//    setSize(he->getSize());
    addHes(value);
}

void GlueEdgeAnnotation::draw()
{
    glLineWidth(getSize());
    glColor3d((float)getColor().red()/255, (float)getColor().green()/255, (float)getColor().blue()/255);
    glBegin(GL_LINE_STRIP);
    glVertex2d(he->getSV2D().x(), he->getSV2D().y());
    glVertex2d(he->getEV2D().x(), he->getEV2D().y());
    glEnd();
}

void GlueEdgeAnnotation::debugPrint()
{
    qDebug()<<"GlueEdgeAnnotation";
}

void GlueEdgeAnnotation::dragEvent(const QVector2D &mouseMove)
{
    const double dx = (double)mouseMove.x();
    static const double dd = 1e-3;
    setD(getD()+dx*dd);
}

double GlueEdgeAnnotation::getD() const
{
    return d;
}

void GlueEdgeAnnotation::setD(double value)
{
    static const double eps = 1e-10;
    if(value>=0. && value <1. && fabs(d-value)>eps){
        d = value;
        emit(dChanged(he, d));
    }
}

