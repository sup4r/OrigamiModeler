#ifndef GLDRAWUTIL_H
#define GLDRAWUTIL_H
#include <QVector2D>
#include <QVector3D>
class GLDrawUtil
{
public:
    /* 2D */
   static void drawArrow2D(const QVector2D &sv, const QVector2D &ev);
   static void drawArrowHead2D(const QVector2D &sv, const QVector2D &ev);
   static void drawLine2D(const QVector2D &sv, const QVector2D &ev);
   static void drawCircle2D(const QVector2D &cv, const double r);
   static void drawFilledCircle2D(const QVector2D &cv, const double r);
   static void drawHalfCircle2D(const QVector2D &cv, const double r);
   static void drawCircleArrow2D(const QVector2D &cv, const double r);
   static void drawSinCurve2D(const QVector2D &sv, const QVector2D &ev);
   static void drawSinArrow2D(const QVector2D &sv, const QVector2D &ev);
   static void drawBezierCurve2D(const QVector2D &v1, const QVector2D &v2, const QVector2D &v3, const QVector2D &v4);
   static void drawBezierArrow2D(const QVector2D &v1, const QVector2D &v2, const QVector2D &v3, const QVector2D &v4);
   /* 3D */
   static void drawRect3D(const int w, const int h);
};

#endif // GLDRAWUTIL_H
