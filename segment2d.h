#ifndef SEGMENT2D_H
#define SEGMENT2D_H
#include <QVector2D>
class Halfedge2d;

class Segment2d{
public:
    QVector2D s;// start
    QVector2D v;// vector

    Segment2d(QVector2D _s, QVector2D _v);
    Segment2d(Halfedge2d *he);

    bool isParallel(Segment2d *seg);
    bool isOn(QVector2D vec);
    bool isOverlapSegment(Segment2d *seg);
    bool isIncludingSegment(Segment2d *seg);
    void debugPrint();
    QVector2D getCrossPoint(Segment2d *seg);
};

#endif // SEGMENT2D_H
