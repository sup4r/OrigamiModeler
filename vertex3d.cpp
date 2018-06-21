#pragma once
#include "vertex2d.h"
#include "vertex3d.h"
#include "halfedge2d.h"
#include <QPoint>
#include <QOpenGLFunctions>
#include <QDebug>


Vertex3d::Vertex3d(double _x, double _y, double _z, QObject *parent)
    :GLObject(parent)
{
    updatePosition(_x, _y, _z);
    v2d = NULL;
    initTest();
}

Vertex3d::~Vertex3d()
{

}

void Vertex3d::updatePosition(double _x, double _y, double _z){
    x= _x; y = _y; z = _z;
    emit(updated(QVector3D(x, y, z)));
}

void Vertex3d::setPosition(double _x, double _y, double _z)
{
    static const double epsilon = 1;
    if(fabs(x-_x)>epsilon || fabs(y-_y)>epsilon || fabs(z-_z)>epsilon){
        updatePosition(_x, _y, _z);
    }
}

void Vertex3d::draw()
{

    glPointSize(getSize());
    glColor3d((float)getColor().red()/255, (float)getColor().green()/255, (float)getColor().blue()/255);


    glBegin(GL_POINTS);

    glVertex3d(x, y, z);

    glEnd();
}

void Vertex3d::drawMirror()
{
    glBegin(GL_POINTS);

    glVertex3d(x, y, -z);

    glEnd();
}

QVector3D Vertex3d::getQVector3D(){
    return QVector3D(x, y, z);
}

Halfedge3d *Vertex3d::getHalfedge() const
{
    return halfedge;
}

void Vertex3d::setHalfedge(Halfedge3d *value)
{
    halfedge = value;
}

Vertex3d *Vertex3d::createMirror()
{
    Vertex3d *mirror = new Vertex3d(this->x, this->y, -this->z);
    connect(this, SIGNAL(updated(QVector3D)), mirror, SLOT(syncMirror(QVector3D)));
    connect(mirror, SIGNAL(updated(QVector3D)), this, SLOT(syncMirror(QVector3D)));
    connect(this, SIGNAL(colorUpdated(QColor)), mirror, SLOT(setColor(QColor)));
    connect(mirror, SIGNAL(colorUpdated(QColor)), this, SLOT(setColor(QColor)));
    connect(this, SIGNAL(sizeUpdated(int)), mirror, SLOT(setSize(int)));
    connect(mirror, SIGNAL(sizeUpdated(int)), this, SLOT(setSize(int)));
    return mirror;
}
void Vertex3d::setMirror(Vertex3d *_mirror){
    mirror = _mirror;
    if(_mirror!= NULL){
        connect(this, SIGNAL(updated(QVector3D)), _mirror, SLOT(syncMirror(QVector3D)));
        connect(_mirror, SIGNAL(updated(QVector3D)), this, SLOT(syncMirror(QVector3D)));
        connect(this, SIGNAL(colorUpdated(QColor)), _mirror, SLOT(setColor(QColor)));
        connect(_mirror, SIGNAL(colorUpdated(QColor)), this, SLOT(setColor(QColor)));
        connect(this, SIGNAL(sizeUpdated(int)), _mirror, SLOT(setSize(int)));
        connect(_mirror, SIGNAL(sizeUpdated(int)), this, SLOT(setSize(int)));
    }
}

Vertex3d *Vertex3d::getMirror()
{
    return mirror;
}


Vertex3d *Vertex3d::createCpy()
{
    Vertex3d *cpyV = new Vertex3d(this->x, this->y, this->z);
    return cpyV;
}

void Vertex3d::debugPrint()
{
    qDebug()<<"Vertex3d::debugPrint: v = ("<<x<<", "<<y<<", "<<z<<")";
}

void Vertex3d::setVertex2d(Vertex2d *_v2d)
{
    v2d = _v2d;
    connect(v2d, SIGNAL(updated(QVector2D)), this, SLOT(sync2D(QVector2D)));
}

Vertex2d *Vertex3d::getVertex2d()
{
    return v2d;
}

QVector3D Vertex3d::getVector3D()
{
    return QVector3D(x, y, z);
}

/* --- test --- */
void Vertex3d::initTest()
{
}

void Vertex3d::sync2D(QVector2D v)
{
    setPosition(v.x(), v.y(), this->z);
}

void Vertex3d::syncMirror(QVector3D v)
{
    setPosition(v.x(), v.y(), -v.z()); // 9/12 yappa kotti de iidesu
//    setPosition(this->x, this->y, -v.z());
}

