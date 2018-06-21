#pragma once
#ifndef VERTEX3D_H
#define VERTEX3D_H
#include <QVector2D>
#include <QVector3D>
#include <QVector>
#include "globject.h"

class Halfedge3d;

class Vertex2d;

class Vertex3d :  public GLObject
{
    Q_OBJECT
    void updatePosition(double _x, double _y, double _z);
public:
    Vertex3d(double _x, double _y, double _z, QObject *parent = 0);
    /* ?????????????????????????? error */
    //    Vertex3d(QVector3D v, QObject *parent=0){
//        Vertex3d(v.x(), v.y(), v.z() ,parent);
//    }

    ~Vertex3d();
    void setPosition(double _x, double _y, double _z);
    void setPosition(QVector3D v){
        setPosition(v.x(), v.y(), v.z());
    }
    void movePosition(double _x, double _y, double _z){
         setPosition(x+_x, y+_y, z+_z);
    }
    void movePosition(QVector3D v){
         movePosition(v.x(), v.y(), v.z());
    }

    void draw();
    void drawMirror();
    QVector3D getQVector3D();

    double x, y, z;

    int exportID;

    Vertex3d *createMirror();
    Vertex3d *createCpy();
    void debugPrint();

    void setMirror(Vertex3d *_mirror);
    Vertex3d *getMirror();

    QVector<int> idsForExport;
private:
    Halfedge3d *halfedge;
    Vertex2d *v2d;
    Vertex3d *mirror;
public:
    void setVertex2d(Vertex2d *_v2d);
    Vertex2d *getVertex2d();

    QVector3D getVector3D();
    /* --- test --- */
    void initTest();
    Vertex3d *getMirror() const;

    Halfedge3d *getHalfedge() const;
    void setHalfedge(Halfedge3d *value);

signals:
    void updated(QVector3D);
public slots:
    void sync2D(QVector2D v);
    void syncMirror(QVector3D v);
};

#endif // VERTEX3D_H
