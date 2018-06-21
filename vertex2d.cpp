#pragma once
#include "vertex2d.h"
#include "vertex3d.h"
#include "halfedge2d.h"
#include <QPoint>
#include <QOpenGLFunctions>
Vertex2d::Vertex2d(double _x, double _y, QObject *parent)
    : GLObject(parent)
{
    initPosition(_x, _y);
    v3d = NULL;
    initTest();
    id = num++;
}
int Vertex2d::num;

void Vertex2d::initPosition(double _x, double _y){
    x = _x; y=_y;
    setMoved(true);
    updated(QVector2D(x,y));
}

void Vertex2d::setPosition(double _x, double _y)
{
    static const double epsilon = 1e-1;
    if(fabs(x-_x)>epsilon||fabs(y-_y)>epsilon){
        initPosition(_x, _y);
    }
}

void Vertex2d::movePosition(double vx, double vy)
{
    setPosition(x+vx, y+vy);
}

void Vertex2d::draw()
{
    glPointSize(getSize());
    glColor3d((float)getColor().red()/255, (float)getColor().green()/255, (float)getColor().blue()/255);
    glBegin(GL_POINTS);
    glVertex2d(x, y);
    glEnd();
}

void Vertex2d::drawWithoutSize()
{
    glColor3d((float)getColor().red()/255, (float)getColor().green()/255, (float)getColor().blue()/255);

    glBegin(GL_POINTS);
    glVertex2d(x, y);
    glEnd();
}

void Vertex2d::drawWithoutAll()
{
    glBegin(GL_POINTS);
    glVertex2d(x, y);
    glEnd();
}

void Vertex2d::setHalfedge(Halfedge2d *he)
{
    halfedge = he;
    if(halfedge->vertex == NULL){
        halfedge->setVertex(this);
    }
}

void Vertex2d::setVertex2d(Vertex2d *v)
{
    connect(this, SIGNAL(updated(QVector2D)), v, SLOT(sync2D(QVector2D)));
    connect(v, SIGNAL(updated(QVector2D)), this, SLOT(sync2D(QVector2D)));

}

bool Vertex2d::getMoved() const
{
    return moved;
}

void Vertex2d::setMoved(bool value)
{
    moved = value;
}

int Vertex2d::getIdForExport() const
{
    return idForExport;
}

void Vertex2d::setIdForExport(int value)
{
    idForExport = value;
}

int Vertex2d::getId() const
{
    return id;
}

void Vertex2d::setId(int value)
{
    id = value;
}

void Vertex2d::setVertex3d(Vertex3d *_v3d)
{
    v3d = _v3d;
    //connect(v3d, SIGNAL(updated(QVector3D)), this, SLOT(sync3D(QVector3D))); // 7/20 comment outed for pleat fold
}

Vertex3d *Vertex2d::getVertex3d()
{
    return v3d;
}

QVector2D Vertex2d::getVector2D()
{
    return QVector2D(x, y);
}

void Vertex2d::initTest()
{
}

void Vertex2d::sync3D(QVector3D v)
{
    setPosition(v.x(), v.y());
}

void Vertex2d::sync2D(QVector2D v)
{
    setPosition(v.x(), v.y());
}
