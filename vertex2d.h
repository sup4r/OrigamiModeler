#pragma once
#ifndef VERTEX2D_H
#define VERTEX2D_H

#include <QVector2D>
#include <QVector3D>

#include "globject.h"
class Halfedge2d;

class Vertex3d;

class Vertex2d :  public GLObject
{
    Q_OBJECT
    static int num;
    void initPosition(double _x, double _y);
public:
    Vertex2d(double _x, double _y, QObject *parent = 0);

    void movePosition(double vx, double vy);
    void movePosition(QVector2D v){
        movePosition(v.x(), v.y());
    }
    void setPosition(double _x, double _y);
    void setPosition(QVector2D v){
        setPosition(v.x(), v.y());
    }
    void draw();
    void drawWithoutAll();
    void drawWithoutSize();
    double x, y;
    void setHalfedge(Halfedge2d *he);
    Halfedge2d *halfedge;

    /* sync 2d 170225 */
    void setVertex2d(Vertex2d *v);
private:
    Vertex3d *v3d;
    int id;
    int idForExport;
    /* 170303 for face2d->move with 2dsync */
    bool moved;
public:
    void setVertex3d(Vertex3d *_v3d);
    Vertex3d *getVertex3d();

    QVector2D getVector2D();
    /* --- test --- */
    void initTest();

    int getId() const;
    void setId(int value);

    int getIdForExport() const;
    void setIdForExport(int value);

    bool getMoved() const;
    void setMoved(bool value);

signals:
    void updated(QVector2D);
public slots:
    void sync3D(QVector3D v);
    void sync2D(QVector2D v);
};

#endif // VERTEX2D_H
