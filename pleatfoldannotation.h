#ifndef PLEATFOLDANNOTATION_H
#define PLEATFOLDANNOTATION_H
#include "annotation.h"
class Halfedge2d;
class PleatFoldAnnotation : public Annotation
{
    Q_OBJECT
public:
    PleatFoldAnnotation(Halfedge2d *_he1, Halfedge2d *_he2, QObject *parent = 0);
    ~PleatFoldAnnotation();
    Halfedge2d *getHe1() const;
    void setHe1(Halfedge2d *value);

    Halfedge2d *getHe2() const;
    void setHe2(Halfedge2d *value);
    void draw();
private:
    Halfedge2d *he1, *he2;

signals:
    void deleted(Halfedge2d*, Halfedge2d*);
};

#endif // PLEATFOLDANNOTATION_H
