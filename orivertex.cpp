#include "orivertex.h"
#include "orihalfedge.h"
#include "oriedge.h"
#include <QDebug>
#include <QtMath>
OriVertex::OriVertex(double X, double Y)
{
    x = X; y = Y;
    halfedge = NULL;
}
// HA????????????
//OriVertex::OriVertex(QVector2D p)
//{
//    OriVertex(p.x(), p.y());
//}

void OriVertex::debugPrint()
{

}

//To store and sort in a clockwise direction
void OriVertex::addEdge(OriEdge *e)
{
    double angle = getAngle(e);
    bool added = false;
    for(int i = 0; i < edges.size(); ++i){
        double tmpAngle = getAngle(edges.at(i));
        if(tmpAngle > angle){
            edges.insert(i, e);
            added = true;
            break;
        }
    }
    if(!added){
        edges.push_back(e);
    }
}

QVector2D OriVertex::getQVector2D()
{
    return QVector2D(x, y);
}

OriEdge *OriVertex::getPrevEdge(OriEdge *e)
{
    int index  = edges.indexOf(e);
    int N = edges.size();
    return edges.at((index - 1 + N) % N);
}

double OriVertex::getAngle(OriEdge *e)
{
    QVector2D dir;
    if(e->sv == this){
        dir = QVector2D(e->ev->x - this->x , e->ev->y - this->y );
    }else if (e->ev == this){
        dir = QVector2D(e->sv->x - this->x , e->sv->y - this->y );
    }else{
        qDebug()<<"OriVertex::getAngle : Err";
    }
    return atan2(dir.y(), dir.x());
}

