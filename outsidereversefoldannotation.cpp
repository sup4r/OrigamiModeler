#include "outsidereversefoldannotation.h"
#include "gldrawutil.h"
#include "halfedge2d.h"
#include "face2d.h"
#include "vertex2d.h"
#include "face3d.h"
#include "halfedge3d.h"
#include "vertex3d.h"
#include <QOpenGLFunctions>
#include <QVector>
#include <QVector2D>
OutsideReverseFoldAnnotation::OutsideReverseFoldAnnotation(Halfedge2d *_he1, Halfedge2d *_he2, QObject *parent)
    :Annotation(parent)
{
    setHe1(_he1);
    setHe2(_he2);

    setColor(QColor(0,255,255));
//    setColor(QColor(160,160,240));
//    setSize(he1->getSize()+2);
    setAnnotationName(OUTSIDE);
//     ZOrder
//    Face3d *f1 = he1->face->getFace3d();
//    Face3d *f2 = he2->face->getFace3d();
//    f1->setZOrderValue(f2->getZOrder()+1);

}

OutsideReverseFoldAnnotation::~OutsideReverseFoldAnnotation()
{
    emit(deleted(he1, he2));
}

Halfedge2d *OutsideReverseFoldAnnotation::getHe1() const
{
    return he1;
}

void OutsideReverseFoldAnnotation::setHe1(Halfedge2d *value)
{
    he1 = value;
    addHes(value);
}
Halfedge2d *OutsideReverseFoldAnnotation::getHe2() const
{
    return he2;
}

void OutsideReverseFoldAnnotation::setHe2(Halfedge2d *value)
{
    he2 = value;
    addHes(value);

}

QVector<Halfedge2d *> OutsideReverseFoldAnnotation::getGlueHalfedge2ds()
{
    QVector<Halfedge2d*>glueEdges;
    if(isOverlapVertexSV1EV2()){
        glueEdges<<he1->next;
    }else{
        glueEdges<<he1->prev;
    }
    return glueEdges;
}

void OutsideReverseFoldAnnotation::draw()
{
    glLineWidth(getSize());
    glColor3d((float)getColor().red()/255, (float)getColor().green()/255, (float)getColor().blue()/255);
    QVector2D sv, ev;
    // edge
    sv = he1->getSV2D();
    ev = he1->getEV2D();
    GLDrawUtil::drawLine2D(sv, ev);
    sv = he2->getSV2D();
    ev = he2->getEV2D();
    GLDrawUtil::drawLine2D(sv, ev);
    // arrowr
    QVector2D v1  = he1->getCenter(),v2, v3, v4 = he2->getCenter();
    QVector2D n = he1->getNormal();
    double d = (v4-v1).length();
    v2 = v1+d*n;
    v3  = getOverlapPoint();
    GLDrawUtil::drawBezierArrow2D(v1,v2,v3,v4);
}

QVector2D OutsideReverseFoldAnnotation::getOverlapPoint()
{
    QVector2D overlapPoint(-1, -1);
    static const double eps  =1;
    QVector<Vertex2d*>vertices1, vertices2;
    vertices1<<he1->vertex<<he1->vertex_next;
    vertices2<<he2->vertex<<he2->vertex_next;
    for(int i = 0; i<vertices1.size(); ++i){
        QVector2D v1 = vertices1.at(i)->getVector2D();
        for(int j = 0; j<vertices2.size(); ++j){
            QVector2D v2 = vertices2.at(j)->getVector2D();
            if((v1-v2).length()<eps){
                overlapPoint = v1;
            }
        }
    }
    return overlapPoint;
}


bool OutsideReverseFoldAnnotation::isOverlapVertexSV1EV2()
{
    double eps = 1;
    return (he1->getSV2D()-he2->getEV2D()).length()<eps;
}
