#include "orikomiedgeannotation.h"
#include "halfedge2d.h"
#include "vertex2d.h"

#include <QOpenGLFunctions>
#include <QtMath>
#include <QVector2D>

OrikomiEdgeAnnotation::OrikomiEdgeAnnotation(Halfedge2d *_he, QObject *parent)
    :Annotation(parent)
{
    setHe(_he);
    //setSize(he->getSize()+1);
//    setColor(Qt::gray);
    setColor(QColor(0,255,0));
    setAnnotationName(ORIKOMI);
}

Halfedge2d *OrikomiEdgeAnnotation::getHe() const
{
    return he;
}

void OrikomiEdgeAnnotation::setHe(Halfedge2d *value)
{
    he = value;
    addHes(value);
}

void OrikomiEdgeAnnotation::draw()
{
    glColor3d((float)getColor().red()/255, (float)getColor().green()/255, (float)getColor().blue()/255);
    glLineWidth(getSize()*2);
    QVector2D c = he->getCenter();
    float theta = atan2( he->getVec2D().y(),  he->getVec2D().x())*180/M_PI;
    glPushMatrix();
    glTranslatef(c.x(), c.y(), 0);
    glRotatef(theta, 0,0,1);
    glScalef(10,10, 1);
    glTranslatef(0, 1, 0);
    glBegin(GL_LINE_STRIP);

    glVertex2d(1, 0.5);
    glVertex2d(0, -0.5);
    glVertex2d(-1, 0.5);

    glEnd();
    glPopMatrix();
    glLineWidth(getSize()*2);
    glBegin(GL_LINES);
    glVertex2d(he->vertex->x, he->vertex->y);
    glVertex2d(he->next->vertex->x, he->next->vertex->y);
    glEnd();
}
