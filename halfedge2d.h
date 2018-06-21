#pragma once
#ifndef HALFEDGE2D_H
#define HALFEDGE2D_H
#include <QObject>
#include <QVector2D>
#include "globject.h"
class Vertex2d;
class Face2d;
class Halfedge3d;

class Halfedge2d: public GLObject
{
    Q_OBJECT
public:
    Halfedge2d(Vertex2d *_vertex, Vertex2d *_vertex_next, QObject *parent = 0);
    void setVertex(Vertex2d *v);
    void setVertexNext(Vertex2d *v);
    void setNext(Halfedge2d *_next);
    void setPrev(Halfedge2d *_prev);
    void setPair(Halfedge2d *_pair);
    void setFace(Face2d *f);
    Face2d *getFace() const;
    void draw();
    void drawWithoutAll();
    void drawWithoutSize();
    bool hasPair();
    void debugPrint();
    void setHalfedge3d(Halfedge3d *_he3d);
    Halfedge3d *getHalfedge3d();
    QVector2D getSV2D();
    QVector2D getEV2D();
    QVector2D getCenter();
    QVector2D getVec2D();
    QVector2D getNormal();
    void reverse();
    QVector<Halfedge3d*> getAllHalfedge3dsInMirrorGroup();
    QVector<Halfedge3d *> getPositiveHalfedge3dsInMirrorGroup();
    void movePosition(QVector2D vec);

private:
    Halfedge3d *he3d;
    int idForExport;
    bool isSelected;
public:
    Vertex2d *vertex, *vertex_next;
    Halfedge2d *next, *prev, *pair;
    Face2d *face;
    int getIdForExport() const;
    void setIdForExport(int value);


    bool getIsSelected() const;
    void setIsSelected(bool value);

signals:
    void updated();
public slots:
    void sync();
};

#endif // HALFEDGE2D_H
