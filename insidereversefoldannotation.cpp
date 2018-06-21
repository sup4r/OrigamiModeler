#include "insidereversefoldannotation.h"
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
#include <QDebug>
InsideReverseFoldAnnotation::InsideReverseFoldAnnotation(Halfedge2d *_he1, Halfedge2d *_he2, QObject *parent)
    :Annotation(parent)
{
    // setting
    setHe1(_he1);
    setHe2(_he2);
    setColor(QColor(120,120,240));
//    setSize(he1->getSize()+1);
    setAnnotationName(INSIDE);
//    // ZOrder
//    if(he1->face->getMirrorTimes()!=he2->face->getMirrorTimes()){
//        qDebug()<<"InsideReverseFoldAnnotation::InsideReverseFoldAnnotation: mT is not equal";
//    }else{
//        Face3d *f1 = he1->face->getFace3d();
//        Face3d *f2 = he2->face->getFace3d();
//        f1->setZOrderValue(f2->getZOrder()+1);
//        calcZOrder();
//    }
    // fix normals
//    he1->face->fixNormal();he2->face->fixNormal();

}

InsideReverseFoldAnnotation::~InsideReverseFoldAnnotation()
{
    emit(deleted(he1, he2));
}

Halfedge2d *InsideReverseFoldAnnotation::getHe2() const
{
    return he2;
}

void InsideReverseFoldAnnotation::setHe2(Halfedge2d *value)
{
    he2 = value;
    addHes(value);
}

QVector<Halfedge2d *> InsideReverseFoldAnnotation::getGlueHalfedge2ds()
{
    QVector<Halfedge2d *> glueHalfedges;
    if(isOverlapVertexSV1EV2()){
        glueHalfedges<<he2->prev;//<<he1->next
    }else{
        glueHalfedges<<he2->next;//<<he1->prev;
    }
    return glueHalfedges;
}

Halfedge2d *InsideReverseFoldAnnotation::getHe1() const
{
    return he1;
}

void InsideReverseFoldAnnotation::setHe1(Halfedge2d *value)
{
    he1 = value;
    addHes(value);
}

void InsideReverseFoldAnnotation::draw()
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
    // arrow
    sv = he1->getCenter();
    ev = he2->getCenter();
    ev = he2->face->getG();
    GLDrawUtil::drawSinArrow2D(sv,ev);

}

void InsideReverseFoldAnnotation::calcZOrder()
{
    int mirrorTimes = he1->face->getMirrorTimes();
    QVector<Face3d*> fs1 = he1->face->getPositiveFace3dsInMirrorGroup(), fs2 = he2->face->getPositiveFace3dsInMirrorGroup();
    QVector<int>orderA, orderB;
    // init
    orderA<<fs1.at(0)->getZOrder(); orderB<<fs2.at(0)->getZOrder();
      // calc positive order
    for(int i  = 0; i < mirrorTimes-1; ++i){
        int firstMirrorZPosition = -1+((orderA.at(orderA.size()-1)<orderB.at(orderB.size()-1))?orderA.at(orderA.size()-1):orderB.at(orderB.size()-1));

        int N = orderA.size();
        // reverse
        for(int j = 0; j < N; ++j){
            orderA<<-orderA.at(j)+firstMirrorZPosition*2+1;
            orderB<<-orderB.at(j)+firstMirrorZPosition*2+1;
        }
        // rift up
        int iRift =( (orderA.at(0)>orderB.at(0))?orderA.at(0):orderB.at(0) ) - firstMirrorZPosition*2;
        for(int j = 0; j < orderA.size(); ++j){
            orderA[j]=orderA.at(j)+iRift;
            orderB[j]=orderB.at(j)+iRift;
        }
    }
    // setZOrder
    for(int i = 0; i < fs1.size(); ++i){
        fs1.at(i)->setZOrderValue(orderA.at(i));
        fs2.at(i)->setZOrderValue(orderB.at(i));
    }


}

QVector3D InsideReverseFoldAnnotation::getOverlapPoint()
{
    QVector2D overlapPoint(-1, -1);
    static const double eps  =1e-6;
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

bool InsideReverseFoldAnnotation::isOverlapVertexSV1EV2()
{
    double eps = 1e-6;
    return (he1->getSV2D()-he2->getEV2D()).length()<eps;
}

