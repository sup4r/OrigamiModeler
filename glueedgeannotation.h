#ifndef GLUEEDGEANNOTATION_H
#define GLUEEDGEANNOTATION_H
#include "annotation.h"
class Halfedge2d;

class GlueEdgeAnnotation : public Annotation
{
    Q_OBJECT
public:
    GlueEdgeAnnotation(Halfedge2d *_he, QObject *parent  = 0);
    ~GlueEdgeAnnotation();
    Halfedge2d *getHe() const;
    void setHe(Halfedge2d *value);
    void draw();
    void debugPrint();
    void dragEvent(const QVector2D &mouseMove);
    double getD() const;
    void setD(double value);
private:
    Halfedge2d *he;
    double d;// [0, 1), 0: default -> 1: nearest
public slots:

signals:
    void deleted(Halfedge2d *he);
    void dChanged(Halfedge2d*, double);
};

#endif // GLUEEDGEANNOTATION_H
