#pragma once

#include "halfedge2d.h"
#include "vertex2d.h"
#include "face2d.h"
#include "halfedge3d.h"
#include "face3d.h"
#include <QDebug>
#include <QOpenGLFunctions>

Halfedge2d::Halfedge2d(Vertex2d *_vertex, Vertex2d *_vertex_next, QObject *parent)
    : GLObject(parent)
{
    setVertex(_vertex);
    setVertexNext(_vertex_next);
    vertex_next = _vertex_next;
    next = prev = pair = NULL;
    face = NULL;
    he3d = NULL;
    setIsSelected(false);
}

void Halfedge2d::setVertex(Vertex2d *v)
{
    vertex = v;
    if(vertex->halfedge==NULL){
        vertex->setHalfedge(this);
    }
}

void Halfedge2d::setVertexNext(Vertex2d *v)
{
    vertex_next = v;
}

void Halfedge2d::setNext(Halfedge2d *_next)
{
    next = _next;
    if(_next->prev==NULL)_next->setPrev(this);
}

void Halfedge2d::setPrev(Halfedge2d *_prev)
{
    prev=_prev;
    if(_prev->next==NULL)_prev->setNext(this);
}

void Halfedge2d::setPair(Halfedge2d *_pair)
{
    pair=_pair;
    if(_pair->pair==NULL)_pair->setPair(this);
}

void Halfedge2d::setFace(Face2d *f)
{
    face = f;
}

void Halfedge2d::draw()
{
    glLineWidth(getSize());
    glColor3d((float)getColor().red()/255, (float)getColor().green()/255, (float)getColor().blue()/255);
    glBegin(GL_LINE_STRIP);
    glVertex2d(vertex->x, vertex->y);
    glVertex2d(vertex_next->x, vertex_next->y);
    glEnd();
}

void Halfedge2d::drawWithoutSize()
{
    glColor3d((float)getColor().red()/255, (float)getColor().green()/255, (float)getColor().blue()/255);
    glBegin(GL_LINE_STRIP);
    glVertex2d(vertex->x, vertex->y);
    glVertex2d(vertex_next->x, vertex_next->y);
    glEnd();
}

void Halfedge2d::drawWithoutAll()
{
    glBegin(GL_LINE_STRIP);
    glVertex2d(vertex->x, vertex->y);
    glVertex2d(vertex_next->x, vertex_next->y);
    glEnd();
}

bool Halfedge2d::hasPair()
{
    return (pair!=NULL);
}

void Halfedge2d::debugPrint()
{
    qDebug()<<"Halfedge2d::debugPrint: s("<<this->vertex->x<<", "<<this->vertex->y<<") -> e("<<this->vertex_next->x<<", "<<this->vertex_next->y<<")";
}

void Halfedge2d::setHalfedge3d(Halfedge3d *_he3d)
{
    he3d = _he3d;
}

Halfedge3d *Halfedge2d::getHalfedge3d()
{
    return he3d;
}

QVector2D Halfedge2d::getSV2D()
{
    return vertex->getVector2D();
}

QVector2D Halfedge2d::getEV2D()
{
    return vertex_next->getVector2D();
}

QVector2D Halfedge2d::getCenter()
{
    return (vertex->getVector2D()+vertex_next->getVector2D())/2;
}

QVector2D Halfedge2d::getVec2D()
{
    return getEV2D()-getSV2D();
}

QVector2D Halfedge2d::getNormal()
{
    Face2d *f = this->face;
    QVector3D fn = QVector3D(0,0,1);
    if(!f->isFront())fn=QVector3D(0,0,-1);
    QVector3D n3d = QVector3D::crossProduct(QVector3D(this->getVec2D().normalized(), 0),fn);
    return QVector2D(n3d.x(), n3d.y());
}
void Halfedge2d::reverse()
{
    Vertex2d *v_tmp = vertex;
    vertex = vertex_next;
    vertex_next = v_tmp;
}

QVector<Halfedge3d *> Halfedge2d::getAllHalfedge3dsInMirrorGroup()
{
    QVector<Halfedge3d *>he3ds;
    Face3d *face_in_group = this->getHalfedge3d()->face;
    do{
        Halfedge3d *he_in_face = face_in_group->halfedge;
        do{
            if(he_in_face->getHalfedge2d() == this){
                he3ds<<he_in_face;
                break;
            }
        }while( (he_in_face= he_in_face->next) != face_in_group->halfedge);
    }while((face_in_group= face_in_group->getMirrorNext()) != this->getHalfedge3d()->face);
    return he3ds;
}
QVector<Halfedge3d *> Halfedge2d::getPositiveHalfedge3dsInMirrorGroup()
{
    QVector<Halfedge3d *>he3ds;
    Face3d *face_in_group = this->getHalfedge3d()->face;
    do{
        if(face_in_group->getZOrder()>0){
            Halfedge3d *he_in_face = face_in_group->halfedge;
            do{
                if(he_in_face->getHalfedge2d() == this){
                    he3ds<<he_in_face;
                    break;
                }
            }while( (he_in_face= he_in_face->next) != face_in_group->halfedge);
        }
    }while((face_in_group= face_in_group->getMirrorNext()) != this->getHalfedge3d()->face);
    return he3ds;
}

void Halfedge2d::movePosition(QVector2D vec)
{
    vertex->movePosition(vec);
    vertex_next->movePosition(vec);
}

bool Halfedge2d::getIsSelected() const
{
    return isSelected;
}

void Halfedge2d::setIsSelected(bool value)
{
    isSelected = value;
}

Face2d *Halfedge2d::getFace() const
{
    return face;
}

int Halfedge2d::getIdForExport() const
{
    return idForExport;
}

void Halfedge2d::setIdForExport(int value)
{
    idForExport = value;
}
void Halfedge2d::sync()
{
    
}
