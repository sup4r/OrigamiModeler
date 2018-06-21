#pragma once
#ifndef MODEL2D_H
#define MODEL2D_H

#include <QVector>
#include <QColor>
#include <QPoint>
#include "modelbase.h"
class Vertex2d;
class Halfedge2d;
class Face2d;
class Model3d;
class Annotation;

class Model2d: public ModelBase
{
    Q_OBJECT
public:
    Model2d(QObject *parent = 0);
    QVector<Vertex2d*> vertices;
    QVector<Halfedge2d*> halfedges;
    QVector<Face2d*> faces;
    Model3d *m3d;
    void draw();
    void drawVertices();
    void drawHalfedges();
    void drawFaces();
    void drawFacesWithAlphaBlend();
    void drawWithoutSize();
    void drawVerticesWithoutSize();
    void drawHalfedgesWithoutSize();
    void drawSmallFaces();
    void drawSmallFaces(const float s);
    void drawNormals();
    void drawFaceOrder();

    void addVertex(Vertex2d *vertex);
    void addHalfedge(Halfedge2d *halfedge);
    void addFace(Face2d *face);
    QColor getBranchFaceColor() const;
    void setBranchFaceColor(const QColor &value);
    Halfedge2d *getHalfedge(int vertexID1, int vertexID2);

    Model3d *getM3d() const;
    void setM3d(Model3d *value);

    void movePosition(QVector2D vec);
    void drawBoundingBox();

/* face flip 170225 */
public:
    bool areFacesConnectedWithNoError(QVector<Annotation*> *annotations);
    void fixFaceNormals(QVector<Annotation*> *annotations);
private:
    QVector<bool> getCurrentFaceNormals();
    bool areFacesConnectedWithNoErrorWithFaceNormals(QVector<Annotation *> *annotations, QVector<bool> faceNormals);
    QVector<bool> getNoErrorFaceNormals(QVector<Annotation*> *annotations);
    void applyFaceNormals(QVector<bool> faceNormals);
/* face overlap 170227 */
public:
    bool areFacesOverlappingWithNoError(QVector<Annotation*> *annotations);
    void fixFaceOrders(QVector<Annotation*> *annotations);
    void drawUnselectedHalfedgesWithoutSize();
    void drawSelectedHalgedgesWithoutSize();
private:
    QVector<int> getCurrentFaceOrders();
    bool areFacesOverlappingWithNoErrorWithFaceOrders(QVector<Annotation*> *annotations, QVector<int> faceOrders);
    QVector<int> getNoErrorFaceOrders(QVector<Annotation*> *annotations);
    void applyFaceOrders(QVector<int> faceOrders);
    QVector<int> searchP(const int p, const QVector<QPair<int, int> > edges, QVector<int> heights);
private:
    QColor branchFaceColor;

    QVector<int> getNoErrorFaceOrders_old(QVector<Annotation *> *annotations);
};

#endif // MODEL2D_H
