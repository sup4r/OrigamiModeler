#ifndef TURNFOLDANNOTATION_H
#define TURNFLODANNOTATION_H
#include "annotation.h"
class Halfedge2d;
class TurnFoldAnnotation : public Annotation
{
    Q_OBJECT
public:
    TurnFoldAnnotation(Halfedge2d *_he1, Halfedge2d *_he2, QObject *parent = 0);
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

#endif // TURNFOLDANNOTATION_H
