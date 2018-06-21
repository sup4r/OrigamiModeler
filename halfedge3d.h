#pragma once
#ifndef HALFEDGE3D_H
#define HALFEDGE3D_H

#include <QVector3D>
#include "globject.h"
class Vertex3d;
class Face3d;
class Halfedge2d;
class Model3d;
class Halfedge3d: public GLObject
{
    Q_OBJECT
public:
    Halfedge3d(Vertex3d *_vertex, Vertex3d *_vertex_next, QObject *parent = 0);
    ~Halfedge3d();
    void setNext(Halfedge3d *_next);
    void setPrev(Halfedge3d *_prev);
    void setPair(Halfedge3d *_pair);
    void setFace(Face3d *_face);
    void setMirror(Halfedge3d *_mirror);
    Halfedge3d *getMirror() const;
    void setZ(double z);
    void draw();
    void drawMirror();
    void drawPairConnection();
    bool hasPair();
    void reverse();
    double getLength();
    Vertex3d *vertex, *vertex_next;
    Halfedge3d *next, *prev, *pair;
    Face3d *face;
    void debugPrint();
private:
    static int num;
    Halfedge2d *he2d;
    int id;
    Halfedge3d *mirror;
public:
    bool checkHalfedge2d(Halfedge2d *he);
    void setHalfedge2d(Halfedge2d *_he2d);
    Halfedge2d *getHalfedge2d();
    QVector3D getQVector3D();
    QVector3D getVector3D();
    QVector3D getSV3D();
    QVector3D getEV3D();
    QVector3D getCenter();
    static bool isZOverlapAndOppositeDirect(Halfedge3d *he1, Halfedge3d *he2);
    double getZHigher();
    double getZLower();
    int getId() const;
    void setId(int value);



    Face3d *getFace() const;
    Model3d *getModel();

signals:
    void updated();
public slots:
    void sync();
};

#endif // HALFEDGE3D_H
