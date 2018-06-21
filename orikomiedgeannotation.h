#ifndef ORIKOMIEDGEANNOTATION_H
#define ORIKOMIEDGEANNOTATION_H
#include "annotation.h"
class Halfedge2d;
class OrikomiEdgeAnnotation:public Annotation
{
    Q_OBJECT
public:
    OrikomiEdgeAnnotation(Halfedge2d *_he, QObject *parent  =0);

    Halfedge2d *getHe() const;
    void setHe(Halfedge2d *value);
    void draw();
private:
    Halfedge2d *he;
signals:
    void deleted(Halfedge2d*);
};

#endif // ORIKOMIEDGEANNOTATION_H
