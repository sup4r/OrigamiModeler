#pragma once

#include "halfedge2d.h"
#include "halfedge3d.h"
#include "vertex2d.h"
#include "vertex3d.h"
#include "model3d.h"
#include "face3d.h"
#include <QDebug>
#include <QOpenGLFunctions>
#include <QVector3D>
#include <QtMath>

Halfedge3d::Halfedge3d(Vertex3d *_vertex, Vertex3d *_vertex_next, QObject *parent)
    : GLObject(parent)
{
    vertex = _vertex;
    vertex->setHalfedge( this);
    vertex_next = _vertex_next;
    next = prev = pair = NULL;
    face = NULL;
    he2d = NULL;
    id = num++;
    mirror = NULL;
}

int Halfedge3d::num;

Halfedge3d::~Halfedge3d()
{

}

void Halfedge3d::setNext(Halfedge3d *_next)
{
    next = _next;
    if(_next->prev==NULL)_next->setPrev(this);
}

void Halfedge3d::setPrev(Halfedge3d *_prev)
{
    prev=_prev;
    if(_prev->next==NULL)_prev->setNext(this);
}

void Halfedge3d::setPair(Halfedge3d *_pair)
{
    pair=_pair;
    if(_pair->pair==NULL)_pair->setPair(this);
}

void Halfedge3d::setFace(Face3d *_face)
{
    face = _face;
}

void Halfedge3d::setMirror(Halfedge3d *_mirror)
{
    mirror = _mirror;
    connect(this, SIGNAL(colorUpdated(QColor)), mirror, SLOT(setColor(QColor)));
    connect(this, SIGNAL(sizeUpdated(int)), mirror, SLOT(setSize(int)));
    if(mirror->getMirror() == NULL){
        mirror->setMirror(this);
    }

}

void Halfedge3d::setZ(double z)
{
    vertex->setPosition(vertex->x, vertex->y, z);
    vertex_next->setPosition(vertex_next->x, vertex_next->y, z);
}
double Halfedge3d::getZHigher(){
    return (vertex->z>vertex_next->z)?vertex->z:vertex_next->z;
}

double Halfedge3d::getZLower()
{
    return (vertex->z<vertex_next->z)?vertex->z:vertex_next->z;
}

void Halfedge3d::draw()
{

    glLineWidth(getSize());
    glColor3d((float)getColor().red()/255, (float)getColor().green()/255, (float)getColor().blue()/255);
    glBegin(GL_LINE_STRIP);

    glVertex3d(vertex->x, vertex->y, vertex->z);
    glVertex3d(vertex_next->x, vertex_next->y, vertex_next->z);

    glEnd();
}

void Halfedge3d::drawMirror()
{
    glBegin(GL_LINE_STRIP);

    glVertex3d(vertex->x, vertex->y, -vertex->z);
    glVertex3d(vertex_next->x, vertex_next->y, -vertex_next->z);

    glEnd();
}

void Halfedge3d::drawPairConnection()
{
    if(!hasPair()){
        return;
    }
    const float length_arrow_head = 5.f;
    const float angle_arrow_head = M_PI/4.f;
    QVector3D vecO((vertex->x+vertex_next->x)/2, (vertex->y+vertex_next->y)/2, (vertex->z+vertex_next->z)/2);
    QVector3D vecA((pair->vertex->x+pair->vertex_next->x)/2, (pair->vertex->y+pair->vertex_next->y)/2, (pair->vertex->z+pair->vertex_next->z)/2);
    QVector3D vecOA =vecA-vecO;
    QVector3D vecZ(0, 0, 1);
    QVector3D vecP(0, 0, vecOA.length());
    QVector3D vecQ(0, length_arrow_head*sin(angle_arrow_head), vecOA.length()-length_arrow_head*cos(angle_arrow_head));
    QVector3D vecR(0, -length_arrow_head*sin(angle_arrow_head), vecOA.length()-length_arrow_head*cos(angle_arrow_head));
    double rot_angle = acos(QVector3D::dotProduct(vecZ, vecOA.normalized()))*180/M_PI;
    QVector3D rot_axis = QVector3D::crossProduct( vecZ, vecOA.normalized()).normalized();
    //qDebug()<<rot_angle;
    glPushMatrix();
    glTranslated(vecO.x(), vecO.y(), vecO.z());
    glRotated(rot_angle, rot_axis.x(), rot_axis.y(), rot_axis.z());

    glBegin(GL_LINE_STRIP);

    glVertex3d(0, 0, 0);
    glVertex3d(vecP.x(), vecP.y(), vecP.z());

    glVertex3d(vecP.x(), vecP.y(), vecP.z());
    glVertex3d(vecQ.x(), vecQ.y(), vecQ.z());

    glVertex3d(vecP.x(), vecP.y(), vecP.z());
    glVertex3d(vecR.x(), vecR.y(), vecR.z());

    glEnd();
    glPopMatrix();

}

bool Halfedge3d::hasPair()
{
    return (pair!=NULL);
}

void Halfedge3d::reverse()
{

    if(vertex->getHalfedge()==this)vertex->setHalfedge(NULL);
    if(vertex_next->getHalfedge()==NULL)vertex_next->setHalfedge(this);
    Vertex3d *v_tmp = vertex;
    vertex = vertex_next;
    vertex_next = v_tmp;
}

double Halfedge3d::getLength()
{
    return getQVector3D().length();
}

Face3d *Halfedge3d::getFace() const
{
    return face;
}

Model3d *Halfedge3d::getModel()
{
    return this->getFace()->getModel();
}

void Halfedge3d::debugPrint()
{
    qDebug()<<"Halfedge3d::debugPrint";
    qDebug();
}

Halfedge3d *Halfedge3d::getMirror() const
{
    return mirror;
}

int Halfedge3d::getId() const
{
    return id;
}

void Halfedge3d::setId(int value)
{
    id = value;
}
bool Halfedge3d::checkHalfedge2d(Halfedge2d *he){
    QVector2D sv2d = he->getSV2D();
    QVector2D ev2d = he->getEV2D();
    QVector3D _sv3d = getSV3D();
    QVector3D _ev3d = getEV3D();
    QVector2D sv3d(_sv3d.x(), _sv3d.y());
    QVector2D ev3d(_ev3d.x(), _ev3d.y());
    static const double eps = 1e-10;
    return (sv2d-sv3d).length()<eps&&(ev2d-ev3d).length()<eps;
}

void Halfedge3d::setHalfedge2d(Halfedge2d *_he2d)
{
    // check
//    if(checkHalfedge2d(_he2d)){
//        //qDebug()<<"Halfedge3d::setHalfedge2d: checkHalfege2d == true";
//        he2d = _he2d;
//    }else{
//        qDebug()<<"Halfedge3d::setHalfedge2d: checkHalfege2d == false";
//    }
    // DONT CHECK 9/11
    he2d = _he2d;

}

Halfedge2d *Halfedge3d::getHalfedge2d()
{
    /*
    if(he2d==NULL){
        qDebug()<<"Halfedge3d::getHalfedge2d(): he2d == NULL";
    }*/
    return he2d;
}
QVector3D Halfedge3d::getQVector3D(){
    return QVector3D(vertex_next->x-vertex->x, vertex_next->y-vertex->y, vertex_next->z-vertex->z);
}

QVector3D Halfedge3d::getVector3D()
{
    return getQVector3D();
}

QVector3D Halfedge3d::getSV3D()
{
    return vertex->getQVector3D();
}

QVector3D Halfedge3d::getEV3D()
{
    return vertex_next->getQVector3D();
}

QVector3D Halfedge3d::getCenter()
{
    return (getSV3D()+getEV3D())/2;
}

bool Halfedge3d::isZOverlapAndOppositeDirect(Halfedge3d *he1, Halfedge3d *he2){
    QVector2D s1(he1->vertex->getQVector3D().x(), he1->vertex->getQVector3D().y());
    QVector2D s2(he2->vertex->getQVector3D().x(), he2->vertex->getQVector3D().y());
    QVector2D v1( he1->getQVector3D().x(), he1->getQVector3D().y());
    QVector2D v2( he2->getQVector3D().x(), he2->getQVector3D().y());
    static const double epsilon = 1e-1;
    return (s1+v1-s2).length()<epsilon&&(s2+v2-s1).length()<epsilon;
}

void Halfedge3d::sync()
{
    
}
