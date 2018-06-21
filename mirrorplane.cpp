#include "mirrorplane.h"
#include <QOpenGLFunctions>
#include "gldrawutil.h"
#include "halfedge3d.h"
#include "model3d.h"
#include "halfedge2d.h"
#include "face2d.h"
#include "face3d.h"
#include <QtMath>
MirrorPlane::MirrorPlane(QObject *parent)
    :QObject(parent)
{
    setPosition(QVector3D(0,0,0));setAlpha(0);setBeta(0);setGamma(0);
    setZ(0);
    setScale(1);
    setParent(NULL);

    /* rotate around an axis */
    setAxis(QVector2D(0,1));//init
    setTheta(0);//init
    setCenter(QVector3D(0,0,0));//init
    /* connected edges */
    setConnectedHe1(NULL);
    setConnectedHe2(NULL);
}

QVector3D MirrorPlane::getPosition() const
{
    return position;
}

void MirrorPlane::setPosition(const QVector3D &value)
{
    position = value;
    if(getParent()==NULL){
        setCenter(value);
    }
}

double MirrorPlane::getAlpha() const
{
    return alpha;
}

void MirrorPlane::setAlpha(double value)
{
    alpha = value;
}

void MirrorPlane::addAlpha(double value)
{
    setAlpha(alpha+value);
}

double MirrorPlane::getBeta() const
{
    return beta;
}

void MirrorPlane::setBeta(double value)
{
    beta = value;
}

void MirrorPlane::addBeta(double value)
{
    setBeta(beta+value);
}

double MirrorPlane::getGamma() const
{
    return gamma;
}

void MirrorPlane::setGamma(double value)
{
    gamma = value;
}

void MirrorPlane::addGamma(double value)
{
    setGamma(gamma+value);
}

double MirrorPlane::getScale() const
{
    return scale;
}

void MirrorPlane::setScale(double value)
{
    scale = value;
    if(scale<0)scale=0;
    emit(updated());
}

void MirrorPlane::execute()
{
    glTranslated(0,0,z);
    glTranslated(position.x(), position.y(), position.z());
    glRotated(alpha, 1, 0, 0);
    glRotated( beta, 0, 1, 0);
    glRotated(gamma, 0, 0, 1);
    glTranslated(-position.x(), -position.y(), -position.z());
    glScaled(1,1, scale);
}



QMatrix4x4 MirrorPlane::getM4x4()
{
    if(axis.length()<1e-1){
            QMatrix4x4 I;
            return I;
    }
    const double x = axis.x(), y = axis.y(), sint = sin(theta), cost = cos(theta), ncost = 1-cos(theta);

    QMatrix4x4 M(x*x*ncost+cost, x*y*ncost, y*sint, 0,
                 x*y*ncost, y*y*ncost+cost, -x*sint, 0,
                 -y*sint, x*sint, cost,0,
                 0,0,0,1
                );

    return M;
}

QMatrix4x4 MirrorPlane::getNM4x4()
{
    if(axis.length()<1e-1){
            QMatrix4x4 I;
            return I;
    }
    const double phi = -theta;
    const double x = axis.x(), y = axis.y(), sint = sin(phi), cost = cos(phi), ncost = 1-cos(phi);

    QMatrix4x4 M(x*x*ncost+cost, x*y*ncost, y*sint, 0,
                 x*y*ncost, y*y*ncost+cost, -x*sint, 0,
                 -y*sint, x*sint, cost,0,
                 0,0,0,1
                );
    return M;
}

QMatrix4x4 MirrorPlane::getT4x4()
{
    QMatrix4x4 T(1,0,0,center.x(),
                 0,1,0,center.y(),
                 0,0,1,center.z(),
                 0,0,0,1);
    T = T.transposed();
    return T;
}

QMatrix4x4 MirrorPlane::getNT4x4()
{
    QMatrix4x4 NT(1,0,0,-center.x(),
                 0,1,0,-center.y(),
                 0,0,1,-center.z(),
                 0,0,0,1);
    NT = NT.transposed();
    return NT;
}

QMatrix4x4 MirrorPlane::getRotateMatrix()
{
    QMatrix4x4 T = this->getT4x4();
    QMatrix4x4 NT = this->getNT4x4();
    QMatrix4x4 R = this->getM4x4();
    R = NT*R*T;
    if(getParent()!=NULL){
        R = R*getParent()->getRotateMatrix();
    }else{
        R.setToIdentity();

    }
//    qDebug()<<R;
    return R;
}

QMatrix4x4 MirrorPlane::getS4x4()
{
    QMatrix4x4 S(
                1,0,0,0
                ,0,1,0,0
                ,0,0,scale,0
                ,0,0,0,1);
    return S;
}

QMatrix4x4 MirrorPlane::getScaleMatrix()
{
    QMatrix4x4 T = this->getT4x4();
    QMatrix4x4 NT = this->getNT4x4();
    QMatrix4x4 S = this->getS4x4();
    S = NT*S*T;
    if(getParent()!=NULL){
        S = S*getParent()->getScaleMatrix();
    }
//    qDebug()<<S;
    return S;
}

void MirrorPlane::getM(double *rt)
{
    QMatrix4x4 R = getM4x4();
    rt[0] = R(0,0); rt[1] = R(0,1); rt[2] = R(0,2); rt[3] = R(0,3);
    rt[4] = R(1,0); rt[5] = R(1,1); rt[6] = R(1,2); rt[7] = R(1,3);
    rt[8] = R(2,0); rt[9] = R(2,1); rt[10] = R(2,2); rt[11] = R(2,3);
    rt[12] = R(3,0); rt[13] = R(3,1); rt[14] = R(3,2); rt[15] = R(3,3);

}

void MirrorPlane::getRT(double *rt)
{
    QMatrix4x4 R = getRotateMatrix();
    rt[0] = R(0,0); rt[1] = R(0,1); rt[2] = R(0,2); rt[3] = R(0,3);
    rt[4] = R(1,0); rt[5] = R(1,1); rt[6] = R(1,2); rt[7] = R(1,3);
    rt[8] = R(2,0); rt[9] = R(2,1); rt[10] = R(2,2); rt[11] = R(2,3);
    rt[12] = R(3,0); rt[13] = R(3,1); rt[14] = R(3,2); rt[15] = R(3,3);
}

void MirrorPlane::getTT(double *rt)
{

    QMatrix4x4 R = getT4x4();
//    qDebug()<<"T="<<R;
    rt[0] = R(0,0); rt[1] = R(0,1); rt[2] = R(0,2); rt[3] = R(0,3);
    rt[4] = R(1,0); rt[5] = R(1,1); rt[6] = R(1,2); rt[7] = R(1,3);
    rt[8] = R(2,0); rt[9] = R(2,1); rt[10] = R(2,2); rt[11] = R(2,3);
    rt[12] = R(3,0); rt[13] = R(3,1); rt[14] = R(3,2); rt[15] = R(3,3);
}

void MirrorPlane::getNTT(double *rt)
{
    QMatrix4x4 R = getNT4x4();
//    qDebug()<<"NT="<<R;
    rt[0] = R(0,0); rt[1] = R(0,1); rt[2] = R(0,2); rt[3] = R(0,3);
    rt[4] = R(1,0); rt[5] = R(1,1); rt[6] = R(1,2); rt[7] = R(1,3);
    rt[8] = R(2,0); rt[9] = R(2,1); rt[10] = R(2,2); rt[11] = R(2,3);
    rt[12] = R(3,0); rt[13] = R(3,1); rt[14] = R(3,2); rt[15] = R(3,3);
}

void MirrorPlane::getST(double *rt)
{
    QMatrix4x4 R = getScaleMatrix();
//    qDebug()<<"S="<<R;
    rt[0] = R(0,0); rt[1] = R(0,1); rt[2] = R(0,2); rt[3] = R(0,3);
    rt[4] = R(1,0); rt[5] = R(1,1); rt[6] = R(1,2); rt[7] = R(1,3);
    rt[8] = R(2,0); rt[9] = R(2,1); rt[10] = R(2,2); rt[11] = R(2,3);
    rt[12] = R(3,0); rt[13] = R(3,1); rt[14] = R(3,2); rt[15] = R(3,3);
}


void MirrorPlane::execute170215()
{

    double rt[16], st[16];
    getRT(rt);getST(st);
//    double tt[16];
//    getTT(tt);
//    double ntt[16];
//    getNTT(ntt);

//    glMultMatrixd(tt);
//    glTranslated(center.x(), center.y(), center.z());
    glMultMatrixd(rt);
    glMultMatrixd(st);
//    glTranslated(-center.x(), -center.y(), -center.z());
//    glMultMatrixd(ntt);

//    glTranslated(center.x(), center.y(), center.z());
//    glScaled(1,1,scale);
//    glTranslated(-center.x(), -center.y(), -center.z());

}
QMatrix4x4 MirrorPlane::getRotateMatrixWithAxisAndTheta(QVector3D axis, double theta){
    const double x = axis.x(), y = axis.y(), z = axis.z(), sint = sin(theta), cost = cos(theta), ncost = 1-cos(theta);

    QMatrix4x4 M(x*x*ncost+cost, x*y*ncost - z*sint, z*x*ncost+y*sint, 0,
                 x*y*ncost+z*sint, y*y*ncost+cost, y*z*ncost-x*sint, 0,
                 z*x*ncost-y*sint, y*z*ncost+x*sint, z*z*ncost+cost,0,
                 0,0,0,1
                );
    return M;
}

QMatrix4x4 MirrorPlane::getM4x4_2()
{
    if(getParent()==NULL){
        QMatrix4x4 I;
        return I;
    }
    QVector3D axis = connectedHe1->getVector3D().normalized();
    QMatrix4x4 M  = getRotateMatrixWithAxisAndTheta(axis, theta);
    return M;
}
QMatrix4x4 MirrorPlane::getM4x4_2_old()
{
    if(getParent()==NULL){
        QMatrix4x4 I;
        return I;
    }
    QVector2D ax = connectedHe1->getHalfedge2d()->getVec2D().normalized();
    const double x = ax.x(), y = ax.y(), sint = sin(theta), cost = cos(theta), ncost = 1-cos(theta);

    QMatrix4x4 M(x*x*ncost+cost, x*y*ncost, y*sint, 0,
                 x*y*ncost, y*y*ncost+cost, -x*sint, 0,
                 -y*sint, x*sint, cost,0,
                 0,0,0,1
                );

    return M;
}

QMatrix4x4 MirrorPlane::getNM4x4_2()
{
    if(getParent()==NULL){
        QMatrix4x4 I;
        return I;
    }
    QVector3D axis = -connectedHe1->getMirror()->getVector3D().normalized();
    QMatrix4x4 M  = getRotateMatrixWithAxisAndTheta(axis, -theta);
    return M;
}
QMatrix4x4 MirrorPlane::getNM4x4_2_old()
{
    if(getParent()==NULL){
        QMatrix4x4 I;
        return I;
    }
    QVector2D ax = connectedHe1->getHalfedge2d()->getVec2D().normalized();
    const double x = ax.x(), y = ax.y(), sint = sin(-theta), cost = cos(-theta), ncost = 1-cos(-theta);

    QMatrix4x4 M(x*x*ncost+cost, x*y*ncost, y*sint, 0,
                 x*y*ncost, y*y*ncost+cost, -x*sint, 0,
                 -y*sint, x*sint, cost,0,
                 0,0,0,1
                );

    return M;
}

QMatrix4x4 MirrorPlane::getT4x4_2()
{
    if(getParent()==NULL){
        QMatrix4x4 I;
        return I;
    }
    QVector3D c = getConnectedHe1()->getCenter();
    QMatrix4x4 T(1,0,0,c.x(),
                 0,1,0,c.y(),
                 0,0,1,c.z(),
                 0,0,0,1);
    T = T.transposed();
    return T;
}

QMatrix4x4 MirrorPlane::getNT4x4_2()
{
    if(getParent()==NULL){
        QMatrix4x4 I;
        return I;
    }
    QVector3D c = -getConnectedHe1()->getCenter();
    QMatrix4x4 T(1,0,0,c.x(),
                 0,1,0,c.y(),
                 0,0,1,c.z(),
                 0,0,0,1);
    T = T.transposed();
    return T;
}

QMatrix4x4 MirrorPlane::getRotateMatrix_2()
{
    QMatrix4x4 R = getSingleRotateMatrix();
    if(getParent()!=NULL){
        R = R*getParent()->getRotateMatrix_2();
    }else{
        R.setToIdentity();

    }
//    qDebug()<<R;
    return R;
}

QMatrix4x4 MirrorPlane::getReverseRotateMatrix()
{

    QMatrix4x4 R = getSingleReverseRotateMatrix();
    if(getParent()!=NULL){
        R = R*getParent()->getReverseRotateMatrix();
    }else{
        R.setToIdentity();

    }
    return R;
}

void MirrorPlane::getMRT(double *rt)
{
    QMatrix4x4 R = getReverseRotateMatrix();
    rt[0] = R(0,0); rt[1] = R(0,1); rt[2] = R(0,2); rt[3] = R(0,3);
    rt[4] = R(1,0); rt[5] = R(1,1); rt[6] = R(1,2); rt[7] = R(1,3);
    rt[8] = R(2,0); rt[9] = R(2,1); rt[10] = R(2,2); rt[11] = R(2,3);
    rt[12] = R(3,0); rt[13] = R(3,1); rt[14] = R(3,2); rt[15] = R(3,3);
}

QMatrix4x4 MirrorPlane::getS4x4_2()
{
    return getS4x4();
}

QMatrix4x4 MirrorPlane::getScaleMatrix_2()
{

    QMatrix4x4 T = this->getT4x4_2();
    QMatrix4x4 NT = this->getNT4x4_2();
    QMatrix4x4 S = this->getS4x4_2();
    S = NT*S*T;
    if(getParent()!=NULL){
        S = S*getParent()->getScaleMatrix_2();
    }
//    qDebug()<<S;
    return S;
}


QMatrix4x4 MirrorPlane::getK4x4()
{
    if(getParent()==NULL){
        QMatrix4x4 I;
        return I;
    }
    QVector3D vec = connectedHe2->getCenter()-connectedHe1->getCenter();
    return QMatrix4x4(1,0,0,vec.x(),
                      0,1,0,vec.y(),
                      0,0,1,vec.z(),
                      0,0,0,1).transposed();
}



QMatrix4x4 MirrorPlane::getNK4x4()
{
    if(getParent()==NULL){
        QMatrix4x4 I;
        return I;
    }
    QVector3D vec = connectedHe2->getCenter()-connectedHe1->getCenter();
    vec = -vec;
    return QMatrix4x4(1,0,0,vec.x(),
                      0,1,0,vec.y(),
                      0,0,1,vec.z(),
                      0,0,0,1).transposed();
}

QMatrix4x4 MirrorPlane::getMK4x4()
{
    if(getParent()==NULL){
        QMatrix4x4 I;
        return I;
    }
    QVector3D vec = connectedHe2->getMirror()->getCenter()-connectedHe1->getMirror()->getCenter();
    return QMatrix4x4(1,0,0,vec.x(),
                      0,1,0,vec.y(),
                      0,0,1,vec.z(),
                      0,0,0,1).transposed();
}

QMatrix4x4 MirrorPlane::getNMK4x4()
{
    if(getParent()==NULL){
        QMatrix4x4 I;
        return I;
    }
    QVector3D vec = connectedHe2->getMirror()->getCenter()-connectedHe1->getMirror()->getCenter();
    vec = -vec;
    return QMatrix4x4(1,0,0,vec.x(),
                      0,1,0,vec.y(),
                      0,0,1,vec.z(),
                      0,0,0,1).transposed();
}

QMatrix4x4 MirrorPlane::getMT4x4()
{
    if(getParent()==NULL){
        QMatrix4x4 I;
        return I;
    }
    QVector3D c = getConnectedHe1()->getMirror()->getCenter();
    QMatrix4x4 T(1,0,0,c.x(),
                 0,1,0,c.y(),
                 0,0,1,c.z(),
                 0,0,0,1);
    T = T.transposed();
    return T;
}

QMatrix4x4 MirrorPlane::getNMT4x4()
{
    if(getParent()==NULL){
        QMatrix4x4 I;
        return I;
    }
    QVector3D c = -getConnectedHe1()->getMirror()->getCenter();
    QMatrix4x4 T(1,0,0,c.x(),
                 0,1,0,c.y(),
                 0,0,1,c.z(),
                 0,0,0,1);
    T = T.transposed();
    return T;
}

void MirrorPlane::getM_2(double *rt)
{
    QMatrix4x4 R = getM4x4_2();
    rt[0] = R(0,0); rt[1] = R(0,1); rt[2] = R(0,2); rt[3] = R(0,3);
    rt[4] = R(1,0); rt[5] = R(1,1); rt[6] = R(1,2); rt[7] = R(1,3);
    rt[8] = R(2,0); rt[9] = R(2,1); rt[10] = R(2,2); rt[11] = R(2,3);
    rt[12] = R(3,0); rt[13] = R(3,1); rt[14] = R(3,2); rt[15] = R(3,3);

}

void MirrorPlane::getRT_2(double *rt)
{
    QMatrix4x4 R = getRotateMatrix_2();
    rt[0] = R(0,0); rt[1] = R(0,1); rt[2] = R(0,2); rt[3] = R(0,3);
    rt[4] = R(1,0); rt[5] = R(1,1); rt[6] = R(1,2); rt[7] = R(1,3);
    rt[8] = R(2,0); rt[9] = R(2,1); rt[10] = R(2,2); rt[11] = R(2,3);
    rt[12] = R(3,0); rt[13] = R(3,1); rt[14] = R(3,2); rt[15] = R(3,3);
}

void MirrorPlane::getTT_2(double *rt)
{

    QMatrix4x4 R = getT4x4_2();
//    qDebug()<<"T="<<R;
    rt[0] = R(0,0); rt[1] = R(0,1); rt[2] = R(0,2); rt[3] = R(0,3);
    rt[4] = R(1,0); rt[5] = R(1,1); rt[6] = R(1,2); rt[7] = R(1,3);
    rt[8] = R(2,0); rt[9] = R(2,1); rt[10] = R(2,2); rt[11] = R(2,3);
    rt[12] = R(3,0); rt[13] = R(3,1); rt[14] = R(3,2); rt[15] = R(3,3);
}

void MirrorPlane::getNTT_2(double *rt)
{
    QMatrix4x4 R = getNT4x4_2();
//    qDebug()<<"NT="<<R;
    rt[0] = R(0,0); rt[1] = R(0,1); rt[2] = R(0,2); rt[3] = R(0,3);
    rt[4] = R(1,0); rt[5] = R(1,1); rt[6] = R(1,2); rt[7] = R(1,3);
    rt[8] = R(2,0); rt[9] = R(2,1); rt[10] = R(2,2); rt[11] = R(2,3);
    rt[12] = R(3,0); rt[13] = R(3,1); rt[14] = R(3,2); rt[15] = R(3,3);
}

void MirrorPlane::getST_2(double *rt)
{
    QMatrix4x4 R = getScaleMatrix_2();
//    qDebug()<<"S="<<R;
    rt[0] = R(0,0); rt[1] = R(0,1); rt[2] = R(0,2); rt[3] = R(0,3);
    rt[4] = R(1,0); rt[5] = R(1,1); rt[6] = R(1,2); rt[7] = R(1,3);
    rt[8] = R(2,0); rt[9] = R(2,1); rt[10] = R(2,2); rt[11] = R(2,3);
    rt[12] = R(3,0); rt[13] = R(3,1); rt[14] = R(3,2); rt[15] = R(3,3);
}


void MirrorPlane::execute170218()
{

    double rt[16], st[16];
    getRT_2(rt);getST_2(st);
    glMultMatrixd(rt);
    glMultMatrixd(st);

}

void MirrorPlane::execute170218Reverse()
{

    double rt[16], st[16];
    getMRT(rt);getST_2(st);
    glMultMatrixd(rt);
    glMultMatrixd(st);

}

QMatrix4x4 MirrorPlane::getSmallRotateMatrix4x4()
{
    /*vvv spell vvv*/
    if(getParent()==NULL){
        QMatrix4x4 I;
        return I;
    }
//    if(connectedHe1->getHalfedge2d()->getFace()->isFront()!=connectedHe2->getHalfedge2d()->getFace()->isFront()){
//        qDebug()<<"MirrorPlane::getSmallRotateMatrix4x4:  Faces does not have same normal direction, returned I ";
//        QMatrix4x4 I;
//        return I;
//    }
    /*^^^ spell ^^^*/

    QMatrix4x4 R;

    QVector3D vec1 = connectedHe1->getVector3D().normalized(), vec2 = connectedHe2->getVector3D().normalized();
    if(QVector3D::dotProduct(vec1, vec2)<0){
        vec2 = -vec2;
    }
    QVector3D axis = QVector3D::crossProduct(vec1, vec2).normalized();
    float dot = QVector3D::dotProduct(vec1,vec2);
    double theta = -qAcos(dot);// minus tuketara tadasikunatta
//    qDebug()<<"theta = "<<theta<<", dot = "<<QVector3D::dotProduct(vec1,vec2);
    static const float eps = 1e-4;
    if(1-fabs(dot)<eps){
        QMatrix4x4 I;
        return I;
    }
    if(fabs(theta)<eps){
        QMatrix4x4 I;
        return I;
    }
    R = getRotateMatrixWithAxisAndTheta(axis, theta);


    return R;

}
QMatrix4x4 MirrorPlane::getMirrorSmallRotateMatrix4x4()// housen janakute setzokusuru edge no muki ga mukaiatteiru ka onaji houkou wo muite iruka
{
    /*vvv spell vvv*/
    if(getParent()==NULL){
        QMatrix4x4 I;
        return I;
    }
//    if(connectedHe1->getHalfedge2d()->getFace()->isFront()!=connectedHe2->getHalfedge2d()->getFace()->isFront()){
//        qDebug()<<"MirrorPlane::getSmallRotateMatrix4x4:  Faces does not have same normal direction, returned I ";
//        QMatrix4x4 I;
//        return I;
//    }
    /*^^^ spell ^^^*/

    QMatrix4x4 R;
    QVector3D vec1 = connectedHe1->getVector3D().normalized(), vec2 = connectedHe2->getVector3D().normalized();
    if(QVector3D::dotProduct(vec1, vec2)<0){
        vec2 = -vec2;
    }
    vec1.setZ(-vec1.z());vec2.setZ(-vec2.z());/////////////////////////

    QVector3D axis = QVector3D::crossProduct(vec1, vec2).normalized();
    float dot = QVector3D::dotProduct(vec1,vec2);
    double theta = -qAcos(dot);//////////////////////
    static const float eps = 1e-4;
    if(1-fabs(dot)<eps){
        QMatrix4x4 I;
        return I;
    }
    if(fabs(theta)<eps){
        QMatrix4x4 I;
        return I;
    }
    R = getRotateMatrixWithAxisAndTheta(axis, theta);

    return R;

}

QMatrix4x4 MirrorPlane::getSingleRotateMatrix()
{
    if(getParent()==NULL){
        QMatrix4x4 I;
        return I;
    }
    QMatrix4x4 T = this->getT4x4_2();
    QMatrix4x4 NT = this->getNT4x4_2();
    QMatrix4x4 R = this->getM4x4_2();
    QMatrix4x4 K = this->getK4x4();

    QMatrix4x4 sR = this->getSmallRotateMatrix4x4();
    return NT*R*sR*K*T;
}

QMatrix4x4 MirrorPlane::getSingleReverseRotateMatrix()
{
    if(getParent()==NULL){
        QMatrix4x4 I;
        return I;
    }
    QMatrix4x4 T = this->getMT4x4();
    QMatrix4x4 NT = this->getNMT4x4();
    QMatrix4x4 R = this->getNM4x4_2();
    QMatrix4x4 K = this->getMK4x4();

    QMatrix4x4 sR = this->getMirrorSmallRotateMatrix4x4();
    return NT*R*sR*K*T;
}

QMatrix4x4 MirrorPlane::getSingleRotateMatrix_old()
{
    if(getParent()==NULL){
        QMatrix4x4 I;
        return I;
    }
    QMatrix4x4 T = this->getT4x4_2();
    QMatrix4x4 NT = this->getNT4x4_2();
    QMatrix4x4 R = this->getM4x4_2();
    QMatrix4x4 K = this->getK4x4();
    return NT*R*K*T;
}

QMatrix4x4 MirrorPlane::getSingleReverseRotateMatrix_old()
{
    if(getParent()==NULL){
        QMatrix4x4 I;
        return I;
    }
    QMatrix4x4 T = this->getMT4x4();
    QMatrix4x4 NT = this->getNMT4x4();
    QMatrix4x4 R = this->getNM4x4_2();
    QMatrix4x4 K = this->getMK4x4();
    return NT*R*K*T;
}

QMatrix4x4 MirrorPlane::getSingleRotateMatrixForExport()
{
    if(getParent()==NULL){
        QMatrix4x4 I;
        return I;
    }
    QMatrix4x4 T = this->getT4x4_2().transposed();
    QMatrix4x4 NT = this->getNT4x4_2().transposed();
    QMatrix4x4 R = this->getM4x4_2().transposed();
    QMatrix4x4 K = this->getK4x4().transposed();
    return T*K*R*NT;
}

QMatrix4x4 MirrorPlane::getSingleReverseRotateMatrixForExport()
{
    if(getParent()==NULL){
        QMatrix4x4 I;
        return I;
    }
    QMatrix4x4 T = this->getMT4x4().transposed();
    QMatrix4x4 NT = this->getNMT4x4().transposed();
    QMatrix4x4 R = this->getNM4x4_2().transposed();
    QMatrix4x4 K = this->getMK4x4().transposed();
    return T*K*R*NT;
}


float MirrorPlane::getZ() const
{
    return z;
}

void MirrorPlane::setZ(float value)
{
    z = value;
}

void MirrorPlane::moveZ(float value)
{
    setZ(z+value);
}

void MirrorPlane::changeScale(float value)
{
    setScale(scale+value);
}

void MirrorPlane::draw()
{
}

MirrorPlane *MirrorPlane::getParent() const
{
    return parent;
}

void MirrorPlane::setParent(MirrorPlane *value)
{
    parent = value;
    if(parent!=NULL){
        parent->addChild(this);
        /*test*/
        setTheta(M_PI/4);
    }
    emit(updated());
}

QVector<MirrorPlane *> MirrorPlane::getChildren() const
{
    return children;
}

void MirrorPlane::setChildren(const QVector<MirrorPlane *> &value)
{
    children = value;
}

QVector<MirrorPlane *> MirrorPlane::getAncestors()
{
    QVector<MirrorPlane *> ancestors;
    if(parent!=NULL){
        ancestors.push_back(parent);
        ancestors += parent->getAncestors();
    }
//    qDebug()<<ancestors.size();
    return ancestors;
}

QVector<MirrorPlane *> MirrorPlane::getMirrorFamily()
{
    QVector<MirrorPlane *>mf;
    mf.push_back(this);
    if(parent!=NULL){
        mf+=parent->getMirrorFamily();
    }
    return mf;
}


QVector3D MirrorPlane::getCenter() const
{
    return center;
}

void MirrorPlane::addChild(MirrorPlane *value)
{
    children.push_back(value);
}

void MirrorPlane::removeChild(MirrorPlane *value)
{
    children.removeOne(value);
}

void MirrorPlane::setCenter(const QVector3D &value)
{
    center = value;
}

QVector2D MirrorPlane::getAxis() const
{
    return axis;
}

void MirrorPlane::setAxis(const QVector2D &value)
{
    axis = value.normalized();
}

double MirrorPlane::getTheta() const
{
    return theta;
}

void MirrorPlane::setTheta(double value)
{
    theta = value;
    emit(updated());
}

void MirrorPlane::changeTheta(double value)
{
    setTheta(theta+value);
}

void MirrorPlane::setParentAndAxis(MirrorPlane *mp, QVector2D ax)
{
    setParent(mp);
    setAxis(ax);
}

Halfedge3d *MirrorPlane::getConnectedHe2() const
{
    return connectedHe2;
}

void MirrorPlane::setConnectedEdges(Halfedge3d *heChild, Halfedge3d *heParent)
{
    setConnectedHe1(heChild);
    setConnectedHe2(heParent);
    MirrorPlane *mirrorParent = heParent->getModel()->getMirrorPlane();
    setParent(mirrorParent);

    QVector2D ax = heChild->getHalfedge2d()->getVec2D();
//    setAxis(ax);
    QVector2D c = heChild->getHalfedge2d()->getCenter();
//    setCenter(c);
}

void MirrorPlane::setConnectedHe2(Halfedge3d *value)
{
    connectedHe2 = value;
}

Halfedge3d *MirrorPlane::getConnectedHe1() const
{
    return connectedHe1;
}

void MirrorPlane::setConnectedHe1(Halfedge3d *value)
{
    connectedHe1 = value;
}


void MirrorPlane::setParentAndAxisAndCenter(MirrorPlane *mp, QVector2D ax, QVector3D c)
{
    setParentAndAxis(mp, ax);
    setCenter(c);

}



