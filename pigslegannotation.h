#ifndef PIGSLEGANNOTATION_H
#define PIGSLEGANNOTATION_H
#include "annotation.h"
class Halfedge2d;
class PigsLegAnnotation : public Annotation
{
    Q_OBJECT
public:
    PigsLegAnnotation(Halfedge2d *he1, Halfedge2d *_he2, Halfedge2d *_he3, QObject *parent = 0);
    ~PigsLegAnnotation();
    void draw();

    Halfedge2d *getHe1() const;
    void setHe1(Halfedge2d *value);
    Halfedge2d *getHe2() const;
    void setHe2(Halfedge2d *value);
    Halfedge2d *getHe3() const;
    void setHe3(Halfedge2d *value);
    void dragEvent(const QVector2D &mouseMove);
private:
    Halfedge2d *he1, *he2, *he3;
signals:
    void deleted(Halfedge2d*,Halfedge2d*,Halfedge2d* );
};

#endif // PIGSLEGANNOTATION_H
