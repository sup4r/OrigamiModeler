#ifndef CANVAS2D_H
#define CANVAS2D_H

#include "vertex2d.h"
#include "halfedge2d.h"
#include "face2d.h"
#include "model2d.h"
#include "mode.h"
#include "canvasdialog2d.h"
#include "canvasbase.h"
#include <QObject>
#include <QWidget>
#include <QOpenGLWidget>
#include <QPoint>
#include <QOpenGLTexture>
#include <QImage>
/* tuika @ 2018 */
#include <opencv2/opencv.hpp>
#include <opencv2/highgui/highgui.hpp>
#include <opencv2/imgproc/imgproc.hpp>
#include <vector>
#include <string>
/* tuika @ 2018 fin */

class Annotation;
class GlueEdgeAnnotation;
class InsideReverseFoldAnnotation;
class PleatFoldAnnotation;
class TurnFoldAnnotation;
class Face3d;
class Canvas3d;
class OutsideReverseFoldAnnotation;
class SinkFoldAnnotation;
class PigsLegAnnotation;
class OrikomiEdgeAnnotation;
class MirrorConnectAnnotation;
class MirrorPlane;


class Canvas2d : public CanvasBase
{
    Q_OBJECT

    /* tuika @ 2018 */
private:
    QVector<QPoint> kouhoPoints;
    QPoint kouhoPoint;
    std::string filePath;
    int saveAndLoadCount = 0;
    void trashCurrentModel2();
    void positionAdjust();
    void adjust_parallel(Halfedge2d *he, Halfedge2d *he2);
    void adjust_nearPoint(Halfedge2d *he, Halfedge2d *he2);
    Vertex2d getCenter(Face2d *f);
    QVector<Halfedge2d*>  halfFoldEdges;
    QVector<int> halfFoldEdgesAmount;
    QPoint startPos;
    QPoint goalPos;
    void autoAnnotation();
    QPoint clossPoint(QPoint p1, QPoint p2,QPoint p3, QPoint p4);
    bool is_parallel(Halfedge2d *he, Halfedge2d *he2);
    void setID();
    void createHalfFoldWithStackEdges();
    void removeHalfFoldWithStackEdges();
    void keyPress_B();
public slots:
    void autoVertex();
public:
    void loadFromFile2();
    void saveToFile2();
    void drawKouhoPoints();
    QPoint snapPos2();
public slots:
    void loadFromFile3();
    /* tuika @ 2018 fin */
public:
    Canvas2d(QWidget *parent = 0);
private:
    // Dont touch directly
    Canvas3d *canvas3d;
public:
    void setCanvas3d(Canvas3d *c);
    void drawMirrorTimes();
    int getCurrentLayerNumber() const;
    void setCurrentLayerNumber(int value);

    bool getIsBranch() const;
    void setIsBranch(bool value);

    /* save/laod */
    Model2d *getModel() const;
    void setModel(Model2d *value);

    void createPairModels();
    void drawAllModels();
    void drawAllAnnotations();

    /* mirror connection */
    QVector<MirrorConnectAnnotation*> getMirrorConnections() const;
    void setMirrorConnections(const QVector<MirrorConnectAnnotation*> &value);
    void addMirrorConnection(MirrorConnectAnnotation *value);
    int getSnapFlag() const;
    void setSnapFlag(int value);


    bool getIsDebugMode() const;
    void setIsDebugMode(bool value);

    /* 170228 */
    QVector<Annotation*> *getAnnotations();


protected:
    void initializeGL();
    void resizeGL(int w, int h);
    void paintGL();
    void mousePressEvent(QMouseEvent *event);
    void mouseMoveEvent(QMouseEvent *event);
    void mouseReleaseEvent(QMouseEvent *event);
    void mouseDoubleClickEvent(QMouseEvent *event);
    void keyPressEvent(QKeyEvent *event);
    void keyReleaseEvent(QKeyEvent *event);
    void enterEvent(QEvent *event);
private:
    Model2d *createModel();
    void setCurrentModelsAsPair();
    void changeModels();
    void backModels();
    Vertex2d *createVertex(const QPoint &pos);
    Halfedge2d *createHalfedge(Vertex2d *v1, Vertex2d *v2);
    Face2d *createFace(Halfedge2d *he);

    // General Members

    Model2d *model;
    QVector<Model2d*>models;
    void addModel(Model2d* m);
    QImage image;
    QOpenGLTexture *texture;
public:
    QOpenGLTexture *getTexture();
    QImage getImage() const;
    void setImage(const QImage &value);

    bool getIsSkelton() const;
    void setIsSkelton(bool value);

private:
    QPoint lastPos;
    int dGrid;
    bool isShiftKeyBeenPressing;
    bool isDebugMode;
    // Picking
    int pickingObject(const QPoint &pos, void (Model2d::*drawObject)());
    QVector<int> pickingObjects(const QPoint &pos, void (Model2d::*drawObject)());
    Vertex2d *vertexAtPosition(const QPoint &pos);
    Halfedge2d *halfedgeAtPosition(const QPoint &pos);
    Face2d *faceAtPosition(const QPoint &pos);
    Vertex2d *vertexAtPositionOLD(const QPoint &pos);
    Halfedge2d *halfedgeAtPositionOLD(const QPoint &pos);

    // Draw
    void draw();
    void drawGrid();
    void drawAxis();
    void drawImage();
    void drawSnapPoint();
    void drawTest();
    void drawActiveModel();
    bool isSkelton;
    // Mode Setting
    enum Mode mode;
    void holdCurrentMode();
    void releaseCurrentMode();
    // Execute Mouse Func
    QVector<void (Canvas2d::*)(QMouseEvent*)> leftMousePressEventFunctions;
    QVector<void (Canvas2d::*)(QMouseEvent*)> rightMousePressEventFunctions;
    QVector<void (Canvas2d::*)(QMouseEvent*)> middleMousePressEventFunctions;
    QVector<void (Canvas2d::*)(QMouseEvent*)> leftMouseReleaseEventFunctions;
    QVector<void (Canvas2d::*)(QMouseEvent*)> rightMouseReleaseEventFunctions;
    QVector<void (Canvas2d::*)(QMouseEvent*)> middleMouseReleaseEventFunctions;
    QVector<void (Canvas2d::*)(QMouseEvent*)> leftMouseMoveEventFunctions;
    QVector<void (Canvas2d::*)(QMouseEvent*)> rightMouseMoveEventFunctions;
    QVector<void (Canvas2d::*)(QMouseEvent*)> middleMouseMoveEventFunctions;
    QVector<void (Canvas2d::*)(QMouseEvent*)> mouseDoubleClickEventFunctions;
    // init mouse func
    void initMouseFunctions();
    void nullMouseEvent(QMouseEvent*);
    // Edit Mode Members
    void leftMousePressEventOfEditMode(QMouseEvent *event);
    void rightMousePressEventOfEditMode(QMouseEvent *event);
    void middleMousePressEventOfEditMode(QMouseEvent *event);
    void leftMouseReleaseEventOfEditMode(QMouseEvent *event);
    void rightMouseReleaseEventOfEditMode(QMouseEvent *event);
    void middleMouseReleaseEventOfEditMode(QMouseEvent *event);
    void leftMouseMoveEventOfEditMode(QMouseEvent *event);
    void rightMouseMoveEventOfEditMode(QMouseEvent *event);
    void middleMouseMoveEventOfEditMode(QMouseEvent *event);
    void mouseDoubleClickEventOfEditMode(QMouseEvent *event);
    // Create Mode Members
    Vertex2d *lastVertex;
    Halfedge2d *firstHalfedge;
    Halfedge2d *lastHalfedge;
    int snapFlag;
    int mirrorTimes;
    void setMirrorTimes(int value);
    int getMirrorTimes();
    void clickCreateVertex(const QPoint &pos);
    void leftMousePressEventOfCreateMode(QMouseEvent *event);
    void rightMousePressEventOfCreateMode(QMouseEvent *event);
    void middleMousePressEventOfCreateMode(QMouseEvent *event);
    void leftMouseReleaseEventOfCreateMode(QMouseEvent *event);
    void rightMouseReleaseEventOfCreateMode(QMouseEvent *event);
    void middleMouseReleaseEventOfCreateMode(QMouseEvent *event);
    void leftMouseMoveEventOfCreateMode(QMouseEvent *event);
    void rightMouseMoveEventOfCreateMode(QMouseEvent *event);
    void middleMouseMoveEventOfCreateMode(QMouseEvent *event);
    void mouseDoubleClickEventOfCreateMode(QMouseEvent *event);

    void cancelCurrentCreation();


    QVector<Vertex2d*>creatingVertices;
    QVector<Halfedge2d*>creatingHalfedges;
    void addCreatingVertices(Vertex2d *v);
    void addCreatingHalfedges(Halfedge2d *he);
    void removeCreatingVertices(Vertex2d *v);
    void removeCreatinghalfedges(Halfedge2d *he);
    void clearCreatingObjects();
    void drawCreatingObjects();

    // Glue Mode Members
    QVector<Halfedge2d*> glueEdges;
    void clearGlueEdges();
    void addGlueEdges(Halfedge2d *he);
    void removeGlueEdges(Halfedge2d *he);
    void drawGlueEdges();

    void leftMousePressEventOfGlueMode(QMouseEvent *event);
    void rightMousePressEventOfGlueMode(QMouseEvent *event);
    void middleMousePressEventOfGlueMode(QMouseEvent *event);
    void leftMouseReleaseEventOfGlueMode(QMouseEvent *event);
    void rightMouseReleaseEventOfGlueMode(QMouseEvent *event);
    void middleMouseReleaseEventOfGlueMode(QMouseEvent *event);
    void leftMouseMoveEventOfGlueMode(QMouseEvent *event);
    void rightMouseMoveEventOfGlueMode(QMouseEvent *event);
    void middleMouseMoveEventOfGlueMode(QMouseEvent *event);
    void mouseDoubleClickEventOfGlueMode(QMouseEvent *event);

    // Select Object
    QVector<Vertex2d*> verticesSelected;
    QVector<Halfedge2d*> halfedgesSelected;
    QVector<Face2d*> facesSelected;
    QVector<Annotation*> annotationsSelected;
    /* Selection */
    GLObject *clickSelectObject(QMouseEvent *event);
    Vertex2d *clickSelectVertex(QMouseEvent *event);
    Halfedge2d *clickSelectHalfedge(QMouseEvent *event);
    Face2d *clickSelectFace(QMouseEvent *event);
    Annotation *clickSelectAnnotation(QMouseEvent *event);
    // 9/14
    QVector<Halfedge2d *> allHalfedgesAtPosition(const QPoint &pos);
    Halfedge2d *clickSelectHalfedge2(QMouseEvent *event);
    void selectVertex(Vertex2d *v);
    void selectHalfedge(Halfedge2d* he);
    void selectFace(Face2d *f);
    void selectAnnotaiton(Annotation *a);
    void unselectVertex(Vertex2d *v);
    void unselectHalfedge(Halfedge2d *he);
    void unselectFace(Face2d* f);
    void unselectAnnotation(Annotation *a);
    void unselectAll();
    void unselectAllVertices();
    void unselectAllHalfedges();
    void unselectAllFaces();
    void unselectAllAnnotations();
    bool isSelected(Vertex2d *v);
    bool isSelected(Halfedge2d *he);
    bool isSelected(Face2d *f);
    bool isSelected(Annotation *a);
    void debugPrintSlection();
    // snap
    QPoint snapPos(QPoint pos);
    // Annotation
    QVector<QVector<Annotation*>*> annotationListList;
    QVector<Annotation*> *createAnnotationList();
    void addAnnotationList(QVector<Annotation*> *annotationList);
    void setAnnotationList(QVector<Annotation*> *annotationList);

    QVector<Annotation*> *annotations;

    void drawAnnotations();
    Annotation *annotationAtPosition(const QPoint &pos);
    void addAnnotation(Annotation *a);
    void removeAnnotation(Annotation *a);
    void deleteAnnotation(Annotation *a);
    // GlueEdgeAnnotation
    GlueEdgeAnnotation *createGlueEdgeAnnotation(Halfedge2d *he);
    // InsideReverseFoldAnnotation
    InsideReverseFoldAnnotation *createInsideReverseFold(Halfedge2d *he1, Halfedge2d *he2);
    // PleatFoldAnnotation
    PleatFoldAnnotation *createPleatFoldAnnotation(Halfedge2d *he1, Halfedge2d *he2);
    PleatFoldAnnotation *createPleatFoldAnnotation(Face2d *f1, Face2d *f2);
    // TurnFoldAnnotation
    TurnFoldAnnotation *createTurnFoldAnnotation(Halfedge2d *he1, Halfedge2d *he2);
    // OutsideReverseFoldAnnotation
    OutsideReverseFoldAnnotation *createOutsideReverseFoldAnnotation(Halfedge2d *he1, Halfedge2d *he2);
    // SinkFoldAnnotation
    SinkFoldAnnotation *createSinkFoldAnnotation(Halfedge2d *he);
    // PigsLegAnnotation
    PigsLegAnnotation *createPigsLegAnnotation(Halfedge2d *he1, Halfedge2d *he2, Halfedge2d *he3);
    // OrikomiEdgeAnnotation
    OrikomiEdgeAnnotation *createOrikomiEdgeAnnotation(Halfedge2d *he);
private:
    /* trunk & branch 170125 */
    int currentLayerNumber;
    bool isBranch;

    /* load file 170126 */
    Annotation *createAnnotation(int annotationName, QVector<Halfedge2d*> hes);
    QVector<Annotation* (Canvas2d::*)(QVector<Halfedge2d*>)> createAnnotationFunctions;
    Annotation *createGlueEdgeAnnotationByVector(QVector<Halfedge2d*> hes);
    Annotation *createInsideReverseFoldAnnotationByVector(QVector<Halfedge2d*> hes);
    Annotation *createPleatFoldAnnotationByVector(QVector<Halfedge2d*> hes);
    Annotation *createOutsideReverseFoldAnnotationByVector(QVector<Halfedge2d*> hes);
    Annotation *createSinkFoldAnnotationByVector(QVector<Halfedge2d*> hes);
    Annotation *createPigsLegAnnotationByVector(QVector<Halfedge2d*> hes);
    Annotation *createOrikomiEdgeAnnotationByVector(QVector<Halfedge2d*> hes);
    void initCreateAnnotationFunctions();

    /* Mirror Connection 170214 */
    QVector<MirrorConnectAnnotation*> mirrorConnections;
    MirrorConnectAnnotation *createMirrorConnection(QVector<Halfedge2d*> hes);
    void drawMirrorConnectAnnotations();

    /* trash model 170307 */
public slots:
    void trashCurrentModel();
private:
    QVector<Model2d*> trashedModels;
    QVector<QVector<Annotation*>*> trashedAnnotations;

public slots:
    void loadImage(QString filename);
    // Set Mode Slot
    void setMode(Mode m);
    // key Press/Release Slot
    void slotKeyPressEvent(QKeyEvent *event);
    void slotKeyReleaseEvent(QKeyEvent *event);

    // select/unselect slot
    void selectVertexSync3D(Vertex3d *v3d);
    void selectHalfedgeSync3D(Halfedge3d *he3d);
    void selectFaceSync3D(Face3d *f3d);
    void unselectVertexSync3D(Vertex3d *v3d);
    void unselectHalfedgeSync3D(Halfedge3d *he3d);
    void unselectFaceSync3D(Face3d *f3d);
    // tool slot
    void addSelectEdgesToGlueEdges();
    void removeSelectEdgesFromGlueEdges();
    void addSelectEdgesToGlueEdges2();
    void createInsideReverseFoldWithSelectEdges();
    void createPleatFoldWithSelectEdges();
    void createPleatFoldWithSelectFaces();
    void createTurnFoldWithSelectEdges();
    void createOutsideReverseFoldWithSelectEdges();
    void createSinkFoldWithSelectedEdge();
    void createPigsLegAnnotationWithSelectedEdges();
    void createOrikomiEdgeAnnotationOnSelectedEdge();
    void deleteSelectAnnotations();

    /* save & load */
    void saveToFile();
    void loadFromFile();

    /* add layer (createPairModels) */
    void createPairModelsSlot();

    /* mirror connection */
    bool isPossibleToCreateMirrorConnection();
    void createMirrorConnectionBetweenSelectedEdges();

    /* others */
    void changeSnapFlag();

    /* dialog2 */
private:
    QVector<MirrorPlane*> getMirrorPlanesFromAnnotationsSelected();
    void setAnnotationSize(Annotation *a, const int size);
    void setAllAnnotationSize(const int size);
    int sizeOfAnnotations;
    int getSizeOfAnnotations() const;
    void setSizeOfAnnotations(int value);
public slots:
    void rotateSelectedMirror(int theta);
    void scaleSelectedMirror(int scale);
signals:
    // face create signal
    void face2dCreated(Face2d*);
    // select/unselect signal
    void vertexSelected(Vertex2d*);
    void halfedgeSelected(Halfedge2d*);
    void faceSelected(Face2d*);
    void vertexUnselected(Vertex2d*);
    void halfedgeUnselected(Halfedge2d*);
    void faceUnselected(Face2d*);
    // glue signal
    void glueEdgeAdded(Halfedge2d*);
    void glueEdgeRemoved(Halfedge2d*);
    void compressGlueBridgeSignal(Halfedge2d*, double);
    // inside reverse fold signal
    void insideReverseFoldAdded(Halfedge2d*, Halfedge2d*);
    void insideReverseFoldRemoved(Halfedge2d*, Halfedge2d*);
    // pleat fold signal
    void pleatFoldAdded(Halfedge2d*, Halfedge2d*);
    void pleatFoldRemoved(Halfedge2d*, Halfedge2d*);
    // turn fold signal
    void turnFoldAdded(Halfedge2d*, Halfedge2d*);
    void turnFoldRemoved(Halfedge2d*, Halfedge2d*);
    // outside reverse fold signal
    void outsideReverseFoldAdded(Halfedge2d*, Halfedge2d*);
    void outsideReverseFoldRemoved(Halfedge2d*, Halfedge2d*);
    // sink fold signal
    void sinkFoldAdded(Halfedge2d*);
    void sinkFoldRemoved(Halfedge2d*);
    // pig's leg signal
    void pigsLegAdded(Halfedge2d*,Halfedge2d*,Halfedge2d*);
    void pigsLegRemoved(Halfedge2d*,Halfedge2d*,Halfedge2d*);
    // orikomi signal
    void orikomiEdgeAdded(Halfedge2d*);
    void orikomiEdgeRemoved(Halfedge2d*);

    void updated();

    /* mirror connect */
    void mirrorConnectAdded(QVector<Halfedge2d*>);
    void mirrorConnectRemoved(QVector<Halfedge2d*>);

    /* mirror rotate */
    void mirrorRotated(MirrorPlane*, int theta);
    void mirrorScaled(MirrorPlane*, int scale);

};

#endif // CANVAS2D_H
