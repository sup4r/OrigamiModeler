#pragma once
#include "annotation.h"
#include "glueedgeannotation.h"
#include "insidereversefoldannotation.h"
#include "canvas2d.h"
#include "canvas3d.h"
#include "vertex2d.h"
#include "halfedge2d.h"
#include "face2d.h"
#include "model2d.h"
#include "gldrawutil.h"
#include "pleatfoldannotation.h"
#include "turnfoldannotation.h"
#include "outsidereversefoldannotation.h"
#include "sinkfoldannotation.h"
#include "pigslegannotation.h"
#include "orikomiedgeannotation.h"
#include <Windows.h>
#include <gl/GLU.h>
#include <QMouseEvent>
#include <QOpenGLFunctions>
#include <QDebug>
#include <QMessageBox>
#include <QFileDialog>
#include "annotationname.h"
#include "mirrorconnectannotation.h"
#include "mirrorplane.h"
#include <vector>
/* tuika @ 2018 */
#include <iostream>
#include <random>
#include <vector>
#include <opencv2/opencv.hpp>
#include <opencv2/highgui/highgui.hpp>
#include <opencv2/imgproc/imgproc.hpp>
#include <opencv2/features2d/features2d.hpp>
#include <cmath>

using namespace cv;

/* tuika @ 2018 fin */
/* tuika @ 2018 */
/* tuika @ 2018 fin */

/* --- Constructor --- */

Canvas2d::Canvas2d(QWidget *parent)
    :CanvasBase(parent)
{
    /* tuika @ 2018 */
    //std::random_device rnd;
    //for(int i=0; i<100; i++){
    //    kouhoPoints.push_back(QPoint(rnd()%700,rnd()%550));
    //}
    //cv::Mat src_image = cv::imread("sign8.jpg");
    //cv::namedWindow("Original");
    /*kouhoPoints.push_back(QPoint(100,100));
    kouhoPoints.push_back(QPoint(200,200));
    kouhoPoint = kouhoPoints.value(0);
    std::cout << kouhoPoint.x() << std::endl;*/
    /* tuika @ 2018 fin */

    // init
    canvas3d = NULL;
    createModel();
    //create mode init
    setMirrorTimes(1);
    lastVertex = NULL;
    firstHalfedge = NULL;
    lastHalfedge = NULL;
    texture = NULL;
    setSnapFlag(0);
    dGrid = 15;
    setMouseTracking(true);
    isShiftKeyBeenPressing = false;
    initMouseFunctions();
    initCreateAnnotationFunctions();
    currentLayerNumber = 0;
    isBranch = false;
    setIsDebugMode(false);

    /* 170428 */
    setSizeOfAnnotations(6);
    /* 171219 */
    setIsSkelton(true);

    /* tuika @ 2018 */
    halfFoldEdgesAmount.push_back(0);
    saveToFile2();
    /* tuika @ 2018 fin */
}

void Canvas2d::setCanvas3d(Canvas3d *c)
{
    canvas3d = c;
    /* glue */
    connect(this, SIGNAL(glueEdgeAdded(Halfedge2d*)), canvas3d, SLOT(createGlueBridge(Halfedge2d*)));
    connect(this, SIGNAL(glueEdgeRemoved(Halfedge2d*)), canvas3d, SLOT(deleteGlueBridge(Halfedge2d*)));
    connect(this, SIGNAL(compressGlueBridgeSignal(Halfedge2d*,double)), canvas3d, SLOT(compressGlueBridge(Halfedge2d*,double)));
    /* select/unselect */
    connect(this, SIGNAL(vertexSelected(Vertex2d*)), canvas3d, SLOT(selectVertexSync2D(Vertex2d*)));
    connect(this, SIGNAL(halfedgeSelected(Halfedge2d*)), canvas3d, SLOT(selectHalfedgeSync2D(Halfedge2d*)));
    connect(this, SIGNAL(faceSelected(Face2d*)), canvas3d, SLOT(selectFaceSync2D(Face2d*)));
    connect(this, SIGNAL(vertexUnselected(Vertex2d*)), canvas3d, SLOT(unselectVertexSync2D(Vertex2d*)));
    connect(this, SIGNAL(halfedgeUnselected(Halfedge2d*)), canvas3d, SLOT(unselectHalfedgeSync2D(Halfedge2d*)));
    connect(this, SIGNAL(faceUnselected(Face2d*)), canvas3d, SLOT(unselectFaceSync2D(Face2d*)));
    /* inside reverse fold */
    connect(this, SIGNAL(insideReverseFoldAdded(Halfedge2d*,Halfedge2d*)), canvas3d, SLOT(createInsideReverseFoldBridge(Halfedge2d*,Halfedge2d*)));
    connect(this, SIGNAL(insideReverseFoldRemoved(Halfedge2d*,Halfedge2d*)), canvas3d, SLOT(deleteInsideReverseFoldBridge(Halfedge2d*,Halfedge2d*)));
    /* pleat fold */
    connect(this, SIGNAL(pleatFoldAdded(Halfedge2d*,Halfedge2d*)), canvas3d, SLOT(createPleatFoldBridge(Halfedge2d*,Halfedge2d*)));
    connect(this, SIGNAL(pleatFoldRemoved(Halfedge2d*,Halfedge2d*)), canvas3d, SLOT(deletePleatReverseFoldBridge(Halfedge2d*,Halfedge2d*)));
    /* turn fold */
    connect(this, SIGNAL(turnFoldAdded(Halfedge2d*,Halfedge2d*)), canvas3d, SLOT(createTurnFoldBridge(Halfedge2d*,Halfedge2d*)));
    connect(this, SIGNAL(turnFoldRemoved(Halfedge2d*,Halfedge2d*)), canvas3d, SLOT(deleteTrunFoldBridge(Halfedge2d*,Halfedge2d*)));
    /* outside reverse fold */
    // turn + reverse
    /* sink fold */
    connect(this, SIGNAL(sinkFoldAdded(Halfedge2d*)), canvas3d, SLOT(createSinkFoldBridge(Halfedge2d*)));
    connect(this, SIGNAL(sinkFoldRemoved(Halfedge2d*)), canvas3d, SLOT(deleteSinkFoldBridge(Halfedge2d*)));
    /* pigs leg */
    connect(this, SIGNAL(pigsLegAdded(Halfedge2d*,Halfedge2d*,Halfedge2d*)), canvas3d, SLOT(createPigsLegBridge(Halfedge2d*,Halfedge2d*,Halfedge2d*)));
    connect(this, SIGNAL(pigsLegRemoved(Halfedge2d*,Halfedge2d*,Halfedge2d*)), canvas3d, SLOT(deletePigsLegBridge(Halfedge2d*,Halfedge2d*,Halfedge2d*)));
    /* orikomi edge */
    connect(this, SIGNAL(orikomiEdgeAdded(Halfedge2d*)), canvas3d, SLOT(createOrikomiEdgeBridge(Halfedge2d*)));
    connect(this, SIGNAL(orikomiEdgeRemoved(Halfedge2d*)), canvas3d, SLOT(deleteOrikomiEdgeBridge(Halfedge2d*)));
    /* mirror conncet*/
    connect(this, SIGNAL(mirrorConnectAdded(QVector<Halfedge2d*>)), canvas3d, SLOT(createMirrorConnect(QVector<Halfedge2d*>)));
    connect(this, SIGNAL(mirrorConnectRemoved(QVector<Halfedge2d*>)), canvas3d, SLOT(deleteMirrorConnect(QVector<Halfedge2d*>)));
    connect(this, SIGNAL(mirrorRotated(MirrorPlane*,int)), canvas3d, SLOT(rotateMirrorPlane(MirrorPlane*,int)));
    connect(this, SIGNAL(mirrorScaled(MirrorPlane*,int)), canvas3d, SLOT(scaleMirrorPlane(MirrorPlane*,int)));
}
/* tuika @ 2018 */
void Canvas2d::autoAnnotation(){
    unselectAll();
    Face2d *f  = faceAtPosition(startPos);
    Face2d *f2  = faceAtPosition(goalPos);
    if(f == NULL) return;
    if(f2 == NULL) return;
    if(f != f2){
        Halfedge2d *he = f->halfedge;
        Halfedge2d *he2;
        bool flag_he4 = false;
        Halfedge2d *he4;
        do{
            QPoint p = clossPoint(startPos, goalPos, QPoint(he->vertex->x, he->vertex->y), QPoint(he->vertex_next->x, he->vertex_next->y));
            //std::cout << pp.x() <<" " << pp.y() << std::endl;
            he2 = halfedgeAtPosition(p);
            if(he2 != NULL && he2->getFace() != f){
                Halfedge2d *he3 = f->halfedge;
                do{
                    if(is_parallel(he2,he3)){
                        he2 = he3;
                    }
                    he3 = he3 -> next;
                }while(he3 != f->halfedge);
            }
            if(he2 != NULL && !(min(startPos.x(),goalPos.x()) < p.x() && p.x() < max(startPos.x(),goalPos.x()) && min(startPos.y(),goalPos.y()) < p.y() && p.y() < max(startPos.y(),goalPos.y()))) {
                selectHalfedge(he2);
            }
            if(he2 != NULL && (min(startPos.x(),goalPos.x()) < p.x() && p.x() < max(startPos.x(),goalPos.x()) && min(startPos.y(),goalPos.y()) < p.y() && p.y() < max(startPos.y(),goalPos.y()))) {
                he4 = he2;
                flag_he4 = true;
            }
            he = he -> next;
        }while(he != f->halfedge);
        std::cout << "a" << std::endl;
        if(flag_he4) selectHalfedge(he4);
        std::cout << "b" << std::endl;
        he = f2->halfedge;
        do{
            QPoint p = clossPoint(startPos, goalPos, QPoint(he->vertex->x, he->vertex->y), QPoint(he->vertex_next->x, he->vertex_next->y));
            he2 = halfedgeAtPosition(p);
            if(he2 != NULL && he2->getFace() != f2){
                Halfedge2d *he3 = f2->halfedge;
                do{
                    if(is_parallel(he2,he3)){
                        he2 = he3;
                    }
                    he3 = he3 -> next;
                }while(he3 != f2->halfedge);
            }
            if(he2 != NULL && (min(startPos.x(),goalPos.x()) < p.x() && p.x() < max(startPos.x(),goalPos.x()) && min(startPos.y(),goalPos.y()) < p.y() && p.y() < max(startPos.y(),goalPos.y()))) {
                selectHalfedge(he2);
            }
            he = he -> next;
        }while(he != f2->halfedge);

        if(halfedgesSelected.size()!=3){
            unselectAll();
            return;
        }{
            Halfedge2d *he = halfedgesSelected.at(0);
            Halfedge2d *he2 = halfedgesSelected.at(1);
            Halfedge2d *he3 = halfedgesSelected.at(2);
            QPoint p1 = QPoint(he2->vertex->x, he2->vertex->y);
            QPoint p2 = QPoint(he2->vertex_next->x, he2->vertex_next->y);
            QPoint p3 = QPoint(he3->vertex->x, he3->vertex->y);
            QPoint p4 = QPoint(he3->vertex_next->x, he3->vertex_next->y);
            if(abs(p1.x()-p3.x())<=1 && abs(p1.y()-p3.y())<=1){
                unselectHalfedge(he2);
                createInsideReverseFoldWithSelectEdges();
            }else if(abs(p1.x()-p4.x())<=1 && abs(p1.y()-p4.y())<=1){
                unselectHalfedge(he2);
                createInsideReverseFoldWithSelectEdges();
            }else if(abs(p2.x()-p3.x())<=1 && abs(p2.y()-p3.y())<=1){
                unselectHalfedge(he2);
                createInsideReverseFoldWithSelectEdges();
            }else if(abs(p2.x()-p4.x())<=1 && abs(p2.y()-p4.y())<=1){
                unselectHalfedge(he2);
                createInsideReverseFoldWithSelectEdges();
            }else{
                unselectHalfedge(he);
                createPleatFoldWithSelectEdges();
            }
        }
    }else{ //pig legs
        Halfedge2d *he = f->halfedge;
        Halfedge2d *he2;
        bool flag_he4 = false;
        Halfedge2d *he4;
        QPoint p4;
        bool flag_he5 = false;
        Halfedge2d *he5;
        QPoint p5;
        do{
            QPoint p = clossPoint(startPos, goalPos, QPoint(he->vertex->x, he->vertex->y), QPoint(he->vertex_next->x, he->vertex_next->y));
            he2 = halfedgeAtPosition(p);
            if(he2 != NULL && he2->getFace() != f){
                Halfedge2d *he3 = f->halfedge;
                do{
                    if(is_parallel(he2,he3)){
                        he2 = he3;
                    }
                    he3 = he3 -> next;
                }while(he3 != f2->halfedge);
            }
            if(he2 != NULL) {
                if(!flag_he4) {
                    he4 = he2;
                    p4 = p;
                    flag_he4 = true;
                }
                else {
                    he5 = he2;
                    p5 = p;
                    flag_he5 = true;
                }
            }
            he = he -> next;
        }while(he != f->halfedge);
        if(flag_he4 && flag_he5) {
            if(pow(startPos.x()-p4.x(),2)+pow(startPos.y()-p4.y(),2) < pow(startPos.x()-p5.x(),2)+pow(startPos.y()-p5.y(),2)){
            }else{
                Halfedge2d *heTmp;
                heTmp = he4;
                he4 = he5;
                he5 = heTmp;
            }
            selectHalfedge(he4);
            Face2d *fDX;
            for(int i=0; i<model->faces.size(); i++){
                fDX  = model->faces.at(i);
                if(fDX != f){
                    Halfedge2d *heDX = fDX->halfedge;
                    do{
                        if(is_parallel(he4,heDX)){
                            selectHalfedge(heDX);
                            i = 10000;
                            break;
                        }
                        heDX = heDX -> next;
                    }while(heDX != fDX->halfedge);
                }
            }
            for(int i=0; i<model->faces.size(); i++){
                fDX  = model->faces.at(i);
                if(fDX != f){
                    Halfedge2d *heDX = fDX->halfedge;
                    do{
                        if(is_parallel(he5,heDX)){
                            selectHalfedge(heDX);
                            i = 10000;
                            break;
                        }
                        heDX = heDX -> next;
                    }while(heDX != fDX->halfedge);
                }
            }
            if(halfedgesSelected.size() != 3){
                unselectAll();
                return;
            }else{
                createPigsLegAnnotationWithSelectedEdges();
            }
        }else{
            return;
        }
    }
}

bool Canvas2d::is_parallel(Halfedge2d *he, Halfedge2d *he2){
    int a = he->vertex_next->y - he->vertex->y;
    int b = he->vertex_next->x - he->vertex->x;
    int c = he2->vertex_next->y - he2->vertex->y;
    int d = he2->vertex_next->x - he2->vertex->x;
    double aDash = (double)(c);
    double bDash = (double)(-1*d);
    double cDash = (double)(d*he2->vertex->y + (-1)*c*he2->vertex->x);
    if(0.1*sqrt(min(pow(a,2)+pow(b,2),pow(c,2)+pow(d,2)))< abs(aDash*he->vertex->x+bDash*he->vertex->y+cDash)/sqrt(pow(aDash,2)+pow(bDash,2))) return false;
    if(0.1*sqrt(min(pow(a,2)+pow(b,2),pow(c,2)+pow(d,2)))< abs(aDash*he->vertex_next->x+bDash*he->vertex_next->y+cDash)/sqrt(pow(aDash,2)+pow(bDash,2))) return false;
    if(a*c+b*d == 0) return false;
    if(b==0 || abs(a/b)>1)
    {
        // tooi baai return
        if((he->vertex->y > max(he2->vertex->y,he2->vertex_next->y) || he->vertex->y < min(he2->vertex->y,he2->vertex_next->y))&&(he2->vertex->y > max(he->vertex->y,he->vertex_next->y) || he2->vertex->y < min(he->vertex->y,he->vertex_next->y))) return false;
        bool flag = false;
        bool flagB = false;
        int i;
        int j;
        if(atan(abs((double)(a*d-b*c)/(double)(a*c+b*d)))<3.14/9){
            for(i=-30; i<=30; i++){
                if(abs(aDash*(he->vertex->x+i)+bDash*(he->vertex->y)+cDash)/sqrt(pow(aDash,2)+pow(bDash,2))<1){
                    flag = true;
                    break;
                }
            }
            for(j=-30; j<=30; j++){
                if(abs(aDash*(he->vertex_next->x+j)+bDash*(he->vertex_next->y)+cDash)/sqrt(pow(aDash,2)+pow(bDash,2))<1){
                    flagB = true;
                    break;
                }
            }
            if(flag && flagB){
                //std::cout <<"true" << std::endl;
                return true;
            }
            //he->setColor(QColor(0,200,0));
        }
    }
    else if(a==0 || abs(b/a)>1)
    {
        // tooi baai return
        if((he->vertex->x > max(he2->vertex->x,he2->vertex_next->x) || he->vertex->x < min(he2->vertex->x,he2->vertex_next->x))&&(he2->vertex->x > max(he->vertex->x,he->vertex_next->x) || he2->vertex->x < min(he->vertex->x,he->vertex_next->x))) return false;
        bool flag = false;
        bool flagB = false;
        int i;
        int j;
        if(atan(abs((double)(a*d-b*c)/(double)(a*c+b*d)))<3.14/9){
            for(i=-30; i<=30; i++){
                if(abs(aDash*(he->vertex->x)+bDash*(he->vertex->y+i)+cDash)/sqrt(pow(aDash,2)+pow(bDash,2))<1){
                    flag = true;
                    break;
                }
            }
            for(j=-30; j<=30; j++){
                if(abs(aDash*(he->vertex_next->x)+bDash*(he->vertex_next->y+j)+cDash)/sqrt(pow(aDash,2)+pow(bDash,2))<1){
                    flagB = true;
                    break;
                }
            }
            if(flag && flagB){
                //std::cout <<"true" << std::endl;
                return true;
            }
            //he->setColor(QColor(0,200,0));
        }
    }
    else if(a*b<0)
    {
        // tooi baai return
        if((he->vertex->x > max(he2->vertex->x,he2->vertex_next->x) || he->vertex->x < min(he2->vertex->x,he2->vertex_next->x))&&(he2->vertex->x > max(he->vertex->x,he->vertex_next->x) || he2->vertex->x < min(he->vertex->x,he->vertex_next->x))) return false;
        if((he->vertex->y > max(he2->vertex->y,he2->vertex_next->y) || he->vertex->y < min(he2->vertex->y,he2->vertex_next->y))&&(he2->vertex->y > max(he->vertex->y,he->vertex_next->y) || he2->vertex->y < min(he->vertex->y,he->vertex_next->y))) return false;
        bool flag = false;
        bool flagB = false;
        int i;
        int j;
        if(atan(abs((double)(a*d-b*c)/(double)(a*c+b*d)))<3.14/9){
            for(i=-30; i<=30; i++){
                if(abs(aDash*(he->vertex->x+i)+bDash*(he->vertex->y+i)+cDash)/sqrt(pow(aDash,2)+pow(bDash,2))<1){
                    flag = true;
                    break;
                    he->vertex->setPosition(he->vertex->x+i,he->vertex->y+i);
                }
            }
            for(j=-30; j<=30; j++){
                if(abs(aDash*(he->vertex_next->x+j)+bDash*(he->vertex_next->y+j)+cDash)/sqrt(pow(aDash,2)+pow(bDash,2))<1){
                    flagB = true;
                    break;
                }
            }
            if(flag && flagB){
                //std::cout <<"true" << std::endl;
                return true;
            }
           // he->setColor(QColor(0,200,0));
        }
    }
    else
    {
        // tooi baai return
        if((he->vertex->x > max(he2->vertex->x,he2->vertex_next->x) || he->vertex->x < min(he2->vertex->x,he2->vertex_next->x))&&(he2->vertex->x > max(he->vertex->x,he->vertex_next->x) || he2->vertex->x < min(he->vertex->x,he->vertex_next->x))) return false;
        if((he->vertex->y > max(he2->vertex->y,he2->vertex_next->y) || he->vertex->y < min(he2->vertex->y,he2->vertex_next->y))&&(he2->vertex->y > max(he->vertex->y,he->vertex_next->y) || he2->vertex->y < min(he->vertex->y,he->vertex_next->y))) return false;
        bool flag = false;
        bool flagB = false;
        int i;
        int j;
        if(atan(abs((double)(a*d-b*c)/(double)(a*c+b*d)))<3.14/9){
            for(i=-30; i<=30; i++){
                if(abs(aDash*(he->vertex->x-i)+bDash*(he->vertex->y+i)+cDash)/sqrt(pow(aDash,2)+pow(bDash,2))<1){
                    flag = true;
                    break;
                    he->vertex->setPosition(he->vertex->x-i,he->vertex->y+i);
                }
            }
            for(j=-30; j<=30; j++){
                if(abs(aDash*(he->vertex_next->x-j)+bDash*(he->vertex_next->y+j)+cDash)/sqrt(pow(aDash,2)+pow(bDash,2))<1){
                    flagB = true;
                    break;
                    he->vertex_next->setPosition(he->vertex_next->x-j,he->vertex_next->y+j);
                }
            }
            if(flag && flagB){
                //std::cout <<"true" << std::endl;
                return true;
            }
            //he->setColor(QColor(0,200,0));
        }
    }
    return false;
}

QPoint Canvas2d::clossPoint(QPoint p1, QPoint p2,QPoint p3, QPoint p4){
    double A;
    double B;
    double a;
    double b;
    if((p1.x() - p2.x())==0) {
        A = 999;
    }else{
        A = ((double)(p1.y() - p2.y()) / (double)(p1.x() - p2.x()));
    }
    B = p2.y() - A * p2.x();
    if((p3.x() - p4.x())==0){
        a = 999;
    }
    else{
        a = ((double)(p3.y() - p4.y()) / (double)(p3.x() - p4.x()));
    }
    b = p4.y() - a * p4.x();
    //std::cout << A << std::endl;
    //std::cout << B << std::endl;
    //std::cout << a << std::endl;
    //std::cout << b << std::endl;
    if(a-A == 0){
        return QPoint(0,0);
    }else{
        return QPoint((int)(B-b)/(a-A),(int)(a*B -A*b)/(a-A));
    }
}

void Canvas2d::keyPress_B(){
    loadFromFile2();
    update();
}

void Canvas2d::positionAdjust(){
    if(model->faces.size() == 0) return;
    Face2d *f  = model->faces.at(model->faces.size()-1);
    Halfedge2d *he = f->halfedge;
    Vertex2d v = getCenter(f);
    //std::cout << v.x << " " << v.y << std::endl;
    do{
        //std::cout << "("<<he->vertex->x<<","<<he->vertex->y<<")"<<"->"<<"("<<he->vertex_next->x<<","<<he->vertex_next->y<<")" << std::endl;
        for(int j = 0; j < model->faces.size()-1; ++j){
            Face2d *f2  = model->faces.at(j);
            Halfedge2d *he2 = f2->halfedge;
            do{
                adjust_parallel(he,he2);
                adjust_nearPoint(he,he2);
                //std::cout << "("<<he2->vertex->x<<","<<he2->vertex->y<<")"<<"->"<<"("<<he2->vertex_next->x<<","<<he2->vertex_next->y<<")" << std::endl;
                he2 = he2 -> next;
            }while(he2 != f2->halfedge);
        }
        he = he -> next;
    }while(he != f->halfedge);
}

Vertex2d Canvas2d::getCenter(Face2d *f){
    Halfedge2d *he = f->halfedge;
    int x=0;
    int y=0;
    int count = 0;
    do{
        count ++;
        x += he->vertex->x;
        y += he->vertex->y;
        he = he -> next;
    }while(he != f->halfedge);
    return Vertex2d(x/count,y/count);
}

void Canvas2d::adjust_parallel(Halfedge2d *he, Halfedge2d *he2){
    //https://mathtrain.jp/nasukaku
    //http://www.ftext.org/text/subsubsection/2365
    int a = he->vertex_next->y - he->vertex->y;
    int b = he->vertex_next->x - he->vertex->x;
    int c = he2->vertex_next->y - he2->vertex->y;
    int d = he2->vertex_next->x - he2->vertex->x;
    //std::cout << he->vertex->getIdForExport() << std::endl;
    //std::cout << he->vertex_next->getIdForExport() << std::endl;
    double aDash = (double)(c);
    double bDash = (double)(-1*d);
    double cDash = (double)(d*he2->vertex->y + (-1)*c*he2->vertex->x);

    // tooi baai return
    if(0.2*sqrt(min(pow(a,2)+pow(b,2),pow(c,2)+pow(d,2)))< abs(aDash*he->vertex->x+bDash*he->vertex->y+cDash)/sqrt(pow(aDash,2)+pow(bDash,2))) return;
    if(0.2*sqrt(min(pow(a,2)+pow(b,2),pow(c,2)+pow(d,2)))< abs(aDash*he->vertex_next->x+bDash*he->vertex_next->y+cDash)/sqrt(pow(aDash,2)+pow(bDash,2))) return;

    if(a*c+b*d == 0) return;
    if(b==0 || abs(a/b)>1)
    {
        // tooi baai return
        if((he->vertex->y > max(he2->vertex->y,he2->vertex_next->y) || he->vertex->y < min(he2->vertex->y,he2->vertex_next->y))&&(he2->vertex->y > max(he->vertex->y,he->vertex_next->y) || he2->vertex->y < min(he->vertex->y,he->vertex_next->y))) return;
        bool flag = false;
        bool flagB = false;
        int i;
        int j;
        if(atan(abs((double)(a*d-b*c)/(double)(a*c+b*d)))<3.14/9){
            for(i=-30; i<=30; i++){
                if(abs(aDash*(he->vertex->x+i)+bDash*(he->vertex->y)+cDash)/sqrt(pow(aDash,2)+pow(bDash,2))<1){
                    flag = true;
                    break;
                }
            }
            for(j=-30; j<=30; j++){
                if(abs(aDash*(he->vertex_next->x+j)+bDash*(he->vertex_next->y)+cDash)/sqrt(pow(aDash,2)+pow(bDash,2))<1){
                    flagB = true;
                    break;
                }
            }
            if(flag && flagB){
                he->vertex->setPosition(he->vertex->x+i,he->vertex->y);
                he->vertex_next->setPosition(he->vertex_next->x+j,he->vertex_next->y);
            }
            //he->setColor(QColor(0,200,0));
        }
    }
    else if(a==0 || abs(b/a)>1)
    {
        // tooi baai return
        if((he->vertex->x > max(he2->vertex->x,he2->vertex_next->x) || he->vertex->x < min(he2->vertex->x,he2->vertex_next->x))&&(he2->vertex->x > max(he->vertex->x,he->vertex_next->x) || he2->vertex->x < min(he->vertex->x,he->vertex_next->x))) return;
        bool flag = false;
        bool flagB = false;
        int i;
        int j;
        if(atan(abs((double)(a*d-b*c)/(double)(a*c+b*d)))<3.14/9){
            for(i=-30; i<=30; i++){
                if(abs(aDash*(he->vertex->x)+bDash*(he->vertex->y+i)+cDash)/sqrt(pow(aDash,2)+pow(bDash,2))<1){
                    flag = true;
                    break;
                }
            }
            for(j=-30; j<=30; j++){
                if(abs(aDash*(he->vertex_next->x)+bDash*(he->vertex_next->y+j)+cDash)/sqrt(pow(aDash,2)+pow(bDash,2))<1){
                    flagB = true;
                    break;
                }
            }
            if(flag && flagB){
                he->vertex->setPosition(he->vertex->x,he->vertex->y+i);
                he->vertex_next->setPosition(he->vertex_next->x,he->vertex_next->y+j);
            }
            //he->setColor(QColor(0,200,0));
        }
    }
    else if(a*b<0)
    {
        // tooi baai return
        if((he->vertex->x > max(he2->vertex->x,he2->vertex_next->x) || he->vertex->x < min(he2->vertex->x,he2->vertex_next->x))&&(he2->vertex->x > max(he->vertex->x,he->vertex_next->x) || he2->vertex->x < min(he->vertex->x,he->vertex_next->x))) return;
        if((he->vertex->y > max(he2->vertex->y,he2->vertex_next->y) || he->vertex->y < min(he2->vertex->y,he2->vertex_next->y))&&(he2->vertex->y > max(he->vertex->y,he->vertex_next->y) || he2->vertex->y < min(he->vertex->y,he->vertex_next->y))) return;
        bool flag = false;
        bool flagB = false;
        int i;
        int j;
        if(atan(abs((double)(a*d-b*c)/(double)(a*c+b*d)))<3.14/9){
            for(i=-30; i<=30; i++){
                if(abs(aDash*(he->vertex->x+i)+bDash*(he->vertex->y+i)+cDash)/sqrt(pow(aDash,2)+pow(bDash,2))<1){
                    flag = true;
                    break;
                    he->vertex->setPosition(he->vertex->x+i,he->vertex->y+i);
                }
            }
            for(j=-30; j<=30; j++){
                if(abs(aDash*(he->vertex_next->x+j)+bDash*(he->vertex_next->y+j)+cDash)/sqrt(pow(aDash,2)+pow(bDash,2))<1){
                    flagB = true;
                    break;
                }
            }
            if(flag && flagB){
                he->vertex->setPosition(he->vertex->x+i,he->vertex->y+i);
                he->vertex_next->setPosition(he->vertex_next->x+j,he->vertex_next->y+j);
            }
           // he->setColor(QColor(0,200,0));
        }
    }
    else
    {
        // tooi baai return
        if((he->vertex->x > max(he2->vertex->x,he2->vertex_next->x) || he->vertex->x < min(he2->vertex->x,he2->vertex_next->x))&&(he2->vertex->x > max(he->vertex->x,he->vertex_next->x) || he2->vertex->x < min(he->vertex->x,he->vertex_next->x))) return;
        if((he->vertex->y > max(he2->vertex->y,he2->vertex_next->y) || he->vertex->y < min(he2->vertex->y,he2->vertex_next->y))&&(he2->vertex->y > max(he->vertex->y,he->vertex_next->y) || he2->vertex->y < min(he->vertex->y,he->vertex_next->y))) return;
        bool flag = false;
        bool flagB = false;
        int i;
        int j;
        if(atan(abs((double)(a*d-b*c)/(double)(a*c+b*d)))<3.14/9){
            for(i=-30; i<=30; i++){
                if(abs(aDash*(he->vertex->x-i)+bDash*(he->vertex->y+i)+cDash)/sqrt(pow(aDash,2)+pow(bDash,2))<1){
                    flag = true;
                    break;
                    he->vertex->setPosition(he->vertex->x-i,he->vertex->y+i);
                }
            }
            for(j=-30; j<=30; j++){
                if(abs(aDash*(he->vertex_next->x-j)+bDash*(he->vertex_next->y+j)+cDash)/sqrt(pow(aDash,2)+pow(bDash,2))<1){
                    flagB = true;
                    break;
                    he->vertex_next->setPosition(he->vertex_next->x-j,he->vertex_next->y+j);
                }
            }
            if(flag && flagB){
                he->vertex->setPosition(he->vertex->x-i,he->vertex->y+i);
                he->vertex_next->setPosition(he->vertex_next->x-j,he->vertex_next->y+j);
            }
            //he->setColor(QColor(0,200,0));
        }
    }
}

void Canvas2d::adjust_nearPoint(Halfedge2d *he, Halfedge2d *he2){
    double dis = 400;
    if(pow(double(he->vertex->x - he2->vertex->x),2)+pow(double(he->vertex->y - he2->vertex->y),2) < dis)
    {
        he->vertex->setPosition(he2->vertex->x,he2->vertex->y);
    }
}

void Canvas2d::trashCurrentModel2()
{
    Model2d *currentModel2d = getModel();
    Model3d *currentModel3d = canvas3d->getModel();
    QVector<Annotation*> *currentAnnotations = getAnnotations();

    //create new empty model 2d & 3d
    if(models.size()==1){
        createPairModels();
    }else{
        changeModels();
    }
    //remove current models
    models.removeOne(currentModel2d);
    canvas3d->models.removeOne(currentModel3d);
    annotationListList.removeOne(currentAnnotations);

    //push trash box
    trashedModels.push_back(currentModel2d);
    trashedAnnotations.push_back(currentAnnotations);
}

void Canvas2d::setID(){
    for(int i = 0; i < model->vertices.size(); ++i){
        Vertex2d *v  = model->vertices.at(i);
        v->setIdForExport(i+1);
    }
    for(int i = 0; i < model->halfedges.size(); ++i){
        Halfedge2d *he = model->halfedges.at(i);
        he->setIdForExport(i+1);
    }

}

void Canvas2d::saveToFile2()
{
    //QString filename = QFileDialog::getSaveFileName(this, "Save File", "", "VFEA File (*.vfea);;All Files (*)");
    //if(filename.isEmpty()){
    //    return;
    //}else{
    saveAndLoadCount++;
        QFile file("back_one"+QString::number(saveAndLoadCount)+".vfea");
        if(!file.open(QIODevice::WriteOnly)){
            return;
        }
        QTextStream out(&file);

        /* OBJ */
        for(int i = 0; i < model->vertices.size(); ++i){
            Vertex2d *v  = model->vertices.at(i);
            //v->setIdForExport(i+1);
            out<<"v "<<v->x<<" "<<v->y<<endl;
        }

        for(int i = 0; i < model->faces.size(); ++i){
            Face2d *f  = model->faces.at(i);
            out<<"f";
            Halfedge2d *he = f->halfedge;
            do{
                out<<" "<<he->vertex->getIdForExport();
                he = he->next;
            }while(he != f->halfedge);
            out<<endl;
        }

        /* Original Information */
        for(int i = 0; i < model->halfedges.size(); ++i){
            Halfedge2d *he = model->halfedges.at(i);
           // he->setIdForExport(i+1);
            out<<"e"<<" "<<he->vertex->getIdForExport()<<" "<<he->vertex_next->getIdForExport()<<endl;
        }

        for(int i = 0; i < annotations->size(); ++i){
            Annotation *a = annotations->at(i);
            int annotationName = a->getAnnotationName();
            out<<"a"<<" "<<annotationName;
            for(int j = 0; j < a->getHesNum(); ++j){
                Halfedge2d *he = a->getHes().at(j);
                out<<" "<<he->getIdForExport();
            }
            out<<endl;
        }

        update();
    //}
}

void Canvas2d::loadFromFile2() //this->model & annotations should be new-ed and cleared
{
    if(halfFoldEdgesAmount.size()*halfFoldEdges.size() !=0){
        for(int i=0; i<halfFoldEdgesAmount.at(halfFoldEdgesAmount.size()-1); i++){
           halfFoldEdges.pop_back();
        }
        halfFoldEdgesAmount.pop_back();
    }

    trashCurrentModel2();
    createPairModels();
    //QString filename = QFileDialog::getOpenFileName(this,"Load File", "", "VFEA File (*.vfea);;All Files (*)" );
    if(saveAndLoadCount>0)
        saveAndLoadCount--;
    QFile file("back_one"+QString::number(saveAndLoadCount)+".vfea");
    if(!file.open(QIODevice::ReadOnly)){
        return;
    }
    QTextStream in(&file);

    int iv = 1;
    int ie = 1;

    while(!in.atEnd()){
        QString str;
        in>>str;

        if(str=="v"){
            int x,y;
            in>>x;
            in>>y;
            Vertex2d *v = createVertex(QPoint(x,y));
            v->setIdForExport(iv++);
        }else if(str == "f"){
            str = in.readLine();
            int spaceCount = str.count(" "); // 170126 spaceCount == Number of Vertices

            QTextStream line(&str);
            QVector<Vertex2d*>vs;
            for(int i = 0; i < spaceCount; ++i){
               int id;
               line>>id;
               vs.push_back(model->vertices.at(id-1));
            }

            QVector<Halfedge2d*>hes;
            for(int i = 0; i < vs.size(); ++i){
                Vertex2d *v1 = vs.at(i);
                Vertex2d *v2 = vs.at((i+1==vs.size())? 0 : i+1);
                Halfedge2d *he = createHalfedge(v1, v2);
                hes.push_back(he);
            }

            for(int i = 0; i < hes.size(); ++i){
                Halfedge2d *he1 = hes.at(i);
                Halfedge2d *he2 = hes.at((i+1==hes.size())? 0 : i+1);
                he1->setNext(he2);
            }

            Face2d *f = createFace(hes.first());
        }else if(str=="e"){
            int vertexID1, vertexID2;
            in>>vertexID1;
            in>>vertexID2;
            Halfedge2d *he = model->getHalfedge(vertexID1-1, vertexID2-1);
            if(he==NULL){
                qDebug()<<"Canvas2d::loadFromFile : he is NULL(1)";
                return;
            }
            he->setIdForExport(ie++);
        }else if(str=="a"){
            int annotationName;
            in>>annotationName;
            str = in.readLine();
            int spaceCount = str.count(" "); // 170126 spaceCount == Number of Halfedges' ID

            QTextStream line(&str);
            QVector<Halfedge2d*>hes;
            for(int i = 0; i < spaceCount; ++i){
                int halfedgeId;
                line>>halfedgeId;
                Halfedge2d *he = NULL;
                for(int j = 0; j < model->halfedges.size(); ++j){
                    Halfedge2d *heAtJ = model->halfedges.at(j);
                    if(heAtJ->getIdForExport()==halfedgeId){
                        he = heAtJ;
                        break;
                    }
                }
                if(he == NULL){
                    qDebug()<<"Canvas2d::loadFromFile : he is NULL(2)";
                    return;
                }
                hes.push_back(he);
            }

            Annotation *a = createAnnotation(annotationName, hes);

        }else{

        }
    }
    update();
}

void Canvas2d::loadFromFile3() //this->model & annotations should be new-ed and cleared
{
    halfFoldEdges.clear();
    halfFoldEdgesAmount.clear();
    trashCurrentModel2();
    createPairModels();
    //QString filename = QFileDialog::getOpenFileName(this,"Load File", "", "VFEA File (*.vfea);;All Files (*)" );
    QFile file("back_one"+QString::number(saveAndLoadCount)+".vfea");
    if(!file.open(QIODevice::ReadOnly)){
        return;
    }
    QTextStream in(&file);

    int iv = 1;
    int ie = 1;

    while(!in.atEnd()){
        QString str;
        in>>str;

        if(str=="v"){
            int x,y;
            in>>x;
            in>>y;
            Vertex2d *v = createVertex(QPoint(x,y));
            v->setIdForExport(iv++);
        }else if(str == "f"){
            str = in.readLine();
            int spaceCount = str.count(" "); // 170126 spaceCount == Number of Vertices

            QTextStream line(&str);
            QVector<Vertex2d*>vs;
            for(int i = 0; i < spaceCount; ++i){
               int id;
               line>>id;
               vs.push_back(model->vertices.at(id-1));
            }

            QVector<Halfedge2d*>hes;
            for(int i = 0; i < vs.size(); ++i){
                Vertex2d *v1 = vs.at(i);
                Vertex2d *v2 = vs.at((i+1==vs.size())? 0 : i+1);
                Halfedge2d *he = createHalfedge(v1, v2);
                hes.push_back(he);
            }

            for(int i = 0; i < hes.size(); ++i){
                Halfedge2d *he1 = hes.at(i);
                Halfedge2d *he2 = hes.at((i+1==hes.size())? 0 : i+1);
                he1->setNext(he2);
            }

            Face2d *f = createFace(hes.first());
        }else if(str=="e"){
            int vertexID1, vertexID2;
            in>>vertexID1;
            in>>vertexID2;
            Halfedge2d *he = model->getHalfedge(vertexID1-1, vertexID2-1);
            if(he==NULL){
                qDebug()<<"Canvas2d::loadFromFile : he is NULL(1)";
                return;
            }
            he->setIdForExport(ie++);
        }else if(str=="a"){
            int annotationName;
            in>>annotationName;
            str = in.readLine();
            int spaceCount = str.count(" "); // 170126 spaceCount == Number of Halfedges' ID

            QTextStream line(&str);
            QVector<Halfedge2d*>hes;
            for(int i = 0; i < spaceCount; ++i){
                int halfedgeId;
                line>>halfedgeId;
                Halfedge2d *he = NULL;
                for(int j = 0; j < model->halfedges.size(); ++j){
                    Halfedge2d *heAtJ = model->halfedges.at(j);
                    if(heAtJ->getIdForExport()==halfedgeId){
                        he = heAtJ;
                        break;
                    }
                }
                if(he == NULL){
                    qDebug()<<"Canvas2d::loadFromFile : he is NULL(2)";
                    return;
                }
                hes.push_back(he);
            }

            Annotation *a = createAnnotation(annotationName, hes);

        }else{

        }
    }
    update();
}
void Canvas2d::autoVertex(){
    int gaus = 5;
    double param = 100;
    double diff = 40;
    int maxN = 200;
    double qual = 0.01;
    double minD = 3;
    int dis = 100;
    int a = 0;
    if(a==0){
        //std::cerr << "0" << std::endl;
    if(texture==NULL)return;
    Mat src_image = imread(filePath);
    if (src_image.empty()){
        std::cerr << "Failed to open image file." << std::endl;
    }
    Mat img = src_image.clone();
    //namedWindow("Original");
    //imshow("Original", src_image);
    GaussianBlur(src_image, src_image, Size(gaus, gaus), 0);
    //namedWindow("GaussianBlur");
    //imshow("GaussianBlur", src_image);
    Mat channels[3];
    split(src_image, channels);
    Mat canny_r, canny_g, canny_b, canny_image;
    Canny(channels[2], canny_r, param, param + diff, 3);
    Canny(channels[1], canny_g, param, param + diff, 3);
    Canny(channels[0], canny_b, param, param + diff, 3);

    bitwise_or(canny_r, canny_g, canny_image);
    bitwise_or(canny_image, canny_b, canny_image);
    //namedWindow("RGB");
    //imshow("RGB", canny_image);
    std::vector<Vec4i>lines_p;
    Vec4i pt;
    HoughLinesP(canny_image, lines_p, 1, CV_PI / 400, 20, 20, 20);

    Mat gray_img;
    cvtColor(img, gray_img, CV_BGR2GRAY);
    normalize(gray_img, gray_img, 0, 255, NORM_MINMAX);
    std::vector<Point2f> corners;
    goodFeaturesToTrack(gray_img, corners, maxN, qual, minD,Mat(),3,true);
    std::vector<Point2f>::iterator it_corner = corners.begin();

    //Mat src_image2 = imread("white.png");
    const int imageWidth = texture->width();
    const int imageHeight = texture->height();
    const float k = (float)width()/imageWidth;
    const float k2 = (float)height()/imageHeight;
    const float dx = (width()-k*imageWidth)/2.f;
    const float dy = (height()-k2*imageHeight)/2.f;
    kouhoPoints.clear();
    for (; it_corner != corners.end(); ++it_corner) {
        for (auto it = lines_p.begin(); it != lines_p.end(); ++it){
            pt = *it;
            double kyori = 0;
            kyori = (it_corner->x - pt[0])*(it_corner->x - pt[0]) + (it_corner->y - pt[1])*(it_corner->y - pt[1]);
            if(kyori < dis){
                kouhoPoints.push_back(QPoint(k*(it_corner->x+dx),k2*(it_corner->y+dy)));
                break;
            }
            kyori = (it_corner->x - pt[2])*(it_corner->x - pt[2]) + (it_corner->y - pt[3])*(it_corner->y - pt[3]);
            if(kyori < dis){
                kouhoPoints.push_back(QPoint(k*(it_corner->x+dx),k2*(it_corner->y+dy)));
                break;
            }
        }
    }
    //namedWindow("HoughLinesP");
    //imshow("HoughLinesP", src_image2);
    }else if (a==1){
        //std::cerr << "1" << std::endl;
        if(texture==NULL)return;
        Mat src_image = imread(filePath);
        if (src_image.empty()){
            std::cerr << "Failed to open image file." << std::endl;
        }
        GaussianBlur(src_image, src_image, Size(gaus, gaus), 0);
        Mat channels[3];
        split(src_image, channels);
        Mat canny_r, canny_g, canny_b, canny_image;
        Canny(channels[2], canny_r, param, param + diff, 3);
        Canny(channels[1], canny_g, param, param + diff, 3);
        Canny(channels[0], canny_b, param, param + diff, 3);
        bitwise_or(canny_r, canny_g, canny_image);
        bitwise_or(canny_image, canny_b, canny_image);
        //namedWindow("RGB");
        //imshow("RGB", canny_image);
        std::vector<Vec4i>lines_p;
        Vec4i pt;
        HoughLinesP(canny_image, lines_p, 1, CV_PI / 400, 20, 20, 20);

        const int imageWidth = texture->width();
        const int imageHeight = texture->height();
        const float k = (float)width()/imageWidth;
        const float k2 = (float)height()/imageHeight;
        const float dx = (width()-k*imageWidth)/2.f;
        const float dy = (height()-k2*imageHeight)/2.f;
        kouhoPoints.clear();
        for (auto it = lines_p.begin(); it != lines_p.end(); ++it){
            pt = *it;
            //line(src_image2, Point(pt[0], pt[1]), Point(pt[2], pt[3]), Scalar(0, 0, 255), 2, CV_AA);
            kouhoPoints.push_back(QPoint(k*(pt[0]+dx),k2*(pt[1]+dy)));
            kouhoPoints.push_back(QPoint(k*(pt[2]+dx),k2*(pt[3]+dy)));
        }
    }else{
        //std::cerr << "2" << std::endl;
        if(texture==NULL)return;
        Mat src_image = imread(filePath);
        Mat img = src_image.clone();
        if (src_image.empty()){
            std::cerr << "Failed to open image file." << std::endl;
        }
        Mat gray_img;
        cvtColor(img, gray_img, CV_BGR2GRAY);
        normalize(gray_img, gray_img, 0, 255, NORM_MINMAX);
        std::vector<Point2f> corners;
        goodFeaturesToTrack(gray_img, corners,maxN,qual,minD,Mat(),3,true);
        std::vector<Point2f>::iterator it_corner = corners.begin();
        const int imageWidth = texture->width();
        const int imageHeight = texture->height();
        const float k = (float)width()/imageWidth;
        const float k2 = (float)height()/imageHeight;
        const float dx = (width()-k*imageWidth)/2.f;
        const float dy = (height()-k2*imageHeight)/2.f;
        kouhoPoints.clear();
        for (; it_corner != corners.end(); ++it_corner) {
            kouhoPoints.push_back(QPoint(k*(it_corner->x+dx),k2*(it_corner->y+dy)));
        }
    }
}

void Canvas2d::drawKouhoPoints(){
    /*glTexCoord2f(0.0f, 0.0f); glVertex2d(0 , k*imageHeight);
    glTexCoord2f(0.0f, 1.f); glVertex2d(0 ,  0);
    glTexCoord2f(1.f, 1.f); glVertex2d( k*imageWidth ,  0);
    glTexCoord2f(1.f, 0.0f); glVertex2d( k*imageWidth , k*imageHeight);*/
    if(getSnapFlag()!=2) return;
    if(texture==NULL) return;
    glPushMatrix();
    /*const int imageWidth = texture->width();
    const int imageHeight = texture->height();
    const float k = (imageWidth>imageHeight) ? (float)width()/imageWidth : (float)height()/imageHeight;
    const float dx = (width()-k*imageWidth)/2.f;
    const float dy = (height()-k*imageHeight)/2.f;
    glTranslated(dx,dy,0);*/
    glColor4d(0, 0, 1, 0.5);
    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA,GL_ONE_MINUS_SRC_ALPHA);
    glDisable(GL_LIGHTING);
    glEnable(GL_TEXTURE_2D);
    glEnable(GL_ALPHA_TEST);
    for(int i=0; i<kouhoPoints.size(); i++){
        kouhoPoint = kouhoPoints.value(i);
        glPointSize(5);
        glBegin(GL_POINTS);
        glVertex2i(kouhoPoint.x(),kouhoPoint.y());
        glEnd();
    }
    glPopMatrix();
    glDisable(GL_ALPHA_TEST);
    glDisable(GL_TEXTURE_2D);
    glDisable(GL_BLEND);
}

QPoint Canvas2d::snapPos2(){
    int minValue = 9999999;
    int minIndex = 0;
    for(int i=0; i<kouhoPoints.size(); i++){
        kouhoPoint = kouhoPoints.value(i);
        int vvv = (kouhoPoint.x() - lastPos.x())*(kouhoPoint.x() - lastPos.x()) + (kouhoPoint.y() - lastPos.y())*(kouhoPoint.y() - lastPos.y());
        if(minValue>vvv){
            minValue = vvv;
            minIndex = i;
        }
    }
    return kouhoPoints.value(minIndex);
}

/* tuika @ 2018 fin */
/* --- Protecteds --- */
void Canvas2d::initializeGL()
{
    glEnable(GL_BLEND);
    glClearColor(1,1,1,1);
    glEnable(GL_DEPTH_TEST);
}

void Canvas2d::resizeGL(int w, int h)
{
    glViewport(0,0,w,h);
    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();
    glOrtho(0,w,h, 0,-10, 1);
    glMatrixMode(GL_MODELVIEW);
    glLoadIdentity();
    autoVertex();
}

void Canvas2d::paintGL()
{
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    draw();
}
void Canvas2d::mousePressEvent(QMouseEvent *event)
{
    if(!this->isActiveWindow()){
        qDebug()<<"Not Active";
        return;
    }

    if(event->button() == Qt::LeftButton){
        (this->*leftMousePressEventFunctions.at(mode))(event);
    }else if (event->button() == Qt::RightButton){
        (this->*rightMousePressEventFunctions.at(mode))(event);
        /* tuika @ 2018 */
        setID();
        positionAdjust();
        halfFoldEdgesAmount.push_back(0);
        saveToFile2();
        /* tuika @ 2018 fin */
    }else if(event->button() == Qt::MiddleButton){
        (this->*middleMousePressEventFunctions.at(mode))(event);
    }
    lastPos = event->pos();
    update();
}
void Canvas2d::mouseMoveEvent(QMouseEvent *event)
{

    if(event->buttons() & Qt::LeftButton){
        (this->*leftMouseMoveEventFunctions.at(mode))(event);
    }else if (event->buttons() & Qt::RightButton){
        (this->*rightMouseMoveEventFunctions.at(mode))(event);
    }else if(event->buttons() & Qt::MiddleButton){
        (this->*middleMouseMoveEventFunctions.at(mode))(event);
    }
    lastPos = event->pos();
    update();
}
void Canvas2d::mouseReleaseEvent(QMouseEvent *event)
{
    if(event->button() == Qt::LeftButton){
        (this->*leftMouseReleaseEventFunctions.at(mode))(event);
    }else if (event->button() == Qt::RightButton){
        (this->*rightMouseReleaseEventFunctions.at(mode))(event);
    }else if(event->button() == Qt::MiddleButton){
        (this->*middleMousePressEventFunctions.at(mode))(event);
    }
    lastPos = event->pos();
    update();
}

void Canvas2d::mouseDoubleClickEvent(QMouseEvent *event)
{
    (this->*mouseDoubleClickEventFunctions.at(mode))(event);
    lastPos = event->pos();
    update();
}

void Canvas2d::keyPressEvent(QKeyEvent *event)
{
    if(getIsDebugMode()){
        qDebug()<<"Canvas2d::keyPressEvent:event->key = "<<event->key();
    }
    if(event->key() == Qt::Key_Shift){
        isShiftKeyBeenPressing = true;
    }
    if(event->key() == Qt::Key_Tab){
        changeModels();
    }
    if(event->key() == Qt::Key_Backtab){
        backModels();
    }
    if(event->key() == Qt::Key_D){
        setIsDebugMode(!getIsDebugMode());
    }
    if(event->key() == 58){
        setSizeOfAnnotations(getSizeOfAnnotations()+1);
    }
    if(event->key() == 59){
        setSizeOfAnnotations(getSizeOfAnnotations()-1);
    }
    if(event->key() == Qt::Key_R){
        setIsSkelton(!getIsSkelton());
    }
    /* tuika @ 2018 */
    if(event->key() == Qt::Key_B){
        keyPress_B();
    }
    /* tuika @ 2018 fin */
    switch(mode){

    case CREATE:
        /* v v v v v JUST TEST v v v v v */
        if(event->key()==Qt::Key_1){
            setMirrorTimes(1);
        }else if(event->key()==Qt::Key_2){
            setMirrorTimes(2);
        }
        /* ^^^^^ JUST TEST ^^^^^ */
        if(event->key()==Qt::Key_S){
            changeSnapFlag();
        }
        break;


    case GLUE:
        if(event->key() == Qt::Key_Return){

        }else if(event->key() == Qt::Key_Delete){

        }
        break;
    }
    update();
}

void Canvas2d::keyReleaseEvent(QKeyEvent *event)
{
    if(event->key() & Qt::Key_Shift){
        isShiftKeyBeenPressing = false;
    }
    update();
}

void Canvas2d::enterEvent(QEvent *event)
{
    //    activateWindow();
}

Model2d *Canvas2d::createModel()
{

    createAnnotationList();
    Model2d *m = new Model2d(this);
    addModel(m);
    setModel(m);

    return model;
}

void Canvas2d::setCurrentModelsAsPair()
{
    model->setM3d(canvas3d->getModel());

}

void Canvas2d::changeModels()
{
    if(model->getM3d()==NULL){
        qDebug()<<"model->getM3d()==NULL";
    }else{
        int i = models.indexOf(model);

        i  = (i==models.size()-1)? 0: i+1;

        setModel(models.at(i));
    }
}

void Canvas2d::backModels()
{
    if(model->getM3d()==NULL){
        qDebug()<<"model->getM3d()==NULL";
    }else{
        int i = models.indexOf(model);

        i = (i==0)?models.size()-1: i-1;

        setModel(models.at(i));
    }
}

/* --- Private---*/
void Canvas2d::loadImage(QString filename)
{
    filePath = filename.toStdString();
    bool ret = image.load(filename);
    //qDebug()<<"image.load = "<<ret;
    texture = new QOpenGLTexture(image.mirrored());
    texture->setMinificationFilter(QOpenGLTexture::LinearMipMapLinear);
    texture->setMagnificationFilter(QOpenGLTexture::Linear);
//    canvas3d->setupTexture(filename);
    update();
}

void Canvas2d::setMode(Mode m)
{
    mode = m;
//    qDebug()<<"mode = "<<m;
}
Vertex2d *Canvas2d::createVertex(const QPoint &pos)
{
    Vertex2d *v = new Vertex2d(pos.x(), pos.y());
    model->addVertex(v);
    return v;
}

Halfedge2d *Canvas2d::createHalfedge(Vertex2d *v1, Vertex2d *v2)
{
    Halfedge2d *he = new Halfedge2d(v1, v2);
    model->addHalfedge(he);
    return he;
}

Face2d *Canvas2d::createFace(Halfedge2d *he)
{
    Face2d *f = new Face2d(he);
    f->setMirrorTimes(this->getMirrorTimes());
    f->setILayer(this->getCurrentLayerNumber());
    f->setIsBranch(this->getIsBranch());
    model->addFace(f);
    emit(face2dCreated(f));
    return f;
}

void Canvas2d::addModel(Model2d *m)
{
    models.push_back(m);
}

QImage Canvas2d::getImage() const
{
    return image;
}

void Canvas2d::setImage(const QImage &value)
{
    image = value;
}

QOpenGLTexture *Canvas2d::getTexture()
{
    return this->texture;
}

bool Canvas2d::getIsDebugMode() const
{
    return isDebugMode;
}

void Canvas2d::setIsDebugMode(bool value)
{
    isDebugMode = value;
    if(canvas3d!=NULL){
        canvas3d->setIsDebugMode(value);
    }
}

QVector<Annotation *> *Canvas2d::getAnnotations()
{
    return annotations;
}

Model2d *Canvas2d::getModel() const
{
    return model;
}

void Canvas2d::setModel(Model2d *value)
{

    model = value;
    const int i = models.indexOf(model);
    setAnnotationList(annotationListList.at(i));
    if(canvas3d!=NULL&&value->getM3d()!=NULL&&canvas3d->getModel()!=value->getM3d()){
        canvas3d->setModel(value->getM3d());
    }
    emit(updated());
}


/* Mouse Picking */
int Canvas2d::pickingObject(const QPoint &pos, void (Model2d::*drawObject)()){
    //    qDebug()<<pos;
   static const int MaxSize = 512;
   static const float range = 10;
   GLuint buffer[MaxSize];
   GLint viewport[4];
   glGetIntegerv(GL_VIEWPORT, viewport);
//   qDebug()<<"viewport = "<<viewport[0]<<", "<<viewport[1]<<", "<<viewport[2]<<", "<<viewport[3];
   glSelectBuffer(MaxSize, buffer);
   glRenderMode(GL_SELECT);
   glInitNames();
   glPushName(-1);
   glMatrixMode(GL_PROJECTION);
   glPushMatrix();
   glLoadIdentity();
   gluPickMatrix(GLdouble(pos.x()), viewport[3]-GLdouble(pos.y()), range, range, viewport);

   glOrtho(viewport[0], viewport[2], viewport[3], viewport[1], -1, 1);
   (model->*drawObject)();

   glMatrixMode(GL_PROJECTION);
   glPopMatrix();
   int hits = glRenderMode(GL_RENDER);

   return (hits<=0)? -1: buffer[3];
}

QVector<int> Canvas2d::pickingObjects(const QPoint &pos, void (Model2d::*drawObject)())
{
    const int MaxSize = 512;
    const float range = 5;
    GLuint buffer[MaxSize];
    GLint viewport[4];
    glGetIntegerv(GL_VIEWPORT, viewport);
    glSelectBuffer(MaxSize, buffer);
    glRenderMode(GL_SELECT);
    glInitNames();
    glPushName(-1);
    glMatrixMode(GL_PROJECTION);
    glPushMatrix();
    glLoadIdentity();
    gluPickMatrix(GLdouble(pos.x()), viewport[3]-GLdouble(pos.y()), range, range, viewport);

    glOrtho(viewport[0], viewport[2], viewport[3], viewport[1], -1, 1);
    (model->*drawObject)();
    glMatrixMode(GL_PROJECTION);
    glPopMatrix();
    int hits = glRenderMode(GL_RENDER);

    QVector<int> ids;
    int iMin = 0;
    for(int i = 0; i<hits; ++i){
        ids<<buffer[4*i+3];
    }
    return ids;
}

Vertex2d *Canvas2d::vertexAtPosition(const QPoint &pos)
{
    void (Model2d::*func)() = &Model2d::drawVertices;
    QVector<int>ids = pickingObjects(pos, func);
    if(ids.empty()){
        return NULL;
    }else{
        int id = ids.front();
        if(ids.size()>1&&isSelected(model->vertices.at(id))){
            id = ids.at(1);
        }
        return model->vertices.at(id);
    }
}

Halfedge2d *Canvas2d::halfedgeAtPosition(const QPoint &pos)
{
    void (Model2d::*func)() = &Model2d::drawHalfedges;
    QVector<int>ids = pickingObjects(pos, func);
    if(ids.empty()){
        return NULL;
    }else{
        int id = ids.front();
        if(ids.size()>1&&isSelected(model->halfedges.at(id))){
            id = ids.at(1);
        }
        return model->halfedges.at(id);
    }
}
QVector<Halfedge2d*>Canvas2d::allHalfedgesAtPosition(const QPoint &pos){
    void (Model2d::*func)() = &Model2d::drawHalfedges;
    QVector<int>ids = pickingObjects(pos, func);
    QVector<Halfedge2d*>hes;
    for(int i = 0; i < ids.size(); ++i){
        hes<<model->halfedges.at(ids.at(i));
    }
    return hes;
}
Face2d *Canvas2d::faceAtPosition(const QPoint &pos)
{
    void (Model2d::*func)() = &Model2d::drawFaces;
    int id = pickingObject(pos, func);
    return (id == -1)? NULL: model->faces.at(id);
}

Vertex2d *Canvas2d::vertexAtPositionOLD(const QPoint &pos)
{
    void (Model2d::*func)() = &Model2d::drawVertices;
    int id = pickingObject(pos, func);
    return (id == -1)? NULL: model->vertices.at(id);
}

Halfedge2d *Canvas2d::halfedgeAtPositionOLD(const QPoint &pos)
{
    void (Model2d::*func)() = &Model2d::drawHalfedges;
    int id = pickingObject(pos, func);
    return (id == -1)? NULL: model->halfedges.at(id);
}



/* --- Draw ---*/
void Canvas2d::draw()
{
    if(mode == CREATE){
        //drawKouhoPoints();
        drawSnapPoint();
        drawCreatingObjects();
    }
    drawTest();
//    drawMirrorTimes();
//    drawAnnotations();
//    model->draw();
    if(model->faces.size()>0&&isDebugMode)model->drawBoundingBox();
    drawMirrorConnectAnnotations();
    drawAllAnnotations();

    drawActiveModel();
    drawAllModels();
    if(snapFlag == 0)
    drawGrid();
    /* tuika @ 2018 */
    if(mode == CREATE){
        drawKouhoPoints();
    }
    /* tuika @ 2018 fin */
    drawImage();
}

void Canvas2d::drawMirrorTimes(){
    for(int i = 0; i< model->faces.size();++i){
        model->faces.at(i)->drawMirrorTimes();
    }
}
void Canvas2d::drawAllAnnotations(){
    int name = 0;
    int N = annotationListList.size();

    for(int i = 0; i < N; ++i){
        QVector<Annotation*> aList = *annotationListList.at(i);
        for(int j = 0; j < aList.size(); ++j){
            glLoadName(name++);
            aList.at(j)->draw();
        }
    }


}

void Canvas2d::drawAllModels(){

    for(int i = 0; i < models.size(); ++i){
        if(models.at(i)!=getModel()){
            models.at(i)->draw();

        }
    }

}

void Canvas2d::drawGrid()
{
    glPushMatrix();
    static const float color = 0.7f;
    int Nx = width()/dGrid;
    int Ny = height()/dGrid;

    glColor3f(color, color, color);


    for(int i = 0; i < Nx+1; ++i){
        glLineWidth(1);
        if(i==Nx/2){
           if(!isDebugMode) glLineWidth(2);
        }
        glBegin(GL_LINES);

        glVertex2d(i*dGrid, 0);
        glVertex2d(i*dGrid, height());
        glEnd();
    }
    for(int i = 0; i < Ny+1; ++i){
        glLineWidth(1);
        if(i==Ny/2){
            if(!isDebugMode)glLineWidth(2);
        }
        glBegin(GL_LINES);

        glVertex2d(0, i*dGrid);
        glVertex2d(width(), i*dGrid);
        glEnd();
    }
    glPopMatrix();
}

void Canvas2d::drawAxis(){
    static const int cap = 50;
    static const int d = 50;
    static const float color = 0.7f;
    glLineWidth(2);
    glDisable(GL_LIGHTING);
    glBegin(GL_LINES);
    glColor3d(color,0,0);
    glVertex2d(0+cap,0+cap);
    glVertex2d(d+cap,0+cap);
    glColor3d(0,color,0);
    glVertex2d(0+cap,0+cap);
    glVertex2d(0+cap,d+cap);
    glEnd();
}

void Canvas2d::drawImage(){
    if(texture!=NULL){
        /*nst int imageWidth = texture->width();
        const int imageHeight = texture->height();
        const float k = (imageWidth>imageHeight) ? (float)width()/imageWidth : (float)height()/imageHeight;
        const float dx = (width()-k*imageWidth)/2.f;
        const float dy = (height()-k*imageHeight)/2.f;
        texture->bind();
        static const float alpha = 0.3f;//AAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAA 170109
        glColor4d(1,1,1,alpha);
        glEnable(GL_BLEND);
        glBlendFunc(GL_SRC_ALPHA,GL_ONE_MINUS_SRC_ALPHA);
        glDisable(GL_LIGHTING);
        glEnable(GL_TEXTURE_2D);
        glEnable(GL_ALPHA_TEST);
        glPushMatrix();
        glTranslatef( dx, dy , 0);
        glBegin(GL_POLYGON);
        glTexCoord2f(0.0f, 0.0f); glVertex2d(0 , k*imageHeight);
        glTexCoord2f(0.0f, 1.f); glVertex2d(0 ,  0);
        glTexCoord2f(1.f, 1.f); glVertex2d( k*imageWidth ,  0);
        glTexCoord2f(1.f, 0.0f); glVertex2d( k*imageWidth , k*imageHeight);*/
        /*glTexCoord2f(0.0f, 0.0f); glVertex2d(0 , imageHeight);
                glTexCoord2f(0.0f, 1.f); glVertex2d(0 ,  0);
                glTexCoord2f(1.f, 1.f); glVertex2d( imageWidth ,  0);
                glTexCoord2f(1.f, 0.0f); glVertex2d( imageWidth , imageHeight);*/
        /*tuika @ 2018*/
        const int imageWidth = texture->width();
                const int imageHeight = texture->height();
                const float k = (float)width()/imageWidth;
                const float k2 =(float)height()/imageHeight;
                const float dx = (width()-k*imageWidth)/2.f;
                const float dy = (height()-k2*imageHeight)/2.f;
                texture->bind();
                static const float alpha = 0.3f;//AAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAA 170109
                glColor4d(1,1,1,alpha);
                glEnable(GL_BLEND);
                glBlendFunc(GL_SRC_ALPHA,GL_ONE_MINUS_SRC_ALPHA);
                glDisable(GL_LIGHTING);
                glEnable(GL_TEXTURE_2D);
                glEnable(GL_ALPHA_TEST);
                glPushMatrix();
                glTranslatef( dx, dy , 0);
                glBegin(GL_POLYGON);
                glTexCoord2f(0.0f, 0.0f); glVertex2d(0 , k2*imageHeight);
                glTexCoord2f(0.0f, 1.f); glVertex2d(0 ,  0);
                glTexCoord2f(1.f, 1.f); glVertex2d( k*imageWidth ,  0);
                glTexCoord2f(1.f, 0.0f); glVertex2d( k*imageWidth , k2*imageHeight);
         /*tuika @ 2018 fin*/
        glEnd();
        glPopMatrix();
        glDisable(GL_ALPHA_TEST);
        glDisable(GL_TEXTURE_2D);
        glDisable(GL_BLEND);
    }
}

void Canvas2d::drawSnapPoint()
{
    if(snapFlag==0){
        QPoint p = snapPos(lastPos);
        glColor3d(1, 0, 0);
        glDisable(GL_LIGHTING);
        glPointSize(5);
        glBegin(GL_POINTS);
        glVertex2i(p.x(), p.y());
        glEnd();
    }/* tuika @ 2018 */else if(snapFlag==2 /*&& kouhoPoints.size()>1*/){
        kouhoPoint = snapPos2();
        glColor3d(0, 0, 1);
        glDisable(GL_LIGHTING);
        glPointSize(10);
        glBegin(GL_POINTS);
        glVertex2i(kouhoPoint.x(), kouhoPoint.y());
        glEnd();
    }else{
        QPoint p = lastPos;
        glColor3d(1, 0, 0);
        glDisable(GL_LIGHTING);
        glPointSize(5);
        glBegin(GL_POINTS);
        glVertex2i(p.x(), p.y());
        glEnd();
    }
    /* tuika @ 2018 fin */
}

void Canvas2d::drawTest()
{

}

void Canvas2d::drawActiveModel()
{
    glPointSize(model->getPointSize()+2);

    if(model->faces.size()>0){
//        model->drawWithoutSize();
        glMatrixMode(GL_MODELVIEW);
        glLoadIdentity();

        model->drawFacesWithAlphaBlend();

        model->drawVerticesWithoutSize();
        glLineWidth(model->getLineWidth()+2);
        model->drawSelectedHalgedgesWithoutSize();
        glLineWidth(model->getLineWidth()+1);
        model->drawUnselectedHalfedgesWithoutSize();

        if(getIsDebugMode()){
            model->drawFaceOrder();
            model->drawNormals();
        }

        if(!isSkelton)model->drawSmallFaces(1);//20171219

    }
}

bool Canvas2d::getIsSkelton() const
{
    return isSkelton;
}

void Canvas2d::setIsSkelton(bool value)
{
    isSkelton = value;
}


/* --- Mode Settings ---*/

void Canvas2d::holdCurrentMode()
{

}

void Canvas2d::releaseCurrentMode()
{

}

void Canvas2d::initMouseFunctions()
{
   leftMousePressEventFunctions<<&Canvas2d::leftMousePressEventOfEditMode<<&Canvas2d::leftMousePressEventOfCreateMode<<&Canvas2d::leftMousePressEventOfGlueMode;
   rightMousePressEventFunctions<<&Canvas2d::rightMousePressEventOfEditMode<<&Canvas2d::rightMousePressEventOfCreateMode<<&Canvas2d::rightMousePressEventOfGlueMode;
   middleMousePressEventFunctions<<&Canvas2d::middleMousePressEventOfEditMode<<&Canvas2d::middleMousePressEventOfCreateMode<<&Canvas2d::middleMousePressEventOfGlueMode;

   mouseDoubleClickEventFunctions<<&Canvas2d::mouseDoubleClickEventOfEditMode<<&Canvas2d::mouseDoubleClickEventOfCreateMode<<&Canvas2d::mouseDoubleClickEventOfGlueMode;

   leftMouseReleaseEventFunctions<<&Canvas2d::leftMouseReleaseEventOfEditMode<<&Canvas2d::leftMouseReleaseEventOfCreateMode<<&Canvas2d::leftMouseReleaseEventOfGlueMode;
   rightMouseReleaseEventFunctions<<&Canvas2d::rightMouseReleaseEventOfEditMode<<&Canvas2d::rightMouseReleaseEventOfCreateMode<<&Canvas2d::rightMouseReleaseEventOfGlueMode;
   middleMouseReleaseEventFunctions<<&Canvas2d::middleMouseReleaseEventOfEditMode<<&Canvas2d::middleMouseReleaseEventOfCreateMode<<&Canvas2d::middleMouseReleaseEventOfGlueMode;

   leftMouseMoveEventFunctions<<&Canvas2d::leftMouseMoveEventOfEditMode<<&Canvas2d::leftMouseMoveEventOfCreateMode<<&Canvas2d::leftMouseMoveEventOfGlueMode;
   rightMouseMoveEventFunctions<<&Canvas2d::rightMouseMoveEventOfEditMode<<&Canvas2d::rightMouseMoveEventOfCreateMode<<&Canvas2d::rightMouseMoveEventOfGlueMode;
   middleMouseMoveEventFunctions<<&Canvas2d::middleMouseMoveEventOfEditMode<<&Canvas2d::middleMouseMoveEventOfCreateMode<<&Canvas2d::middleMouseMoveEventOfGlueMode;
}

void Canvas2d::nullMouseEvent(QMouseEvent *)
{
    qDebug()<<"this event  is NULL on current mode";
}
/* --- Edit Mode --- */
void Canvas2d::leftMousePressEventOfEditMode(QMouseEvent *event)
{
    Vertex2d *v = NULL;
    Halfedge2d *he = NULL;
    Face2d *f = NULL;
    v = vertexAtPosition(event->pos());
    he = halfedgeAtPosition(event->pos());
    f = faceAtPosition(event->pos());
    if(v != NULL){
        selectVertex(v);
    }else if(he != NULL){
        selectHalfedge(he);
    }else if(f != NULL){
        selectFace(f);
    }
    if(v!=NULL|| he!=NULL|| f!=NULL){
        holdCurrentMode();
    }
}

void Canvas2d::rightMousePressEventOfEditMode(QMouseEvent *event)
{

}

void Canvas2d::middleMousePressEventOfEditMode(QMouseEvent *event)
{

}

void Canvas2d::leftMouseMoveEventOfEditMode(QMouseEvent *event)
{
    /* Vertex Move */
    QVector2D vec(event->pos().x()-lastPos.x(), event->pos().y()-lastPos.y());
    for(int i = 0; i<verticesSelected.size(); ++i){
        verticesSelected.at(i)->movePosition(vec);
    }
    /* Edge Move */
    for(int i = 0; i  < halfedgesSelected.size(); ++i){

    }
    /* Face Move -> Model Move*/
//    for(int i = 0; i < facesSelected.size(); ++i){
//        facesSelected.at(i)->movePosition(vec);
//    }
    if(facesSelected.size()>0){
        model->movePosition(vec);
    }
}

void Canvas2d::rightMouseMoveEventOfEditMode(QMouseEvent *event)
{

}

void Canvas2d::middleMouseMoveEventOfEditMode(QMouseEvent *event)
{

}

void Canvas2d::mouseDoubleClickEventOfEditMode(QMouseEvent *event)
{

}

int Canvas2d::getSnapFlag() const
{
    return snapFlag;
}

void Canvas2d::setSnapFlag(int value)
{
    snapFlag = value%3;
}

void Canvas2d::leftMouseReleaseEventOfEditMode(QMouseEvent *event)
{

    unselectAll();
    releaseCurrentMode();
}

void Canvas2d::rightMouseReleaseEventOfEditMode(QMouseEvent *event)
{

}

void Canvas2d::middleMouseReleaseEventOfEditMode(QMouseEvent *event)
{

}
/* --- Create Mode --- */

void Canvas2d::setMirrorTimes(int value)
{
    mirrorTimes  = value;
}

int Canvas2d::getMirrorTimes()
{
    return mirrorTimes;
}

void Canvas2d::leftMousePressEventOfCreateMode(QMouseEvent *event){
    QPoint pos = event->pos();
    if(snapFlag==0){// SNAP MODE
        pos = snapPos(pos);
    }else if(snapFlag==2){
        pos = snapPos2();
    }
    clickCreateVertex(pos);

    holdCurrentMode();
}

void Canvas2d::clickCreateVertex(const QPoint &pos)
{
    Vertex2d *v = createVertex(pos); //model ni add(vertex(pos,pos))
    Halfedge2d *he = NULL;
    if(lastVertex != NULL){
        he = createHalfedge(lastVertex, v);

        if(firstHalfedge == NULL){
            firstHalfedge = he;
        }else{
            he->setPrev(lastHalfedge);
        }
        lastHalfedge = he;
        addCreatingHalfedges(he);
    }
    lastVertex = v;

    addCreatingVertices(v);
}
void Canvas2d::rightMousePressEventOfCreateMode(QMouseEvent *event){

    if(firstHalfedge!=NULL&& firstHalfedge!=lastHalfedge){
        Halfedge2d *he = createHalfedge(lastVertex, firstHalfedge->vertex);
        he->setNext(firstHalfedge);
        he->setPrev(lastHalfedge);
        createFace(firstHalfedge);
        lastVertex = NULL;
        lastHalfedge = NULL;
        firstHalfedge = NULL;
        clearCreatingObjects();
        releaseCurrentMode();
    }

    /* tuika @ 2018 */
    //positionAdjust();
    /* tuika @ 2018 end */
}

void Canvas2d::middleMousePressEventOfCreateMode(QMouseEvent *event)
{

}

void Canvas2d::leftMouseReleaseEventOfCreateMode(QMouseEvent *event)
{

}

void Canvas2d::rightMouseReleaseEventOfCreateMode(QMouseEvent *event)
{

}

void Canvas2d::middleMouseReleaseEventOfCreateMode(QMouseEvent *event)
{

}

void Canvas2d::leftMouseMoveEventOfCreateMode(QMouseEvent *event)
{

}

void Canvas2d::rightMouseMoveEventOfCreateMode(QMouseEvent *event)
{

}

void Canvas2d::middleMouseMoveEventOfCreateMode(QMouseEvent *event)
{

}

void Canvas2d::mouseDoubleClickEventOfCreateMode(QMouseEvent *event)
{

}

void Canvas2d::cancelCurrentCreation()
{

}

void Canvas2d::addCreatingVertices(Vertex2d *v)
{
    creatingVertices.push_back(v);
}

void Canvas2d::addCreatingHalfedges(Halfedge2d *he)
{
    creatingHalfedges.push_back(he);
}

void Canvas2d::removeCreatingVertices(Vertex2d *v)
{
    creatingVertices.removeOne(v);
}

void Canvas2d::removeCreatinghalfedges(Halfedge2d *he)
{
    creatingHalfedges.removeOne(he);
}

void Canvas2d::clearCreatingObjects()
{
    creatingVertices.clear();
    creatingHalfedges.clear();
}

void Canvas2d::drawCreatingObjects()
{
    glPointSize(4);
    glLineWidth(3);
    glColor3d(1,0,0);
    for(int i = 0; i < creatingVertices.size(); ++i){
        creatingVertices.at(i)->drawWithoutAll();
    }
    for(int i = 0; i < creatingHalfedges.size(); ++i){
        creatingHalfedges.at(i)->drawWithoutAll();
    }
}




/* --- Glue Mode --- */

void Canvas2d::clearGlueEdges()
{
    glueEdges.clear();
}

void Canvas2d::addGlueEdges(Halfedge2d *he)
{
    if(glueEdges.indexOf(he)==-1){
        glueEdges.push_back(he);
        emit(glueEdgeAdded(he));
    }
}

void Canvas2d::removeGlueEdges(Halfedge2d *he)
{
    if(glueEdges.indexOf(he)!=-1){
        glueEdges.removeOne(he);
        emit(glueEdgeRemoved(he));
    }
}

void Canvas2d::addSelectEdgesToGlueEdges()
{
    for(int i = 0; i < halfedgesSelected.size(); ++i){
        addGlueEdges(halfedgesSelected.at(i));
    }
}

void Canvas2d::removeSelectEdgesFromGlueEdges()
{
    for(int i = 0; i < halfedgesSelected.size(); ++i){
        removeGlueEdges(halfedgesSelected.at(i));
    }
}

void Canvas2d::addSelectEdgesToGlueEdges2()
{
    for(int i = 0; i < halfedgesSelected.size(); ++i){
        createGlueEdgeAnnotation(halfedgesSelected.at(i));
    }
    /* tuika @ 2018*/
    halfFoldEdgesAmount.push_back(0);
    saveToFile2();
    /* tuika @ 2018 fin */
    update();
}

void Canvas2d::createInsideReverseFoldWithSelectEdges()
{
    removeHalfFoldWithStackEdges();
    Halfedge2d *he2 = NULL;
    Halfedge2d *he = NULL;
    if(halfedgesSelected.size()==2){
        /* tuika @ 2018 */
        //std::cout << " " << std::endl;
        //std::cout << halfedgesSelected.at(0)->vertex->x << " " << halfedgesSelected.at(0)->vertex_next->x << " " << halfedgesSelected.at(1)->vertex->x << " " << halfedgesSelected.at(1)->vertex_next->x << std::endl;
        //std::cout << halfedgesSelected.at(0)->vertex->y << " " << halfedgesSelected.at(0)->vertex_next->y << " " << halfedgesSelected.at(1)->vertex->y << " " << halfedgesSelected.at(1)->vertex_next->y << std::endl;
        if(abs(halfedgesSelected.at(0)->vertex->x - halfedgesSelected.at(1)->vertex->x)<=1&& abs(halfedgesSelected.at(0)->vertex->y - halfedgesSelected.at(1)->vertex->y)<=1){
            he = halfedgesSelected.at(1)->next;
            he2 = halfedgesSelected.at(0)->next;
            Halfedge2d *he3 = halfedgesSelected.at(1)->prev;
            Halfedge2d *he4 = halfedgesSelected.at(0)->prev;
            createInsideReverseFold(halfedgesSelected.at(0),halfedgesSelected.at(1));
            he3->setColor(QColor(150,150,150));
            he4->setColor(QColor(150,150,150));
            //std::cout << " a" << std::endl;
        }else if(abs(halfedgesSelected.at(0)->vertex_next->x - halfedgesSelected.at(1)->vertex->x)<=1 && abs(halfedgesSelected.at(0)->vertex_next->y - halfedgesSelected.at(1)->vertex->y)<=1){
            he = halfedgesSelected.at(1)->next;
            he2 = halfedgesSelected.at(0)->prev;
            Halfedge2d *he3 = halfedgesSelected.at(1)->prev;
            Halfedge2d *he4 = halfedgesSelected.at(0)->next;
            createInsideReverseFold(halfedgesSelected.at(0),halfedgesSelected.at(1));
            he3->setColor(QColor(150,150,150));
            he4->setColor(QColor(150,150,150));
            //std::cout << " b" << std::endl;
        }else if(abs(halfedgesSelected.at(0)->vertex_next->x - halfedgesSelected.at(1)->vertex_next->x)<=1 && abs(halfedgesSelected.at(0)->vertex_next->y - halfedgesSelected.at(1)->vertex_next->y)<=1){
            he = halfedgesSelected.at(1)->prev;
            he2 = halfedgesSelected.at(0)->prev;
            Halfedge2d *he3 = halfedgesSelected.at(1)->next;
            Halfedge2d *he4 = halfedgesSelected.at(0)->next;
            createInsideReverseFold(halfedgesSelected.at(0),halfedgesSelected.at(1));
            he3->setColor(QColor(150,150,150));
            he4->setColor(QColor(150,150,150));
            //std::cout << " c" << std::endl;
        }else if(abs(halfedgesSelected.at(0)->vertex->x - halfedgesSelected.at(1)->vertex_next->x)<=1 && abs(halfedgesSelected.at(0)->vertex->y - halfedgesSelected.at(1)->vertex_next->y)<=1){
            he = halfedgesSelected.at(1)->prev;
            he2 = halfedgesSelected.at(0)->next;
            Halfedge2d *he3 = halfedgesSelected.at(1)->next;
            Halfedge2d *he4 = halfedgesSelected.at(0)->prev;
            createInsideReverseFold(halfedgesSelected.at(0),halfedgesSelected.at(1));
            he3->setColor(QColor(150,150,150));
            he4->setColor(QColor(150,150,150));
            //std::cout << " d" << std::endl;
        }else{
            return;
        }
    }else{
        return;
    }
    createHalfFoldWithStackEdges();
    /* tuika @ 2018*/
    halfFoldEdgesAmount.push_back(0);
    saveToFile2();
    halfFoldEdges.push_back(he);
    halfFoldEdges.push_back(he2);
    createGlueEdgeAnnotation(he);
    createGlueEdgeAnnotation(he2);
    halfFoldEdgesAmount.push_back(2);
    saveToFile2();
    /* tuika @ 2018 fin */
    update();
}

void Canvas2d::createHalfFoldWithStackEdges(){
    for(int i=0; i<halfFoldEdges.size(); i++){
        createGlueEdgeAnnotation(halfFoldEdges.at(i));
    }
    //saveToFile2();
    update();
}

void Canvas2d::removeHalfFoldWithStackEdges(){
    annotationsSelected.clear();
    for(int i=0; i<halfFoldEdges.size(); i++){
        Annotation *a = annotationAtPosition(QPoint((halfFoldEdges.at(i)->vertex->x + halfFoldEdges.at(i)->vertex_next->x)/2,(halfFoldEdges.at(i)->vertex->y + halfFoldEdges.at(i)->vertex_next->y)/2));
        if(!isSelected(a)){
            annotationsSelected.push_back(a);
        }
    }

    for(int i = 0; i < annotationsSelected.size(); i++){
        removeAnnotation(annotationsSelected.at(i));
    }
    annotationsSelected.clear();
}

void Canvas2d::createPleatFoldWithSelectEdges()
{
    if(halfedgesSelected.size()==2){
        createPleatFoldAnnotation(halfedgesSelected.at(0), halfedgesSelected.at(1));
    }
    /* tuika @ 2018*/
    halfFoldEdgesAmount.push_back(0);
    saveToFile2();
    /* tuika @ 2018 fin */
    update();
}

void Canvas2d::createPleatFoldWithSelectFaces()
{
    if(facesSelected.size()==2){
        createPleatFoldAnnotation(facesSelected.at(0), facesSelected.at(1));
    }
    /* tuika @ 2018*/
    halfFoldEdgesAmount.push_back(0);
    saveToFile2();
    /* tuika @ 2018 fin */
    update();
}

void Canvas2d::createTurnFoldWithSelectEdges()
{
    if(halfedgesSelected.size()==2){
        createTurnFoldAnnotation(halfedgesSelected.at(0), halfedgesSelected.at(1));
    }
    /* tuika @ 2018*/
    halfFoldEdgesAmount.push_back(0);
    saveToFile2();
    /* tuika @ 2018 fin */
    update();
}

void Canvas2d::createOutsideReverseFoldWithSelectEdges()
{
    removeHalfFoldWithStackEdges();
    Halfedge2d *he2 = NULL;
    Halfedge2d *he = NULL;
    if(halfedgesSelected.size()==2){
        /* tuika @ 2018 */
        //std::cout << " " << std::endl;
        //std::cout << halfedgesSelected.at(0)->vertex->x << " " << halfedgesSelected.at(0)->vertex_next->x << " " << halfedgesSelected.at(1)->vertex->x << " " << halfedgesSelected.at(1)->vertex_next->x << std::endl;
        //std::cout << halfedgesSelected.at(0)->vertex->y << " " << halfedgesSelected.at(0)->vertex_next->y << " " << halfedgesSelected.at(1)->vertex->y << " " << halfedgesSelected.at(1)->vertex_next->y << std::endl;
        if(abs(halfedgesSelected.at(0)->vertex->x - halfedgesSelected.at(1)->vertex->x)<=1&& abs(halfedgesSelected.at(0)->vertex->y - halfedgesSelected.at(1)->vertex->y)<=1){
            he = halfedgesSelected.at(1)->next;
            he2 = halfedgesSelected.at(0)->next;
            Halfedge2d *he3 = halfedgesSelected.at(1)->prev;
            Halfedge2d *he4 = halfedgesSelected.at(0)->prev;
            createOutsideReverseFoldAnnotation(halfedgesSelected.at(0),halfedgesSelected.at(1));
            he3->setColor(QColor(150,150,150));
            he4->setColor(QColor(150,150,150));
           // std::cout << " a" << std::endl;
        }else if(abs(halfedgesSelected.at(0)->vertex_next->x - halfedgesSelected.at(1)->vertex->x)<=1 && abs(halfedgesSelected.at(0)->vertex_next->y - halfedgesSelected.at(1)->vertex->y)<=1){
            he = halfedgesSelected.at(1)->next;
            he2 = halfedgesSelected.at(0)->prev;
            Halfedge2d *he3 = halfedgesSelected.at(1)->prev;
            Halfedge2d *he4 = halfedgesSelected.at(0)->next;
            createOutsideReverseFoldAnnotation(halfedgesSelected.at(0),halfedgesSelected.at(1));
            he3->setColor(QColor(150,150,150));
            he4->setColor(QColor(150,150,150));
            //std::cout << " b" << std::endl;
        }else if(abs(halfedgesSelected.at(0)->vertex_next->x - halfedgesSelected.at(1)->vertex_next->x)<=1 && abs(halfedgesSelected.at(0)->vertex_next->y - halfedgesSelected.at(1)->vertex_next->y)<=1){
            he = halfedgesSelected.at(1)->prev;
            he2 = halfedgesSelected.at(0)->prev;
            Halfedge2d *he3 = halfedgesSelected.at(1)->next;
            Halfedge2d *he4 = halfedgesSelected.at(0)->next;
            createOutsideReverseFoldAnnotation(halfedgesSelected.at(0),halfedgesSelected.at(1));
            he3->setColor(QColor(150,150,150));
            he4->setColor(QColor(150,150,150));
            //std::cout << " c" << std::endl;
        }else if(abs(halfedgesSelected.at(0)->vertex->x - halfedgesSelected.at(1)->vertex_next->x)<=1 && abs(halfedgesSelected.at(0)->vertex->y - halfedgesSelected.at(1)->vertex_next->y)<=1){
            he = halfedgesSelected.at(1)->prev;
            he2 = halfedgesSelected.at(0)->next;
            Halfedge2d *he3 = halfedgesSelected.at(1)->next;
            Halfedge2d *he4 = halfedgesSelected.at(0)->prev;
            createOutsideReverseFoldAnnotation(halfedgesSelected.at(0),halfedgesSelected.at(1));
            he3->setColor(QColor(150,150,150));
            he4->setColor(QColor(150,150,150));
            //std::cout << " d" << std::endl;
        }
    }
    createHalfFoldWithStackEdges();
    /* tuika @ 2018*/
    halfFoldEdgesAmount.push_back(0);
    saveToFile2();
    halfFoldEdges.push_back(he);
    halfFoldEdges.push_back(he2);
    createGlueEdgeAnnotation(he);
    createGlueEdgeAnnotation(he2);
    halfFoldEdgesAmount.push_back(2);
    saveToFile2();
    /* tuika @ 2018 fin */
    update();
}

void Canvas2d::createSinkFoldWithSelectedEdge()
{
    if(halfedgesSelected.size()==1){
        createSinkFoldAnnotation(halfedgesSelected.at(0));
    }
    /* tuika @ 2018*/
    halfFoldEdgesAmount.push_back(0);
    saveToFile2();
    /* tuika @ 2018 fin */
    update();
}

void Canvas2d::createPigsLegAnnotationWithSelectedEdges()
{
    removeHalfFoldWithStackEdges();
    Halfedge2d *he = NULL;
    if(halfedgesSelected.size()==3){
        if(abs(halfedgesSelected.at(0)->vertex->x - halfedgesSelected.at(2)->vertex->x)<=1&& abs(halfedgesSelected.at(0)->vertex->y - halfedgesSelected.at(2)->vertex->y)<=1){
            he = halfedgesSelected.at(0)->prev;
            createPigsLegAnnotation(halfedgesSelected.at(0), halfedgesSelected.at(1),halfedgesSelected.at(2));
            //std::cout << " a" << std::endl;
        }else if(abs(halfedgesSelected.at(0)->vertex_next->x - halfedgesSelected.at(2)->vertex->x)<=1 && abs(halfedgesSelected.at(0)->vertex_next->y - halfedgesSelected.at(2)->vertex->y)<=1){
            he = halfedgesSelected.at(0)->next;
            createPigsLegAnnotation(halfedgesSelected.at(0), halfedgesSelected.at(1),halfedgesSelected.at(2));
            //std::cout << " b" << std::endl;
        }else if(abs(halfedgesSelected.at(0)->vertex_next->x - halfedgesSelected.at(2)->vertex_next->x)<=1 && abs(halfedgesSelected.at(0)->vertex_next->y - halfedgesSelected.at(2)->vertex_next->y)<=1){
            he = halfedgesSelected.at(0)->next;
            createPigsLegAnnotation(halfedgesSelected.at(0), halfedgesSelected.at(1),halfedgesSelected.at(2));
            //std::cout << " c" << std::endl;
        }else if(abs(halfedgesSelected.at(0)->vertex->x - halfedgesSelected.at(2)->vertex_next->x)<=1 && abs(halfedgesSelected.at(0)->vertex->y - halfedgesSelected.at(2)->vertex_next->y)<=1){
            he = halfedgesSelected.at(0)->prev;
            createPigsLegAnnotation(halfedgesSelected.at(0), halfedgesSelected.at(1),halfedgesSelected.at(2));
            //std::cout << " d" << std::endl;
        }else{
            return;
        }
    }else{
        return;
    }
    createHalfFoldWithStackEdges();
    /* tuika @ 2018*/
    halfFoldEdgesAmount.push_back(0);
    saveToFile2();
    halfFoldEdges.push_back(he);
    createGlueEdgeAnnotation(he);
    halfFoldEdgesAmount.push_back(1);
    saveToFile2();
    /* tuika @ 2018 fin */
    update();
}

void Canvas2d::createOrikomiEdgeAnnotationOnSelectedEdge()
{
    for(int i = 0; i < halfedgesSelected.size(); ++i){
        createOrikomiEdgeAnnotation(halfedgesSelected.at(i));
    }
    /* tuika @ 2018*/
    halfFoldEdgesAmount.push_back(0);
    saveToFile2();
    /* tuika @ 2018 fin */
    update();
}

void Canvas2d::deleteSelectAnnotations()
{
    QVector<Annotation*>as  = annotationsSelected;
    for(int i = 0; i < as.size(); ++i){
        deleteAnnotation(as.at(i));
    }
    /* tuika @ 2018*/
    halfFoldEdgesAmount.push_back(0);
    saveToFile2();
    /* tuika @ 2018 fin */
    update();
}

void Canvas2d::saveToFile()
{
    QString filename = QFileDialog::getSaveFileName(this, "Save File", "", "VFEA File (*.vfea);;All Files (*)");
    if(filename.isEmpty()){
        return;
    }else{
        QFile file(filename);
        if(!file.open(QIODevice::WriteOnly)){
            return;
        }
        QTextStream out(&file);

        /* OBJ */
        for(int i = 0; i < model->vertices.size(); ++i){
            Vertex2d *v  = model->vertices.at(i);
            v->setIdForExport(i+1);
            out<<"v "<<v->x<<" "<<v->y<<endl;
        }

        for(int i = 0; i < model->faces.size(); ++i){
            Face2d *f  = model->faces.at(i);
            out<<"f";
            Halfedge2d *he = f->halfedge;
            do{
                out<<" "<<he->vertex->getIdForExport();
                he = he->next;
            }while(he != f->halfedge);
            out<<endl;
        }

        /* Original Information */
        for(int i = 0; i < model->halfedges.size(); ++i){
            Halfedge2d *he = model->halfedges.at(i);
            he->setIdForExport(i+1);
            out<<"e"<<" "<<he->vertex->getIdForExport()<<" "<<he->vertex_next->getIdForExport()<<endl;
        }

        for(int i = 0; i < annotations->size(); ++i){
            Annotation *a = annotations->at(i);
            int annotationName = a->getAnnotationName();
            out<<"a"<<" "<<annotationName;
            for(int j = 0; j < a->getHesNum(); ++j){
                Halfedge2d *he = a->getHes().at(j);
                out<<" "<<he->getIdForExport();
            }
            out<<endl;
        }

    }
}
void Canvas2d::createPairModels(){
    createModel();
    canvas3d->createModel();
    setCurrentModelsAsPair();
}

void Canvas2d::loadFromFile()//this->model & annotations should be new-ed and cleared
{
    createPairModels();

    QString filename = QFileDialog::getOpenFileName(this,"Load File", "", "VFEA File (*.vfea);;All Files (*)" );
    QFile file(filename);
    if(!file.open(QIODevice::ReadOnly)){
        return;
    }
    QTextStream in(&file);

    int iv = 1;
    int ie = 1;

    while(!in.atEnd()){
        QString str;
        in>>str;

        if(str=="v"){
            int x,y;
            in>>x;
            in>>y;
            Vertex2d *v = createVertex(QPoint(x,y));
            v->setIdForExport(iv++);
        }else if(str == "f"){
            str = in.readLine();
            int spaceCount = str.count(" "); // 170126 spaceCount == Number of Vertices

            QTextStream line(&str);
            QVector<Vertex2d*>vs;
            for(int i = 0; i < spaceCount; ++i){
               int id;
               line>>id;
               vs.push_back(model->vertices.at(id-1));
            }

            QVector<Halfedge2d*>hes;
            for(int i = 0; i < vs.size(); ++i){
                Vertex2d *v1 = vs.at(i);
                Vertex2d *v2 = vs.at((i+1==vs.size())? 0 : i+1);
                Halfedge2d *he = createHalfedge(v1, v2);
                hes.push_back(he);
            }

            for(int i = 0; i < hes.size(); ++i){
                Halfedge2d *he1 = hes.at(i);
                Halfedge2d *he2 = hes.at((i+1==hes.size())? 0 : i+1);
                he1->setNext(he2);
            }

            Face2d *f = createFace(hes.first());
        }else if(str=="e"){
            int vertexID1, vertexID2;
            in>>vertexID1;
            in>>vertexID2;
            Halfedge2d *he = model->getHalfedge(vertexID1-1, vertexID2-1);
            if(he==NULL){
                qDebug()<<"Canvas2d::loadFromFile : he is NULL(1)";
                return;
            }
            he->setIdForExport(ie++);
        }else if(str=="a"){
            int annotationName;
            in>>annotationName;
            str = in.readLine();
            int spaceCount = str.count(" "); // 170126 spaceCount == Number of Halfedges' ID

            QTextStream line(&str);
            QVector<Halfedge2d*>hes;
            for(int i = 0; i < spaceCount; ++i){
                int halfedgeId;
                line>>halfedgeId;
                Halfedge2d *he = NULL;
                for(int j = 0; j < model->halfedges.size(); ++j){
                    Halfedge2d *heAtJ = model->halfedges.at(j);
                    if(heAtJ->getIdForExport()==halfedgeId){
                        he = heAtJ;
                        break;
                    }
                }
                if(he == NULL){
                    qDebug()<<"Canvas2d::loadFromFile : he is NULL(2)";
                    return;
                }
                hes.push_back(he);
            }

            Annotation *a = createAnnotation(annotationName, hes);

        }else{

        }
    }

}

void Canvas2d::createPairModelsSlot()
{
    createPairModels();
}

/* mirror connection */
bool Canvas2d::isPossibleToCreateMirrorConnection()
{

    if(halfedgesSelected.size()<2){
        qDebug()<<"Canvas2d::isPossibleToCreateMirrorConnection : halfedgesSelected.size()<3";
        return false;
    }else if(halfedgesSelected.size()==2){
        /* simple connect*/
       // std::cout << halfedgesSelected.front()->getFace()->getModel()->getM3d()->getMirrorPlane() != halfedgesSelected.last()->getFace()->getModel()->getM3d()->getMirrorPlane() << std::endl;
       // std::cout << (halfedgesSelected.last()->getFace()->getModel()->getM3d()->getMirrorPlane()->getParent()==NULL) << std::endl;
        return halfedgesSelected.front()->getFace()->getModel()->getM3d()->getMirrorPlane() != halfedgesSelected.last()->getFace()->getModel()->getM3d()->getMirrorPlane()
                && halfedgesSelected.last()->getFace()->getModel()->getM3d()->getMirrorPlane()->getParent()==NULL;
    }else if(halfedgesSelected.size()>2){
        /* branch connect */
        MirrorPlane *parentMirror = NULL;
        QVector<MirrorPlane*>mirrorPlanes;
        for(int i = 0; i < halfedgesSelected.size(); ++i){
            Halfedge2d *he = halfedgesSelected.at(i);

            if(i == 0){
                parentMirror = he->getFace()->getModel()->getM3d()->getMirrorPlane();
                mirrorPlanes.push_back(parentMirror);
            }else if( i == halfedgesSelected.size()-1){
                MirrorPlane *sameWithParentMirror = he->getFace()->getModel()->getM3d()->getMirrorPlane();
                if(parentMirror != sameWithParentMirror){
                    //std::cout << "aa" << std::endl;
                    return false;
                }
            }else{
                MirrorPlane *childMirror = he->getFace()->getModel()->getM3d()->getMirrorPlane();
                if(mirrorPlanes.indexOf(childMirror)!=-1){
                    /* dupllicate */
                    return false;
                }else if(childMirror->getParent()!=NULL){
                    /* this child has a parent already */
                    return false;
                }else{
                    mirrorPlanes.push_back(childMirror);
                }
            }

        }
    }

    return true;
}

void Canvas2d::createMirrorConnectionBetweenSelectedEdges()
{
   // std::cerr << isPossibleToCreateMirrorConnection() << std::endl;
    if(isPossibleToCreateMirrorConnection()){
        MirrorConnectAnnotation *a = createMirrorConnection(halfedgesSelected);
        addMirrorConnection(a);
        emit(mirrorConnectAdded(a->getHes()));
    }else{
        qDebug()<<"Canvas2d::createMirrorConnectionBetweenSelectedEdges : isPossibleToCreateMirrorConnection() = false";
    }
}

void Canvas2d::changeSnapFlag()
{
    setSnapFlag(getSnapFlag()+1);
}

QVector<MirrorPlane *> Canvas2d::getMirrorPlanesFromAnnotationsSelected()
{
    QVector<MirrorPlane*>mps;
    for(int i = 0; i < annotationsSelected.size(); ++i){
        Annotation *a = annotationsSelected.at(i);
        if(a->getAnnotationName()==MIRROR){
            QVector<Halfedge2d*> hes = a->getHes();
            //get mirror children
            for(int j = 1; j < hes.size()-1; ++j){
                MirrorPlane *mp = hes.at(j)->getHalfedge3d()->getModel()->getMirrorPlane();
                if(mps.indexOf(mp)==-1){
                    mps.push_back(mp);
                }
            }
        }
    }

    return mps;
}

void Canvas2d::rotateSelectedMirror(int theta)
{
//    QVector<MirrorPlane*>mps = getMirrorPlanesFromAnnotationsSelected();

//    for(int i = 0; i < mps.size(); ++i){
//        emit(mirrorRotated(mps.at(i), theta));
//    }
    /* 170306 */

    Model2d *m2d = getModel();
    if(m2d!=NULL){
        Model3d *m3d = m2d->getM3d();
        if(m3d!=NULL){
            MirrorPlane *mp = m3d->getMirrorPlane();
            if(mp!=NULL){
                emit(mirrorRotated(mp,theta));
            }
        }
    }
}

void Canvas2d::scaleSelectedMirror(int scale)
{
//    QVector<MirrorPlane*>mps = getMirrorPlanesFromAnnotationsSelected();

//    for(int i = 0; i < mps.size(); ++i){
//        emit(mirrorScaled(mps.at(i), scale));
//    }
    Model2d *m2d = getModel();
    if(m2d!=NULL){
        Model3d *m3d = m2d->getM3d();
        if(m3d!=NULL){
            MirrorPlane *mp = m3d->getMirrorPlane();
            if(mp!=NULL){
                emit(mirrorScaled(mp,scale));
            }
        }
    }
}

/* mouse events */
void Canvas2d::leftMousePressEventOfGlueMode(QMouseEvent *event)
{
    if(clickSelectObject(event) == NULL){
        startPos = event->pos();
    }else{
        startPos.setX(0);
        startPos.setY(0);
    }
}

void Canvas2d::rightMousePressEventOfGlueMode(QMouseEvent *event)
{
}

void Canvas2d::middleMousePressEventOfGlueMode(QMouseEvent *event)
{

}

void Canvas2d::leftMouseReleaseEventOfGlueMode(QMouseEvent *event)
{
    int dis = 600;
    if(startPos.x() != 0 && startPos.y() !=0){
        goalPos = event->pos();
        if(pow(goalPos.x() - startPos.x(),2) + pow(goalPos.y() - startPos.y(), 2) > dis){
            //std::cout << pow(goalPos.x() - startPos.x(),2) + pow(goalPos.y() - startPos.y(), 2) << std::endl;
            autoAnnotation();
        }
        //std::cout << startPos.x() << " " << startPos.y() << std::endl;
        //std::cout << goalPos.x() << " " << goalPos.y() << std::endl;
    }else{
        goalPos.setX(0);
        goalPos.setY(0);
        //std::cout << startPos.x() << " " << startPos.y() << std::endl;
        //std::cout << goalPos.x() << " " << goalPos.y() << std::endl;
    }
}

void Canvas2d::rightMouseReleaseEventOfGlueMode(QMouseEvent *event)
{

}

void Canvas2d::middleMouseReleaseEventOfGlueMode(QMouseEvent *event)
{

}

void Canvas2d::leftMouseMoveEventOfGlueMode(QMouseEvent *event)
{

}

void Canvas2d::rightMouseMoveEventOfGlueMode(QMouseEvent *event)
{

}

void Canvas2d::middleMouseMoveEventOfGlueMode(QMouseEvent *event)
{
    QVector2D mouseMove(event->pos().x()-lastPos.x(), event->pos().y()-lastPos.y());
    for(int i = 0; i<annotationsSelected.size(); ++i){
        annotationsSelected.at(i)->dragEvent(mouseMove);
    }
}

void Canvas2d::mouseDoubleClickEventOfGlueMode(QMouseEvent *event)
{

}
/* --- Selection --- */

GLObject *Canvas2d::clickSelectObject(QMouseEvent *event)
{
//    qDebug()<<"Cnavas2d::clickSelectObject";
    GLObject *obj = NULL;
    if((obj = clickSelectVertex(event)) == NULL){

        if((obj = clickSelectAnnotation(event) )== NULL){

            if((obj = clickSelectHalfedge2(event))==NULL){
                if(!isShiftKeyBeenPressing){
                    unselectAll();
                }
//                if((obj = clickSelectFace(event)) == NULL){}
            }
        }
    }
    return obj;
}

Vertex2d * Canvas2d::clickSelectVertex(QMouseEvent *event){

    Vertex2d *v = vertexAtPosition(event->pos());
    if(!isShiftKeyBeenPressing){
        unselectAllVertices();
    }
    if(v!=NULL){
        if(isSelected(v)){
            unselectVertex(v);
        }else{
            selectVertex(v);
        }
    }
    return v;
}

Halfedge2d *Canvas2d::clickSelectHalfedge(QMouseEvent *event)
{

    Halfedge2d* he = halfedgeAtPosition(event->pos());
   // he->vertex->setPosition(0,0);
    if(!isShiftKeyBeenPressing){
        unselectAllHalfedges();
    }
    if(he!=NULL){
        if(isSelected(he)){
            unselectHalfedge(he);
        }else{
            selectHalfedge(he);
        }
    }
    return he;
}
// 9/14
Halfedge2d *Canvas2d::clickSelectHalfedge2(QMouseEvent *event){
    QVector<Halfedge2d*>hes = allHalfedgesAtPosition(event->pos());
    Halfedge2d *he = NULL;
    if(!hes.empty()){
        if(hes.size()==1){
            he = hes.at(0);
        }else if(hes.size()>1){
            // set hes(i) if hes(i)->face->halfedges are not selected
            for(int i = 0; i < hes.size(); ++i){
                Face2d *f = hes.at(i)->face;
                Halfedge2d *he_f = f->halfedge;
                bool selectFlag = true;
                do{
                    if(isSelected(he_f)){
                        selectFlag = false;
                        break;
                    }
                }while( (he_f = he_f->next) != f->halfedge);

                if(selectFlag){
                    he = hes.at(i);
                    break;
                }
            }
        }
        if(he != NULL){
            if(isSelected(he)){
                unselectHalfedge(he);
                unselectFace(he->face);//-------------------------------------------------------TEST
            }else{
                if(!isShiftKeyBeenPressing){
                    unselectAllHalfedges();
                    unselectAllFaces();//-------------------------------------------------------TEST
                }
                selectHalfedge(he);
                selectFace(he->face);//-------------------------------------------------------TEST
            }
        }
    }
    return he;
}

Face2d *Canvas2d::clickSelectFace(QMouseEvent *event)
{

    Face2d *f = faceAtPosition(event->pos());
    if(!isShiftKeyBeenPressing){
        unselectAllFaces();
    }
    if(f!=NULL){
        if(isSelected(f)){
            unselectFace(f);
        }else{
            selectFace(f);
        }
    }
    return f;
}

Annotation *Canvas2d::clickSelectAnnotation(QMouseEvent *event)
{

    Annotation *a = annotationAtPosition(event->pos());
    if(!isShiftKeyBeenPressing){
        unselectAllAnnotations();
    }
    if(a!=NULL){
        if(isSelected(a)){
            unselectAnnotation(a);
        }else{
            selectAnnotaiton(a);
        }
    }
    return a;
}

void Canvas2d::selectVertex(Vertex2d *v)
{
    if(isSelected(v))return;
    v->setColor(model->getSelectColor());
    verticesSelected.push_back(v);
    emit(vertexSelected(v));
}

void Canvas2d::selectHalfedge(Halfedge2d *he)
{
    if(isSelected(he))return;
    he->setColor(model->getSelectColor());
    he->setSize(model->getLineWidth()+2);
    halfedgesSelected.push_back(he);
    he->setIsSelected(true);
    emit(halfedgeSelected(he));
}

void Canvas2d::selectFace(Face2d *f)
{
    if(isSelected(f))return;
    f->setColor(model->getSelectedFaceColor());
    facesSelected.push_back(f);
    emit(faceSelected(f));
}

void Canvas2d::selectAnnotaiton(Annotation *a)
{
    if(isSelected(a))return;
    a->setSize(a->getSize()+2);
    annotationsSelected.push_back(a);
}

void Canvas2d::unselectVertex(Vertex2d *v)
{
    if(!isSelected(v))return;
    v->setColor(model->getVertexColor());
    verticesSelected.removeOne(v);
    emit(vertexUnselected(v));
}

void Canvas2d::unselectHalfedge(Halfedge2d *he)
{
    if(!isSelected(he))return;
    he->setColor(model->getHalfedgeColor());
    he->setSize(model->getLineWidth());
    halfedgesSelected.removeOne(he);
    he->setIsSelected(false);
    emit(halfedgeUnselected(he));
}

void Canvas2d::unselectFace(Face2d *f)
{
    if(!isSelected(f))return;
    f->setColor(model->getFaceColor());
    facesSelected.removeOne(f);
    emit(faceUnselected(f));
}

void Canvas2d::unselectAnnotation(Annotation *a)
{
    if(!isSelected(a))return;
    a->setSize(a->getSize()-2);
    annotationsSelected.removeOne(a);
}

void Canvas2d::unselectAll()
{
    QVector<Vertex2d *>vs  =verticesSelected;
    QVector<Halfedge2d *>hs = halfedgesSelected;
    QVector<Face2d *>fs = facesSelected;
    QVector<Annotation *>as = annotationsSelected;
    for(int i = 0; i<vs.size(); ++i){
        unselectVertex(vs.at(i));
    }
    for(int i = 0; i<hs.size(); ++i){
        unselectHalfedge(hs.at(i));
    }
    for(int i = 0; i<fs.size(); ++i){
        unselectFace(fs.at(i));
    }
    for(int i  = 0; i<as.size(); ++i){
        unselectAnnotation(as.at(i));
    }
}

void Canvas2d::unselectAllVertices()
{
    QVector<Vertex2d *>vs  =verticesSelected;
    for(int i = 0; i<vs.size(); ++i){
        unselectVertex(vs.at(i));
    }

}

void Canvas2d::unselectAllHalfedges()
{
    QVector<Halfedge2d *>hs = halfedgesSelected;
    for(int i = 0; i<hs.size(); ++i){
        unselectHalfedge(hs.at(i));
    }
}

void Canvas2d::unselectAllFaces()
{
    QVector<Face2d *>fs = facesSelected;
    for(int i = 0; i<fs.size(); ++i){
        unselectFace(fs.at(i));
    }
}

void Canvas2d::unselectAllAnnotations()
{
    QVector<Annotation *>as = annotationsSelected;
    for(int i  = 0; i<as.size(); ++i){
        unselectAnnotation(as.at(i));
    }
}
bool Canvas2d::isSelected(Vertex2d *v)
{
    return verticesSelected.indexOf(v)!=-1;
}

bool Canvas2d::isSelected(Halfedge2d *he)
{
    return halfedgesSelected.indexOf(he)!=-1;
}

bool Canvas2d::isSelected(Face2d *f)
{
    return facesSelected.indexOf(f)!=-1;
}

bool Canvas2d::isSelected(Annotation *a)
{
    return annotationsSelected.indexOf(a)!=-1;
}
void Canvas2d::debugPrintSlection()
{
    // he
    qDebug()<<"halfedgesSlected.size() =  "<<halfedgesSelected.size();
    for(int i = 0; i<halfedgesSelected.size(); ++i){
        halfedgesSelected.at(i)->face;
    }
    // face
    qDebug()<<"facesSelected.size() = "<<facesSelected.size();
    for(int i = 0; i<facesSelected.size(); ++i){
        facesSelected.at(i)->halfedge;
    }
}
/* ---  snap --- */
QPoint  Canvas2d::snapPos(QPoint pos)
{
    int  x = (pos.x()/dGrid)*dGrid+((2*(pos.x()%dGrid))/dGrid)*dGrid;
    int  y = (pos.y()/dGrid)*dGrid+((2*(pos.y()%dGrid))/dGrid)*dGrid;
    return QPoint(x, y);
}

QVector<Annotation *> *Canvas2d::createAnnotationList()
{
    annotations = new QVector<Annotation*>();
    addAnnotationList(annotations);
    return annotations;
}

void Canvas2d::addAnnotationList(QVector<Annotation *> *annotationList)
{
    annotationListList.push_back(annotationList);
}

void Canvas2d::setAnnotationList(QVector<Annotation *> *annotationList)
{
    annotations = annotationList;
}



/* --- Annotation --- */
void Canvas2d::drawGlueEdges()
{
//    qDebug()<<"drawGlueEdges";

    for(int i = 0; i < glueEdges.size(); ++i)
    {
        Halfedge2d *he = glueEdges.at(i);
        if(isSelected(he)){
            glLineWidth(3);
            glColor3d(0.9, 0.3, 0.3);
        }else{
            glLineWidth(2);
            glColor3d(0.9, 0.6, 0.6);
        }
//        he->draw();
        glBegin(GL_LINE_STRIP);
        glVertex2d(he->vertex->x, he->vertex->y);
        glVertex2d(he->vertex_next->x, he->vertex_next->y);
        glEnd();
        //glueEdges.at(i)->debugPrint();
    }
}

void Canvas2d::drawAnnotations()//only use picking
{

    for(int i  = 0; i  < annotations->size(); ++i){
        glLoadName(i);
        annotations->at(i)->draw();
    }
    int N = annotations->size();
    for(int i = 0; i < mirrorConnections.size(); ++i){
        glLoadName(i + N);
        mirrorConnections.at(i)->draw();
    }

}

Annotation *Canvas2d::annotationAtPosition(const QPoint &pos)
{
    const int MaxSize = 512;
    const float range = 10;
    GLuint buffer[MaxSize];
    GLint viewport[4];
    glGetIntegerv(GL_VIEWPORT, viewport);
    glSelectBuffer(MaxSize, buffer);
    glRenderMode(GL_SELECT);
    glInitNames();
    glPushName(-1);
    glMatrixMode(GL_PROJECTION);
    glPushMatrix();
    glLoadIdentity();
    gluPickMatrix(GLdouble(pos.x()), viewport[3]-GLdouble(pos.y()), range, range, viewport);
    glOrtho(viewport[0], viewport[2], viewport[3], viewport[1], -1, 1);
    drawAnnotations();
    glMatrixMode(GL_PROJECTION);
    glPopMatrix();
    int hits = glRenderMode(GL_RENDER);
    if(hits<=0){
        return NULL;
    }
    const int N = annotations->size();
    int i = buffer[3];
    return (i<N)? annotations->at(i):mirrorConnections.at(i-N);
}
void Canvas2d::addAnnotation(Annotation *a)
{
    if(annotations->indexOf(a)==-1){
//        a->setSize(6);//20170323//20170427
        setAnnotationSize(a, getSizeOfAnnotations());
        annotations->push_back(a);
        /* check face order */
        if(!model->areFacesOverlappingWithNoError(annotations)){
            model->fixFaceOrders(annotations);
        }
        /* check face normal connection */
        if(!model->areFacesConnectedWithNoError(annotations)){
            model->fixFaceNormals(annotations);
//            qDebug()<<"model->areFacesConnectedWithNoError(annotations)"<<model->areFacesConnectedWithNoError(annotations);
        }
    }else{
        qDebug()<<"Canvas2d::addAnnotation: annotations.indexOf(a)!=-1";
    }
}

void Canvas2d::removeAnnotation(Annotation *a)
{
    annotations->removeOne(a);
}

void Canvas2d::deleteAnnotation(Annotation *a)
{
    removeAnnotation(a);
    unselectAnnotation(a);
    delete a;
}

void Canvas2d::setAnnotationSize(Annotation *a, const int size){
    a->setSize(size);
}

void Canvas2d::setAllAnnotationSize(const int size){
    for(int i = 0; i < annotations->size(); ++i){
        Annotation *a = annotations->at(i);
        setAnnotationSize(a, size);
    }
}

int Canvas2d::getSizeOfAnnotations() const
{
    return sizeOfAnnotations;
}

void Canvas2d::setSizeOfAnnotations(int value)
{
    sizeOfAnnotations = value;
    if(annotations->size()>0){
        setAllAnnotationSize(sizeOfAnnotations);
    }
}



/* --- GlueEdgeAnnotation --- */
GlueEdgeAnnotation *Canvas2d::createGlueEdgeAnnotation(Halfedge2d *he)
{
    GlueEdgeAnnotation *a =  new GlueEdgeAnnotation(he, this);
    addAnnotation(a);
    connect(a, SIGNAL(deleted(Halfedge2d*)), this, SIGNAL(glueEdgeRemoved(Halfedge2d*)));
    connect(a, SIGNAL(dChanged(Halfedge2d*,double)), this, SIGNAL(compressGlueBridgeSignal(Halfedge2d*,double)));
    emit(glueEdgeAdded(a->getHe()));
    //!!!
    a->dragEvent(QVector2D(1,0));
    return a;
}

/* --- InsiedReverseFoldAnnotation --- */
InsideReverseFoldAnnotation *Canvas2d::createInsideReverseFold(Halfedge2d *he1, Halfedge2d *he2)
{
    unselectAll();
//    // check normals
//    if((he1->face->isFront()&&he2->face->isFront())||(!he1->face->isFront()&&!he2->face->isFront())){
//        // need to flip
//        if(he2->face->isNormalFixed()){
//            if(he1->face->isNormalFixed()){
//                qDebug()<<"Canvas2d::createInsideReverseFold: cant create";
//                return NULL;
//            }else{
//                he1->face->flip2DAnd3D();
//            }
//        }else{
//            he2->face->flip2DAnd3D();
//        }
//    }
    // Arrow?
    InsideReverseFoldAnnotation *a = new InsideReverseFoldAnnotation(he1, he2, this);
    addAnnotation(a);
    connect(a, SIGNAL(deleted(Halfedge2d*,Halfedge2d*)), this, SIGNAL(insideReverseFoldRemoved(Halfedge2d*,Halfedge2d*)));
    emit(insideReverseFoldAdded(a->getHe1(), a->getHe2()));
    // Gluec
//    QVector<Halfedge2d*> ghes = a->getGlueHalfedge2ds();
//    for(int i = 0; i < ghes.size(); ++i){
//        createGlueEdgeAnnotation(ghes.at(i));
//    }

    return a;
}

PleatFoldAnnotation *Canvas2d::createPleatFoldAnnotation(Halfedge2d *he1, Halfedge2d *he2)
{
    unselectAll();
    PleatFoldAnnotation *a = new PleatFoldAnnotation(he1, he2, this);
    addAnnotation(a);
    connect(a, SIGNAL(deleted(Halfedge2d*,Halfedge2d*)), this, SIGNAL(pleatFoldRemoved(Halfedge2d*,Halfedge2d*)));
    emit(pleatFoldAdded(a->getHe1(), a->getHe2()));
    return a;
}

PleatFoldAnnotation *Canvas2d::createPleatFoldAnnotation(Face2d *f1, Face2d *f2)
{
    unselectAll();
    static const double eps = 1e-6;
    Halfedge2d *pair1 = NULL, *pair2 = NULL;
    Halfedge2d *he1 = f1->halfedge;
    do{
        Halfedge2d *he2 = f2->halfedge;
        do{
            if((he1->getSV2D()-he2->getEV2D()).length()<eps&&(he1->getEV2D()-he2->getSV2D()).length()<eps){
                pair1 = he1;
                pair2 = he2;
            }
            he2 = he2->next;
        }while(he2 != f2->halfedge);
        he1 = he1->next;
    }while(he1!=f1->halfedge);
    if(pair1 == NULL || pair2 == NULL){
        return NULL;
    }
    return createPleatFoldAnnotation(pair1, pair2);
}

TurnFoldAnnotation *Canvas2d::createTurnFoldAnnotation(Halfedge2d *he1, Halfedge2d *he2)
{
    unselectAll();
    TurnFoldAnnotation *a = new TurnFoldAnnotation(he1, he2);
    addAnnotation(a);
    connect(a, SIGNAL(deleted(Halfedge2d*,Halfedge2d*)), this, SIGNAL(turnFoldRemoved(Halfedge2d*,Halfedge2d*)));
    emit(turnFoldAdded(a->getHe1(), a->getHe2()));
    return a;
}

OutsideReverseFoldAnnotation *Canvas2d::createOutsideReverseFoldAnnotation(Halfedge2d *he1, Halfedge2d *he2)
{
    unselectAll();
//    // check normals
//    if((he1->face->isFront()&&he2->face->isFront())||(!he1->face->isFront()&&!he2->face->isFront())){
//        // need to flip
//        if(he2->face->isNormalFixed()){
//            if(he1->face->isNormalFixed()){
//                qDebug()<<"Canvas2d::createInsideReverseFold: cant create";
//                return NULL;
//            }else{
//                he1->face->flip2DAnd3D();
//            }
//        }else{
//            he2->face->flip2DAnd3D();
//        }
//    }
    // create
    OutsideReverseFoldAnnotation *a = new OutsideReverseFoldAnnotation(he1, he2);
    addAnnotation(a);
    connect(a, SIGNAL(deleted(Halfedge2d*,Halfedge2d*)), this, SIGNAL(outsideReverseFoldRemoved(Halfedge2d*,Halfedge2d*)));
//    createTurnFoldAnnotation(he1, he2);
//    createGlueEdgeAnnotation(a->getGlueHalfedge2ds().at(0));
//    emit(outsideReverseFoldAdded(a->getHe1(), a->getHe2()));
    emit(insideReverseFoldAdded(a->getHe1(), a->getHe2()));
    return a;
}

SinkFoldAnnotation *Canvas2d::createSinkFoldAnnotation(Halfedge2d *he)
{
    unselectAll();
    SinkFoldAnnotation *a = new SinkFoldAnnotation(he);
    addAnnotation(a);
    connect(a, SIGNAL(deleted(Halfedge2d*)), this, SIGNAL(sinkFoldRemoved(Halfedge2d*)));
    emit(sinkFoldAdded(a->getHe()));
    return a;
}

double powOfLengthBeteweenVertices(Vertex2d *v1, Vertex2d *v2){
    return (v1->x-v2->x)*(v1->x-v2->x)+(v1->y-v2->y)*(v1->y-v2->y);
}

PigsLegAnnotation *Canvas2d::createPigsLegAnnotation(Halfedge2d *he1, Halfedge2d *he2, Halfedge2d *he3)
{
    unselectAll();
    if(he1->getFace()->getMirrorTimes()==1){
        canvas3d->addMirrorFace(he1->getFace()->getFace3d());
        he1->getFace()->setMirrorTimes(2);
    }
    PigsLegAnnotation *a = new PigsLegAnnotation(he1, he2, he3);
    addAnnotation(a);
    connect(a, SIGNAL(deleted(Halfedge2d*,Halfedge2d*,Halfedge2d*)), this, SIGNAL(pigsLegRemoved(Halfedge2d*,Halfedge2d*,Halfedge2d*)));
    emit(pigsLegAdded(a->getHe1(), a->getHe2(), a->getHe3()));

    Vertex2d *va = he1->vertex, *vb = he1->vertex_next, *vc = he3->vertex, *vd = he3->vertex_next;
    Vertex2d *vShare=NULL;
    static const float eps = 2;
    if(powOfLengthBeteweenVertices(va,vc)<eps)
        vShare=va;
    else if(powOfLengthBeteweenVertices(vb,vc)<eps)
        vShare=vb;
    else if(powOfLengthBeteweenVertices(va,vd)<eps)
        vShare=va;
    else if(powOfLengthBeteweenVertices(vb,vd)<eps)
        vShare = vb;
    if(vShare==NULL)qDebug()<<"Canvas2d::createPigsLegAnnotation: vShare ==NULL ";
    Halfedge2d *glueEdge = he1;
    do{
        if(glueEdge->vertex == vShare){
            break;
        }
    }while( (glueEdge = glueEdge->next )!= he1);
    //if(glueEdge!=he1)createGlueEdgeAnnotation(glueEdge);
    return a;
}

OrikomiEdgeAnnotation *Canvas2d::createOrikomiEdgeAnnotation(Halfedge2d *he)
{
    OrikomiEdgeAnnotation *a = new OrikomiEdgeAnnotation(he);
    addAnnotation(a);
    connect(a, SIGNAL(deleted(Halfedge2d*)), this, SIGNAL(orikomiEdgeRemoved(Halfedge2d*)));
    emit(orikomiEdgeAdded(a->getHe()));
    return a;
}

Annotation *Canvas2d::createAnnotation(int annotationName, QVector<Halfedge2d *> hes)
{
    Annotation *a = (this->*createAnnotationFunctions.at(annotationName))(hes);
    return a;
}

Annotation *Canvas2d::createGlueEdgeAnnotationByVector(QVector<Halfedge2d *> hes)
{
//    return new GlueEdgeAnnotation(hes.first());
    return createGlueEdgeAnnotation(hes.first());
}

Annotation *Canvas2d::createInsideReverseFoldAnnotationByVector(QVector<Halfedge2d *> hes)
{
//    return new InsideReverseFoldAnnotation(hes.at(0), hes.at(1));
    return createInsideReverseFold(hes.at(0), hes.at(1));
}

Annotation *Canvas2d::createPleatFoldAnnotationByVector(QVector<Halfedge2d *> hes)
{
//    return new PleatFoldAnnotation(hes.at(0), hes.at(1));
    return createPleatFoldAnnotation(hes.at(0), hes.at(1));
}

Annotation *Canvas2d::createOutsideReverseFoldAnnotationByVector(QVector<Halfedge2d *> hes)
{
//    return new OutsideReverseFoldAnnotation(hes.at(0), hes.at(1));
    return createOutsideReverseFoldAnnotation(hes.at(0), hes.at(1));
}

Annotation *Canvas2d::createSinkFoldAnnotationByVector(QVector<Halfedge2d *> hes)
{
//    return new SinkFoldAnnotation(hes.at(0));
    return createSinkFoldAnnotation(hes.first());
}

Annotation *Canvas2d::createPigsLegAnnotationByVector(QVector<Halfedge2d *> hes)
{
//    return new PigsLegAnnotation(hes.at(0), hes.at(1), hes.at(2));
    return createPigsLegAnnotation(hes.at(0), hes.at(1), hes.at(2));
}

Annotation *Canvas2d::createOrikomiEdgeAnnotationByVector(QVector<Halfedge2d *> hes)
{
//    return new OrikomiEdgeAnnotation(hes.first());
    return createOrikomiEdgeAnnotation(hes.first());
}

void Canvas2d::initCreateAnnotationFunctions()
{
    createAnnotationFunctions<<&Canvas2d::createGlueEdgeAnnotationByVector
                            <<&Canvas2d::createInsideReverseFoldAnnotationByVector
                           <<&Canvas2d::createPleatFoldAnnotationByVector
                          <<&Canvas2d::createOutsideReverseFoldAnnotationByVector
                         <<&Canvas2d::createSinkFoldAnnotationByVector
                        <<&Canvas2d::createPigsLegAnnotationByVector
                       <<&Canvas2d::createOrikomiEdgeAnnotationByVector;
}

MirrorConnectAnnotation *Canvas2d::createMirrorConnection(QVector<Halfedge2d *> hes)
{
    MirrorConnectAnnotation *a = new MirrorConnectAnnotation(hes, this);
    return a;
}

void Canvas2d::drawMirrorConnectAnnotations()
{
    for(int i = 0; i < getMirrorConnections().size(); ++i){
        MirrorConnectAnnotation *a = getMirrorConnections().at(i);
        a->draw();
    }
}
/* 170307 trash model */
void Canvas2d::trashCurrentModel()
{
    Model2d *currentModel2d = getModel();
    Model3d *currentModel3d = canvas3d->getModel();
    QVector<Annotation*> *currentAnnotations = getAnnotations();

    //create new empty model 2d & 3d
    if(models.size()==1){
        createPairModels();
    }else{
        changeModels();
    }
    //remove current models
    models.removeOne(currentModel2d);
    canvas3d->models.removeOne(currentModel3d);
    annotationListList.removeOne(currentAnnotations);

    //push trash box
    trashedModels.push_back(currentModel2d);
    trashedAnnotations.push_back(currentAnnotations);
}

QVector<MirrorConnectAnnotation *> Canvas2d::getMirrorConnections() const
{
    return mirrorConnections;
}

void Canvas2d::setMirrorConnections(const QVector<MirrorConnectAnnotation *> &value)
{
    mirrorConnections = value;
}

void Canvas2d::addMirrorConnection(MirrorConnectAnnotation *value)
{
//    value->setSize(6);
    setAnnotationSize(value, getSizeOfAnnotations());
    mirrorConnections.push_back(value);
}



bool Canvas2d::getIsBranch() const
{
    return isBranch;
}

void Canvas2d::setIsBranch(bool value)
{
    isBranch = value;
}

int Canvas2d::getCurrentLayerNumber() const
{
    return currentLayerNumber;
}

void Canvas2d::setCurrentLayerNumber(int value)
{
    currentLayerNumber = value;
}

/* --- Slot --- */
void Canvas2d::slotKeyPressEvent(QKeyEvent *event)
{
    keyPressEvent(event);
}

void Canvas2d::slotKeyReleaseEvent(QKeyEvent *event)
{
    keyReleaseEvent(event);
}

void Canvas2d::selectVertexSync3D(Vertex3d *v3d)
{

}

void Canvas2d::selectHalfedgeSync3D(Halfedge3d *he3d)
{

}

void Canvas2d::selectFaceSync3D(Face3d *f3d)
{

}

void Canvas2d::unselectVertexSync3D(Vertex3d *v3d)
{

}

void Canvas2d::unselectHalfedgeSync3D(Halfedge3d *he3d)
{

}

void Canvas2d::unselectFaceSync3D(Face3d *f3d)
{

}
