#ifndef MYCAMERA3D_H
#define MYCAMERA3D_H
#include <QVector3D>
#include <QMatrix4x4>
class MyCamera3d
{
public:
    MyCamera3d(float px, float py, float pz, float lx, float ly, float lz);
    void projection(int wx, int wy);

    void updateRotation(int x, int y);
    void setCV(int x, int y);
    void endRotation();
    void init();
    void scale(int d);
    void move(float dx, float dy);
    void debugQ(double q[]);
    void debugRT();
//private:
    QVector3D position, lookAtPoint;
    QVector3D cv, tv, sv, tmp_v;
    QVector3D initPos, initLook;
    double scl;
    double cq[4], tq[4], rt[16];
    static void qmul(double r[], const double p[], const double q[]);
    void  qrot(double r[], double q[]);

    void setXY(float _x, float _y);
    void setZ(float _z);
    void scaleZ(float d);
    /* test */
    void perspective(int wx, int wy);
    void modelview();
    void modelview2();
    QMatrix4x4 getRT();
};

#endif // MYCAMERA3D_H
