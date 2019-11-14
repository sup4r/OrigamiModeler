#include "sinkfoldannotation.h"
#include "halfedge2d.h"
#include "vertex2d.h"

#include <QOpenGLFunctions>
#include <QtMath>
#include <QVector2D>

SinkFoldAnnotation::SinkFoldAnnotation(Halfedge2d *he, QObject *parent)
    :Annotation(parent)
{
    setHe(he);
//    setSize(he->getSize()+1);
//    setColor(Qt::gray);
    setColor(QColor(0,0,255));
    setAnnotationName(SINK);
}

SinkFoldAnnotation::~SinkFoldAnnotation()
{
    emit(deleted(he));
}

Halfedge2d *SinkFoldAnnotation::getHe() const
{
    return he;
}

void SinkFoldAnnotation::setHe(Halfedge2d *value)
{
    he = value;    addHes(value);
}

void SinkFoldAnnotation::draw()
{
    static const int s = getSize()*2.5;
    glColor3d((float)getColor().red()/255, (float)getColor().green()/255, (float)getColor().blue()/255);
    QVector2D c = he->getCenter();
    float theta = atan2( he->getVec2D().y(),  he->getVec2D().x())*180/M_PI;
    glPushMatrix();
    glTranslatef(c.x(), c.y(), 0);
    glRotatef(theta, 0,0,1);
    glScalef(s,s, 1);
    glTranslatef(0, 1, 0);
    glBegin(GL_TRIANGLES);
    glVertex2d(1, 0.5);
    glVertex2d(-1, 0.5);
    glVertex2d(0, -0.5);
    glEnd();
    glPopMatrix();
    glLineWidth(getSize()*2);
    glBegin(GL_LINES);
    glVertex2d(he->vertex->x, he->vertex->y);
    glVertex2d(he->next->vertex->x, he->next->vertex->y);
    glEnd();
}
