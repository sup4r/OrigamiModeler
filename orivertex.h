#ifndef ORIVERTEX_H
#define ORIVERTEX_H

#include <QVector>
#include <QVector2D>

class OriHalfedge;
class OriEdge;

class OriVertex
{
public:
    OriVertex(double X, double Y);
    double x, y;
    OriHalfedge *halfedge;
    QVector<OriEdge*>edges;
    void debugPrint();
    void addEdge(OriEdge *e);
    QVector2D getQVector2D();
    OriEdge *getPrevEdge(OriEdge *e);
private:
    double getAngle(OriEdge *e);
};

#endif // ORIVERTEX_H
