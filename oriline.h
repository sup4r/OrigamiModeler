#ifndef ORILINE_H
#define ORILINE_H

#include <QVector2D>

class Halfedge3d;

class OriLine
{
public:
    OriLine(Halfedge3d *he, bool _isCut=true);
    OriLine(double p0x, double p0y, double p1x, double p1y, bool _isCut);
    OriLine(QVector2D _p0, QVector2D _p1, bool _isCut=true);
    QVector2D p0, p1;

    void debugPrint();
    void set(double x0, double y0, double x1, double y1);
    void set(QVector2D _p0, QVector2D _p1);
    void draw();
    void setCutType(bool _isCut);
    bool getCutType();
private:
    bool isCut;
};

#endif // ORILINE_H
