#pragma once
#include"vertex3d.h"
#include "halfedge3d.h"
#include "face3d.h"
#include "face2d.h"
#include "vertex2d.h"
#include "model3d.h"
#include <QOpenGLFunctions>
#include <QDebug>
#include <QVector3D>

int Face3d::num;

void Face3d::init(Halfedge3d *_halfedge, int _zOrder, int _faceZDistance)
{
    setHalfedge(_halfedge);
    initZOrder(_zOrder);
    initFaceZDistance(_faceZDistance);
    setFace2d(NULL);
    setMirrorNext(NULL);
    updateGeometry(zOrder*faceZDistance);
    id = num++;
    setModel(NULL);
    setTexture(NULL);
    setLengthOfMenuBar(21);
}
void Face3d::initZOrder(int _zOrder){
    zOrder = _zOrder;
}
void Face3d::initFaceZDistance(int _faceZDistance){
    faceZDistance = _faceZDistance;
}


Face3d::Face3d(Halfedge3d *_halfedge, int _zOrder, int _faceZDistance, QObject *parent)
    : GLObject(parent)
{

    Halfedge3d *he = _halfedge;
    do{
        Vertex3d *v = he->vertex;
        connect(v, SIGNAL(updated(QVector3D)), this, SLOT(updateNormal()));
        he->setFace(this);
        he = he->next;
    }while(he != _halfedge);
    init(_halfedge, _zOrder, _faceZDistance);
}

Face3d::Face3d(QVector<Vertex3d *> vertices, int _zOrder, int _faceZDistance, QObject *parent)
    :GLObject(parent)
{
    QVector<Halfedge3d*>heList;
    for(int i = 0; i<vertices.size(); ++i){
        Halfedge3d *he = new Halfedge3d(vertices.at(i), vertices.at((i==vertices.size()-1)? 0: i+1));
        he->setFace(this);
        heList.push_back(he);
    }
    for(int i =0; i<heList.size(); ++i){
        heList.at(i)->setNext(heList.at((i==heList.size()-1)? 0: i+1));
    }
    init(heList.at(0), _zOrder, _faceZDistance);
}

Face3d::~Face3d()
{
}

void Face3d::draw()
{
    static const QColor selectColor(156,255,156);// 170119 yabai
    if(this->getColor()==selectColor){
        glColor3d((float)getColor().red()/255, (float)getColor().green()/255, (float)getColor().blue()/255);
    }else{
        glColor3d(1,1,1);
    }
   if(texture!=NULL){
        texture->bind();
        glEnable(GL_TEXTURE_2D);
//        glDisable(GL_TEXTURE_2D); //
    }

    glBegin(GL_POLYGON);

    glNormal3d(normal.x(), normal.y(), normal.z());
    Halfedge3d *he = halfedge;
    do{
        if(texture!=NULL)glTexCoord2f(he->vertex->x/800.f, (he->vertex->y+lengthOfMenuBar)/600.f);
        glVertex3d(he->vertex->x, he->vertex->y, he->vertex->z);
        he = he->next;
    }while(he != halfedge);

    glEnd();
    if(texture!=NULL){

         glDisable(GL_TEXTURE_2D);
         texture->release();
     }

}
void Face3d::draw_tmp()
{
    static const QColor selectColor(156,255,156);// 170119 yabai
    if(this->getColor()==selectColor){
        glColor3d((float)getColor().red()/255, (float)getColor().green()/255, (float)getColor().blue()/255);
    }else{
        glColor3d(1,1,1);
    }
    glBegin(GL_POLYGON);

    glNormal3d(normal.x(), normal.y(), normal.z());
    Halfedge3d *he = halfedge;
    do{
        glVertex3d(he->vertex->x, he->vertex->y, he->vertex->z);
        he = he->next;
    }while(he != halfedge);

    glEnd();

}

QOpenGLTexture *Face3d::getTexture() const
{
    return texture;
}

void Face3d::setTexture(QOpenGLTexture *value)
{
    texture = value;
}

int Face3d::getLengthOfMenuBar() const
{
    return lengthOfMenuBar;
}

void Face3d::setLengthOfMenuBar(int value)
{
    lengthOfMenuBar = value;
}
void Face3d::draw2()
{
    glColor3d((float)getColor().red()/255, (float)getColor().green()/255, (float)getColor().blue()/255);
    //    glColor3d(1,1,1);
    //BACK
    if(texture!=NULL){
         texture->bind();
         glEnable(GL_TEXTURE_2D);
     }

    glBegin(GL_POLYGON);

    glNormal3d(-normal.x(), -normal.y(), -normal.z());
    Halfedge3d *he = halfedge;
    do{
        if(texture!=NULL)glTexCoord2f(he->vertex->x/800.f, (he->vertex->y+lengthOfMenuBar)/600.f);
        glVertex3d(he->vertex->x, he->vertex->y, he->vertex->z);
        he = he->prev;
    }while(he != halfedge);

    glEnd();
    if(texture!=NULL){
        glDisable(GL_TEXTURE_2D);
        texture->release();
    }
}

void Face3d::drawNormal(){
    const float length =20.f;
    QVector3D center(0,0,0);
    int N=0;
    Halfedge3d *he = halfedge;
    do{
        QVector3D vV(he->vertex->x, he->vertex->y, he->vertex->z);
        center +=vV;
        N++;
        he = he->next;
    }while(he != halfedge);
    center /= (float)N;

    glBegin(GL_LINES);

    glVertex3f(center.x(), center.y(), center.z());
    glVertex3f(center.x()+normal.x()*length, center.y()+normal.y()*length, center.z()+normal.z()*length);

    glEnd();
}

void Face3d::drawMirror()
{
    glBegin(GL_POLYGON);

    glNormal3d(normal.x(), normal.y(), -normal.z());
    Halfedge3d *he = halfedge;
    do{
        glVertex3d(he->vertex->x, he->vertex->y, -he->vertex->z);
        he = he->prev;
    }while(he != halfedge);

    glEnd();
}
void Face3d::drawMirrorNormal()
{
    const float length =20.f;
    QVector3D center(0,0,0);
    float sum_edge_length = 0;
    Halfedge3d *he = halfedge;
    do{
        QVector3D vV(he->vertex->x, he->vertex->y, he->vertex->z);
        QVector3D vH(he->next->vertex->x - he->vertex->x, he->next->vertex->y - he->vertex->y, he->next->vertex->z-he->vertex->z);
        center+=vH.length()*vV;
        sum_edge_length +=vH.length();
        he = he->next;
    }while(he != halfedge);
    center/=sum_edge_length;

    glBegin(GL_LINES);

    glVertex3f(center.x(), center.y(), -center.z());
    glVertex3f(center.x()+normal.x()*length, center.y()+normal.y()*length, -center.z()-normal.z()*length);

    glEnd();
}
void Face3d::flip(){
    Halfedge3d *he = this->halfedge;
    QVector<Halfedge3d *>hes;
    do{
        hes.push_back(he);
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
    calcNormal();
    emit(updated());
}

bool Face3d::isBridgeFace()
{
    return f2d==NULL;
}

QVector<Vertex3d *> Face3d::getVertices()
{
    QVector<Vertex3d *> vertices;
    Halfedge3d *he  = this->halfedge;
    do{
        vertices.push_back(he->vertex);
        he = he->next;
    }while(he != this->halfedge);

    return vertices;
}

Face3d *Face3d::createMirror()
{
    QVector<Vertex3d*>vertices = getVertices();
//    qDebug()<<"Face3d::createMirror: vertices.size() = "<<vertices.size();
    // vertex
    QVector<Vertex3d*>mirrorVertices;
    for(int i=0; i < vertices.size(); ++i){
        Vertex3d *mirrorVertex = vertices.at(i)->createMirror();
        mirrorVertices.push_back(mirrorVertex);
    }
    // face
    Halfedge3d *he = this->halfedge;
    QVector<Halfedge3d*>mirrorHalfedges;
    do{
        int isv, iev;
        isv = vertices.indexOf(he->vertex);
        iev = vertices.indexOf(he->next->vertex);
        Halfedge3d *mirrorHe = new Halfedge3d(mirrorVertices.at(iev), mirrorVertices.at(isv));// mirror ev->sv
        he->setMirror(mirrorHe);
        mirrorHalfedges.push_back(mirrorHe);
        he  = he->next;
    }while(he != this->halfedge);

    for(int i =0; i<mirrorHalfedges.size(); ++i){
        mirrorHalfedges.at(i)->setPrev(mirrorHalfedges.at((i==mirrorHalfedges.size()-1)? 0: i+1));// mirror setPrev
    }
    Face3d *mirrorFace = new Face3d(mirrorHalfedges.front(), -this->getZOrder(), this->getFaceZDistance());
    connect(this, SIGNAL(colorUpdated(QColor)), mirrorFace, SLOT(setColor(QColor)));
    connect(this, SIGNAL(sizeUpdated(int)), mirrorFace, SLOT(setSize(int)));
    connect(mirrorFace, SIGNAL(colorUpdated(QColor)), this, SLOT(setColor(QColor)));
    connect(mirrorFace, SIGNAL(sizeUpdated(int)), this, SLOT(setSize(int)));
    return mirrorFace;
}

void Face3d::debugPrint()
{
    qDebug()<<"Face3d::debugPrint";
    QVector<Vertex3d*>vertices = getVertices();
    qDebug()<<"ZOrder = "<<getZOrder();
    qDebug()<<"FaceZDistance = "<<getFaceZDistance();
    qDebug()<<"vertices.size = "<<vertices.size();
    for(int i = 0; i < vertices.size(); ++i){
        vertices.at(i)->debugPrint();
    }

}
void Face3d::setFace2d(Face2d *_f2d)
{
    f2d = _f2d;
}
void Face3d::setFace2dAndConnectVertices2d(Face2d *_f2d){
    f2d = _f2d;
    QVector<Vertex2d*>vs2d;
    QVector<Vertex3d*>vs3d;
    vs2d = f2d->getVertices();
    vs3d = this->getVertices();
    for(int i = 0; i < vs2d.size(); ++i){
        Vertex2d *vi = vs2d.at(i);
        for(int j = 0; j < vs3d.size(); ++j){
            Vertex3d *vj = vs3d.at(j);
            static const float eps = 1e-6;
            if(fabs(vi->x-vj->x)<eps&&fabs(vi->y-vj->y)<eps){
                vj->setVertex2d(vi);
            }
        }
    }
}

Face2d *Face3d::getFace2d()
{
    return f2d;
}

int Face3d::getZOrder()
{
    return zOrder;
}

int Face3d::getFaceZDistance()
{
    return faceZDistance;
}

Face3d *Face3d::getMirrorNext() const
{
    return mirrorNext;
}

void Face3d::setMirrorNext(Face3d *value)
{
    mirrorNext = value;
}

Face3d *Face3d::createCpy()
{
    QVector<Vertex3d*>vs = this->getVertices();
    QVector<Vertex3d*>newVertices;
    for(int i = 0; i  < vs.size(); ++i){
        newVertices.push_back(vs.at(i)->createCpy());
    }
    Face3d *cpyF =  new Face3d(newVertices, this->getZOrder(), this->getFaceZDistance());
    // set he2d
    Halfedge3d *he = this->halfedge;
    Halfedge3d *cpyHe = cpyF->halfedge;
    do{
        cpyHe->setHalfedge2d(he->getHalfedge2d());
        cpyHe = cpyHe->next;
        he = he->next;
    }while(he != this->halfedge);
    return cpyF;
}

void Face3d::setMirrorFace(Face3d *mirrorFace)
{
    // vertex mirror
    QVector<Vertex3d*>vs1 = this->getVertices(), vs2 = mirrorFace->getVertices();
    if(vs1.size() != vs2.size()){
        qDebug()<<"Face3d::setMirrorFace: vs1.size()!=vs2.size(), return";
        return;
    }
    for(int i = 0; i < vs1.size(); ++i){
        Vertex3d *vi = vs1.at(i);
        for(int j = 0; j < vs2.size(); ++j){
            Vertex3d *vj = vs2.at(j);
            static const float eps = 1e-6;
            if(fabs(vi->x-vj->x)<eps&&fabs(vi->y-vj->y)<eps){
                vi->setMirror(vj);
            }
        }
    }
    // edge mirror
    Halfedge3d * he1 = this->halfedge;
    do{
        Vertex3d *v11 = he1->vertex,  *v12 = he1->vertex_next;
        Halfedge3d * he2 = mirrorFace->halfedge;
        do{
            Vertex3d *v21 = he2->vertex, *v22 = he2->vertex_next;
            static const float eps = 1e-6;
            if(fabs(v11->x-v22->x)<eps&&fabs(v11->y-v22->y)<eps&&fabs(v12->x-v21->x)<eps&&fabs(v12->y-v21->y)<eps){
                he1->setMirror(he2);
                break;
            }
        }while((he2 = he2->next) != mirrorFace->halfedge);
    }while( (he1 = he1->next) != this->halfedge);
    // face mirror

        connect(this, SIGNAL(colorUpdated(QColor)), mirrorFace, SLOT(setColor(QColor)));
        connect(mirrorFace, SIGNAL(colorUpdated(QColor)), this, SLOT(setColor(QColor)));

    connect(this, SIGNAL(sizeUpdated(int)), mirrorFace, SLOT(setSize(int)));
    connect(this, SIGNAL(updatedZOrder(int)), mirrorFace, SLOT(syncZOrderWithMirror(int)));
    connect(mirrorFace, SIGNAL(sizeUpdated(int)), this, SLOT(setSize(int)));
    connect(mirrorFace, SIGNAL(updatedZOrder(int)), this, SLOT(syncZOrderWithMirror(int)));
}

void Face3d::checkAndReCaluculationGeometry()
{

}

int Face3d::getId() const
{
    return id;
}

void Face3d::setId(int value)
{
    id = value;
}

Model3d *Face3d::getModel() const
{
    return model;
}

void Face3d::setModel(Model3d *value)
{
    model = value;
}

QVector3D Face3d::getG()
{
    Halfedge3d *he = halfedge;
    QVector3D G;
    int cnt = 0;
    do{
        G+=he->getSV3D();
        cnt++;
    }while((he = he->next)!=halfedge);
    G/=cnt;
    return G;
}
void Face3d::setHalfedge(Halfedge3d *_halfedge)
{
    halfedge = _halfedge;
}

void Face3d::calcZAverage()
{
    zAverage = 0;
    int n = 0;
    Halfedge3d *he = halfedge;
    do{
        Vertex3d *v = he->vertex;
        zAverage += v->z;
        n++;
        he = he->next;
    }while(he != halfedge);

    zAverage/=n;
}
void Face3d::calcNormal()
{
    QVector3D v1, v2;
    Halfedge3d *he = this->halfedge;
    v1  = QVector3D(-he->vertex->x + he->prev->vertex->x, -he->vertex->y + he->prev->vertex->y, -he->vertex->z + he->prev->vertex->z);
    he = he->next;
    v2  = QVector3D(he->vertex->x - he->prev->vertex->x, he->vertex->y - he->prev->vertex->y, he->vertex->z - he->prev->vertex->z);
    normal = QVector3D::crossProduct(v2,v1).normalized();

}
void Face3d::updateGeometry(double prevZStandard){
    if( faceZDistance!=-1){
        // NOT Bridge Face
        Halfedge3d *he = halfedge;
        do{
            Vertex3d *v = he->vertex;
            v->setPosition(v->x, v->y, zOrder*faceZDistance);
            he = he->next;
        }while(he != halfedge);
    }else{
        //Bridge Face
    }
    calcZAverage();
    updateNormal();

}

void Face3d::setZOrderValue(int _zOrder)
{
    int prevZOrder = zOrder;
    zOrder = _zOrder;
    updateGeometry(prevZOrder*faceZDistance);
    emit(updatedZOrder(this->getZOrder()));
}

void Face3d::setFaceZDistanceValue(int _faceZDistance)
{
    int prevFaceZDistance = faceZDistance;
    faceZDistance = _faceZDistance;
    updateGeometry(zOrder*prevFaceZDistance);
}

void Face3d::updateNormal(){

    calcNormal();

    emit(updated());
}

void Face3d::syncMirror(Face3d *f)
{
    if(f==NULL){
        return;
    }

    if(this->getZOrder()!=-f->getZOrder()){
        this->setZOrderValue(-f->getZOrder());
    }
    if(this->getFaceZDistance()!=f->getFaceZDistance()){
        this->setFaceZDistanceValue(f->getFaceZDistance());
    }

}

void Face3d::syncZOrderWithMirror(int _zOrder)
{
    if(_zOrder>0&&this->getZOrder()!=-_zOrder){
        this->setZOrderValue(-_zOrder);
    }
}





