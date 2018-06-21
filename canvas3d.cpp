#pragma once
#include "canvas2d.h"
#include "canvas3d.h"
#include "vertex3d.h"
#include "vertex2d.h"
#include "halfedge3d.h"
#include "halfedge2d.h"
#include "face3d.h"
#include "face2d.h"
#include "model3d.h"
#include "segment2d.h"
#include <Windows.h>
#include <gl/GLU.h>
#include <QMouseEvent>
#include <QOpenGLFunctions>
#include <QDebug>
#include <QMatrix3x3>
#include <cstdio>
#include <QVector4D>
#include <QMatrix4x4>
#include <QtMath>
#include <QPair>
#include "mirrorplane.h"
#include "annotation.h"

Canvas3d::Canvas3d(QWidget *parent)
    :CanvasBase(parent)
{
    canvas2d = NULL;
    model=NULL;// 170228 NEED
    createModel();
    lastVertex = NULL;
    firstHalfedge = NULL;
    lastHalfedge = NULL;
    setIsDebugMode(false);
    isDrawVertex = false;//20170323
    setTexture(NULL);

}

Model3d *Canvas3d::createModel(){

    setModel(new Model3d(this));
    connect(model, SIGNAL(updated()), this, SLOT(update()));

    addModel(model);
    return model;
}

void Canvas3d::setCanvas2d(Canvas2d *c)
{
    canvas2d = c;
}

Canvas2d *Canvas3d::getCanvas2d()
{
    return canvas2d;
}
/* --- protected --- */
void Canvas3d::initializeGL()
{

    camera = new MyCamera3d(width()/2, height()/2, 400*1.25, width()/2, height()/2, 0);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    glClearColor(1, 1, 1, 1);
    glEnable(GL_DEPTH_TEST);
    glEnable(GL_NORMALIZE);
    glEnable(GL_LIGHTING);
    glEnable(GL_LIGHT0);
    static GLfloat lightPosition[4] = {0,0,-1,1 };
    static GLfloat lightDiffuse[3]  = { 1.0, 1.0, 1.0  };
    static GLfloat lightAmbient[3]  = {0.25,0.25,0.25 };
    static GLfloat lightSpecular[3] = { 1.0, 1.0, 1.0  };
    glLightfv(GL_LIGHT0, GL_POSITION, lightPosition);
//    glLightfv(GL_LIGHT0, GL_DIFFUSE,  lightDiffuse);
    glLightfv(GL_LIGHT0, GL_AMBIENT,  lightAmbient);
//    glLightfv(GL_LIGHT0, GL_SPECULAR, lightSpecular);
//    setupTexture();
}

void Canvas3d::resizeGL(int w, int h)
{
    glViewport(0, 0, w, h);
    viewport[0]=0;viewport[1]=0;viewport[2]=w;viewport[3]=h;
    glMatrixMode(GL_PROJECTION);
    camera->projection(w, h);
}

void Canvas3d::paintGL()
{
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    glLoadIdentity();

    camera->projection(viewport[2], viewport[3]);

    draw();
}
/* --- mouse --- */
void Canvas3d::mousePressEvent(QMouseEvent *event)
{
    if(event->button() == Qt::LeftButton){
        leftMousePressEvent(event);
    }else if(event->button() == Qt::MiddleButton){
        middleMousePressEvent(event);
    }else if(event->button() == Qt::RightButton){
        rightMousePressEvent(event);
    }
    lastPos = event->pos();
    update();
}

void Canvas3d::mouseMoveEvent(QMouseEvent *event)
{
    if(event->buttons() & Qt::LeftButton){
        leftMouseMoveEvent(event);
    }else if(event->buttons()&Qt::MiddleButton){
        middleMouseMoveEvent(event);
    }else if(event->buttons()&Qt::RightButton){
        rightMouseMoveEvent(event);
    }
    lastPos = event->pos();
    update();
}

void Canvas3d::mouseReleaseEvent(QMouseEvent *event)
{
   if(event->button()&Qt::LeftButton){
       leftMouseReleaseEvent(event);
   }else if(event->button()&Qt::RightButton){
       rightMouseReleaseEvent(event);
    }
   lastPos = event->pos();
   update();
}

void Canvas3d::mouseDoubleClickEvent(QMouseEvent *event)
{

}

/* Left Mouse Button Event */
void Canvas3d::leftMousePressEvent(QMouseEvent *event){
    /* Select Object */
    unselectAll();
    Vertex3d *vertexOnMouse = NULL;
    Halfedge3d *halfedgeOnMouse = NULL;
    Face3d *faceOnMouse = NULL;
//    qDebug()<<"selectMode = "<<selectMode;
    switch(selectMode)
    {
    case ALL:
        vertexOnMouse = vertexAtPosition(event->pos());
        halfedgeOnMouse = halfedgeAtPosition(event->pos());
        faceOnMouse = faceAtPosition(event->pos());
        break;
    case VERTEX:
        vertexOnMouse = vertexAtPosition(event->pos());
        break;
    case HALFEDGE:
        halfedgeOnMouse = halfedgeAtPosition(event->pos());
        break;
    case FACE:
        faceOnMouse = faceAtPosition(event->pos());
        break;
    }
    if(vertexOnMouse!=NULL){
        selectVertex(vertexOnMouse);

    }else if(halfedgeOnMouse!=NULL){
        selectHalfedge(halfedgeOnMouse);

        /*test*/
//        qDebug()<<"id = "<<halfedgeOnMouse->getId();
    }else if(faceOnMouse!=NULL){
        selectFace(faceOnMouse);

    }

}
void Canvas3d::leftMouseMoveEvent(QMouseEvent *event){//NEED TO FIX

    QVector2D vec((event->pos()-lastPos).x(), (event->pos()-lastPos).y());
    QVector4D vecZ(0,0, 1, 1);
    QMatrix4x4 R = camera->getRT();

    QVector4D tmpVec = R*vecZ;
    QVector2D vecLine(tmpVec.x(), tmpVec.y());
    vecLine.normalize();

    double dz =vec.dotProduct(vec, vecLine);

    /* vertex move */
    for(int i = 0; i<verticesSelected.size(); ++i){
        verticesSelected.at(i)->movePosition(QVector3D(0, 0, dz));
    }
    /* halfedgemove */

    /* face move -> model move*/
    if(facesSelected.size()>0){
        model->moveZ(dz);
    }
}
void Canvas3d::leftMouseReleaseEvent(QMouseEvent *event)
{
}

/* Middle Mouse Button Event */
void Canvas3d::middleMousePressEvent(QMouseEvent *event){
//    if(halfedgesSelected.size()!=1){
//        return;
//    }
//    Halfedge3d *he = halfedgeAtPosition(event->pos());
//    if(he!=NULL){
////        pairingHalfedges(he, halfedgesSelected.front());
//        createBridge(he, halfedgesSelected.front());
//        unselectAll();
//    }
    if(models.size()==0)return;
    const int id = pickingModel(event->pos());
    if(id == -1)return;
    Model3d *m = models.at(id);
    if(m==NULL){
        qDebug()<<"picked Model3d *m is NULL";
        return;
    }
    setModel(m);
}

void Canvas3d::middleMouseMoveEvent(QMouseEvent *event){

}

void Canvas3d::middleMouseReleaseEvent(QMouseEvent *event){

}


/* --- Right Mouse Button Event --- */
void Canvas3d::rightMousePressEvent(QMouseEvent *event)
{
    camera->setCV(event->pos().x(), event->pos().y());
    //qDebug()<<event->pos();
}
void Canvas3d::rightMouseMoveEvent(QMouseEvent *event)
{
    camera->updateRotation(event->pos().x(), event->pos().y());

}
void Canvas3d::rightMouseReleaseEvent(QMouseEvent *event)
{
    camera->endRotation();
    QMatrix4x4 R = camera->getRT();
//    qDebug()<<R;
//    qDebug()<<event->pos();
}
/* --- Mouse Wheel Event --- */
void Canvas3d::wheelEvent(QWheelEvent *wEvent)
{
    int d =wEvent->delta()/8;
    camera->scale(d);
    update();
}

/* --- Key Press Event --- */
void Canvas3d::keyPressEvent(QKeyEvent *event)
{
    if(event->key()==Qt::Key_S){
        //model->makeSubFace();
    }
    if(event->key()==Qt::Key_Space){
        camera->init();
    }


    static const float theta = 22.5;
    if(event->key()==Qt::Key_Right){
        model->rotateY(theta);
    }else if(event->key()==Qt::Key_Left){
        model->rotateY(-theta);
    }
    static const float dScale = 0.1;
//    qDebug()<<"event->key() = "<<event->key();
    if(event->key()==Qt::Key_Period){

        model->changeScale(dScale);
    }else if(event->key()==Qt::Key_Comma){
        model->changeScale(-dScale);
    }

    update();
}

void Canvas3d::enterEvent(QEvent *event)
{
//    activateWindow();
}

/* --- Create Object --- */
Vertex3d *Canvas3d::createVertex(const QVector3D &pos)
{
    Vertex3d *v = new Vertex3d(pos.x(), pos.y(), pos.z());
    return v;
}
Halfedge3d *Canvas3d::createHalfedge(Vertex3d *v1, Vertex3d *v2)
{
    Halfedge3d *he = new Halfedge3d(v1, v2);
    return he;
}
Face3d *Canvas3d::createFace(Halfedge3d *he)
{
    Face3d *f = new Face3d(he);
    return f;
}

/* --- Mouse Picking  --- */
int Canvas3d::pickingObject(const QPoint &pos, void (Model3d::*drawObject)()){

   const int MaxSize = 512;
   const float range = 5;
   GLuint buffer[MaxSize];
   glSelectBuffer(MaxSize, buffer);
   glRenderMode(GL_SELECT);

   glInitNames();
   glPushName(-1);

   glMatrixMode(GL_PROJECTION);
   glPushMatrix();
   glLoadIdentity();

   gluPickMatrix(GLdouble(pos.x()), viewport[3]-GLdouble(pos.y()), range, range, viewport);
   //camera->projection(viewport[2], viewport[3]);
   camera->perspective(viewport[2], viewport[3]);
   glMatrixMode(GL_MODELVIEW);
   glLoadIdentity();
   camera->modelview();  
   model->getMirrorPlane()->execute();
   (model->*drawObject)();
   glMatrixMode(GL_PROJECTION);
   glPopMatrix();
   int hits = glRenderMode(GL_RENDER);
//   qDebug()<<"hits = "<<hits;
   if(hits<=0)return -1;

   int iMin = 0;
   for(int i = 0; i<hits; ++i){
       if(buffer[iMin+1]>buffer[4*i+1])iMin = 4*i;
   }
   return buffer[iMin + 3];
}

int Canvas3d::pickingModel(const QPoint &pos)
{
    const int MaxSize = 512;
    const float range = 5;
    GLuint buffer[MaxSize];
    glSelectBuffer(MaxSize, buffer);
    glRenderMode(GL_SELECT);

    glInitNames();
    glPushName(-1);

    glMatrixMode(GL_PROJECTION);
    glPushMatrix();
    glLoadIdentity();

    gluPickMatrix(GLdouble(pos.x()), viewport[3]-GLdouble(pos.y()), range, range, viewport);
    //camera->projection(viewport[2], viewport[3]);
    camera->perspective(viewport[2], viewport[3]);
    glMatrixMode(GL_MODELVIEW);
    glLoadIdentity();
    camera->modelview();
    for(int i = 0; i < models.size(); ++i){
       glLoadName(i);
       Model3d* m = models.at(i);
       m->getMirrorPlane()->execute();
       m->drawFacesForPickingModel();
    }
    glMatrixMode(GL_PROJECTION);
    glPopMatrix();
    int hits = glRenderMode(GL_RENDER);
    qDebug()<<"hits = "<<hits;
    if(hits<=0)return -1;

    int iMin = 0;
    for(int i = 0; i<hits; ++i){
        if(buffer[iMin+1]>buffer[4*i+1])iMin = 4*i;
    }
    return buffer[iMin + 3];
}
Vertex3d *Canvas3d::vertexAtPosition(const QPoint &pos)
{
    if(!isDrawVertex)return NULL;
    void (Model3d::*func)() = &Model3d::drawVertices;
    int id = pickingObject(pos, func);
    if(id>=model->vertices.size())id-=model->vertices.size();//mirror
    return (id == -1 || !isDrawVertex)? NULL: model->vertices.at(id);
}
Halfedge3d *Canvas3d::halfedgeAtPosition(const QPoint &pos)
{
    if(!isDrawEdge)return NULL;
    void (Model3d::*func)() = &Model3d::drawHalfedges;
    int id = pickingObject(pos, func);
    if(id>=model->halfedges.size())id-=model->halfedges.size();//mirror
    return (id == -1 || !isDrawEdge)? NULL: model->halfedges.at(id);
}
Face3d *Canvas3d::faceAtPosition(const QPoint &pos)
{
    if(!isDrawFace)return NULL;
    void (Model3d::*func)() = &Model3d::drawFaces;
    int id = pickingObject(pos, func);
    if(id>=model->faces.size())id-=model->faces.size();//mirror
    return (id == -1 || !isDrawFace)? NULL: model->faces.at(id);
}

/* --- Drawing --- */
void Canvas3d::draw()
{
//    model->draw(isDrawVertex, isDrawEdge, isDrawFace, isDrawNormal, isDrawEdgeConnection);
    drawTest();
    if(getIsDebugMode()){
        if(model->faces.size()>0){
            glPushMatrix();
            model->drawBoundingBox();
            glPopMatrix();
        }
    }
//    drawAllModels();
    drawAllMirrorModels();
    if(isDrawAxis)drawAxis();
    if(isDrawGrid)drawGrid();
//    drawArrow();
}
void Canvas3d::drawGrid()
{
    static const int d =25;
    static const int Nx = viewport[2]/d;
    static const int Ny = viewport[3]/d;
    glLineWidth(1);
    float color = 0.7f;
    glColor3f(color, color, color);
    glDisable(GL_LIGHTING);
    glBegin(GL_LINES);
    for(int i = 0; i < Nx+1; ++i){
        glVertex3d(i*d, 0, 0);
        glVertex3d(i*d, Ny*d, 0);
    }
    for(int i = 0; i < Ny+1; ++i){
        glVertex3d(0, i*d, 0);
        glVertex3d(Nx*d, i*d, 0);
    }
    glEnd();
}
void Canvas3d::drawAxis(){
    static const int d = 50;
    glLineWidth(2);
    glDisable(GL_LIGHTING);
    static float color = 0.7f;
    glBegin(GL_LINES);
    glColor3d(color,0,0);
    glVertex3d(0,0,0);
    glVertex3d(d,0,0);
    glColor3d(0,color,0);
    glVertex3d(0,0,0);
    glVertex3d(0,d,0);
    glColor3d(0,0,color);
    glVertex3d(0,0,0);
    glVertex3d(0,0,d);
    glEnd();
}
void Canvas3d::drawCone(const int r, const float h, const float n){
    glDisable(GL_LIGHTING);
    for(int i = 0; i < n; ++i){
        float theta = 2*M_PI * i /n;
        float phi = 2*M_PI*(i+1)/n;
        glBegin(GL_POLYGON);
        glVertex3d(0, 0, h);
        glVertex3d(r*cos(theta), r*sin(theta), 0);
        glVertex3d(r*cos(phi), r*sin(phi), 0);
        glEnd();
    }
}

void Canvas3d::drawAllModels()
{
    for(int i = 0; i < models.size(); ++i){

        if(models.at(i)->faces.size()>0){
            glPushMatrix();
            models.at(i)->getMirrorPlane()->execute170218();
            models.at(i)->draw(isDrawVertex, isDrawEdge, isDrawFace, isDrawNormal, isDrawEdgeConnection);
            glPopMatrix();
            if(models.at(i)->getMirrorPlane()->getParent()!=NULL){
                glPushMatrix();
                models.at(i)->getMirrorPlane()->execute170218Reverse();

                models.at(i)->draw(isDrawVertex, isDrawEdge, isDrawFace, isDrawNormal, isDrawEdgeConnection);
                glPopMatrix();
            }
        }

        /* test */
//        if(models.at(i)->getMirrorPlane()->getParent()!=NULL){

//            double reverse[16]={1,0,0,0
//                                ,0,1,0,0
//                                ,0,0,-1,0
//                                ,0,0,0,1};
//            glPushMatrix();
//            glMultMatrixd(reverse);
//            models.at(i)->getMirrorPlane()->execute170215();
//            models.at(i)->draw(isDrawVertex, isDrawEdge, isDrawFace, isDrawNormal, isDrawEdgeConnection);
//            glPopMatrix();
//        }

    }
}
void multXYReverseMatrix(){
    double reverse[16]={1,0,0,0
                        ,0,1,0,0
                        ,0,0,-1,0
                        ,0,0,0,1};

    glMultMatrixd(reverse);
}
void convertQMatrix4x4ToDoubleStar(double *rt, QMatrix4x4 R){
    rt[0] = R(0,0); rt[1] = R(0,1); rt[2] = R(0,2); rt[3] = R(0,3);
    rt[4] = R(1,0); rt[5] = R(1,1); rt[6] = R(1,2); rt[7] = R(1,3);
    rt[8] = R(2,0); rt[9] = R(2,1); rt[10] = R(2,2); rt[11] = R(2,3);
    rt[12] = R(3,0); rt[13] = R(3,1); rt[14] = R(3,2); rt[15] = R(3,3);
}

void Canvas3d::drawAllMirrorModels()
{
//    qDebug()<<"---";
    for(int i = 0; i < models.size(); ++i){// for each models
        Model3d *m = models.at(i);
        QVector<MirrorPlane*>mirrorFamily = m->getMirrorPlane()->getMirrorFamily();
        /* rotate */

        const int N = pow(2, m->getMirrorPlane()->getAncestors().size());
//        qDebug()<<i<<", "<<N;

        for(int j = 0; j < N; ++j){//for each model mirrors
            /* calc R */
            QMatrix4x4 R;

            for(int k = 0; k < mirrorFamily.size(); ++k){
                MirrorPlane *mirror = mirrorFamily.at(k);

                bool bTest = !(j&(int)qPow(2,k));


                if(bTest){
                    /* normal rotate */
                    R = R*mirror->getSingleRotateMatrix();

                }else{
                    /* reverse rotate */
                    R = R*mirror->getSingleReverseRotateMatrix();
                }

            }

            /* draw */
//            qDebug()<<R;
            double rt[16],st[16];
            convertQMatrix4x4ToDoubleStar(rt, R);
            m->getMirrorPlane()->getST(st);//should be fixed

            glPushMatrix();
            glMultMatrixd(rt);
            glMultMatrixd(st);

            m->draw(isDrawVertex, isDrawEdge, isDrawFace, isDrawNormal);
            if(getModel()==m){
//                m->drawBoundingBox();
            }
            glPopMatrix();
        }

    }
}

void Canvas3d::drawTest()
{
//    /*test*/
//    QVector4D vecZ(0,0, 1, 1);
//    QMatrix4x4 R = camera->getRT();
//    QVector4D tmpVec = R*vecZ;// keisan atteru
//    qDebug()<<tmpVec;
//    double d = 100;
//    QVector2D v(tmpVec.x(), tmpVec.y());
//    v*=d;
//    QVector2D c(this->width()/2, this->height()/2);
//    v+=c;

//    glColor3d(0, 0, 0);
//    glPointSize(5);
//    glBegin(GL_POINTS);
//    glVertex3d(v.x(), v.y(), 0);
//    glEnd();
//    glColor3d(0,0,1);
//    glLineWidth(3);
//    glBegin(GL_LINES);
//    glVertex3d(c.x(), c.y(), 0);
//    glVertex3d(v.x(), v.y(), 0);
//    glEnd();
}

void Canvas3d::drawArrow(){
    if(verticesSelected.empty()){
        return;
    }
    static const float scl = 0.035;
    float d = camera->position.z()*scl;
    float r = 0.4*d;
    float h = 1.5*d;
    static const int n = 16;

    QVector3D arrowPos(0, 0, 0);
    for(int i = 0; i < verticesSelected.size(); ++i){
        arrowPos += verticesSelected.at(i)->getQVector3D();
    }
    arrowPos/=verticesSelected.size();
    glMatrixMode(GL_MODELVIEW);
    glDisable(GL_LIGHTING);
    glPushMatrix();
    glTranslatef(arrowPos.x(), arrowPos.y(), arrowPos.z());
    glColor3d(0, 0, 1);
    glLineWidth(3);
    glBegin(GL_LINES);
    glVertex3d(0,0,0);
    glVertex3d(0,0,d);
    glEnd();
    glTranslatef(0,0,d);
    drawCone(r, h, n);
    glPopMatrix();
}

/*--- Selection --- */
void Canvas3d::selectVertex(Vertex3d *v)
{
    v->setColor(model->getSelectColor());
    v->setSize(model->getPointSize());
    verticesSelected.push_back(v);
    update();
}
void Canvas3d::selectHalfedge(Halfedge3d *he)
{
    he->setColor(model->getSelectColor());
    he->setSize(2*model->getLineWidth());
    halfedgesSelected.push_back(he);
    update();
}
void Canvas3d::selectFace(Face3d *f)
{
    if(f==NULL){
        qDebug()<<"Canvas3d::selectFace: f == NULL";
        return;
    }
//    f->setColor(model->getSelectColor());//20170326
    facesSelected.push_back(f);
    update();
}
void Canvas3d::unselectVertex(Vertex3d *v)
{
    v->setColor(model->getVertexColor());
    v->setSize(model->getPointSize());
    verticesSelected.removeOne(v);
    update();
}
void Canvas3d::unselectHalfedge(Halfedge3d *he)
{
    he->setColor(model->getHalfedgeColor());
    he->setSize(model->getLineWidth());
    halfedgesSelected.removeOne(he);
    update();
}
void Canvas3d::unselectFace(Face3d *f)
{
    f->setColor(model->getFaceColor());
    facesSelected.removeOne(f);
    update();
}
void Canvas3d::unselectAll()
{
    QVector<Vertex3d*>vs = verticesSelected;
    QVector<Halfedge3d*>hs = halfedgesSelected;
    QVector<Face3d*>fs = facesSelected;
    for(int i = 0; i<vs.size(); ++i){
        unselectVertex(vs.at(i));
    }
    for(int i = 0; i<hs.size(); ++i){
        unselectHalfedge(hs.at(i));
    }
    for(int i = 0; i<fs.size(); ++i){
        unselectFace(fs.at(i));
    }
}

bool Canvas3d::isSelected(Vertex3d *v)
{
    return verticesSelected.indexOf(v)!=-1;
}

bool Canvas3d::isSelected(Halfedge3d *he)
{
    return halfedgesSelected.indexOf(he)!=-1;
}

bool Canvas3d::isSelected(Face3d *f)
{
    return facesSelected.indexOf(f)!=-1;
}



void Canvas3d::addModel(Model3d *m)
{
    models.push_back(m);
}

bool Canvas3d::getIsDebugMode() const
{
    return isDebugMode;
}

void Canvas3d::setIsDebugMode(bool value)
{
    isDebugMode = value;
    emit(updated());
}

Model3d *Canvas3d::getModel() const
{
    return model;
}

void Canvas3d::setModel(Model3d *value)
{
    /* 170228 */
    if(model!=NULL){
        disconnect(model, SIGNAL(updated()), this, SLOT(updateAnnotationBridge()));
    }
    model = value;
    if(canvas2d!=NULL){
        if(canvas2d->getModel()!=NULL&&value->getM2d()!=NULL&&canvas2d->getModel()!=value->getM2d()){
            canvas2d->setModel(value->getM2d());
        }
    }
    /* 170228 */
    connect(model, SIGNAL(updated()), this, SLOT(updateAnnotationBridge()));

    emit(updated());
}

/* --- Halfedge Pairing --- */
void Canvas3d::pairingHalfedges(Halfedge3d *he1, Halfedge3d *he2){
    he1->setPair(he2);
}

/* --- Test Function --- */
void Canvas3d::showProjectionMatrix(){
    GLfloat m[16];
    getProjectionMatrix(m);
    qDebug()<<"ProjectionMatrix  = ";
    qDebug()<<(float)m[0]<<" "<<(float)m[1]<<" "<<(float)m[2]<<" "<<(float)m[3];
    qDebug()<<(float)m[4]<<" "<<(float)m[5]<<" "<<(float)m[6]<<" "<<(float)m[7];
    qDebug()<<(float)m[8]<<" "<<(float)m[9]<<" "<<(float)m[10]<<" "<<(float)m[11];
    qDebug()<<(float)m[12]<<" "<<(float)m[13]<<" "<<(float)m[14]<<" "<<(float)m[15];
}
void Canvas3d::showModelviewMatrix(){
    GLfloat m[16];
    getModelviewMatrix(m);
    qDebug()<<"ModelviewMatrix  = ";
    qDebug()<<(float)m[0]<<" "<<(float)m[1]<<" "<<(float)m[2]<<" "<<(float)m[3];
    qDebug()<<(float)m[4]<<" "<<(float)m[5]<<" "<<(float)m[6]<<" "<<(float)m[7];
    qDebug()<<(float)m[8]<<" "<<(float)m[9]<<" "<<(float)m[10]<<" "<<(float)m[11];
    qDebug()<<(float)m[12]<<" "<<(float)m[13]<<" "<<(float)m[14]<<" "<<(float)m[15];
}
void Canvas3d::showProjectionMatrix2(){
    GLfloat m[16];
    getProjectionMatrix2(m);
    qDebug()<<"ProjectionMatrix2  = ";
    qDebug()<<(float)m[0]<<" "<<(float)m[1]<<" "<<(float)m[2]<<" "<<(float)m[3];
    qDebug()<<(float)m[4]<<" "<<(float)m[5]<<" "<<(float)m[6]<<" "<<(float)m[7];
    qDebug()<<(float)m[8]<<" "<<(float)m[9]<<" "<<(float)m[10]<<" "<<(float)m[11];
    qDebug()<<(float)m[12]<<" "<<(float)m[13]<<" "<<(float)m[14]<<" "<<(float)m[15];
}
void Canvas3d::showModelviewMatrix2(){
    GLfloat m[16];
    getModelviewMatrix2(m);
    qDebug()<<"ModelviewMatrix 2 = ";
    qDebug()<<(float)m[0]<<" "<<(float)m[1]<<" "<<(float)m[2]<<" "<<(float)m[3];
    qDebug()<<(float)m[4]<<" "<<(float)m[5]<<" "<<(float)m[6]<<" "<<(float)m[7];
    qDebug()<<(float)m[8]<<" "<<(float)m[9]<<" "<<(float)m[10]<<" "<<(float)m[11];
    qDebug()<<(float)m[12]<<" "<<(float)m[13]<<" "<<(float)m[14]<<" "<<(float)m[15];
}
void Canvas3d::getProjectionMatrix(GLfloat *m){
    glMatrixMode(GL_PROJECTION);
    glPushMatrix();
    glLoadIdentity();
    camera->projection(viewport[2], viewport[3]);
    glGetFloatv(GL_PROJECTION_MATRIX, m);
    glPopMatrix();
}
void Canvas3d::getProjectionMatrix2(GLfloat *m){
    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();
    camera->perspective(viewport[2], viewport[3]);
    glGetFloatv(GL_PROJECTION_MATRIX, m);
}
void Canvas3d::getModelviewMatrix(GLfloat *m){
    glMatrixMode(GL_MODELVIEW);
    glPushMatrix();
    glLoadIdentity();
    glViewport(viewport[0], viewport[1], viewport[2], viewport[3]);
    camera->projection(viewport[2], viewport[3]);
    glGetFloatv(GL_MODELVIEW_MATRIX, m);
    glPopMatrix();
}
void Canvas3d::getModelviewMatrix2(GLfloat *m){
    glMatrixMode(GL_MODELVIEW);
    glPushMatrix();
    glLoadIdentity();
    camera->modelview();
    glGetFloatv(GL_MODELVIEW_MATRIX, m);
    glPopMatrix();
}
QMatrix4x4 Canvas3d::getUnkownQMatrix4x4(){
    GLfloat m[16];
    getModelviewMatrix(m);
    QMatrix4x4 M(
                m[0], m[1], m[2], m[3],
            m[4], m[5], m[6], m[7],
            m[8], m[9], m[10], m[11],
            m[12], m[13], m[14], m[15]
            );
    return M;
}
QMatrix4x4 Canvas3d::getModelviewQMatrix4x4(){
    GLfloat m[16];
    getModelviewMatrix(m);
    QMatrix4x4 M(
                m[0], m[1], m[2], m[3],
            m[4], m[5], m[6], m[7],
            m[8], m[9], m[10], m[11],
            m[12], m[13], m[14], m[15]
            );
    return M;
}
QMatrix4x4 Canvas3d::getModelviewQMatrix4x4_2(){
    GLfloat m[16];
    getModelviewMatrix2(m);
    QMatrix4x4 M(
                m[0], m[1], m[2], m[3],
            m[4], m[5], m[6], m[7],
            m[8], m[9], m[10], m[11],
            m[12], m[13], m[14], m[15]
            );
    return M.transposed();
}
QMatrix4x4 Canvas3d::getModelviewQMatrix4x4_3(){
    GLfloat m[16];
    getModelviewMatrix2(m);
    QMatrix4x4 M(
                m[0], m[1], m[2], m[3],
            m[4], m[5], m[6], m[7],
            m[8], m[9], m[10], m[11],
            -m[12], -m[13], -m[14], m[15]
            );
    return M;
}
QMatrix4x4 Canvas3d::getProjectionQMatrix4x4(){
    GLfloat m[16];
    getProjectionMatrix(m);
    QMatrix4x4 M(
                m[0], m[1], m[2], m[3],
            m[4], m[5], m[6], m[7],
            m[8], m[9], m[10], m[11],
            m[12], m[13], m[14], m[15]
            );
    return M;
}
QMatrix4x4 Canvas3d::getProjectionQMatrix4x4_2(){
    GLfloat m[16];
    getProjectionMatrix2(m);
    QMatrix4x4 M(
                m[0], m[1], m[2], m[3],
            m[4], m[5], m[6], m[7],
            m[8], m[9], m[10], m[11],
            m[12], m[13], m[14], m[15]
            );
    return M.transposed();
}
QMatrix4x4 Canvas3d::getViewportQMatrix4x4(){
    return QMatrix4x4(
                viewport[2]/2, 0, 0, 0,
            0, -viewport[3]/2, 0, 0,
            0, 0, 1, 0,
            viewport[2]/2, viewport[3]/2, 0, 1
            );
}
QMatrix4x4 Canvas3d::getViewportQMatrix4x4_2(){
    return QMatrix4x4(
                viewport[2]/2, 0, 0, viewport[2]/2,
            0, -viewport[3]/2, 0, viewport[3]/2,
            0, 0, 1, 1,
            0, 0, 0, 1
            );
}
QMatrix4x4 Canvas3d::getViewportQMatrix4x4_3(){
    return QMatrix4x4(
                viewport[2]/2, 0, 0, viewport[2]/2,
            0, -viewport[3]/2, 0, viewport[3]/2,
            0, 0, 10000/9999, -10000/9999*1,
            0, 0, 0, 1
            );
}
QMatrix4x4 Canvas3d::getViewportQMatrix4x4_4(){
    return QMatrix4x4(
                1, 0, 0, viewport[2]/2,
            0, -1, 0, viewport[3]/2,
            0, 0, 10000/9999, -10000/9999*1,
            0, 0, 0, 1
            );
}
/* --- private slots --- */

/* --- public slots --- */
void Canvas3d::addFace(Face3d *face3d)
{
    model->addVertex(face3d);
    model->addHalfedge(face3d);
    model->addFace(face3d);
    face3d->checkAndReCaluculationGeometry();
    emit(faceAdded());
}
void Canvas3d::addFaceFromC2d(Face3d *face3d)
{
    // mirroring
    QVector<Face3d*>mirrorFaceGroup;
    mirrorFaceGroup<<face3d;
    Face2d *face2d = face3d->getFace2d();
    int mirrorTimes = face2d->getMirrorTimes();
    for(int i = 0; i < pow(2, mirrorTimes)-1; ++i){
        Face3d *f  = face3d->createCpy();
        if(i%2==0){
            f->flip();
        }
        mirrorFaceGroup<<f;
    }
    // setting and adding
    for(int i = 0; i  <mirrorFaceGroup.size(); ++i){
        int zOrder  = pow(2, mirrorTimes-1)-i;
        if(zOrder<=0)zOrder--;
        mirrorFaceGroup.at(i)->setZOrderValue(zOrder);
        mirrorFaceGroup.at(i)->setMirrorNext((i==mirrorFaceGroup.size()-1)? mirrorFaceGroup.at(0): mirrorFaceGroup.at(i+1));
        mirrorFaceGroup.at(i)->setFace2dAndConnectVertices2d(mirrorFaceGroup.at(0)->getFace2d());
        addFace(mirrorFaceGroup.at(i));
    }
    // connection
    for(int i = 0; i < mirrorFaceGroup.size()/2; ++i){
        Face3d *f1 = mirrorFaceGroup.at(i), *f2 = mirrorFaceGroup.at(mirrorFaceGroup.size()-1-i);
        f1->setMirrorFace(f2);
    }
    emit(faceAdded());
}
void Canvas3d::addMirrorFace(Face3d *face3d){
    if(face3d->getFace2d()->getMirrorTimes()==1){
        qDebug()<<"Canvas3d::addMirrorFace: begin";
        // mirroring
        QVector<Face3d*>mirrorFaceGroup;
        mirrorFaceGroup<<face3d;
        const int mirrorTimes = 4;
        for(int i = 0; i<2; i++){
            Face3d *f  = face3d->createCpy();
            if(i==0)f->flip();
            mirrorFaceGroup<<f;
        }
        mirrorFaceGroup<<face3d->getMirrorNext();
        qDebug()<<"MirrorFaceGroup.size() = "<<mirrorFaceGroup.size();
        // setting and adding
        for(int i = 0; i  <mirrorFaceGroup.size(); ++i){
            int zOrder  = pow(2, mirrorTimes-1)-i;
            if(zOrder<=0)zOrder--;
            mirrorFaceGroup.at(i)->setZOrderValue(zOrder);
            mirrorFaceGroup.at(i)->setMirrorNext((i==mirrorFaceGroup.size()-1)? mirrorFaceGroup.at(0): mirrorFaceGroup.at(i+1));
            mirrorFaceGroup.at(i)->setFace2dAndConnectVertices2d(mirrorFaceGroup.at(0)->getFace2d());
            if(i!=0||i!=3)addFace(mirrorFaceGroup.at(i));
        }
        // connection
        for(int i = 0; i < mirrorFaceGroup.size()/2; ++i){
            Face3d *f1 = mirrorFaceGroup.at(i), *f2 = mirrorFaceGroup.at(mirrorFaceGroup.size()-1-i);
            f1->setMirrorFace(f2);
        }
        emit(faceAdded());
    }else{
        qDebug()<<"Canvas3d::addMirrorFace: getMirrorTimes()!=1";
    }
}

void Canvas3d::slotKeyPressEvent(QKeyEvent *event)
{
    keyPressEvent(event);
}
/*--- Select Mode Slots---*/
void Canvas3d::allSelectMode()
{
    selectMode = ALL;
}
void Canvas3d::vertexSelectMode()
{
    selectMode = VERTEX;
}
void Canvas3d::halfedgeSelectMode()
{
    selectMode = HALFEDGE;
}
void Canvas3d::faceSelectMode()
{
    selectMode = FACE;
}
void Canvas3d::setDrawAxisFlag(bool flag)
{
    isDrawAxis = flag;
    update();
}
void Canvas3d::setDrawGridFlag(bool flag)
{
    isDrawGrid = flag;
    update();
}
void Canvas3d::setDrawNormalFlag(bool flag)
{
    isDrawNormal = flag;
    update();
}
void Canvas3d::setDrawEdgeConnectionFlag(bool flag)
{
    isDrawEdgeConnection = flag;
    update();
}
void Canvas3d::setDrawVertexFlag(bool flag)
{
    isDrawVertex = flag;
    update();
}
void Canvas3d::setDrawEdgeFlag(bool flag)
{
    isDrawEdge = flag;
    update();
}
void Canvas3d::setDrawFaceFlag(bool flag)
{
    isDrawFace = flag;
    update();
}

void Canvas3d::exportOBJ(QString filename){
    if(filename.isEmpty()){
        return;
    }else{
        QFile file(filename);
        if(!file.open(QIODevice::WriteOnly)){
            return;
        }
        QTextStream out(&file);

        for(int i = 0; i < model->vertices.size(); ++i){
            Vertex3d *v  = model->vertices.at(i);
            v->exportID = i+1;
            out<<"v "<<(v->x-400)/100.f<<" "<<(v->y-300)/100.f<<" "<<v->z/100.f<<endl;
        }
        /*texture*/
        for(int i =0; i < model->vertices.size(); ++i){
            Vertex3d *v  = model->vertices.at(i);
            out<<"vt "<<v->x/800.f<<" "<<-(v->y+21)/600.f<<endl;
        }
        for(int i = 0; i < model->faces.size(); ++i){
            Face3d *f  = model->faces.at(i);
            out<<"f";
            Halfedge3d *he = f->halfedge;
            do{
                out<<" "<<he->vertex->exportID<<"/"<<he->vertex->exportID;
                he = he->next;
            }while(he != f->halfedge);
            out<<endl;
        }
    }
}

void Canvas3d::exportOBJ170223(QString filename)
{
    if(filename.isEmpty()){
        return;
    }else{
        QFile file(filename);
        if(!file.open(QIODevice::WriteOnly)){
            return;
        }
        QTextStream out(&file);
        exportVertices(&out);
        exportFaces(&out);

    }
}

void Canvas3d::exportVertices(QTextStream *out)
{

    int vertexID = 1;
    for(int i = 0; i < models.size(); ++i){// for each models
        Model3d *m = models.at(i);
        QVector<MirrorPlane*>mirrorFamily = m->getMirrorPlane()->getMirrorFamily();
        /* rotate */

        const int N = pow(2, m->getMirrorPlane()->getAncestors().size());


        for(int j = 0; j < N; ++j){//for each model mirrors
            /* calc R */
            QMatrix4x4 R;

            for(int k = 0; k < mirrorFamily.size(); ++k){
                MirrorPlane *mirror = mirrorFamily.at(k);

                bool bTest = !(j&(int)qPow(2,k));

                if(bTest){
                    /* normal rotate */
                    R = mirror->getSingleRotateMatrixForExport()*R;

                }else{
                    /* reverse rotate */
                    R = mirror->getSingleReverseRotateMatrixForExport()*R;
                }

            }

            /* calc true vertices position with R & S*/
            QMatrix4x4 S = m->getMirrorPlane()->getScaleMatrix();

            QVector<Vertex3d*>vertices = m->vertices;
            for(int i = 0; i <vertices.size(); ++i){
               QVector3D p = vertices.at(i)->getVector3D();
               QVector4D p2(p.x(), p.y(), p.z(), 1);
               p2 = R*S*p2;
               p = QVector3D(p2.x(), p2.y(), p2.z());

               /* bad normalize */
               p=p-QVector3D(400, 300,0);
               p=p/400;

               *out<<"v "<<p2.x()<<" "<<p2.y()<<" "<<p2.z()<<endl;
               vertices.at(i)->idsForExport.push_back(vertexID++);
            }
        }

    }
}

void Canvas3d::exportFaces(QTextStream *out)
{
    for(int i = 0; i < models.size(); ++i){// for each models
        Model3d *m = models.at(i);
        QVector<MirrorPlane*>mirrorFamily = m->getMirrorPlane()->getMirrorFamily();
        /* rotate */

        const int N = pow(2, m->getMirrorPlane()->getAncestors().size());


        for(int j = 0; j < N; ++j){//for each model mirrors
            for(int k = 0; k<m->faces.size(); ++k){
                Face3d *f = m->faces.at(k);
                *out<<"f";
                Halfedge3d *he = f->halfedge;
                do{
                    *out<<" "<<he->vertex->idsForExport.front();
                    he->vertex->idsForExport.pop_front();
                }while((he=he->next)!=f->halfedge);
                *out<<endl;
            }
        }

    }
}

Model3d *Canvas3d::mergeModels()
{
    Model3d *newModel = new Model3d();

    return newModel;
}

/* Delete Object */
void Canvas3d::deleteFacesSelected()
{
    for(int i = 0; i < facesSelected.size(); ++i){
        model->deleteFace(facesSelected.at(i));
    }
    unselectAll();
    update();
}

/* Flip Face*/
void Canvas3d::flipFacesSelected(){
    for(int i = 0; i < facesSelected.size(); ++i ){
        facesSelected.at(i)->flip();
    }
    update();
}

/*  Bridge */
Face3d *Canvas3d::createBridge(Halfedge3d *connectedHe1, Halfedge3d *connectedHe2, int div)
{
    Face3d *f = NULL;
    if(connectedHe1==NULL||connectedHe2==NULL){
        qDebug()<<"Canvas3d::createBridge : connectedHe1==NULL||connectedHe2==NULL";
        return NULL;
    }
    if(div == 0){
        Vertex3d *v1, *v2, *v3, *v4;
        v2 = connectedHe1->vertex;
        v1 = connectedHe1->vertex_next;
        v4 = connectedHe2->vertex;
        v3 = connectedHe2->vertex_next;
        if(v1==NULL||v2==NULL||v3==NULL||v4==NULL){
            qDebug()<<"Canvas3d::createBridge : v1==NULL||v2==NULL||v3==NULL||v4==NULL";
            return NULL;
        }
        Halfedge3d *he[4];
        he[0] = new Halfedge3d(v1, v2);
        he[1] = new Halfedge3d(v2, v3);
        he[2] = new Halfedge3d(v3, v4);
        he[3] = new Halfedge3d(v4, v1);
        for(int i = 0; i<4; ++i){
            int j = (i == 3)? 0 : i+1;
            he[i]->setNext(he[j]);
        }
        f = new Face3d(he[0]);
        addBridge(f);
        connectedHe1->setPair(he[0]);
        connectedHe2->setPair(he[2]);

        //selectFace(f);//test
    }else if(div == 1){

    }
    if(f == NULL){
        qDebug()<<"f == NULL";
    }
//    qDebug()<<"*Canvas3d::createBridge : done";
    return f;
}

void Canvas3d::connectEdges(Halfedge3d *connectedHe1, Halfedge3d *connectedHe2){
    Vertex3d *v1, *v2, *v3, *v4;
    v1 = connectedHe1->vertex;
    v2 = connectedHe1->vertex_next;
    v3 = connectedHe2->vertex;
    v4 = connectedHe2->vertex_next;

    connectedHe2->vertex_next = v1;
    connectedHe2->next->vertex = v1;
    connectedHe2->vertex = v2;
    connectedHe2->prev->vertex_next = v2;

    v1->setPosition((v1->getVector3D()+v4->getVector3D())/2);
    v2->setPosition((v2->getVector3D()+v3->getVector3D())/2);

    deleteVertex(v3);
    deleteVertex(v4);
}

void Canvas3d::addBridge(Face3d *f)
{
    addFace(f);
    bridgeFaces.push_back(f);
}

void Canvas3d::deleteBridge(Face3d *f)
{
    if(f==NULL){
        qDebug()<<"Canvas3d::deleteBridge: f is NULL";
       return;
    }
    if(!isBridgeFace(f)){
        qDebug()<<"Canvas3d::deleteBridge: this is not bridge";
        selectFace(f);
        return;
    }
    bridgeFaces.removeOne(f);
    unselectAll();
    model->deleteFace(f);

}

void Canvas3d::deleteBridge(Halfedge3d *connectedHe1)
{
    deleteBridge(connectedHe1->pair->face);
}

bool Canvas3d::isBridgeFace(Face3d *f)
{
    return bridgeFaces.indexOf(f)!=-1;
}

void Canvas3d::movePleatBridges(Halfedge3d *connectedHe1, Halfedge3d *connectedHe2)
{
    static const double eps_pararell = 1e-6;
    QVector3D P, Q;
    // P
    QVector3D A = connectedHe2->prev->getSV3D();
    QVector3D u = connectedHe2->prev->getQVector3D();
    QVector3D B = connectedHe1->next->getSV3D();
    QVector3D v = connectedHe1->next->getQVector3D();
    QVector3D C = connectedHe1->next->getCenter();
    QVector3D w = -connectedHe1->getQVector3D();
    double s;

    s = (w.x()*(A.y()-C.y())-w.y()*(A.x()-C.x()))/(u.x()*w.y()-u.y()*w.x());

    P = QVector3D(s*u.x()+A.x(), s*u.y()+A.y(), A.z());
    // Q
    A = connectedHe2->next->getEV3D();
    u = -connectedHe2->next->getQVector3D();
    B = connectedHe1->prev->getEV3D();
    v = -connectedHe1->prev->getQVector3D();
    C = connectedHe1->prev->getCenter();
    w = connectedHe1->getQVector3D();

    s = (w.x()*(A.y()-C.y())-w.y()*(A.x()-C.x()))/(u.x()*w.y()-u.y()*w.x());

    Q = QVector3D(s*u.x()+A.x(), s*u.y()+A.y(), A.z());
    // setPos
    connectedHe2->vertex->setPosition(P);
    connectedHe2->vertex_next->setPosition(Q);

}
void Canvas3d::movePleatBridges170118(Halfedge3d *connectedHe1, Halfedge3d *connectedHe2)
{
    static const double eps_pararell = 1e-6;
    QVector3D P, Q;
    // P
    QVector3D A = connectedHe2->prev->getSV3D();
    QVector3D u = connectedHe2->prev->getQVector3D();
    QVector3D B = connectedHe1->next->getSV3D();
    QVector3D v = connectedHe1->next->getQVector3D();
    QVector3D C = connectedHe1->next->getCenter();
    QVector3D w = -connectedHe1->getQVector3D();
    double s;
    double tmp = fabs(QVector3D::dotProduct(u.normalized(), v.normalized())-1);
    if(tmp<eps_pararell){
        // pararell
        s = (w.x()*(A.y()-C.y())-w.y()*(A.x()-C.x()))/(u.x()*w.y()-u.y()*w.x());
    }else{
        // not pararell
        s = (v.x()*(A.y()-B.y())-v.y()*(A.x()-B.x()))/(u.x()*v.y()-u.y()*v.x());
    }
    P = QVector3D(s*u.x()+A.x(), s*u.y()+A.y(), A.z());
    // Q
    A = connectedHe2->next->getEV3D();
    u = -connectedHe2->next->getQVector3D();
    B = connectedHe1->prev->getEV3D();
    v = -connectedHe1->prev->getQVector3D();
    C = connectedHe1->prev->getCenter();
    w = connectedHe1->getQVector3D();

    tmp = fabs(QVector3D::dotProduct(u.normalized(), v.normalized())-1);
    if(tmp<eps_pararell){
        // pararell
        s = (w.x()*(A.y()-C.y())-w.y()*(A.x()-C.x()))/(u.x()*w.y()-u.y()*w.x());
    }else{
        // not pararell
        s = (v.x()*(A.y()-B.y())-v.y()*(A.x()-B.x()))/(u.x()*v.y()-u.y()*v.x());
    }
    Q = QVector3D(s*u.x()+A.x(), s*u.y()+A.y(), A.z());
    // setPos
    connectedHe2->vertex->setPosition(P);
    connectedHe2->vertex_next->setPosition(Q);

}
void Canvas3d::movePleatBridges_new_old(Halfedge3d *connectedHe1, Halfedge3d *connectedHe2)
{
    static const double eps_pararell = 1e-6;
    QVector3D P, Q;
    // P
    QVector3D A = connectedHe2->prev->getSV3D();
    QVector3D u = connectedHe2->prev->getQVector3D();
    QVector3D B = connectedHe1->next->getSV3D();
    QVector3D v = connectedHe1->next->getQVector3D();
    QVector3D C = connectedHe1->next->getCenter();
    QVector3D w = -connectedHe1->getQVector3D();
    double s;
    double tmp = fabs(QVector3D::dotProduct(u.normalized(), v.normalized())-1);
    if(tmp<eps_pararell){
        // pararell
        s = (w.x()*(A.y()-C.y())-w.y()*(A.x()-C.x()))/(u.x()*w.y()-u.y()*w.x());
    }else{
        // not pararell
        s = (v.x()*(A.y()-B.y())-v.y()*(A.x()-B.x()))/(u.x()*v.y()-u.y()*v.x());
    }
    P = QVector3D(s*u.x()+A.x(), s*u.y()+A.y(), A.z());
    // Q
    A = connectedHe2->next->getEV3D();
    u = -connectedHe2->next->getQVector3D();
    B = connectedHe1->prev->getEV3D();
    v = -connectedHe1->prev->getQVector3D();
    C = connectedHe1->prev->getCenter();
    w = connectedHe1->getQVector3D();

    tmp = fabs(QVector3D::dotProduct(u.normalized(), v.normalized())-1);
    if(tmp<eps_pararell){
        // pararell
        s = (w.x()*(A.y()-C.y())-w.y()*(A.x()-C.x()))/(u.x()*w.y()-u.y()*w.x());
    }else{
        // not pararell
        s = (v.x()*(A.y()-B.y())-v.y()*(A.x()-B.x()))/(u.x()*v.y()-u.y()*v.x());
    }
    Q = QVector3D(s*u.x()+A.x(), s*u.y()+A.y(), A.z());
    // setPos
    connectedHe2->vertex->setPosition(P);
    connectedHe2->vertex_next->setPosition(Q);

}

void Canvas3d::movePleatBridges170225(Halfedge3d *connectedHe1, Halfedge3d *connectedHe2)
{
    QVector3D A,B;
    A = connectedHe2->vertex->getVector3D();
    B = connectedHe2->vertex_next->getVector3D();
    QVector3D u,v;
    u = connectedHe2->prev->getVector3D().normalized();
    v = -connectedHe2->next->getVector3D().normalized();

    float d = 30;//20170323
    A += d*u;
    B += d*v;
    connectedHe2->vertex->setPosition(A);
    connectedHe2->vertex_next->setPosition(B);
}
bool Canvas3d::isVerticesOverlapping(Vertex3d *v1, Vertex3d *v2){
    QVector2D vec1 = v1->getVertex2d()->getVector2D(), vec2 = v2->getVertex2d()->getVector2D();
    static const float eps = 1.f;
    const float tmp = (vec1-vec2).length();
    return tmp<eps;
}

void Canvas3d::compressVertices(Vertex3d *v1, Vertex3d *v2)
{
    v2->setPosition(v1->getQVector3D());
}
void Canvas3d::createPleatBridges_old(Halfedge3d *connectedHe1, Halfedge3d *connectedHe2)
{
    Vertex3d *sv, *ev;
    static const double d = 0.2;//test
    QVector3D svec, evec;
    svec = connectedHe2->getSV3D()+d*connectedHe2->prev->getQVector3D();
    evec = connectedHe2->getEV3D()-d*connectedHe2->next->getQVector3D();
    sv = createVertex(svec);
    ev = createVertex(evec);
    QVector<Halfedge3d*> hes;
    Vertex3d *sv2 = connectedHe2->vertex;
    Vertex3d *ev2 = connectedHe2->vertex_next;
    hes<<createHalfedge(ev2, sv2)<<createHalfedge(sv2, sv)<<createHalfedge(sv, ev)<<createHalfedge(ev, ev2);
    for(int i  = 0; i<hes.size(); ++i){
        hes.at(i)->setNext(hes.at((i==hes.size()-1)? 0: i+1));
    }
    Face3d *bridge2 = createFace(hes.front());
    addFace(bridge2);
    bridgeFaces.push_back(bridge2);
    connectedHe2->setPair(hes.front());
    Halfedge3d *connectedHe2_2 = hes.at(2);//test
    createBridge(connectedHe2_2, connectedHe1);
}

void Canvas3d::createOrikomiFace(Halfedge3d *connctedHe1, QVector3D v)
{

}


void Canvas3d::bridgeAllOverlapEdges()
{
    for(int i = 0; i<model->halfedges.size(); ++i){
        Halfedge3d *he1 = model->halfedges.at(i);
        for(int j = i+1; j<model->halfedges.size(); ++j){
            Halfedge3d *he2 = model->halfedges.at(j);
            bool bool_test = Halfedge3d::isZOverlapAndOppositeDirect(he1, he2);
            qDebug()<<bool_test;
            if(!he1->hasPair()&&!he2->hasPair()&&Halfedge3d::isZOverlapAndOppositeDirect(he1, he2)){
                createBridge(he1, he2);
            }
        }
    }
    update();
}

void Canvas3d::moveUnderVertexPositionIfThereAreZOverlapVerticesInTwoFaces(Face3d *upperFace, Face3d *lowerFace)
{
    Halfedge3d *connectedHe1 = upperFace->halfedge, *connectedHe2 = lowerFace->halfedge;

    QVector<QPair<Vertex3d*, Vertex3d*>> overlappingVerticesPairs;
    static const double eps = 10;
    Halfedge3d *ihe = connectedHe1;
    do{
        Vertex3d *iv = ihe->vertex;
        QVector3D ivec = iv->getQVector3D();
        ivec.setZ(0);
        Halfedge3d *jhe = connectedHe2;
        do{
            Vertex3d *jv = jhe->vertex;
            QVector3D jvec = jv->getQVector3D();
            jvec.setZ(0);
            if((ivec-jvec).length()<eps){
                //overlapping
                overlappingVerticesPairs.push_back(QPair<Vertex3d*, Vertex3d*>(iv, jv));
            }
            jhe = jhe->next;
        }while(jhe != connectedHe2);
        ihe = ihe->next;
    }while(ihe != connectedHe1);

    for(int i = 0; i<overlappingVerticesPairs.size(); ++i){
        Vertex3d *v1 = overlappingVerticesPairs.at(i).first;
        Vertex3d *v2 = overlappingVerticesPairs.at(i).second;
        v2->setPosition(v1->x, v1->y, v1->z);
    }
}

/* --- glue edge --- */
void Canvas3d::createGlueBridge(Halfedge2d *he2d)
{
//    qDebug()<<"Canvas3d::createGlueBridge : Begin";
    if(he2d->face->getMirrorTimes()==1){
        Halfedge3d *connectedHe1 = he2d->getHalfedge3d();
        if(connectedHe1==NULL){
            qDebug()<<"connectedHe1 == NULL";
            return;
        }
        Halfedge3d *connectedHe2 = connectedHe1->getMirror();
        if(connectedHe2==NULL){
            qDebug()<<"connectedHe2 == NULL";
            return;
        }
        Face3d *bridge  =createBridge(connectedHe1, connectedHe2);
//        connectEdges(connectedHe1, connectedHe2);
    }else{

        Face3d *face_in_group = he2d->getHalfedge3d()->face;
        Halfedge3d *connectedHe = NULL;
        do{
            Halfedge3d *he_in_face = face_in_group->halfedge;
            do{
                if(he_in_face->getHalfedge2d() == he2d){
                    if(connectedHe == NULL){
                        connectedHe = he_in_face;
                        break;
                    }else{
                        createBridge(he_in_face, connectedHe);
                        connectedHe = NULL;
                    }
                }
                he_in_face = he_in_face->next;
            }while(he_in_face != face_in_group->halfedge);
            face_in_group = face_in_group->getMirrorNext();
        }while(face_in_group != he2d->getHalfedge3d()->face);
    }
//    selectFace(bridge);
//    qDebug()<<"Canvas3d::createGlueBridge : End";
}

void Canvas3d::deleteGlueBridge(Halfedge2d *he2d)
{
    if(he2d->face->getMirrorTimes()==1){
        Halfedge3d *connectedHe1 = he2d->getHalfedge3d();
        deleteBridge(connectedHe1);
    }
}

Face3d *Canvas3d::getBridge(Halfedge3d *connectedHe1)
{
    Face3d *bridge = NULL;
    if(connectedHe1->hasPair()){
        for(int i = 0; i  < bridgeFaces.size(); ++i){
            if(connectedHe1->pair->face == bridgeFaces.at(i)){
                bridge = bridgeFaces.at(i);
            }
        }
    }
    return bridge;
}

void Canvas3d::compressGlueBridge(Halfedge2d *he2d, const double d)
{
    static const float eps = 1;
    if(he2d->face->getMirrorTimes()==1){
        Halfedge3d *connectedHe1 = he2d->getHalfedge3d();
        int L = connectedHe1->face->getFaceZDistance()*connectedHe1->face->getZOrder();
        Vertex3d *v1 = connectedHe1->vertex, *v2 =connectedHe1->next->vertex;
        const double z = L*(1-d);
        v1->setPosition(v1->x, v1->y, z);
        v2->setPosition(v2->x, v2->y, z);
    }else{
        QVector<Halfedge3d*>hes = he2d->getPositiveHalfedge3dsInMirrorGroup();
        for(int i = 0; i < hes.size()-1; i=i+2){
            int orderU = hes.at(i)->face->getZOrder();
            int orderD = hes.at(i+1)->face->getZOrder();
            float faceZDistance = hes.at(i)->face->getFaceZDistance();
            float base = (orderU+orderD)*faceZDistance/2.f;

            float length = (orderU-orderD)*faceZDistance/2.f;

            double zU = base + length*(1-d), zD = base - length*(1-d);
            Vertex3d *v1 = hes.at(i)->vertex, *v2 = hes.at(i)->vertex_next, *v3 = hes.at(i+1)->vertex, *v4 = hes.at(i+1)->vertex_next;
            if(fabs(v1->z-v4->z)>eps){
                v1->setPosition(v1->x, v1->y, zU);v4->setPosition(v4->x, v4->y, zD);
            }
            if(fabs(v2->z-v3->z)>eps){
                v2->setPosition(v2->x, v2->y,zU);v3->setPosition(v3->x, v3->y, zD);
            }
        }
    }
}



/* --- select/unselect slot --- */
void Canvas3d::selectVertexSync2D(Vertex2d *v2d)
{
    Vertex3d *v3d = v2d->getVertex3d();
    if(!isSelected(v3d)){
        selectVertex(v3d);
    }
}
void Canvas3d::selectHalfedgeSync2D(Halfedge2d *he2d)
{
    // he
    Halfedge3d *he3d = he2d->getHalfedge3d();
    if(!isSelected(he3d)){
        selectHalfedge(he3d);
    }
    // bridge face
    Face3d *bridge = getBridge(he3d);
    if(bridge!=NULL&&!isSelected(bridge)){
        selectFace(bridge);
    }
}
void Canvas3d::selectFaceSync2D(Face2d *f2d)
{
    Face3d *f3d = f2d->getFace3d();
    if(!isSelected(f3d)){
        selectFace(f3d);
    }
}
void Canvas3d::unselectVertexSync2D(Vertex2d *v2d)
{
    Vertex3d *v3d = v2d->getVertex3d();
    if(isSelected(v3d)){
        unselectVertex(v3d);
    }
}
void Canvas3d::unselectHalfedgeSync2D(Halfedge2d *he2d)
{
    // he
    Halfedge3d *he3d = he2d->getHalfedge3d();
    if(isSelected(he3d)){
        unselectHalfedge(he3d);
    }
    // bridge face
    Face3d *bridge = getBridge(he3d);
    if(bridge!=NULL&&isSelected(bridge)){
        unselectFace(bridge);
    }
}
void Canvas3d::unselectFaceSync2D(Face2d *f2d)
{
    Face3d *f3d = f2d->getFace3d();
    if(isSelected(f3d)){
        unselectFace(f3d);
    }
}
/* --- inside reverse fold bridge --- */
void Canvas3d::collapseEdgeInBridge(Halfedge3d *he){
    Vertex3d *v1 = he->vertex, *v2 = he->vertex_next;
    Halfedge3d *heL = he->next->pair;

    he->next->prev = he->prev;
    he->prev->next = he->next;

    he->next->vertex = v1;
    he->prev->vertex_next = v1;

    heL->vertex_next = v1;
    heL->next->vertex = v1;

    v2->getVertex2d()->setVertex3d(v1);
//    Vertex3d *mv = v2->getMirror();
//    if(mv!=NULL){
//        mv->setMirror(NULL);
//    }

    deleteVertex(v2);
    deleteHalfedge(he);

}

QVector<Annotation *> *Canvas3d::getAnnotations()
{
    if(canvas2d==NULL)return NULL;
    return canvas2d->getAnnotations();
}

QOpenGLTexture *Canvas3d::getTexture() const
{
    return texture;
}

void Canvas3d::setTexture(QOpenGLTexture *value)
{
    texture = value;
    if(texture==NULL){
        qDebug()<<"texture is NULL";
    }
    model->setTexture(texture);
}
QOpenGLTexture *Canvas3d::loadImage(QString filename){
    bool ret = image.load(filename);
    qDebug()<<"Canvas3d::setupTexture: image.load = "<<ret;
    QOpenGLTexture *t = new QOpenGLTexture(image);
    t->setMinificationFilter(QOpenGLTexture::LinearMipMapLinear);
    t->setMagnificationFilter(QOpenGLTexture::Linear);
    return t;
}

void Canvas3d::setupTexture(QString filename)
{
    qDebug()<<"Canvas3d::setupTexture::"<<filename;
    QOpenGLTexture *t = loadImage(filename);
    setTexture(t);

}

void Canvas3d::edgeCollapseOnInsideReverseFoldBridge(Face3d *bridge){
    Halfedge3d *he = bridge->halfedge;
    do{
        QVector3D vec = he->getQVector3D();
        vec.setZ(0);
        static const float eps = 10;
//        qDebug()<<vec.length();
        if(vec.length()<eps){
            collapseEdgeInBridge(he);
//            he->vertex->movePosition(he->getQVector3D());//170306 merge is too hard
            break;
        }
    }while((he=he->next)!= bridge->halfedge);
}

void Canvas3d::createInsideReverseFoldBridge(Halfedge2d *he1, Halfedge2d *he2)
{
    if(he1->face->getMirrorTimes()!=he2->face->getMirrorTimes()){
       qDebug()<<"Canvas3d::createInsideReverseFoldBridge: he1->face->getMirrorTimes()!=he2->face->getMirrorTimes()";
       return;
    }
    QVector<Halfedge3d*>hes1  = he1->getAllHalfedge3dsInMirrorGroup(), hes2 = he2->getAllHalfedge3dsInMirrorGroup();
    for(int i = 0; i < hes1.size(); ++i){
        Face3d *bridge = createBridge(hes1.at(i), hes2.at(i));
//        moveHalfedgeSVEVToEVSV(hes2.at(i), hes1.at(i), 0);
        edgeCollapseOnInsideReverseFoldBridge(bridge);

    }
    /* reconnect */
    Halfedge3d *he3d1 = he1->getHalfedge3d(), *he3d2 = he2->getHalfedge3d();
    static const double eps = 3;
    he3d1->vertex_next->setMirror(he3d1->getMirror()->vertex);
    he3d1->vertex->setMirror(he3d1->getMirror()->vertex_next);
    /* vertex2d sync */
    Vertex2d *v1 = NULL,*v2 = NULL;
    Halfedge2d *hei = he1;
    do{
        Vertex2d *vi = hei->vertex;
        Halfedge2d *hej = he2;
        do{
            Vertex2d *vj = hej->vertex;
            if((vi->getVector2D()-vj->getVector2D()).length()<eps){
                v1 = vi;
                v2 = vj;
                break;
            }
        }while((hej = hej->next) != he2);
        if(v1!=NULL&&v2!=NULL){
            break;
        }
    }while((hei = hei->next) != he1);

    v1->setVertex2d(v2);//sync

}

void Canvas3d::deleteInsideReverseFoldBridge(Halfedge2d *he1, Halfedge2d *he2)
{
//    if(he1->face->getMirrorTimes()==1){
//        Halfedge3d *connectedHe1 = he1->getHalfedge3d();
//        Halfedge3d *connectedMirrorHe1 = model->getMirror(connectedHe1);
//        Face3d *bridge1 = getBridge(connectedHe1);
//        Face3d *bridge2 = getBridge(connectedMirrorHe1);

//        Halfedge3d *he = bridge1->halfedge;
//        Halfedge3d *heLongest = NULL;
//        do{
//            if(getBridge(he)!=NULL){
//                heLongest = he;
//            }
//            he = he->next;
//        }while(he != bridge1->halfedge);

//        Face3d *bridge3 =  getBridge(heLongest); // this maybe danger after editing in canvas3d

//        deleteBridge(bridge3);
//        deleteBridge(bridge2);
//        deleteBridge(bridge1);
//    }

}
/* --- pleat fold bridge --- */
void Canvas3d::createPleatFoldBridge(Halfedge2d *he1, Halfedge2d *he2)
{
    Halfedge3d *connectedHe1 = he1->getHalfedge3d();
    Halfedge3d *connectedHe2 = he2->getHalfedge3d();
    Halfedge3d *connectedMirrorHe1 = model->getMirror(connectedHe1);
    Halfedge3d *connectedMirrorHe2 = model->getMirror(connectedHe2);
    createBridge(connectedHe1, connectedHe2);
    createBridge(connectedMirrorHe1, connectedMirrorHe2);
    movePleatBridges170225(connectedHe1, connectedHe2);
//    moveUnderVertexPositionIfThereAreZOverlapVerticesInTwoFaces(connectedHe1->face, connectedHe2->face);

    if(he2->face->getMirrorTimes()==2){
        QVector<Halfedge3d*>mhes = he2->getAllHalfedge3dsInMirrorGroup();
       moveHalfedgeSVEVToEVSVXY(mhes.at(1), mhes.at(0));
       moveHalfedgeSVEVToEVSVXY(mhes.at(2), mhes.at(3));
    }
}


void Canvas3d::deletePleatReverseFoldBridge(Halfedge2d *he1, Halfedge2d *he2)
{

}
/* --- turn fold bridge --- */
void Canvas3d::createTurnFoldBridge(Halfedge2d *he1, Halfedge2d *he2)
{
    if(he1->face->getMirrorTimes()==1&&he2->face->getMirrorTimes()==1){
        Halfedge3d *connectedHe1 = he1->getHalfedge3d();
        Halfedge3d *connectedHe2 = he2->getHalfedge3d();
        Halfedge3d *connectedMirrorHe1 = model->getMirror(connectedHe1);
        Halfedge3d *connectedMirrorHe2 = model->getMirror(connectedHe2);
        createBridge(connectedHe1, connectedHe2);
        createBridge(connectedMirrorHe1, connectedMirrorHe2);

        // check if there are overlap vertices in 2 faces
//        moveUnderVertexPositionIfThereAreZOverlapVerticesInTwoFaces(connectedHe1->face, connectedHe2->face);
    }
}

void Canvas3d::deleteTrunFoldBridge(Halfedge2d *he1, Halfedge2d *he2)
{

}

void Canvas3d::createSinkFoldBridge(Halfedge2d *he2d)
{
    if(he2d->face->getMirrorTimes()==1){
        // real
        Halfedge3d *he3d = he2d->getHalfedge3d();

        QVector3D A, B, C, D;
        A = he3d->getSV3D();
        B = he3d->next->getSV3D();
        C = he3d->next->next->getSV3D();
        D = he3d->prev->getSV3D();
        Vertex3d *vA = he3d->vertex, *vB = he3d->next->vertex, *vC = he3d->next->next->vertex, *vD = he3d->prev->vertex;

        static const float testF = 0.55;
        QVector3D O,  P, Q, R, S;
        O = (A+B+C+D)/4; O.setZ(0);
        P = testF*A+(1-testF)*B;
        Q = (1-testF)*A+testF*B;
        P.setZ(0); Q.setZ(0);
        R =testF*C+(1-testF)*D;
        S = (1-testF)*C+testF*D;
        R.setZ(0); S.setZ(0);
        Vertex3d *vO = createVertex(O), *vP = createVertex(P), *vQ = createVertex(Q), *vR = createVertex(R), *vS = createVertex(S);
        QVector<Face3d*>fL;
        QVector<Vertex3d* > vL1, vL2, vL3, vL4, vL5;
        vL1<<vB<<vA<<vO;
        vL2<<vA<<vP<<vO;
        vL3<<vQ<<vB<<vO;
        vL4<<vQ<<vR<<vC<<vB;
        vL5<<vP<<vA<<vD<<vS;
        Face3d *f1 = new Face3d(vL1), *f2 = new Face3d(vL2), *f3 = new Face3d(vL3), *f4 = new Face3d(vL4), *f5 = new Face3d(vL5);
        fL<<f1<<f2<<f3<<f4<<f5;

        // mirror
        Halfedge3d *mhe3d = model->getMirror(he3d);
        Vertex3d *vB2 = mhe3d->vertex, *vA2 = mhe3d->next->vertex, *vD2 = mhe3d->next->next->vertex, *vC2 = mhe3d->prev->vertex;
        QVector<Vertex3d* > vL1b, vL2b, vL3b, vL4b, vL5b;
        vL1b<<vA2<<vB2<<vO;
        vL2b<<vP<<vA2<<vO;
        vL3b<<vB2<<vQ<<vO;
        vL4b<<vQ<<vB2<<vC2<<vR;
        vL5b<<vP<<vS<<vD2<<vA2;
        Face3d *f1b = new Face3d(vL1b), *f2b = new Face3d(vL2b), *f3b = new Face3d(vL3b), *f4b = new Face3d(vL4b), *f5b = new Face3d(vL5b);
        fL<<f1b<<f2b<<f3b<<f4b<<f5b;

        // pairing
        for(int i =0; i<fL.size()-1; ++i){
            Face3d *f = fL.at(i);
            Halfedge3d *he = f->halfedge;
            do{
                for(int j = i+1; j<fL.size(); ++j){
                    Face3d *f2  = fL.at(j);
                    Halfedge3d *he2 = f2->halfedge;
                    do{
                        if(he->vertex == he2->next->vertex && he->next->vertex == he2->vertex){
                            he->setPair(he2);
                        }
                        he2 = he2->next;
                    }while(he2!=f2->halfedge);
                }
                he = he -> next;
            }while(he != f->halfedge);
        }

        for(int i = 0; i<fL.size(); ++i){
            addBridge(fL.at(i));
        }
    }
}

void Canvas3d::deleteSinkFoldBridge(Halfedge2d *he2d)
{

}

void Canvas3d::createPigsLegBridge(Halfedge2d *he1, Halfedge2d *he2, Halfedge2d *he3)
{
    Halfedge3d *he3d1U = he1->getHalfedge3d(), *he3d2 = he2->getHalfedge3d(), *he3d3 = he3->getHalfedge3d();
    Halfedge3d *he3d1D;
    Face3d *f1 = he3d1U->face;
    // get he3d1D
    Halfedge3d *he_in_D = f1->getMirrorNext()->halfedge;
    do{
        if(he_in_D->getHalfedge2d()==he3d1U->getHalfedge2d()){
            he3d1D  =he_in_D;
            break;
        }
        he_in_D = he_in_D->next;
    }while(he_in_D!=f1->getMirrorNext()->halfedge);
    // create bridges
    createBridge(he3d1U, he3d2);
    createBridge(he3d1D, he3d3);
//    createBridge(model->getMirror(he3d1U), model->getMirror(he3d2));
//    createBridge(model->getMirror(he3d1D), model->getMirror(he3d3));
    Halfedge3d *heTest1 = he3d1U->getMirror(), *heTest2 = he3d2->getMirror(), *heTest3 = he3d1D->getMirror(), *heTest4 = he3d3->getMirror();

    createBridge(heTest1, heTest2);
    createBridge(heTest3, heTest4);

    // move edges
    double z = (he3d2->getZHigher()>he3d3->getZHigher())?he3d2->getZHigher():he3d3->getZHigher();
    he3d2->setZ(z);he3d3->setZ(z);
    moveHalfedgeSVEVToEVSV(he3d2, he3d3, 0.5);
    moveHalfedgeSVEVToEVSV(he3d1D, he3d3, 0.5);

//     move overlap vertices -----> these should be done after 170125

    if(f1->getVertices().size()==3){
        moveUnderVertexPositionIfThereAreZOverlapVerticesInTwoFaces(he3d3->face, he3d1D->face);
        moveUnderVertexPositionIfThereAreZOverlapVerticesInTwoFaces(he3d2->face, he3d1U->face);
    }

}
void Canvas3d::moveHalfedgeSVEVToEVSV(Halfedge3d *heMove, Halfedge3d *heBase, double alpha){
    Vertex3d *svMove  = heMove->vertex, *evMove = heMove->vertex_next, *svBase = heBase->vertex, *evBase = heBase->vertex_next;
    QVector3D svV =  (1-alpha)*(evBase->getVector3D() - svMove->getVector3D());
    QVector3D evV =  (1-alpha)*(svBase->getVector3D() - evMove->getVector3D());
    svMove->movePosition( svV );
    evMove->movePosition( evV );
}

void Canvas3d::moveHalfedgeSVEVToEVSVXY(Halfedge3d *heMove, Halfedge3d *heBase, double alpha)
{
    Vertex3d *svMove  = heMove->vertex, *evMove = heMove->vertex_next, *svBase = heBase->vertex, *evBase = heBase->vertex_next;
    QVector3D svV =  (1-alpha)*(evBase->getVector3D() - svMove->getVector3D());svV.setZ(0);
    QVector3D evV =  (1-alpha)*(svBase->getVector3D() - evMove->getVector3D());evV.setZ(0);
    svMove->movePosition( svV );
    evMove->movePosition( evV );
}
void Canvas3d::moveHalfedgeSVEVToSVEV(Halfedge3d *heMove, Halfedge3d *heBase, double alpha){
    Vertex3d *svMove  = heMove->vertex, *evMove = heMove->vertex_next, *svBase = heBase->vertex, *evBase = heBase->vertex_next;
    svMove->movePosition( (1-alpha)*(svBase->getVector3D() - svMove->getVector3D()) );
    evMove->movePosition( (1-alpha)*(evBase->getVector3D() - evMove->getVector3D()) );
}

void Canvas3d::moveHalfedgeSVEVToSVEVXY(Halfedge3d *heMove, Halfedge3d *heBase, double alpha){
    Vertex3d *svMove  = heMove->vertex, *evMove = heMove->vertex_next, *svBase = heBase->vertex, *evBase = heBase->vertex_next;
    QVector3D svV = (1-alpha)*(svBase->getVector3D() - svMove->getVector3D());svV.setZ(0);
    QVector3D evV = (1-alpha)*(evBase->getVector3D() - evMove->getVector3D());evV.setZ(0);
    svMove->movePosition( svV );
    evMove->movePosition( evV );

}
void Canvas3d::deletePigsLegBridge(Halfedge2d *he1, Halfedge2d *he2, Halfedge2d *he3)
{

}

QPair<QVector3D, QVector3D> Canvas3d::getOrikomiEdgeBridgesP1P2(Halfedge3d *he3d1){
    Halfedge3d *he3d2 = he3d1->getMirror();
//    QVector3D g = (he3d1->getFace()->getG()+he3d2->getFace()->getG())/2;
//    QVector3D p1 = he3d1->getSV3D()+(g-he3d1->getCenter()), p2 = he3d1->getEV3D()+(g-he3d1->getCenter());
    QVector3D p1 = (he3d1->getSV3D()+he3d2->getEV3D())/2 + ((he3d2->next->getVector3D()-he3d1->prev->getVector3D())/4), p2 = (he3d1->getEV3D()+he3d2->getSV3D())/2 + ((he3d1->next->getVector3D()-he3d2->prev->getVector3D())/4);

    return QPair<QVector3D, QVector3D>(p1, p2);
}

void Canvas3d::createOrikomiEdgeBridge(Halfedge2d *he2d)
{


    if(he2d->face->getMirrorTimes()==1){

        Halfedge3d *he3d1 = he2d->getHalfedge3d(), *he3d2 = he3d1->getMirror();
//        QVector3D g = (he3d1->getFace()->getG()+he3d2->getFace()->getG())/2;
//        QVector3D p1 = he3d1->getSV3D()+(g-he3d1->getCenter()), p2 = he3d1->getEV3D()+(g-he3d1->getCenter());
        QVector3D p1 = getOrikomiEdgeBridgesP1P2(he3d1).first, p2 = getOrikomiEdgeBridgesP1P2(he3d1).second;
        Vertex3d *v1 = new Vertex3d(p1.x(), p1.y(), p1.z()), *v2 = new Vertex3d(p2.x(), p2.y(), p2.z() );
        Halfedge3d *he3d3 = new Halfedge3d(v1,v2), *he3d4 = new Halfedge3d(v2,v1);
        he3d3->setPair(he3d4);

        Face3d *b1 = createBridge(he3d1, he3d4);
        Face3d *b2 = createBridge(he3d2, he3d3);

        qDebug()<<"Canvas3d::createOrikomiEdgeBridge : done";
    }
}

void Canvas3d::deleteOrikomiEdgeBridge(Halfedge2d *he2d)
{

}
void Canvas3d::setHalfedgePair(Halfedge3d *he1, Halfedge3d *he2){
    he1->pair = he2;
    he2->pair = he1;
}
void Canvas3d::deleteVertex(Vertex3d *v){
    model->deleteVertex(v);
//    qDebug()<<"test";
}

void Canvas3d::deleteHalfedge(Halfedge3d *he)
{
    model->deleteHalfedge(he);
}
void Canvas3d::deleteFace(Face3d* f){
    model->deleteFace(f);
}

void Canvas3d::edgeCollapse(Halfedge3d *he)
{
    Halfedge3d *heLT = he->prev->pair;
    Halfedge3d *heRT = he->next->pair;
    Halfedge3d *heLB = he->pair->next->pair;
    Halfedge3d  *heRB = he->pair->prev->pair;

    he->next->vertex->x = ( he->vertex->x + he->next->vertex->x ) / 2;
    he->next->vertex->y = ( he->vertex->y + he->next->vertex->y ) / 2;
    he->next->vertex->z = ( he->vertex->z + he->next->vertex->z ) / 2;

    he->next->vertex->setHalfedge(heRB);
    he->prev->vertex->setHalfedge(heRT);
    he->pair->prev->vertex->setHalfedge(heLB);

    Halfedge3d *he1 = he->prev->pair;
    do {
        he1->vertex = he->next->vertex;
        he1 = he1->prev->pair;
    } while(he1 != he);

    setHalfedgePair(heRT, heLT);
    setHalfedgePair(heLB, heRB);

    deleteVertex(he->vertex);
    deleteFace(he->pair->face);
    deleteFace(he->face);
}

void Canvas3d::createMirrorConnect(QVector<Halfedge2d *> hes)
{
    if(hes.size()==2){
        hes.last()->getFace()->getModel()->getM3d()->getMirrorPlane()->setConnectedEdges(hes.last()->getHalfedge3d(), hes.front()->getHalfedge3d());//->setParentAndAxisAndCenter(hes.front()->getFace()->getModel()->getM3d()->getMirrorPlane(), hes.last()->getVec2D(),hes.last()->getHalfedge3d()->getCenter());

    }else if(hes.size()>2){
        MirrorPlane *parentMirror = NULL;
        for(int i = 0; i < hes.size(); ++i){
            Halfedge2d *he = hes.at(i);

            if(i == 0){
                parentMirror = he->getFace()->getModel()->getM3d()->getMirrorPlane();

            }else if( i == hes.size()-1){
                /* do nothing */
            }else{
                MirrorPlane *childMirror = he->getFace()->getModel()->getM3d()->getMirrorPlane();
//                childMirror->setParentAndAxisAndCenter(parentMirror, he->getVec2D(), he->getHalfedge3d()->getCenter());
                childMirror->setConnectedEdges(he->getHalfedge3d(), hes.at(0)->getHalfedge3d());
            }

        }
    }
}

void Canvas3d::deleteMirrorConnect(QVector<Halfedge2d *> hes)
{

}

void Canvas3d::rotateMirrorPlane(MirrorPlane *mirrorPlane, int theta)
{
    double theta2 = theta*2*M_PI/360;
    mirrorPlane->setTheta(theta2);
}

void Canvas3d::scaleMirrorPlane(MirrorPlane *mirrorPlane, int scale)
{
    float scale2 = scale/10.f;
    mirrorPlane->setScale(scale2);
}

void Canvas3d::updateAnnotationBridge()
{
//    qDebug()<<"test";
    QVector<Annotation*> *annotations = getAnnotations();
    for(int i = 0; i<annotations->size(); ++i){
        Annotation *a = annotations->at(i);
        if(a->getAnnotationName()==ORIKOMI){
            Halfedge3d *he = a->getHes().front()->getHalfedge3d();
            updateOrikomiBridgeIfItIsNeed(he);
        }
    }
}

void Canvas3d::updateOrikomiBridgeIfItIsNeed(Halfedge3d *he)
{
    if(isNeedToUpdateOrikomiBridge(he)){
        updateOrikomiBridge(he);
    }
}

bool Canvas3d::isNeedToUpdateOrikomiBridge(Halfedge3d *he)
{
    static const float eps = 1;
    bool b = false;
    if(he->pair==NULL){
        return false;
    }
    QVector3D p1 = he->pair->next->getEV3D(), p2 = he->pair->prev->getSV3D(),
            pa = getOrikomiEdgeBridgesP1P2(he).first, pb = getOrikomiEdgeBridgesP1P2(he).second;

    b = (p1-pa).length()>eps||(p2-pb).length()>eps;

    return b;
}

void Canvas3d::updateOrikomiBridge(Halfedge3d *he)
{
    QVector3D pa = getOrikomiEdgeBridgesP1P2(he).first, pb = getOrikomiEdgeBridgesP1P2(he).second;
    Vertex3d *v1 = he->pair->next->vertex_next, *v2 = he->pair->prev->vertex;
    v1->setPosition(pa);v2->setPosition(pb);
    emit(updated());
}




