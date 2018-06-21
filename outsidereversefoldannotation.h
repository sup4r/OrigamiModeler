#ifndef OUTSIDEREVERSEFOLDANNOTATION_H
#define OUTSIDEREVERSEFOLDANNOTATION_H
#include "annotation.h"
class Halfedge2d;
class Face3d;
class OutsideReverseFoldAnnotation : public Annotation
{
    Q_OBJECT
public:
    OutsideReverseFoldAnnotation(Halfedge2d *_he1, Halfedge2d *_he2, QObject *parent = 0);
    ~OutsideReverseFoldAnnotation();

    Halfedge2d *getHe1() const;
    void setHe1(Halfedge2d *value);

    Halfedge2d *getHe2() const;
    void setHe2(Halfedge2d *value);

    QVector<Halfedge2d*>getGlueHalfedge2ds();

protected:
    void draw();
private:
    Halfedge2d *he1, *he2;
    QVector2D getOverlapPoint();
    bool isOverlapVertexSV1EV2();
public slots:
signals:
    void deleted(Halfedge2d *_he1, Halfedge2d *_he2);
};

#endif // OUTSIDEREVERSEFOLDANNOTATION_H
