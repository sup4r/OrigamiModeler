#pragma once
#include "mycamera3d.h"
#include <Windows.h>
#include <gl/GLU.h>
#include <QtMath>
#include <QDebug>
MyCamera3d::MyCamera3d(float px, float py, float pz, float lx, float ly, float lz)
{
    scl = 1.0 / (double)512;// !!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!
    initPos  =  QVector3D(px, py, pz);
    initLook  = QVector3D(lx, ly, lz);
    init();


}

void MyCamera3d::init(){
    position = initPos;
    lookAtPoint = initLook;
    cv = tv = sv = QVector3D(0, 0, 0);
    cq[0] = 1.0; cq[1]= 0.0; cq[2]= 0.0; cq[3] = 0.0;
    tq[0] =1.0; tq[1] = 0.0; tq[2]=0.0; tq[3] = 0.0;
    qrot(rt, cq);
    //debugRT();
}
void MyCamera3d::perspective(int wx, int wy){
    gluPerspective(60.0, wx/(float)wy, 1.0, 10000.0);
}
void MyCamera3d::modelview(){
    glTranslated(-position.x(), -position.y(), -position.z());

    /* Camera Rotation*/
    glTranslated(lookAtPoint.x(), lookAtPoint.y(), lookAtPoint.z());
    glMultMatrixd(rt);
//    glRotated(45, 1,0,0);//20170323
//    glRotated(45, 0,1,0);//20170323
    glTranslated(-lookAtPoint.x(), -lookAtPoint.y(), -lookAtPoint.z());

    // 2D NO VIEW NI AWASERU
    glTranslated(lookAtPoint.x(), lookAtPoint.y(), lookAtPoint.z());
    glRotated(180, 1,0,0);
    glTranslated(-lookAtPoint.x(), -lookAtPoint.y(), -lookAtPoint.z());
}
void MyCamera3d::modelview2(){
    glMultMatrixd(rt);
}

void MyCamera3d::projection(int wx, int wy)
{
    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();
    perspective(wx, wy);
    glMatrixMode(GL_MODELVIEW);
    glLoadIdentity();
    modelview();
}

/* --- Move --- */
void MyCamera3d::move(float dx, float dy){

}
/* --- Scale --- */
void MyCamera3d::scale(int d){
    const float scaleScale = -1.0;
    position.setZ(position.z()+d*scaleScale);
}
/* --- Rotation --- */
void MyCamera3d::qmul(double r[], const double p[], const double q[])
{
    r[0] = p[0] * q[0] - p[1] * q[1] - p[2] * q[2] - p[3] * q[3];
    r[1] = p[0] * q[1] + p[1] * q[0] + p[2] * q[3] - p[3] * q[2];
    r[2] = p[0] * q[2] - p[1] * q[3] + p[2] * q[0] + p[3] * q[1];
    r[3] = p[0] * q[3] + p[1] * q[2] - p[2] * q[1] + p[3] * q[0];
}

void MyCamera3d::qrot(double r[], double q[])
{
    double x2 = q[1] * q[1] * 2.0;
    double y2 = q[2] * q[2] * 2.0;
    double z2 = q[3] * q[3] * 2.0;
    double xy = q[1] * q[2] * 2.0;
    double yz = q[2] * q[3] * 2.0;
    double zx = q[3] * q[1] * 2.0;
    double xw = q[1] * q[0] * 2.0;
    double yw = q[2] * q[0] * 2.0;
    double zw = q[3] * q[0] * 2.0;

    r[0] = 1.0 - y2 - z2;
    r[1] = xy + zw;
    r[2] = zx - yw;
    r[4] = xy - zw;
    r[5] = 1.0 - z2 - x2;
    r[6] = yz + xw;
    r[8] = zx + yw;
    r[9] = yz - xw;
    r[10] = 1.0 - x2 - y2;
    r[3] = r[7] = r[11] = r[12] = r[13] = r[14] = 0.0;
    r[15] = 1.0;
}

void MyCamera3d::setCV(int x, int y){
    cv.setX(x);cv.setY(y);cv.setZ(0);
}

void MyCamera3d::updateRotation(int x, int y){
    QVector3D dv(x, y, 0);
    dv -= cv;
    dv *= scl;
    double len = dv.length();
    const double cap = 1e-10;
   // qDebug()<<len;
    if(len>cap){
        double dq[4] = {cos(len*M_PI), dv.y()*sin(len*M_PI) / len, dv.x()*sin(len*M_PI) / len, 0.};
        qmul(tq, dq, cq);
        qrot(rt, tq);
    }
}

void MyCamera3d::endRotation(){
    cq[0] = tq[0];
    cq[1] = tq[1];
    cq[2] = tq[2];
    cq[3] = tq[3];
    //debugRT();
}

/* --- End Rotation ---*/

/* --- Debug Tool --- */
void MyCamera3d::debugQ(double q[])
{
    qDebug()<<q[0]<<q[1]<<q[2]<<q[3];
}

void MyCamera3d::debugRT(){
    qDebug()<<rt[0]<<rt[1]<<rt[2]<<rt[3];
    qDebug()<<rt[4]<<rt[5]<<rt[6]<<rt[7];
    qDebug()<<rt[8]<<rt[9]<<rt[10]<<rt[11];
    qDebug()<<rt[12]<<rt[13]<<rt[14]<<rt[15];
}

QMatrix4x4 MyCamera3d::getRT(){
    return QMatrix4x4 (
                rt[0], rt[1], rt[2], rt[3],
            rt[4],rt[5], rt[6], rt[7],
            rt[8], rt[9], rt[10], rt[11],
            rt[12], rt[13], rt[14], rt[15]
            );
}

/* --- End Debug Tool --- */
