#pragma once
#ifndef FACE2D_H
#define FACE2D_H

#include <QVector>
#include <QVector2D>
#include "globject.h"

class Vertex2d;
class Halfedge2d;
class Face3d;
class Model2d;

class Face2d :  public GLObject
{
    Q_OBJECT
public:
    Face2d(Halfedge2d *_halfedge, QObject *parent = 0);

    Halfedge2d *halfedge;
    void draw();
    void drawWithoutColor();
    void displayNormal();
    QVector<Vertex2d*> getVertices();
    bool isFront();
    void flip();
    void setFace3d(Face3d *_f3d);
    Face3d *getFace3d();
    QVector2D getG();
    int getMirrorTimes() const;
    void setMirrorTimes(int value);
    QVector<Face3d*> getAllFace3dsInMirrorGroup();
    QVector<Face3d *> getPositiveFace3dsInMirrorGroup();
    void drawMirrorTimes();
    void flip2DAnd3D();
    bool isNormalFixed() const;
    void fixNormal();
    void addMirrorTimes();

    int getZOrder();
    void setZOrder(int value);

    /* vvvvvvvvvvv garbage vvvvvvvvvvvv */
    bool getIsBranch() const;
    void setIsBranch(bool value);

    int getILayer() const;
    void setILayer(int value);
    /* ^^^^^^^^^^ garbage ^^^^^^^^^^^^^*/
    QVector<Halfedge2d*> getHalfedges();

    void movePosition(QVector2D vec);

    Model2d *getModel() const;
    void setModel(Model2d *value);

    void displayFaceOrder();
private:
    Face3d *f3d;
    int mirrorTimes;
    void setHalfedge(Halfedge2d *_halfedge);
    bool normalFixed;

    /* 170125 trunk & branch*/
    bool isBranch;// if isBranch == true then this is branch else this is trunk. main is trunk.
    int iLayer;

    /* 170214 mirror connect */
    Model2d *model;
signals:
    void updated();
public slots:
    void sync();
};

#endif // FACE2D_H
