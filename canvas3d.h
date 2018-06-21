#pragma once
#ifndef CANVAS3D_H
#define CANVAS3D_H

#include "vertex3d.h"
#include "halfedge3d.h"
#include "face3d.h"
#include "model3d.h"
#include "canvasbase.h"
#include "mycamera3d.h"

#include <QObject>
#include <QWidget>
#include <QPoint>
#include <QMouseEvent>
#include <QPair>
#include <QImage>
#include <QOpenGLTexture>
class Face2d;
class Canvas2d;
class Annotation;

class Canvas3d : public CanvasBase
{
    Q_OBJECT

public:
    Canvas3d(QWidget *parent = 0);
private:
    // dont touch directly
    Canvas2d *canvas2d;
public:
    void setCanvas2d(Canvas2d *c);
    Canvas2d *getCanvas2d();

    Model3d *getModel() const;
    void setModel(Model3d *value);
    Model3d *createModel();
    bool getIsDebugMode() const;
    void setIsDebugMode(bool value);

protected:
    void initializeGL();
    void resizeGL(int w, int h);
    void paintGL();
    void mousePressEvent(QMouseEvent *event);
    void mouseMoveEvent(QMouseEvent *event);
    void mouseReleaseEvent(QMouseEvent *event);
    void mouseDoubleClickEvent(QMouseEvent *event); 
    void wheelEvent(QWheelEvent *wEvent);
    void keyPressEvent(QKeyEvent *event);
    void enterEvent(QEvent *event);
private:
    /* Create Object*/ //tukattenai -> 7/20 Edited for PleatFoldBridge; DONT model->addObject here, use Canvas3d::AddFace later
    Vertex3d *createVertex(const QVector3D &pos);
    Halfedge3d *createHalfedge(Vertex3d *v1, Vertex3d *v2);
    Face3d *createFace(Halfedge3d *he);
    /* Picking */
    Vertex3d *vertexAtPosition(const QPoint &pos);
    Halfedge3d *halfedgeAtPosition(const QPoint &pos);
    Face3d *faceAtPosition(const QPoint &pos);

    /* Mouse Event*/
    void leftMousePressEvent(QMouseEvent *event);
    void leftMouseMoveEvent(QMouseEvent *event);
    void leftMouseReleaseEvent(QMouseEvent *event);
    void rightMousePressEvent(QMouseEvent *event);
    void rightMouseMoveEvent(QMouseEvent *event);
    void rightMouseReleaseEvent(QMouseEvent *event);
    void middleMousePressEvent(QMouseEvent *event);
    void middleMouseMoveEvent(QMouseEvent *event);
    void middleMouseReleaseEvent(QMouseEvent *event);

    /* Drawing */
    void draw();
    void drawGrid();
    void drawAxis();
    void drawArrow();
    void drawCone(const int r, const float h, const float n);
    void drawAllModels();
    void drawAllMirrorModels();
    void drawTest();
    /* Selection */
    QVector<Vertex3d*> verticesSelected;
    QVector<Halfedge3d*> halfedgesSelected;
    QVector<Face3d*> facesSelected;
    void selectVertex(Vertex3d *v);
    void selectHalfedge(Halfedge3d* he);
    void selectFace(Face3d *f);
    void unselectVertex(Vertex3d *v);
    void unselectHalfedge(Halfedge3d *he);
    void unselectFace(Face3d* f);
    void unselectAll();
    bool isSelected(Vertex3d *v);
    bool isSelected(Halfedge3d *he);
    bool isSelected(Face3d *f);
    /* Member Variable*/
    Model3d *model;
public:
    QVector<Model3d*>models;
    QOpenGLTexture *getTexture() const;
    void setTexture(QOpenGLTexture *value);

    QOpenGLTexture *loadImage(QString filename);
private:
    void addModel(Model3d* m);
    MyCamera3d *camera;
    QPoint lastPos;
    Vertex3d *lastVertex;
    Halfedge3d *firstHalfedge;
    Halfedge3d *lastHalfedge;
    GLint viewport[4];
    bool isDebugMode;
    enum SelectMode { ALL, VERTEX, HALFEDGE, FACE};
    enum SelectMode selectMode;

    int pickingObject(const QPoint &pos, void (Model3d::*drawObjectModel3d)());

    int pickingModel(const QPoint &pos);

    void pairingHalfedges(Halfedge3d *he1, Halfedge3d *he2);
    /* --- draw flag --- */
    bool isDrawAxis, isDrawGrid, isDrawNormal, isDrawEdgeConnection, isDrawVertex, isDrawEdge, isDrawFace;

    /* Create Bridge */
    QVector<Face3d*> bridgeFaces;

    Face3d *createBridge(Halfedge3d *connectedHe1, Halfedge3d *connectedHe2, int div = 0);
    void addBridge(Face3d *f);
    void deleteBridge(Face3d *f);
    void deleteBridge(Halfedge3d *connectedHe1);
    bool isBridgeFace(Face3d *f);

    void movePleatBridges(Halfedge3d *connectedHe1, Halfedge3d *connectedHe2);
    void movePleatBridges170118(Halfedge3d *connectedHe1, Halfedge3d *connectedHe2);
    void movePleatBridges_new_old(Halfedge3d *connectedHe1, Halfedge3d *connectedHe2);
    void movePleatBridges170225(Halfedge3d *connectedHe1, Halfedge3d *connectedHe2);
    bool isVerticesOverlapping(Vertex3d *v1, Vertex3d *v2);
    void compressVertices(Vertex3d *v1, Vertex3d *v2);

    void createPleatBridges_old(Halfedge3d *connectedHe1, Halfedge3d *connectedHe2);
    void createOrikomiFace(Halfedge3d *connctedHe1, QVector3D v);
    /* --- test --- */
    void showProjectionMatrix();
    void getProjectionMatrix(GLfloat *m);
    QMatrix4x4 getProjectionQMatrix4x4();
    QMatrix4x4 getProjectionQMatrix4x4_2();
    void getModelviewMatrix(GLfloat *m);
    void showModelviewMatrix();
    QMatrix4x4 getModelviewQMatrix4x4_2();
    void getProjectionMatrix2(GLfloat *m);
    void getModelviewMatrix2(GLfloat *m);
    void showProjectionMatrix2();
    void showModelviewMatrix2();
    QMatrix4x4 getViewportQMatrix4x4();
    QMatrix4x4 getViewportQMatrix4x4_2();
    QMatrix4x4 getUnkownQMatrix4x4();
    QMatrix4x4 getViewportQMatrix4x4_3();
    QMatrix4x4 getModelviewQMatrix4x4_3();
    QMatrix4x4 getModelviewQMatrix4x4();
    QMatrix4x4 getViewportQMatrix4x4_4();


    void moveHalfedgeSVEVToEVSV(Halfedge3d *heMove, Halfedge3d *heBase, double alpha = 0.1);
    void moveHalfedgeSVEVToEVSVXY(Halfedge3d *heMove, Halfedge3d *heBase, double alpha = 0.1);
    void moveHalfedgeSVEVToSVEV(Halfedge3d *heMove, Halfedge3d *heBase, double alpha = 0.1);
    void moveHalfedgeSVEVToSVEVXY(Halfedge3d *heMove, Halfedge3d *heBase, double alpha = 0.1);

    void setHalfedgePair(Halfedge3d *he1, Halfedge3d *he2);
    void deleteVertex(Vertex3d *v);
    void deleteHalfedge(Halfedge3d *he);
    void deleteFace(Face3d *f);

    void connectEdges(Halfedge3d *connectedHe1, Halfedge3d *connectedHe2);
    void edgeCollapseOnInsideReverseFoldBridge(Face3d *bridge);
    void collapseEdgeInBridge(Halfedge3d *he);

    /* 172028 */
    QVector<Annotation*> *getAnnotations();

    /* 170428 */
    QImage image;
    QOpenGLTexture *texture;
public:
    void setupTexture(QString filename = "./DSC_0274.jpg");
signals:
    void faceAdded();
    void updated();
public slots:
    void addFace(Face3d *face3d);
    void addFaceFromC2d(Face3d *face3d);
    void slotKeyPressEvent(QKeyEvent *event);

    /* --- selection flag --- */
    void allSelectMode();
    void vertexSelectMode();
    void halfedgeSelectMode();
    void faceSelectMode();

    /* --- set draw flag --- */
    void setDrawAxisFlag(bool flag);
    void setDrawGridFlag(bool flag);
    void setDrawNormalFlag(bool flag);
    void setDrawEdgeConnectionFlag(bool flag);
    void setDrawVertexFlag(bool flag);
    void setDrawEdgeFlag(bool flag);
    void setDrawFaceFlag(bool flag);

    /* --- Function --- */
    void exportOBJ(QString filename);

    void exportOBJ170223(QString filename);
    void exportVertices(QTextStream *out);
    void exportFaces(QTextStream *out);
    Model3d *mergeModels();

    void deleteFacesSelected();

    void flipFacesSelected();

    void bridgeAllOverlapEdges();

    void moveUnderVertexPositionIfThereAreZOverlapVerticesInTwoFaces(Face3d *upperFace, Face3d *lowerFace);


    /* --- glue bridge --- */
    void createGlueBridge(Halfedge2d* he2d);
    void deleteGlueBridge(Halfedge2d* he2d);
    Face3d *getBridge(Halfedge3d *connectedHe1);
    void compressGlueBridge(Halfedge2d *he2d, const double d);
    /* --- select/unselect slot --- */
    void selectVertexSync2D(Vertex2d *v2d);
    void selectHalfedgeSync2D(Halfedge2d *he2d);
    void selectFaceSync2D(Face2d *f2d);
    void unselectVertexSync2D(Vertex2d *v2d);
    void unselectHalfedgeSync2D(Halfedge2d *he2d);
    void unselectFaceSync2D(Face2d *f2d);
    /* --- inside reverse fold bridge --- */
    void createInsideReverseFoldBridge(Halfedge2d *he1, Halfedge2d *he2);
    void deleteInsideReverseFoldBridge(Halfedge2d *he1, Halfedge2d *he2);
    /* --- pleat fold bridge --- */
    void createPleatFoldBridge(Halfedge2d *he1, Halfedge2d *he2);
    void deletePleatReverseFoldBridge(Halfedge2d *he1, Halfedge2d *he2);
    /* --- turn fold bridge --- */
    void createTurnFoldBridge(Halfedge2d *he1, Halfedge2d *he2);
    void deleteTrunFoldBridge(Halfedge2d *he1, Halfedge2d *he2);

    /* --- sink fold bridge --- */
    void createSinkFoldBridge(Halfedge2d *he2d);
    void deleteSinkFoldBridge(Halfedge2d *he2d);
    /* --- pigs legs bridge --- */
    void createPigsLegBridge(Halfedge2d *he1, Halfedge2d *he2, Halfedge2d *he3);
    void deletePigsLegBridge(Halfedge2d *he1, Halfedge2d *he2, Halfedge2d *he3);
    /* --- orikomi edge bridge --- */
    void createOrikomiEdgeBridge(Halfedge2d *he2d);
    void deleteOrikomiEdgeBridge(Halfedge2d *he2d);

    /* --- merge --- */
    void edgeCollapse(Halfedge3d * he);

    /* mirror connect */
    void createMirrorConnect(QVector<Halfedge2d*> hes);
    void deleteMirrorConnect(QVector<Halfedge2d*> hes);
    /* mirror rotate */
    void rotateMirrorPlane(MirrorPlane *mirrorPlane, int theta);
    void scaleMirrorPlane(MirrorPlane *mirrorPlane, int scale);

    /* addMirrorFace 171219 */
    void addMirrorFace(Face3d *face3d);
private slots:
    void updateAnnotationBridge();
    void updateOrikomiBridgeIfItIsNeed(Halfedge3d *he);
private:
    bool isNeedToUpdateOrikomiBridge(Halfedge3d *he);
    void updateOrikomiBridge(Halfedge3d *he);
    QPair<QVector3D, QVector3D> getOrikomiEdgeBridgesP1P2(Halfedge3d *he3d1);
signals:
    /* --- select/unselect signal --- */
    void vertexSelected(Vertex3d*);
    void halfedgeSelected(Halfedge3d*);
    void faceSelected(Face3d*);
    void vertexUnselected(Vertex3d*);
    void halfedgeUnselected(Halfedge3d*);
    void faceUnselected(Face3d*);
};

#endif // CANVAS3D_H
