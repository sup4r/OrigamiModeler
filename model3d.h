#pragma once
#ifndef MODEL3D_H
#define MODEL3D_H

#include <QVector>
#include <QColor>
#include <QPoint>
#include <QOpenGLTexture>
#include "modelbase.h"
class Vertex3d;
class Halfedge3d;
class Face3d;
class SubFace;
class OriLine;//test
class Model2d;
class MirrorPlane;

class Model3d : public ModelBase
{
    Q_OBJECT
public:
    Model3d(QObject *parent=0);
    QVector<Vertex3d*> vertices;
    QVector<Halfedge3d*> halfedges;
    QVector<Face3d*> faces;
    Model2d *m2d;
    /* --- Draw --- */
    void draw(bool v=1, bool e=1, bool f=1, bool n=0, bool c=0);
    void drawVertices();
    void drawHalfedges();
    void drawFaces();
    void drawFacesForPickingModel();
    void drawNormals();
    void drawPairConnection();
    void drawBoundingBox();
    /* --- Add Object --- */
    void addVertex(Vertex3d *vertex);
    void addVertex(Face3d *face);
    void addHalfedge(Halfedge3d *halfedge);
    void addHalfedge(Face3d *face);
    void addFace(Face3d *face);
    /* --- Delete Object --- */
    void deleteVertex(Vertex3d *vertex);
    void deleteHalfedge(Halfedge3d *halfedge);
    void deleteFace(Face3d *face);
    /* --- Get Mirror Object --- */
    Vertex3d *getMirror(Vertex3d *v);
    Halfedge3d *getMirror(Halfedge3d *he);
    /* --- SubFace --- */
    QVector<SubFace*> subfaces;
    void makeSubFace();
    void drawSubFaces(bool flag = true);
    QVector<OriLine*> lines;//test
    void makeOriLines();//test
    void drawOriLines(bool flag = true);//test
    void debugPrintSubfaces();
    Model2d *getM2d() const;
    void setM2d(Model2d *value);
    /* --- MirrorPlane --- */

    QVector3D getG();
    MirrorPlane *createMirrorPlane();
    MirrorPlane *getMirrorPlane() const;
    void setMirrorPlane(MirrorPlane *value);
    void rotateX(double value);
    void rotateY(double value);
    void rotateZ(double value);
    void moveX(double value);
    void moveY(double value);
    void moveZ(double value);
    void changeScale(double value);


    QOpenGLTexture *getTexture() const;
    void setTexture(QOpenGLTexture *value);

private:
    MirrorPlane *mirrorPlane;
    QOpenGLTexture *texture;

signals:
    void updated();
public slots:
    void updateMirrorPlane();
};

#endif // MODEL3D_H
