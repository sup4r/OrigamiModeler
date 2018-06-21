#include "segment2d.h"
#include "halfedge2d.h"
#include <QDebug>
#include <QVector3D>
#define eps 1e-10
Segment2d::Segment2d(QVector2D _s, QVector2D _v)
{
    s=_s; v = _v;
}

Segment2d::Segment2d(Halfedge2d *he)
{
    s = he->getSV2D();
    v = he->getVec2D();
}

bool Segment2d::isParallel(Segment2d *seg)
{

    QVector2D vecA, vecB;
    vecA = v;
    vecB = seg->v;
    vecA.normalize();
    vecB.normalize();
    return (vecA - vecB).length() < eps || (vecA + vecB).length()< eps;
}

bool Segment2d::isOn(QVector2D vec)
{
    // 1-2, 3 ; vec is vertex
    if ((s - vec).length() < eps)return true;

    QVector2D vecA, vecB, vecA_, vecB_;
    vecA_ = vecA = v;
    vecB_ = vecB = vec - s;
    vecA.normalize();
    vecB.normalize();
    // isOn line
    return (vecA - vecB).length() < eps && vecA_.length() >= vecB_.length();
}

bool Segment2d::isOverlapSegment(Segment2d *seg)
{
    // 1-2, 3-4
    QVector3D s_v = s + v;
    QVector3D seg_s_v = seg->s + seg->v;

    return isParallel(seg) && (isOn(QVector2D(seg->s.x(), seg->s.y())) || isOn(QVector2D(seg_s_v.x(), seg_s_v.y())) || seg->isOn(QVector2D(s.x(), s.y())) || seg->isOn(QVector2D(s_v.x(), s_v.y())));
}

bool Segment2d::isIncludingSegment(Segment2d *seg)
{
    QVector3D seg_s_v = s + v;
    return isOn(QVector2D(seg->s.x(), seg->s.y())) && isOn(QVector2D(seg_s_v.x(), seg_s_v.y()));
}

void Segment2d::debugPrint()
{
    qDebug()<< "Segment2d::debugPrint:\ns : " << s << "\nv :" << v;
}

QVector2D Segment2d::getCrossPoint(Segment2d *seg)
{
    return QVector2D();
}
