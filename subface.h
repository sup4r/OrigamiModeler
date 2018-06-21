#ifndef SUBFACE_H
#define SUBFACE_H

#include <QVector>
class OriHalfedge;
class Face3d;

class SubFace
{
public:
    SubFace();
    QVector<OriHalfedge*>halfedges;
    void debugPrint();
    QVector<Face3d*> OverlappFaces;
    void makeHalfedgeLoop();
    void draw();
    void drawHalfedges();
    void drawVertices();
    void drawFace();
    void calcNormal();
    float normal;
private:
};

#endif // SUBFACE_H
