#pragma once
#include "vertex3d.h"
#include "halfedge3d.h"
#include "face3d.h"
#include "halfedge2d.h"
#include "face2d.h"
#include "model2d.h"
#include "model3d.h"
#include "subface.h"
#include "subfacebuilder.h"
#include "mirrorplane.h"
#include <QOpenGLFunctions>
#include <QDebug>
#include "gldrawutil.h"
Model3d::Model3d(QObject *parent)
    :ModelBase(parent)
{
    m2d = NULL;
    setTexture(NULL);
    createMirrorPlane();
    connect(this, SIGNAL(updated()), this, SLOT(updateMirrorPlane()));
}

Model2d *Model3d::getM2d() const
{
    return m2d;
}

void Model3d::setM2d(Model2d *value)
{
    m2d = value;
    if(m2d->getM3d()==NULL){
        m2d->setM3d(this);
    }
}

void Model3d::updateMirrorPlane()
{
    mirrorPlane->setPosition(getG());

}

QVector3D Model3d::getG()
{
    QVector3D g;
    for(int i = 0; i < vertices.size(); ++i){
        g += vertices.at(i)->getVector3D();
    }
    g/=vertices.size();
    return g;
}

MirrorPlane *Model3d::createMirrorPlane()
{
    MirrorPlane *mp = new MirrorPlane();
    setMirrorPlane(mp);
    return mp;
}

MirrorPlane *Model3d::getMirrorPlane() const
{
    return mirrorPlane;
}

void Model3d::setMirrorPlane(MirrorPlane *value)
{
    mirrorPlane = value;
    /* 170214 */
    connect(mirrorPlane, SIGNAL(updated()), this, SIGNAL(updated()));
}

void Model3d::rotateX(double value)
{
    mirrorPlane->addAlpha(value);
}

void Model3d::rotateY(double value)
{
    mirrorPlane->addBeta(value);
}

void Model3d::rotateZ(double value)
{
    mirrorPlane->addGamma(value);
}

void Model3d::moveX(double value)
{

}

void Model3d::moveY(double value)
{

}

void Model3d::moveZ(double value)
{
    mirrorPlane->moveZ(value);
}

void Model3d::changeScale(double value)
{
    mirrorPlane->changeScale(value);
}

QOpenGLTexture *Model3d::getTexture() const
{
    return texture;
}

void Model3d::setTexture(QOpenGLTexture *value)
{
    texture = value;
    for(int i = 0; i<faces.size(); ++i){
        faces.at(i)->setTexture(getTexture());
    }
}



void Model3d::draw(bool v, bool e, bool f, bool n, bool c)
{
    //    glPushMatrix();
    //mirrorPlane->execute();
    //    getMirrorPlane()->execute170215();
    if(v)drawVertices();
    
    if(f){
        glEnable(GL_CULL_FACE);
        drawFaces();
        glDisable(GL_CULL_FACE);
    }
    if(e)drawHalfedges();

    if(n)drawNormals();
    //if(c)drawPairConnection();
    //drawOriLines(1);
//    drawSubFaces(1);


//    glPopMatrix();
}
/* --- Draw --- */
void Model3d::drawVertices()
{
    if(vertices.empty()){
        return;
    }
    glDisable(GL_LIGHTING);
    for(int i = 0; i<vertices.size(); ++i){

        glLoadName(i);

        Vertex3d *v = vertices.at(i);
        v->draw();
    }

}

void Model3d::drawHalfedges()
{
    if(halfedges.empty()){
        return;
    }

    glDisable(GL_LIGHTING);
    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA,GL_ONE_MINUS_SRC_ALPHA);
    glHint(GL_LINE_SMOOTH_HINT, GL_DONT_CARE);
    glEnable(GL_LINE_SMOOTH);
    for(int i=0; i<halfedges.size(); ++i){

        glLoadName(i);

        Halfedge3d *he = halfedges.at(i);
        he->draw();
    }
    glDisable(GL_LINE_SMOOTH);
}
//-------------------------------------------
struct MaterialStruct {
    GLfloat ambient[4];
    GLfloat diffuse[4];
    GLfloat specular[4];
    GLfloat shininess;
};
MaterialStruct ms_jade = {
    {0.135,     0.2225,   0.1575,   1.0},
    {0.54,      0.89,     0.63,     1.0},
    {0.316228,  0.316228, 0.316228, 1.0},
    12.8};
MaterialStruct ms_ruby  = {
    {0.1745,   0.01175,  0.01175,   1.0},
    {0.61424,  0.04136,  0.04136,   1.0},
    {0.727811, 0.626959, 0.626959,  1.0},
    76.8};
MaterialStruct ms_white ={
    {0.9,0.9,0.9,1},
    {0.6, 0.6, 0.6 , 1},
    {0,0,0,1},
    10};

MaterialStruct ms_gray ={
    {0.6,0.6,0.6,1},
    {0.3, 0.3, 0.3 , 1},
    {0.25,0.25,0.25,1},
    10};
GLfloat white[] ={.9, .9,.9, 1};
GLfloat red[] = {.9, .6, .6, 1};
//-------------------------------------------

void Model3d::drawFaces()
{
    if(faces.empty()){
        return;
    }


    glDisable(GL_LIGHTING);
    glEnable(GL_POLYGON_OFFSET_FILL);
    glPolygonOffset(1, 1);

    for(int i = 0; i < faces.size(); ++i){
        Face3d *f = faces.at(i);
        if(f == NULL){
            qDebug()<<"Model3d::drawFaces() : f == NULL";
        }
        glLoadName(i);

        f->draw();
        f->draw2();
    }
    glDisable(GL_POLYGON_OFFSET_FILL);
    glDisable(GL_LIGHTING);
}

void Model3d::drawFacesForPickingModel()
{
    if(faces.empty()){
        return;
    }


    glDisable(GL_LIGHTING);
    glEnable(GL_POLYGON_OFFSET_FILL);
    glPolygonOffset(1, 5);

    for(int i = 0; i < faces.size(); ++i){
        Face3d *f = faces.at(i);
        if(f == NULL){
            qDebug()<<"Model3d::drawFaces() : f == NULL";
        }
//        glLoadName(i);

        f->draw();
        f->draw2();
    }
    glDisable(GL_POLYGON_OFFSET_FILL);
    glDisable(GL_LIGHTING);
}

void Model3d::drawNormals()
{
    if(faces.empty()){
        return;
    }
    glDisable(GL_LIGHTING);
    glLineWidth(2);
    glColor3f((float)getSelectColor().red()/255, (float)getSelectColor().green()/255, (float)getSelectColor().blue()/255);

    for(int i = 0; i < faces.size(); ++i){
        faces.at(i)->drawNormal();
    }
}

void Model3d::drawPairConnection()
{
    if(halfedges.empty()){
        return;
    }
    glDisable(GL_LIGHTING);
    glLineWidth(2);
    glColor3f((float)getSelectColor().red()/255, (float)getSelectColor().green()/255, (float)getSelectColor().blue()/255);

    for(int i = 0; i < halfedges.size(); ++i){
        halfedges.at(i)->drawPairConnection();
    }
}

void Model3d::drawBoundingBox()
{
    /* calc xmin, ymin, xmax, ymax */
    static const double M = 800;
    double xmin = M,ymin = M,xmax = 0, ymax = 0;
    for(int i = 0; i<vertices.size(); ++i){
        QVector3D v = vertices.at(i)->getVector3D();
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

/* --- Add Object --- */
void Model3d::addVertex(Vertex3d *vertex)
{
    if(vertices.indexOf(vertex)==-1){
        vertices.push_back(vertex);
        vertex->setColor(getVertexColor());
        vertex->setSize(getPointSize());
    }else{
//        qDebug()<<"Model3d::addVertex: vertices.indexOf(vertex)!=-1";
    }
}

void Model3d::addVertex(Face3d *face)
{

    Halfedge3d *he = face->halfedge;
    do{
        addVertex(he->vertex);
         he = he->next;
    }while(he != face->halfedge);
}

void Model3d::addHalfedge(Halfedge3d *halfedge)
{
    if(halfedges.indexOf(halfedge)==-1){
        halfedges.push_back(halfedge);
        halfedge->setSize(getLineWidth());
        halfedge->setColor(getHalfedgeColor());
    }else{
        qDebug()<<"!!!WARNIG!!!: Model3d::addHalfedge: duplicate addition";
    }
}

void Model3d::addHalfedge(Face3d *face)
{
    Halfedge3d *he = face->halfedge;
    do{
        addHalfedge(he);
        he = he->next;
    }while(he != face->halfedge);
}

void Model3d::addFace(Face3d *face)
{

    if(faces.indexOf(face)==-1){
        face->setModel(this);//170214
        faces.push_back(face);
        face->setColor(getFaceColor());
        face->setTexture(getTexture());
        connect(face, SIGNAL(updated()), this, SIGNAL(updated()));
        emit(updated());

    }else{
        qDebug()<<"Model3d::addFace: duplicate addition";
    }
}

/* --- Delete Object --- */
void Model3d::deleteVertex(Vertex3d *vertex)
{
    vertices.removeOne(vertex);
    delete vertex;
}

void Model3d::deleteHalfedge(Halfedge3d *halfedge)
{
    if(halfedge->hasPair()){
        halfedge->pair->pair=NULL;
    }
    if(halfedge->vertex->getHalfedge()==halfedge){
        halfedge->vertex->setHalfedge(NULL); //YABAI
    }
    halfedges.removeOne(halfedge);
    delete halfedge;

}

void Model3d::deleteFace(Face3d *face)
{
    Halfedge3d *he = face->halfedge;
    int n = 0;
    do{
        n++;
        he = he->next;
    }while(he != face->halfedge);
    for(int i = 0; i<n; ++i){
        Halfedge3d *he_delete = he;
        if(i!=n-1)he=he->next;
        deleteHalfedge(he_delete);
    }
    faces.removeOne(face);
    delete face;
    emit(updated());
}

Vertex3d *Model3d::getMirror(Vertex3d *v)
{
    Vertex3d *mirrorVertex = NULL;
    //// vertices always overlapping ////
    return mirrorVertex;
}

Halfedge3d *Model3d::getMirror(Halfedge3d *he)
{
    Halfedge3d *mirror = NULL;
    static const double eps = 1e-6;
    if(he->getHalfedge2d()==NULL||he->getHalfedge2d()->face->getMirrorTimes()==1){
        QVector3D sv1 = he->vertex->getQVector3D();
        QVector3D ev1 = he->vertex_next->getQVector3D();
        for(int i = 0; i<halfedges.size(); ++i){
            QVector3D sv2 = halfedges.at(i)->vertex->getQVector3D();
            QVector3D ev2 = halfedges.at(i)->vertex_next->getQVector3D();
            QVector2D sv1_ev2_2D = QVector2D((sv1-ev2).x(), (sv1-ev2).y());
            QVector2D sv2_ev1_2D = QVector2D((sv2-ev1).x(), (sv2-ev1).y());
            double sv1_ev2_Z = (sv1+ev2).z();
            double sv2_ev1_Z = (sv2+ev1).z();
            if(sv1_ev2_2D.length()<eps&&sv2_ev1_2D.length()<eps&&sv1_ev2_Z<eps&&sv2_ev1_Z<eps){
                mirror = halfedges.at(i);
            }
        }
    }else{
        QVector<Halfedge3d*> hes = he->getHalfedge2d()->getAllHalfedge3dsInMirrorGroup();
        for(int i = 0; i<hes.size(); ++i){
            const double z1 = hes.at(i)->vertex->z, z2 = he->vertex->z;
            if(fabs(z1 + z2)<eps){
//                qDebug()<<"z1 = "<<z1<<", z2 = "<<z2;
                mirror = hes.at(i);
                break;
            }
        }
    }

    return mirror;
}
/* --- SubFace --- */
void Model3d::makeSubFace()
{
    subfaces.clear();// need to delete
    subfaces = SubFaceBuilder::build(faces);
//    debugPrintSubfaces();
    //makeOriLines();//test
}

void Model3d::drawSubFaces(bool flag)
{
    glPushMatrix();
    static const int n = 7;
    static const QColor colors[n] = {Qt::black, Qt::red, Qt::green, Qt::blue, Qt::yellow, Qt::cyan, Qt::magenta};
    if(!flag)return;
    glDisable(GL_LIGHTING);
    for(int i = 0; i < subfaces.size(); ++i){
        glColor3f((float)colors[i%n].red(), (float)colors[i%n].green(), (float)colors[i%n].blue());
        glTranslatef(0,0,-1);
        subfaces.at(i)->drawFace();
//        subfaces.at(i)->drawEdges();
    }
    glPopMatrix();
}

void Model3d::makeOriLines()
{
    lines.clear();
    lines = SubFaceBuilder::getOriLines(faces);
}

void Model3d::drawOriLines(bool flag)
{
    static const int n = 7;
    static const QColor colors[n] = {Qt::black, Qt::red, Qt::green, Qt::blue, Qt::yellow, Qt::cyan, Qt::magenta};
    if(!flag)return;
    glDisable(GL_LIGHTING);
    glLineWidth(2);
    for(int i = 0; i < lines.size(); ++i){
        //glColor3f((i%3)/2, ((i+1)%3)/2, ((i+2)%3)/2);
        glColor3f((float)colors[i%n].red(), (float)colors[i%n].green(), (float)colors[i%n].blue());
        lines.at(i)->draw();
    }
}

void Model3d::debugPrintSubfaces()
{
    qDebug()<<"Model3d::debugPrintSubfaces : subfaces.size() = "<<subfaces.size();
    for(int i = 0; i < subfaces.size(); ++i){
        subfaces.at(i)->debugPrint();
    }
}
