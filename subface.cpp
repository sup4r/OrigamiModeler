#include "subface.h"
#include "oriedge.h"
#include "orihalfedge.h"
#include "orivertex.h"
#include  <QOpenGLFunctions>
#include <QDebug>
#include <QVector3D>
SubFace::SubFace()
{

}

void SubFace::debugPrint()
{
    qDebug()<<"Subface : ";
    qDebug()<<"halfedges.size() : "<<halfedges.size();
    for(int j = 0; j  <halfedges.size(); ++j){
        OriVertex *v = halfedges.at(j)->vertex;
        qDebug()<<"OriVertex : "<<v->getQVector2D();
    }
}

void SubFace::makeHalfedgeLoop()
{
    int N = halfedges.size();
    for(int i = 0; i<halfedges.size(); ++i){
        OriHalfedge *pre_he = halfedges.at((i - 1 + N) % N);
        OriHalfedge *he = halfedges.at(i);
        OriHalfedge *nxt_he = halfedges.at((i + 1) % N);
        he->next = nxt_he;
        he->prev = pre_he;
    }
}
void SubFace::draw(){

}

void SubFace::drawVertices(){

}

void SubFace::drawHalfedges(){
    glBegin(GL_LINES);
    for(int i = 0; i<halfedges.size(); ++i){
        OriHalfedge *he = halfedges.at(i);
        glVertex3d(he->vertex->x, he->vertex->y, 0);
        glVertex3d(he->next->vertex->x, he->next->vertex->y, 0);
    }
    glEnd();
}

void SubFace::drawFace()
{
    static const int z = 0;
    glBegin(GL_POLYGON);
    OriHalfedge *he = halfedges.front();
    do{
        glVertex3d(he->vertex->x, he->vertex->y, z);
        he = he->next;
    }while(he != halfedges.front());
    glEnd();
}

void SubFace::calcNormal()
{
    QVector3D nv;
    OriHalfedge *he0 = halfedges.at(0);
    OriHalfedge *he1 = halfedges.at(1);
    QVector3D v0(he0->vertex->getQVector2D(), 0);
    QVector3D v1(-he1->vertex->getQVector2D(), 0);
    v0.normalize();
    v1.normalize();
    nv = QVector3D::crossProduct(v0, v1);
    nv.normalize();
    normal = nv.z();
    qDebug()<<"SubFace::calcNormal : normal = "<<normal;
}
