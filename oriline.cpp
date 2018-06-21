#include "oriline.h"
#include <QDebug>
#include "orivertex.h"
#include "halfedge3d.h"
#include "vertex3d.h"
#include <QOpenGLFunctions>
OriLine::OriLine(Halfedge3d *he, bool _isCut)
{
    //OriLine(he->vertex->x, he->vertex->y, he->next->vertex->x, he->next->vertex->y, _isCut);
    p0 = QVector2D(he->vertex->x, he->vertex->y);
    p1 = QVector2D(he->next->vertex->x, he->next->vertex->y);
    setCutType(_isCut);
}

OriLine::OriLine(double p0x, double p0y, double p1x, double p1y, bool _isCut)
{
    p0 = QVector2D(p0x, p0y);
    p1 = QVector2D(p1x, p1y);
    setCutType(_isCut);
}

OriLine::OriLine(QVector2D _p0, QVector2D _p1, bool _isCut)//main
{
    p0 = QVector2D(_p0);
    p1 = QVector2D(_p1);
    setCutType(_isCut);
}

void OriLine::set(double x0, double y0, double x1, double y1){
    p0 = QVector2D(x0, y0);
    p1 = QVector2D(x1, y1);
}

void OriLine::set(QVector2D _p0, QVector2D _p1)
{
    p0 = _p0; p1 = _p1;
}

void OriLine::draw()
{
    static const int z = -1;
    glBegin(GL_LINES);
    glVertex3d(p0.x(), p0.y(), z);
    glVertex3d(p1.x(), p1.y(), z);
    glEnd();
}

void OriLine::setCutType(bool _isCut)
{
    isCut = _isCut;
}

bool OriLine::getCutType()
{
    return isCut;
}

void OriLine::debugPrint()
{
    qDebug()<<"OriLine p0"<<p0<<", p1"<<p1<<"CutType = "<<isCut;
}
