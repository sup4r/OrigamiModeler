#include "face3d.h"
#include "halfedge3d.h"
#include "subfacebuilder.h"
#include "subface.h"
#include "oriedge.h"
#include "oriline.h"
#include "orihalfedge.h"
#include "orivertex.h"
#include <QtMath>
#include <QtAlgorithms>
#include <QDebug>
#define eps 1e-6

QVector<SubFace *> SubFaceBuilder::build(const QVector<Face3d *> faces)
{
    QVector<SubFace*>subfaces;

    //make lines
    QVector<OriLine*>lines;
    for(int i=0; i<faces.size(); ++i){
        Face3d *face = faces.at(i);
        Halfedge3d *he = face->halfedge;
        do{
            OriLine *line = new OriLine(he, !he->hasPair());
            //line->debugPrint();
            addOriLine(line, lines);
            he = he->next;
        }while(he != face->halfedge);
    }

    //clean
    cleanDuplicatedOriLines(lines);

    //make subfaces
    subfaces = buildSubfaceFromOriLines(lines);

    //delete overlap subface
    qDebug()<<subfaces.size();
    deleteOverlappSubFace(subfaces);
    qDebug()<<subfaces.size();
    //test

//    qDebug()<<"SubFaceBuilder::build : ";
//    for(int i = 0; i < faces.size(); ++i){
//        Face3d *f  =faces.at(i);
//        Halfedge3d *he = f->halfedge;
//        do{
//            //qDebug()<<"Halfedge3d hasPair  = "<<he->hasPair();
//            he = he->next;
//        }while(he != f->halfedge);
//    }

    return subfaces;
}

// Test Function 160608
QVector<OriLine *> SubFaceBuilder::getOriLines(const QVector<Face3d *> faces)
{
    //make lines
    QVector<OriLine*>lines;
    for(int i=0; i<faces.size(); ++i){
        Face3d *face = faces.at(i);
        Halfedge3d *he = face->halfedge;
        do{
            OriLine *line = new OriLine(he);
            addOriLine(line, lines);
            he = he->next;
        }while(he != face->halfedge);
    }

    //clean
    cleanDuplicatedOriLines(lines);
    return lines;
}

void SubFaceBuilder::addOriLine(OriLine *inputLine, QVector<OriLine *> &lines)
{
    QVector<OriLine*> crossingLines;
    QVector<OriLine*> tmpLines;
    // if it already exists, do nothing
    for(int i = 0; i<lines.size(); ++i){
        OriLine *line = lines.at(i);
        tmpLines.push_back(line);//init tmpLines
        if(isSameLineSegment(inputLine, line))return;
    }
    // if it intersects other line, devide them
    for(int i = 0; i<tmpLines.size(); ++i){
        OriLine *line = tmpLines.at(i);
        QVector2D *crossPoint = getCrossPoint(inputLine, line);
        if(crossPoint == NULL){
            continue;
        }
        crossingLines.push_back(line);
        lines.removeOne(line);
        if(((*crossPoint)-line->p0).length()>eps){
            lines.push_back(new OriLine(line->p0, *crossPoint, line->getCutType()));
        }
        if(((*crossPoint)-line->p1).length()>eps){
            lines.push_back(new OriLine(line->p1, *crossPoint, line->getCutType()));
        }
    }
    // devide inputLine and add list
    QVector<QVector2D>points;
    points.push_back(inputLine->p0); points.push_back(inputLine->p1);
    for(int i = 0; i<lines.size(); ++i){
        OriLine *line  =lines.at(i);
        if((inputLine->p0-line->p0).length()<eps
                ||(inputLine->p0-line->p1).length()<eps
                ||(inputLine->p1-line->p0).length()<eps
                ||(inputLine->p1-line->p1).length()<eps)
        {
            continue;
        }
        if(distancePointToSegment(line->p0, inputLine)<eps){
            points.push_back(line->p0);
        }
        if(distancePointToSegment(line->p1, inputLine)<eps){
            points.push_back(line->p1);
        }
        QVector2D *crossPoint =getCrossPoint(inputLine, line);
        if(crossPoint != NULL)points.push_back(*crossPoint);
    }
    bool sortByX = fabs(inputLine->p0.x()-inputLine->p1.x())>fabs(inputLine->p0.y()-inputLine->p1.y());
    if(sortByX){
        qSort(points.begin(), points.end(), compQVector2DX);
    }else{
        qSort(points.begin(), points.end(), compQVector2DY);
    }
    QVector2D prePoint = points.at(0);
    for(int i = 1; i < points.size(); ++i){
        QVector2D p = points.at(i);
        if((prePoint-p).length()<eps){
            continue;
        }
        lines.push_back(new OriLine(prePoint, p, inputLine->getCutType()));
        prePoint=p;
    }
}
void SubFaceBuilder::cleanDuplicatedOriLines(QVector<OriLine *> &lines)
{
    qDebug()<<"SubFaceBuilder::cleanDuplicatedOriLines : ";
    qDebug()<<"pre-clean lines.size() = "<<lines.size();
    QVector<OriLine*>tmpLines;
    for(int i = 0; i<lines.size(); ++i){
        OriLine *l = lines.at(i);
        bool bSame = false;
        for(int j = 0; j < tmpLines.size(); ++j){
            OriLine *ll = tmpLines.at(j);
            if(isSameLineSegment(l, ll)){
                // same
                if(!ll->getCutType()){
                    l->setCutType(false);
                }
                bSame = true;
                break;
            }
        }
        if(!bSame){
            tmpLines.push_back(l);
        }
    }
    lines.clear();
    for(int i = 0; i<tmpLines.size(); ++i){
        lines.push_back(tmpLines.at(i));
    }
    qDebug()<<"cleaned lines.size() = "<<lines.size();
}

QVector<SubFace *> SubFaceBuilder::buildSubfaceFromOriLines(QVector<OriLine *> &lines)
{
    QVector<SubFace*> subfaces;
    QVector<OriVertex*> vertices;
    QVector<OriEdge*> edges;

    for(int i = 0; i<lines.size(); ++i){
        OriLine *line = lines.at(i);
        OriVertex *sv = addAndGetVertexfromVVec(line->p0, vertices);
        OriVertex *ev = addAndGetVertexfromVVec(line->p1, vertices);
        OriEdge *eg = new OriEdge(sv, ev, line->getCutType());
        edges.push_back(eg);
        sv->addEdge(eg);
        ev->addEdge(eg);
    }
    //test
    qDebug()<<"SubFaceBuilder::buildSubfaceFromOriLines : test";
    qDebug()<<"Line : "<<lines.size();
    for(int i = 0; i < lines.size(); ++i){
        OriLine *l = lines.at(i);
//        l->debugPrint();
    }
    qDebug()<<"Vertex : "<<vertices.size();
    for(int i  = 0;  i<vertices.size(); ++i){
        OriVertex *v = vertices.at(i);
    }
//    qDebug()<<"Edge : "<<edges.size();
//    for(int j = 0; j < edges.size(); ++j){
//        OriEdge *e = edges.at(j);
//        qDebug()<<"e->getCutType() : "<<e->getCutType();
//    }
//    qDebug()<<"---";
//    qDebug()<<"edges and v->edges check";
//    for(int i = 0; i < edges.size(); ++i){
//        OriEdge *e = edges.at(i);
//        e->itmp = i;
//        qDebug()<<e->itmp<<", "<<e->getCutType();
//    }
//    qDebug()<<"---";
//    for(int i = 0; i < vertices.size(); ++i){
//        OriVertex *v  = vertices.at(i);
//        for(int j  = 0 ; j < v->edges.size(); ++j){
//            OriEdge *e = v->edges.at(j);
//            qDebug()<<e->itmp<<", "<<e->getCutType();
//        }
//    }
//    qDebug()<<"---";
    // edge walking
    for(int i = 0; i < vertices.size(); ++i){
        OriVertex *v = vertices.at(i);
        for(int j = 0; j < v->edges.size(); ++j){
            OriEdge *e = v->edges.at(j);
//            qDebug()<<e->itmp<<", "<<e->getCutType();
//            qDebug()<<"e->getCutType() : "<<e->getCutType();
            if(e->getCutType()){
//                qDebug()<<"continue 0";
                //continue;//need to consider
            }
            if(v == e->sv){
                if( e->left != NULL){
//                    qDebug()<<"continue 1";
                    continue;
                }
            }else{
                if( e->right != NULL){
//                    qDebug()<<"continue 2";
                    continue;
                }
            }
//            qDebug()<<"subface create begin";
            SubFace *subface = new SubFace();
            subfaces.push_back(subface);
            OriVertex *walkV = v;
            OriEdge *walkE = e;
            do{
                OriHalfedge *he = new OriHalfedge(walkV, subface);
                subface->halfedges.push_back(he);
                if(walkE->sv == walkV){
                    walkE->left  = he;
                }else{
                    walkE->right  =he;
                }
                walkV = walkE->getOppositeVertex(walkV);
                walkE = walkV->getPrevEdge(walkE);
            }while(walkV != v);
            subface->makeHalfedgeLoop();
            subface->calcNormal();
        }
    }
    return subfaces;
}

bool SubFaceBuilder::isSameLineSegment(OriLine *l0, OriLine *l1)
{
    return ((l0->p0-l1->p0).length()<eps&&(l0->p1-l1->p1).length()<eps)
            ||((l0->p1-l1->p0).length()<eps&&(l0->p0-l1->p1).length()<eps);
}

QVector2D *SubFaceBuilder::getCrossPoint(OriLine *l0, OriLine *l1)
{
    QVector2D p0 = QVector2D(l0->p0);
    QVector2D p1 = QVector2D(l0->p1);
    QVector2D d0= QVector2D(p1-p0);
    QVector2D d1 = QVector2D(l1->p1-l1->p0);
    QVector2D diff =QVector2D(l1->p0-p0);
    QVector2D* crossPoint = NULL;
    double det = d1.x()*d0.y() -d1.y()*d0.x();
    if(det * det  > eps  * d0.lengthSquared() * d1.lengthSquared()){
        double invDet = 1.0 / det;
        double s = (d1.x() * diff.y() - d1.y() * diff.x()) * invDet;
        double t = (d0.x() * diff.y() - d0.y() * diff.x()) * invDet;

        if (t < 0.0 - eps || t > 1.0 + eps) {
            return NULL;
        } else if (s < 0.0 - eps || s > 1.0 + eps) {
            return NULL;
        } else {
            crossPoint = new QVector2D();
            crossPoint->setX( (1.0 - t) * l1->p0.x() + t * l1->p1.x() );
            crossPoint->setY( (1.0 - t) * l1->p0.y() + t * l1->p1.y() );
            return crossPoint;
        }
    }
    return NULL;
}

double SubFaceBuilder::distancePointToSegment(QVector2D p, OriLine *line)
{
    double x0 = line->p0.x();
    double y0 = line->p0.y();
    double x1 = line->p1.x();
    double y1 = line->p1.y();
    double px = p.x();
    double py = p.y();
    QVector2D sub0, sub, sub0b;

    sub0 = QVector2D(x0 - px, y0 - py);
    sub0b = QVector2D(-sub0.x(), -sub0.y());
    sub = QVector2D(x1 - x0, y1 - y0);

    double t = ((sub.x() * sub0b.x()) + (sub.y() * sub0b.y())) / ((sub.x() * sub.x()) + (sub.y() * sub.y()));

    if (t < 0.0) {
        return QVector2D(p-line->p0).length();//return Distance(px, py, x0, y0);
    } else if (t > 1.0) {
        return QVector2D(p-line->p1).length();//return Distance(px, py, x1, y1);
    } else {
        return QVector2D(QVector2D(x0+t*sub.x(), y0+t*sub.y())-p).length();//Distance(x0 + t * sub.x, y0 + t * sub.y, px, py);
    }
}

bool SubFaceBuilder::compQVector2DX(const QVector2D p0, const QVector2D p1)
{
    return p0.x()>p1.x();
}

bool SubFaceBuilder::compQVector2DY(const QVector2D p0, const QVector2D p1)
{
    return p0.y()>p1.y();
}

OriVertex *SubFaceBuilder::addAndGetVertexfromVVec(QVector2D p, QVector<OriVertex *> &vertices)
{
    OriVertex *vertex = NULL;
    for(int i = 0; i < vertices.size(); ++i){
        OriVertex *vertexInList = vertices.at(i);
        //qDebug()<<p<<", "<<vertexInList->getQVector2D()<<", "<<QVector2D(p-vertexInList->getQVector2D()).length();
        if(QVector2D(p-vertexInList->getQVector2D()).length()<eps){
            vertex = vertexInList;
        }
    }
    if(vertex == NULL){
        vertex = new OriVertex(p.x(), p.y());
        vertices.push_back(vertex);
    }
    return vertex;
}

void SubFaceBuilder::deleteOverlappSubFace(QVector<SubFace *> &subfaces)
{

    SubFace *biggestSubFace;
    int ibig = 0;
    for(int i = 1; i<subfaces.size(); ++i)
    {
        SubFace *sub = subfaces.at(i);
        ibig = sub->halfedges.size()>subfaces.at(ibig)->halfedges.size() ? i:ibig;
    }
    qDebug()<<ibig;
    biggestSubFace = subfaces.at(ibig);
    subfaces.removeAt(ibig);
}

