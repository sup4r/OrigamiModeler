#pragma once
#ifndef FACE3D_H
#define FACE3D_H

#include <QVector>
#include <QVector3D>
#include <QOpenGLTexture>
#include "globject.h"
class Vertex3d;
class Halfedge3d;
class Face2d;
class Model3d;

class Face3d : public GLObject
{
    Q_OBJECT
public:

    Face3d(Halfedge3d *_halfedge, int _zOrder = -1, int _faceZDistance = -1, QObject *parent =  0);//After Halfedge Pairing
    Face3d(QVector<Vertex3d*> vertices, int _zOrder = -1, int _faceZDistance = -1, QObject *parent = 0);//Before Halfedge Pairing
    ~Face3d();
    Halfedge3d *halfedge;
    void draw();
    void draw2();
    void drawNormal();
    void drawMirror();
    void drawMirrorNormal();
    void flip();
    bool isBridgeFace();
    QVector<Vertex3d*> getVertices();
    Face3d *createMirror();
    void debugPrint();
    void setFace2d(Face2d *_f2d);
    void setFace2dAndConnectVertices2d(Face2d *_f2d);
    Face2d *getFace2d();
    int getZOrder();
    int getFaceZDistance();
    Face3d *getMirrorNext() const;
    void setMirrorNext(Face3d *value);
    Face3d *createCpy();
    void setMirrorFace(Face3d *mirrorFace);
    void checkAndReCaluculationGeometry();
    int getId() const;
    void setId(int value);

    Model3d *getModel() const;
    void setModel(Model3d *value);

    QVector3D getG();
    void draw_tmp();
    QOpenGLTexture *getTexture() const;
    void setTexture(QOpenGLTexture *value);

    int getLengthOfMenuBar() const;
    void setLengthOfMenuBar(int value);

private:
    Face2d *f2d;
    int zOrder;
    double zAverage;
    int faceZDistance;
    QVector3D normal;
    Face3d *mirrorNext;// 9/7 mirror face loop
    int id;
    int lengthOfMenuBar;
    static int num;

    Model3d *model;
    QOpenGLTexture *texture;
private:
    void setHalfedge(Halfedge3d *_halfedge);
    void calcZAverage();
    void calcNormal();
    void updateGeometry(double prevZStandard);
    void init(Halfedge3d *_halfedge, int _zOrder, int _faceZDistance);
    void initZOrder(int _zOrder);
    void initFaceZDistance(int _faceZDistance);
public slots:
    void setZOrderValue(int _zOrder);// dont use this for initialization
    void setFaceZDistanceValue(int _faceZDistance);// ^same
    void updateNormal();
    void syncMirror(Face3d *f);
    void syncZOrderWithMirror(int zOrder);
signals:
    void updated(Face3d *f = NULL);
    void updatedZOrder(int _zOrder);



};

#endif // FACE3D_H
