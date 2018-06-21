#ifndef INSIDEREVERSEFOLDANNOTATION_H
#define INSIDEREVERSEFOLDANNOTATION_H
#include "annotation.h"
class Halfedge2d;
class Face3d;
class InsideReverseFoldAnnotation : public Annotation
{
    Q_OBJECT
public:
    InsideReverseFoldAnnotation(Halfedge2d *_he1, Halfedge2d *_he2, QObject *parent = 0);
    ~InsideReverseFoldAnnotation();
    Halfedge2d *getHe1() const;
    void setHe1(Halfedge2d *value);
    Halfedge2d *getHe2() const;
    void setHe2(Halfedge2d *value);

    QVector<Halfedge2d*>getGlueHalfedge2ds();
protected:
    void draw();
    void calcZOrder();
private:
    Halfedge2d *he1, *he2;
    QVector3D getOverlapPoint();
    bool isOverlapVertexSV1EV2();
public slots:
signals:
    void deleted(Halfedge2d *_he1, Halfedge2d *_he2);
};

#endif // INSIDEREVERSEFOLDANNOTATION_H
