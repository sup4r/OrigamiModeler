#pragma once
#include "vertex2d.h"
#include "halfedge2d.h"
#include "face2d.h"
#include "model2d.h"
#include "model3d.h"
#include "annotation.h"
#include <QOpenGLFunctions>
#include <QDebug>
#include "gldrawutil.h"
#include <QtMath>
#include <QPair>

Model2d::Model2d(QObject *parent)
    : ModelBase(parent)
{

//    setFaceColor(QColor(200,220,240));
    setFaceColor(QColor(235,235,235));
    setFlipedFaceColor(QColor(230,230,230));

    setBranchFaceColor(QColor(240,220,200));
    m3d = NULL;

}

Model3d *Model2d::getM3d() const
{
    return m3d;
}

void Model2d::setM3d(Model3d *value)
{
    m3d = value;
    if(m3d->getM2d()==NULL){
        m3d->setM2d(this);
    }
}

void Model2d::movePosition(QVector2D vec)
{
    for(int i = 0; i < vertices.size(); ++i){
        vertices.at(i)->setMoved(false);
    }
    for(int i = 0; i< vertices.size(); ++i){
        if(!vertices.at(i)->getMoved())vertices.at(i)->movePosition(vec);
    }
}

void Model2d::drawBoundingBox()
{
    /* calc xmin, ymin, xmax, ymax */
    static const double M = 800;
    double xmin = M,ymin = M,xmax = 0, ymax = 0;
    for(int i = 0; i<vertices.size(); ++i){
        QVector2D v = vertices.at(i)->getVector2D();
        if(xmin>v.x())xmin=v.x();
        if(ymin>v.y())ymin=v.y();
        if(xmax<v.x())xmax=v.x();
        if(ymax<v.y())ymax=v.y();
    }
    const float scl = 1.5;
    const double w = scl*(xmax-xmin), h = scl*(ymax-ymin);
    const double dx = xmin-(scl-1)*(xmax-xmin)/2, dy = ymin-(scl-1)*(ymax-ymin)/2;
    /* draw */
    glDisable(GL_LIGHTING);
    glColor3d(1,0,0);
    glLineWidth(1);
    glPushMatrix();
    glTranslated(dx,dy,0);
    GLDrawUtil::drawRect3D(w,h);
    glPopMatrix();
}

bool Model2d::areFacesConnectedWithNoError(QVector<Annotation *> *annotations)
{

    QVector<bool> bz;
    for(int i = 0; i < annotations->size(); ++i){
        Annotation *a = annotations->at(i);
        int name = a->getAnnotationName();
        if(name == GLUE_E||name == SINK|| name==ORIKOMI){
            continue;
        }else{
            Halfedge2d *he1, *he2;
            he1 = a->getHes().at(0);
            he2 = a->getHes().at(1);
            bool b = true;
            if(name==INSIDE || name == OUTSIDE){
                b = (he1->getFace()->isFront() != he2->getFace()->isFront());
            }else if(name == PLEAT){
                b = (he1->getFace()->isFront() == he2->getFace()->isFront());
            }else if(name == PIG){
                Halfedge2d *he3 = a->getHes().at(2);
                b = ((he1->getFace()->isFront() == he2->getFace()->isFront())&&(he1->getFace()->isFront() == he3->getFace()->isFront()));
            }
            bz.push_back(b);
        }
    }
    bool b = true;
    for(int i = 0; i < bz.size(); ++i){
        b &= bz.at(i);
    }
//    qDebug()<<bz;
//    qDebug()<<b;
    return b;
}

void Model2d::fixFaceNormals(QVector<Annotation *> *annotations)
{
    QVector<bool> faceNormals = getNoErrorFaceNormals(annotations);
    applyFaceNormals(faceNormals);
}

QVector<bool> Model2d::getCurrentFaceNormals()
{
    QVector<bool> faceNormals;
    for(int i = 0; i< faces.size(); ++i){
        faceNormals.push_back(faces.at(i)->isFront());
    }
    return faceNormals;
}

bool Model2d::areFacesConnectedWithNoErrorWithFaceNormals(QVector<Annotation *> *annotations, QVector<bool> faceNormals)
{
    QVector<bool> bz;
    for(int i = 0; i < annotations->size(); ++i){
        Annotation *a = annotations->at(i);
        int name = a->getAnnotationName();
        if(name == GLUE_E||name == SINK|| name==ORIKOMI){
            continue;
        }
        Halfedge2d *he1, *he2;
        he1 = a->getHes().at(0);
        he2 = a->getHes().at(1);
        int id1 = faces.indexOf(he1->getFace()), id2 = faces.indexOf(he2->getFace());
        bool b = true;
        if(name==INSIDE || name == OUTSIDE){
            b = (faceNormals.at(id1) != faceNormals.at(id2));
        }else if(name == PLEAT){
            b = (faceNormals.at(id1) == faceNormals.at(id2));
        }else if(name == PIG){
            Halfedge2d *he3 = a->getHes().at(2);
            int id3 = faces.indexOf(he3->getFace());
            b = ((faceNormals.at(id1) == faceNormals.at(id2))&&(faceNormals.at(id1) == faceNormals.at(id3)));
        }
        bz.push_back(b);
    }
    bool b = true;
    for(int i = 0; i < bz.size(); ++i){
        b &= bz.at(i);
    }
//    qDebug()<<bz;
//    qDebug()<<b;
    return b;
}



QVector<bool> Model2d::getNoErrorFaceNormals(QVector<Annotation *> *annotations)
{
    const QVector<bool> baseNormals = getCurrentFaceNormals();
    QVector<bool> faceNormals2;
    int N = qPow(2, faces.size());
    bool isFound = false;
    for(int i = 0; i < N; ++i){
        /* flip */
        QVector<bool>faceNormals;
       for(int j = 0; j<baseNormals.size(); ++j){
           bool bTest = !(i&(int)qPow(2,j));
           faceNormals.push_back((bTest)?baseNormals.at(j):!baseNormals.at(j));
       }
       /* check */
       if(areFacesConnectedWithNoErrorWithFaceNormals(annotations, faceNormals)){
           isFound = true;
           faceNormals2 = faceNormals;
           break;
       }
    }
//    qDebug()<<"Model2d::getNoErrorFaceNormals : isFound == "<<isFound;
//    qDebug()<<faceNormals2;
    return faceNormals2;
}

void Model2d::applyFaceNormals(QVector<bool> faceNormals)
{
    for(int i = 0; i < faces.size(); ++i){
        Face2d *f = faces.at(i);
        if(f->isFront()!=faceNormals.at(i)){
            f->flip2DAnd3D();
        }
    }
}

bool Model2d::areFacesOverlappingWithNoError(QVector<Annotation *> *annotations)
{
    QVector<int> test = getCurrentFaceOrders();
//    qDebug()<<"current faceOrders are"<<test;
    bool bTest = areFacesOverlappingWithNoErrorWithFaceOrders(annotations, test);
    return bTest;
}

void Model2d::fixFaceOrders(QVector<Annotation *> *annotations)
{
    QVector<int> faceOrders = getNoErrorFaceOrders(annotations);
    applyFaceOrders(faceOrders);
}

QVector<int> Model2d::getCurrentFaceOrders()
{
    QVector<int> faceOrders;
    for(int i = 0; i < faces.size(); ++i){
        faceOrders.push_back(faces.at(i)->getZOrder());
    }
    return faceOrders;
}

bool Model2d::areFacesOverlappingWithNoErrorWithFaceOrders(QVector<Annotation *> *annotations, QVector<int> faceOrders)
{
    QVector<bool> bz;
    for(int i = 0; i < annotations->size(); ++i){
        Annotation *a = annotations->at(i);
        int name = a->getAnnotationName();
        if(name == GLUE_E||name == SINK|| name==ORIKOMI|| name==PIG){
            continue;
        }
        Halfedge2d *he1, *he2;
        he1 = a->getHes().at(0);
        he2 = a->getHes().at(1);
        int id1 = faces.indexOf(he1->getFace()), id2 = faces.indexOf(he2->getFace());
        int order1 = faceOrders.at(id1), order2 = faceOrders.at(id2);
        bool b = true;
        if(name==INSIDE || name == OUTSIDE || name == PLEAT){
            b = order1>order2;
        }/*else if(name==PIG){
            Halfedge2d *he3 = a->getHes().at(2);
            int id3 = faces.indexOf(he3->getFace());
            int order3 = faceOrders.at(id3);
            b = order1>order2&&order1>order3;
        }*/
        bz.push_back(b);
    }
    bool b = true;
    for(int i = 0; i < bz.size(); ++i){
        b &= bz.at(i);
    }
//    qDebug()<<bz;
//    qDebug()<<b;
    return b;
}
QVector<int> Model2d::getNoErrorFaceOrders(QVector<Annotation *> *annotations){
    QVector<int> faceOrders;
    /* make graph */
    QVector<QPair<int,int>> edges;
    for(int i = 0; i < annotations->size(); ++i){
        Annotation *a = annotations->at(i);
        int name = a->getAnnotationName();
        if(name == GLUE_E||name == SINK|| name==ORIKOMI||name==PIG){
            continue;
        }
        Halfedge2d *he1, *he2;
        he1 = a->getHes().at(0);
        he2 = a->getHes().at(1);
        int id1 = faces.indexOf(he1->getFace()), id2 = faces.indexOf(he2->getFace());
        if(name==INSIDE || name == OUTSIDE || name == PLEAT){
            QPair<int, int> edge(id1, id2);
            edges.push_back(edge);
        }/*else if(name = PIG){
            Halfedge2d *he3 = a->getHes().at(2);
            int id3 = faces.indexOf(he3->getFace());
            QPair<int, int> edge1(id1, id2), edge2(id1, id3);
            edges.push_back(edge1);edges.push_back(edge2);
        }*/
    }
    /* count indeg */

    QVector<int> S;
    for(int i = 0; i < faces.size(); ++i){// i is faces[i]

        int indeg = 0;
        for(int j = 0; j < edges.size(); ++j){
            if(edges.at(j).second == i){
                indeg++;
            }
        }
        if(indeg==0){
            S.push_back(i);
        }
    }
//    qDebug()<<"S is "<<S;
    /* calculate heights */
    QVector<int> heights;
    for(int i = 0; i < faces.size(); ++i){
        heights.push_back(0);
    }
    for(int i = 0; i < S.size(); ++i){
        int p = S.at(i);
        heights = searchP(p,edges, heights);
    }
//    qDebug()<<"heights = "<<heights;

    /* calculate face orders */
    int Max = 0;
    for(int i = 0; i < heights.size(); ++i){
        Max = qMax(heights.at(i), Max);
    }
    for(int i = 0; i < faces.size(); ++i){
        faceOrders.push_back(Max-heights.at(i)+1);
    }
//    qDebug()<<"new faceOrders are"<<faceOrders;

    if(areFacesOverlappingWithNoErrorWithFaceOrders(annotations, faceOrders)){
        qDebug()<<"Model2d::getNoErrorFaceOrders: change face orders completed";
    }else{
        qDebug()<<"Model2d::getNoErrorFaceOrders: ERROR";
    }
    return faceOrders;
}
QVector<int> Model2d::searchP(const int p, const QVector<QPair<int, int>> edges, QVector<int> heights){
    /* get edges strated from p */
    QVector<QPair<int, int>> es;
    for(int j = 0; j < edges.size(); ++j){
        if(edges.at(j).first==p){
            es.push_back(edges.at(j));
        }
    }

    for(int i = 0; i <es.size(); ++i){
        int child = es.at(i).second;
        heights[child] = (heights[p]+1>heights[child])?heights[p]+1:heights[child];
    }
    for(int i = 0; i <es.size(); ++i){
        int child = es.at(i).second;
        heights = searchP(child, edges, heights);
    }
//    qDebug()<<"heights after "<<p<<" is "<<heights;
    return heights;
}

QVector<int> Model2d::getNoErrorFaceOrders_old(QVector<Annotation *> *annotations)
{

    QVector<int> faceOrders;

    /* vvvvv topological sort vvvvv */

    QVector<QPair<int, int>> edges;//graph edges are represented by QPair<int, int>. graph nodes are faces' id(this->faces.indexOf(f));

    /* make graph */
    for(int i = 0; i < annotations->size(); ++i){
        Annotation *a = annotations->at(i);
        int name = a->getAnnotationName();
        if(name == GLUE_E||name == SINK|| name==ORIKOMI || name == PIG){
            continue;
        }
        Halfedge2d *he1, *he2;
        he1 = a->getHes().at(0);
        he2 = a->getHes().at(1);
        int id1 = faces.indexOf(he1->getFace()), id2 = faces.indexOf(he2->getFace());
        if(name==INSIDE || name == OUTSIDE || name == PLEAT){
            QPair<int, int> edge(id1, id2);
            edges.push_back(edge);
        }
    }
    /* count indeg */
    QVector<int> indegs;
    QVector<int> S;
    for(int i = 0; i < faces.size(); ++i){// i is faces[i]
        int indeg = 0;
        for(int j = 0; j < edges.size(); ++j){
            if(edges.at(j).second == i){
                indeg++;
            }
        }
        indegs.push_back(indeg);
        if(indeg==0){
            S.push_back(i);
        }
    }
    /* pop S */
    QVector<int> ans;
    while(S.size()>0){
        int iPop = S.front();
        ans.push_back(iPop);
        S.pop_front();
        for(int i = 0; i < edges.size(); ++i){
            if(edges.at(i).first == iPop){
                int j = edges.at(i).second;
                indegs[j] = indegs.at(j)-1;
                if(indegs.at(j)==0){
                    S.push_back(j);
                }
            }
        }
    }

    /* ^^^^^ topological sort ^^^^^ */

    /* caclulate face orders using topological answer */

    for(int i = 0; i <faces.size(); ++i){
        for(int j = 0; j < ans.size(); ++j){
            if(i == ans.at(j)){
                int faceOrder = ans.size()-j;
                faceOrders.push_back(faceOrder);
                break;
            }
        }
    }


    qDebug()<<"new faceOrders are"<<faceOrders;
    return faceOrders;
}

void Model2d::applyFaceOrders(QVector<int> faceOrders)
{
    for(int i = 0; i < faces.size(); ++i){
        faces.at(i)->setZOrder(faceOrders.at(i));
    }
}

void Model2d::draw()
{
    glMatrixMode(GL_MODELVIEW);
    glLoadIdentity();
    drawVertices();
    drawFacesWithAlphaBlend();
//    drawFaces();
    drawHalfedges();

}

void Model2d::drawWithoutSize()
{
    glMatrixMode(GL_MODELVIEW);
    glLoadIdentity();

    drawFacesWithAlphaBlend();

    drawVerticesWithoutSize();
    drawHalfedgesWithoutSize();


}

void Model2d::drawVerticesWithoutSize()
{
    if(vertices.empty()){
        return;
    }
    glDisable(GL_BLEND);
    for(int i = 0; i<vertices.size(); ++i){
        glLoadName(i);
        Vertex2d *v = vertices.at(i);

        v->drawWithoutSize();


    }

}

void Model2d::drawHalfedgesWithoutSize()
{
    if(halfedges.empty()){
        return;
    }
    for(int i=0; i<halfedges.size(); ++i){
        glLoadName(i);
        Halfedge2d *he = halfedges.at(i);

        glEnable(GL_BLEND);
        glBlendFunc(GL_SRC_ALPHA,GL_ONE_MINUS_SRC_ALPHA);
        glEnable(GL_LINE_SMOOTH);
        if(!he->getIsSelected())he->drawWithoutSize();
        glDisable(GL_LINE_SMOOTH);
        glDisable(GL_BLEND);
    }

    for(int i=0; i<halfedges.size(); ++i){
        glLoadName(i);
        Halfedge2d *he = halfedges.at(i);

        glEnable(GL_BLEND);
        glBlendFunc(GL_SRC_ALPHA,GL_ONE_MINUS_SRC_ALPHA);
        glEnable(GL_LINE_SMOOTH);
        if(he->getIsSelected())he->drawWithoutSize();
        glDisable(GL_LINE_SMOOTH);
        glDisable(GL_BLEND);
    }

}
void Model2d::drawUnselectedHalfedgesWithoutSize(){
    if(halfedges.empty()){
        return;
    }
    for(int i=0; i<halfedges.size(); ++i){
        glLoadName(i);
        Halfedge2d *he = halfedges.at(i);

        glEnable(GL_BLEND);
        glBlendFunc(GL_SRC_ALPHA,GL_ONE_MINUS_SRC_ALPHA);
        glEnable(GL_LINE_SMOOTH);
        if(!he->getIsSelected())he->drawWithoutSize();
        glDisable(GL_LINE_SMOOTH);
        glDisable(GL_BLEND);
    }
}
void Model2d::drawSelectedHalgedgesWithoutSize(){
    if(halfedges.empty()){
        return;
    }
    for(int i=0; i<halfedges.size(); ++i){
        glLoadName(i);
        Halfedge2d *he = halfedges.at(i);

        glEnable(GL_BLEND);
        glBlendFunc(GL_SRC_ALPHA,GL_ONE_MINUS_SRC_ALPHA);
        glEnable(GL_LINE_SMOOTH);
        if(he->getIsSelected())he->drawWithoutSize();
        glDisable(GL_LINE_SMOOTH);
        glDisable(GL_BLEND);
    }
}

void Model2d::drawSmallFaces()
{
    if(faces.empty()){
        return;
    }

    static const float c = 1.;
    static const float s = 0.5;
    glColor3d(c,c,c);
    for(int i = 0; i < faces.size(); ++i){
        Face2d *f = faces.at(i);
        if(f == NULL){
            qDebug()<<"Model2d::drawFaces() : f == NULL";
        }
        QVector2D g = f->getG();
        glLoadName(i);
        glPushMatrix();


        glTranslated(g.x(), g.y(), 0);
        glScaled(s,s,0);
        glTranslated(-g.x(), -g.y(), 0);
        f->drawWithoutColor();
        glPopMatrix();
    }
}

void Model2d::drawSmallFaces(const float s)
{
    if(faces.empty()){
        return;
    }

    static const float c = 1.;

    glColor3d(c,c,c);
    for(int i = 0; i < faces.size(); ++i){
        Face2d *f = faces.at(i);
        if(f == NULL){
            qDebug()<<"Model2d::drawFaces() : f == NULL";
        }
        QVector2D g = f->getG();
        glLoadName(i);
        glPushMatrix();


        glTranslated(g.x(), g.y(), 0);
        glScaled(s,s,0);
        glTranslated(-g.x(), -g.y(), 0);
        f->drawWithoutColor();
        glPopMatrix();
    }
}

void Model2d::drawNormals()
{

    glLineWidth(1);
    glColor3d(0,0,0);
    for(int i = 0; i < faces.size(); ++i){
        faces.at(i)->displayNormal();
    }

}

void Model2d::drawFaceOrder()
{
    glLineWidth(1);
    glColor3d(1,0,0);
    for(int i =0; i < faces.size();++i){
        faces.at(i)->displayFaceOrder();
    }
}

void Model2d::addVertex(Vertex2d *vertex)
{
    vertices.push_back(vertex);
    vertex->setColor(getVertexColor());
    vertex->setSize(getPointSize());
}

void Model2d::addHalfedge(Halfedge2d *halfedge)
{
    halfedges.push_back(halfedge);
    halfedge->setColor(getHalfedgeColor());
    halfedge->setSize(getLineWidth());
}

void Model2d::addFace(Face2d *face)
{
    face->setModel(this);//170214
    faces.push_back(face);
    if(!face->isFront()){
        if(face->getIsBranch()){
            face->setColor(getBranchFaceColor());
            face->setColor(getBranchFaceColor());
        }else{
            face->setColor(getFaceColor());
            face->setDefaultColor(getFaceColor());
        }
    }else{
        face->setColor(getFlipedFaceColor());
        face->setDefaultColor(getFlipedFaceColor());
    }
}

QColor Model2d::getBranchFaceColor() const
{
    return branchFaceColor;
}

void Model2d::setBranchFaceColor(const QColor &value)
{
    branchFaceColor = value;
}

Halfedge2d *Model2d::getHalfedge(int vertexID1, int vertexID2)
{
    Vertex2d *v1 = vertices.at(vertexID1), *v2 = vertices.at(vertexID2);
    Halfedge2d *he = NULL;
    for(int i = 0; i < halfedges.size(); ++i){
        Vertex2d *va = halfedges.at(i)->vertex, *vb = halfedges.at(i)->vertex_next;
        if(v1==va&&v2==vb){
            he = halfedges.at(i);
            break;
        }
    }

    return he;
}


void Model2d::drawVertices()
{
    if(vertices.empty()){
        return;
    }
    glDisable(GL_BLEND);
    for(int i = 0; i<vertices.size(); ++i){
        glLoadName(i);
        Vertex2d *v = vertices.at(i);
        v->draw();
    }

}

void Model2d::drawHalfedges()
{
    if(halfedges.empty()){
        return;
    }
    for(int i=0; i<halfedges.size(); ++i){
        glLoadName(i);
        Halfedge2d *he = halfedges.at(i);
        glEnable(GL_BLEND);
        glBlendFunc(GL_SRC_ALPHA,GL_ONE_MINUS_SRC_ALPHA);
        glEnable(GL_LINE_SMOOTH);
        if(!he->getIsSelected())he->draw();
        glDisable(GL_LINE_SMOOTH);
        glDisable(GL_BLEND);
    }
    for(int i=0; i<halfedges.size(); ++i){
        glLoadName(i);
        Halfedge2d *he = halfedges.at(i);
        glEnable(GL_BLEND);
        glBlendFunc(GL_SRC_ALPHA,GL_ONE_MINUS_SRC_ALPHA);
        glEnable(GL_LINE_SMOOTH);
//        if(he->getIsSelected())he->draw();
        glDisable(GL_LINE_SMOOTH);
        glDisable(GL_BLEND);
    }

}

void Model2d::drawFaces()
{
    if(faces.empty()){
        return;
    }
    glDisable(GL_BLEND);
    for(int i = 0; i < faces.size(); ++i){
        Face2d *f = faces.at(i);
        if(f == NULL){
            qDebug()<<"Model2d::drawFaces() : f == NULL";
        }
        glLoadName(i);

        f->draw();
    }


}
void Model2d::drawFacesWithAlphaBlend()
{
    if(faces.empty()){
        return;
    }
    glDisable(GL_DEPTH_TEST);
    glEnable(GL_BLEND);
//    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
    glBlendFunc(GL_ZERO, GL_SRC_COLOR);
    for(int i = 0; i < faces.size(); ++i){
        Face2d *f = faces.at(i);
        if(f == NULL){
            qDebug()<<"Model2d::drawFaces() : f == NULL";
        }
        glLoadName(i);
        f->draw();
    }

    glDisable(GL_BLEND);
    glEnable(GL_DEPTH_TEST);
}
