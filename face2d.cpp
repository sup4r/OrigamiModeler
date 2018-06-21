#pragma once
#include"vertex2d.h"
#include "halfedge2d.h"
#include "face2d.h"
#include "halfedge3d.h"
#include "face3d.h"
#include <QVector2D>
#include <QVector3D>
#include <QOpenGLFunctions>
#include <QDebug>
#include "gldrawutil.h"
#include "model2d.h"
Face2d::Face2d(Halfedge2d *_halfedge, QObject *parent)
    :GLObject(parent)
{
    setHalfedge(_halfedge);
    setMirrorTimes(1);
    setModel(NULL);
    f3d = NULL;
    normalFixed = false;
    isBranch = false;
    iLayer = 0;
}



void Face2d::draw()
{
    const float alpha = .2f;
    QColor col = getColor();
    glColor4d((float)col.red()/255 , (float)col.green()/255, (float)col.blue()/255, alpha);
    glBegin(GL_POLYGON);
    Halfedge2d *he = halfedge;
    do{
        glVertex2d(he->vertex->x, he->vertex->y);
        he = he->next;
    }while(he != halfedge);
    glEnd();
}

void Face2d::drawWithoutColor()
{
    glBegin(GL_POLYGON);
    Halfedge2d *he = halfedge;
    do{
        glVertex2d(he->vertex->x, he->vertex->y);
        he = he->next;
    }while(he != halfedge);
    glEnd();
}

void Face2d::displayNormal()
{
    static const int r = 3;
    if(isFront()){
        GLDrawUtil::drawCircle2D(getG(), r);
    }else{
        GLDrawUtil::drawFilledCircle2D(getG(), r);
    }
}
void Face2d::displayFaceOrder(){
    static const int l = 5;
    glPushMatrix();
    QVector2D g = getG();
    glTranslated(g.x(),g.y(),0);
    for(int i = 0; i < getZOrder(); ++i){
        int x = i*2;
        glBegin(GL_LINES);
        glVertex2d(x, 0);
        glVertex2d(x,l);
        glEnd();
    }
    glPopMatrix();
}

void Face2d::drawMirrorTimes(){
    glLineWidth(1);
    QColor col = Qt::gray;
    glColor3d((float)col.red()/255, (float)col.green()/255, (float)col.blue()/255);
    QVector2D g = this->getG();
    int r = 3;
    float eps = 1;
    for(int i = 0; i < mirrorTimes; ++i){
        QVector2D c  = g +QVector2D(2*i*(r+eps), 0);
        GLDrawUtil::drawCircle2D(c, r);
    }
}

QVector<Vertex2d *> Face2d::getVertices()
{
    QVector<Vertex2d*> vertices;
    Halfedge2d *he = halfedge;
    do{
        vertices.push_back(he->vertex);
        he = he->next;
    }while(he != halfedge);
    return vertices;
}

bool Face2d::isFront()
{
    QVector2D vec1  =halfedge->getVec2D().normalized();
    QVector2D vec2 = -halfedge->prev->getVec2D().normalized();
    float normal = QVector3D::crossProduct(vec1, vec2).z();
    return normal>0;
}

void Face2d::flip()
{
    Halfedge2d *he = this->halfedge;
    QVector<Halfedge2d *>hes;
    do{
        hes.push_back(he);
        if(he->hasPair()){
            qDebug()<<"Face2d::flip : he->hasPair()==true";
        }
        he = he->next;
    }while(he != this->halfedge);

    for(int i = 0; i < hes.size(); ++i){
        hes.at(i)->reverse();
    }
    /* Reverse ReConnect */
    for(int i =0; i<hes.size(); ++i){
        hes.at(i)->setPrev(hes.at((i==hes.size()-1)? 0: i+1));
        hes.at(i)->setNext(hes.at((i==0)? hes.size()-1: i-1));
    }
}
void Face2d::flip2DAnd3D(){
    if(isNormalFixed()){
        qDebug()<<"Face2d::flip2DAnd3D: this noraml is fixed. cant flip normals.";
        return;
    }
    this->flip();
    // 3d
    QVector<Face3d*>fs = this->getAllFace3dsInMirrorGroup();
    for(int i = 0; i < fs.size(); ++i){
        fs.at(i)->flip();
    }
}

int Face2d::getMirrorTimes() const
{
    return mirrorTimes;
}

void Face2d::setMirrorTimes(int value)
{
    mirrorTimes = value;
}

QVector<Face3d*> Face2d::getAllFace3dsInMirrorGroup()
{
    QVector<Face3d*> faces;
    Face3d *face_in_group = this->getFace3d();
    do{
        faces<<face_in_group;
    }while((face_in_group = face_in_group->getMirrorNext())!=this->getFace3d());
    return faces;
}
QVector<Face3d*> Face2d::getPositiveFace3dsInMirrorGroup()
{
    QVector<Face3d*> faces;
    Face3d *face_in_group = this->getFace3d();
    do{
        if(face_in_group->getZOrder()>0)faces<<face_in_group;
    }while((face_in_group = face_in_group->getMirrorNext())!=this->getFace3d());
    return faces;
}
void Face2d::setFace3d(Face3d *_f3d)
{
    f3d = _f3d;
}

Face3d *Face2d::getFace3d()
{
    return f3d;
}

QVector2D Face2d::getG()
{
    QVector2D g(0,0);
    int n = 0;
    Halfedge2d *he = halfedge;
    do{
        g=g+he->getSV2D();
        n++;
        he = he->next;
    }while(he != halfedge);
    return g/n;
}


void Face2d::setHalfedge(Halfedge2d *_halfedge)
{
    halfedge = _halfedge;
    Halfedge2d *he = halfedge;
    do{
        he->setFace(this);
        he = he->next;
    }while(he != halfedge);
}

Model2d *Face2d::getModel() const
{
    return model;
}

void Face2d::setModel(Model2d *value)
{
    model = value;
}

int Face2d::getILayer() const
{
    return iLayer;
}

void Face2d::setILayer(int value)
{
    iLayer = value;
}

QVector<Halfedge2d *> Face2d::getHalfedges()
{
    QVector<Halfedge2d *> hes;
    Halfedge2d *he = this->halfedge;
    do{
        hes.push_back(he);
    }while((he=he->next)!=this->halfedge);

    return hes;
}

void Face2d::movePosition(QVector2D vec)
{
    QVector<Vertex2d*>vertices = getVertices();
    for(int i = 0; i  <vertices.size(); ++i){
        vertices.at(i)->movePosition(vec);
    }
}

bool Face2d::getIsBranch() const
{
    return isBranch;
}

void Face2d::setIsBranch(bool value)
{
    isBranch = value;
}

bool Face2d::isNormalFixed() const
{
    return normalFixed;
}

void Face2d::fixNormal()
{
    normalFixed = true;
}

void Face2d::addMirrorTimes()
{

}

int Face2d::getZOrder()
{
    return getFace3d()->getZOrder();
}

void Face2d::setZOrder(int value)
{
    getFace3d()->setZOrderValue(value);
}



void Face2d::sync()
{
    
}
