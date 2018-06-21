#ifndef MIRRORPLANE_H
#define MIRRORPLANE_H

#include <QObject>
#include <QVector3D>
#include <QVector2D>

#include <QMatrix4x4>
class Halfedge3d;

class MirrorPlane : public QObject
{
    Q_OBJECT
public:
    explicit MirrorPlane(QObject *parent = 0);

    QVector3D getPosition() const;
    void setPosition(const QVector3D &value);

    double getAlpha() const;
    void setAlpha(double value);
    void addAlpha(double value);

    double getBeta() const;
    void setBeta(double value);
    void addBeta(double value);

    double getGamma() const;
    void setGamma(double value);
    void addGamma(double value);

    double getScale() const;
    void setScale(double value);


    void execute();// old function 170215. this function uses position, alpha, beta, gamma, z, scale.

    QMatrix4x4 getM4x4();
    QMatrix4x4 getNM4x4();
    QMatrix4x4 getT4x4();
    QMatrix4x4 getNT4x4();
    QMatrix4x4 getRotateMatrix();
    QMatrix4x4 getS4x4();
    QMatrix4x4 getScaleMatrix();
    void getM(double *rt);
    void getRT(double *rt);
    void getTT(double *rt);
    void getNTT(double *rt);
    void getST(double *rt);
    void execute170215();// old function 170216. u should use getRT and getST in Canvas3d then glMultMatrixd;

    QMatrix4x4 getM4x4_2();
    QMatrix4x4 getNM4x4_2();
    QMatrix4x4 getT4x4_2();
    QMatrix4x4 getNT4x4_2();
    QMatrix4x4 getRotateMatrix_2();

    QMatrix4x4 getS4x4_2();
    QMatrix4x4 getScaleMatrix_2();

    void getM_2(double *rt);
    void getRT_2(double *rt);
    void getTT_2(double *rt);
    void getNTT_2(double *rt);
    void getST_2(double *rt);

    QMatrix4x4 getK4x4();
    QMatrix4x4 getNK4x4();
    QMatrix4x4 getMK4x4();
    QMatrix4x4 getNMK4x4();

    QMatrix4x4 getMT4x4();
    QMatrix4x4 getNMT4x4();
    void execute170218();// test function
    QMatrix4x4 getReverseRotateMatrix();
    void getMRT(double *rt);
    void execute170218Reverse();



    QMatrix4x4 getSingleRotateMatrix();
    QMatrix4x4 getSingleReverseRotateMatrix();

    QMatrix4x4 getSingleRotateMatrixForExport();
    QMatrix4x4 getSingleReverseRotateMatrixForExport();

    /* 140417 back up */
    QMatrix4x4 getSingleReverseRotateMatrix_old();
    QMatrix4x4 getSingleRotateMatrix_old();

    float getZ() const;
    void setZ(float value);
    void moveZ(float value);

    void changeScale(float value);

    void draw();

    MirrorPlane *getParent() const;

    QVector2D getAxis() const;

    double getTheta() const;
    void setTheta(double value);
    void changeTheta(double value);


    QVector3D getCenter() const;

    void addChild(MirrorPlane *value);
    void removeChild(MirrorPlane *value);

    QVector<MirrorPlane *> getChildren() const;
    void setChildren(const QVector<MirrorPlane *> &value);

    QVector<MirrorPlane *> getAncestors();
    QVector<MirrorPlane *> getMirrorFamily();
    Halfedge3d *getConnectedHe1() const;


    Halfedge3d *getConnectedHe2() const;

    void setConnectedEdges(Halfedge3d *heChild, Halfedge3d *heParent);//All variable(cHe1,cHe2,parent,axis,center) setter 170218, but should not use axis and center,maybe.

    QMatrix4x4 getM4x4_2_old();
    QMatrix4x4 getNM4x4_2_old();

private:
    QVector3D position;
    float z;
    double alpha, beta, gamma; //degree


    double scale;
    MirrorPlane *parent;
    void setParent(MirrorPlane *value);
    QVector<MirrorPlane*> children;

    /* rotate around an axis 170215 */
    QVector2D axis;
    QVector3D center;
    void setAxis(const QVector2D &value);
    void setCenter(const QVector3D &value);
    void setParentAndAxis(MirrorPlane *mp, QVector2D ax);
    double theta; // raddian
    void setParentAndAxisAndCenter(MirrorPlane *mp, QVector2D ax, QVector3D c);
    /* connected edges 170218 */
    Halfedge3d *connectedHe1, *connectedHe2;// 1->child's(self), 2->parent's
    void setConnectedHe2(Halfedge3d *value);
    void setConnectedHe1(Halfedge3d *value);

    QMatrix4x4 getRotateMatrixWithAxisAndTheta(QVector3D axis, double theta);
    QMatrix4x4 getSmallRotateMatrix4x4();//170417
    QMatrix4x4 getMirrorSmallRotateMatrix4x4();
signals:
    void updated();
public slots:

};



#endif // MIRRORPLANE_H
