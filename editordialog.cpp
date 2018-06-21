#include "editordialog.h"
#include <QBoxLayout>
#include <QLabel>
#include <QSpinBox>
#include <QSlider>
#include <QDebug>

#include "face2d.h"
#include "face3d.h"
#include "halfedge2d.h"
#include "vertex2d.h"
#include "halfedge3d.h"
#include "vertex3d.h"

EditORDialog::EditORDialog(QWidget *parent)
    :QDialog(parent)
{
    mainLayout = new QVBoxLayout(this);
    setLayout(mainLayout);

    faceDistanceSpinBox = new QSpinBox(this);
    faceDistanceSlider = new QSlider(Qt::Horizontal,this);
    faceDistanceSpinBox->setRange(0, 100);
    faceDistanceSlider->setRange(0, 100);
    faceDistanceSlider->setFixedWidth(100);
    connect(faceDistanceSpinBox, SIGNAL(valueChanged(int)), faceDistanceSlider, SLOT(setValue(int)));
    connect(faceDistanceSlider, SIGNAL(valueChanged(int)), faceDistanceSpinBox, SLOT(setValue(int)));
    connect(faceDistanceSpinBox, SIGNAL(valueChanged(int)), this, SLOT(setFaceDistance(int)));
    connect(faceDistanceSlider, SIGNAL(valueChanged(int)), this, SLOT(setFaceDistance(int)));

    static const int fD = 10;//20171215
    faceDistanceSpinBox->setValue(fD);
    QHBoxLayout *hLayout = new QHBoxLayout;
    hLayout->addWidget(new QLabel("x: "));
    hLayout->addWidget(faceDistanceSpinBox);
    hLayout->addWidget(faceDistanceSlider);
    mainLayout->addLayout(hLayout);

    setGeometry(100, 750, 100, 100);

    cnt = 0;
}

void EditORDialog::setFaceDistance(int value)
{
    faceDistance = value;
}

void EditORDialog::createHBox(Face2d *face2d)
{
    static const int MaxOrder = 100;
    QHBoxLayout *hBoxLayout = new QHBoxLayout;
    QLabel *fLabel = new QLabel("Z order: ");
    QSpinBox *faceOrderSpinBox = new QSpinBox;
    faceOrderSpinBox->setRange(0, MaxOrder);
    //set init overlap order
    faceOrderSpinBox->setValue(1);

    Face3d *face3d = createFace3d(face2d, faceOrderSpinBox->value(), this->faceDistanceSpinBox->value());
    connect(faceOrderSpinBox, SIGNAL(valueChanged(int)), face3d, SLOT(setZOrderValue(int)));
    connect(this->faceDistanceSpinBox, SIGNAL(valueChanged(int)), face3d, SLOT(setFaceZDistanceValue(int)));
    connect(this->faceDistanceSlider, SIGNAL(valueChanged(int)), face3d, SLOT(setFaceZDistanceValue(int)));

    /* 170228 disabled */
//    hBoxLayout->addWidget(fLabel);
//    hBoxLayout->addWidget(faceOrderSpinBox);
    this->mainLayout->addLayout(hBoxLayout);
    cnt++;

    //resize();

}
void EditORDialog::resize(){
    setFixedHeight(sizeHint(). height());
    setFixedWidth(sizeHint().width());
}

// 9/10
Face3d *EditORDialog::createFace3d(Face2d *face2d, const int zOrder, const int faceZDistance){

    // first face3d
    QVector<Vertex3d *> vertices3d;
    Halfedge2d *he2d = face2d->halfedge;
    do{
        Vertex2d *v2d = he2d->vertex;
        Vertex3d *v3d = new Vertex3d(v2d->x, v2d->y, zOrder*faceZDistance);
        v2d->setVertex3d(v3d);
        v3d->setVertex2d(v2d);
        vertices3d.push_back(v3d);
        he2d = he2d->next;
    }while(he2d != face2d->halfedge);

    QVector<Vertex2d *> vertices2d = face2d->getVertices();
    QVector<Halfedge3d *>heList;
    for(int i = 0; i<vertices3d.size(); ++i){
        Halfedge3d *he3d = new Halfedge3d(vertices3d.at(i), vertices3d.at((i==vertices3d.size()-1)? 0: i+1));
        heList.push_back(he3d);
        he2d->setHalfedge3d(he3d);
        he3d->setHalfedge2d(he2d);
        he2d=he2d->next;
    }
    for(int i =0; i<heList.size(); ++i){
        heList.at(i)->setNext(heList.at((i==heList.size()-1)? 0: i+1));
    }
    Face3d *face3d = new Face3d(heList.at(0), zOrder, faceZDistance);//MOTO NO Face2d TO Face3d NO Halfedge{2,3}d WO SOROE MASU
    face2d->setFace3d(face3d);
    face3d->setFace2d(face2d);

    emit(face3dCreated(face3d));

    return face3d;
}
// ~9/9
Face3d *EditORDialog::createFace3d_old(Face2d *face2d, const int zOrder, const int faceZDistance){

    QVector<Vertex3d *> vertices3d;
    Halfedge2d *he2d = face2d->halfedge;
    do{
        Vertex2d *v2d = he2d->vertex;
        Vertex3d *v3d = new Vertex3d(v2d->x, v2d->y, zOrder*faceZDistance);
        v2d->setVertex3d(v3d);
        v3d->setVertex2d(v2d);
        vertices3d.push_back(v3d);
        he2d = he2d->next;
    }while(he2d != face2d->halfedge);

    QVector<Vertex2d *> vertices2d = face2d->getVertices();
    QVector<Halfedge3d *>heList;
    for(int i = 0; i<vertices3d.size(); ++i){
        Halfedge3d *he3d = new Halfedge3d(vertices3d.at(i), vertices3d.at((i==vertices3d.size()-1)? 0: i+1));
        heList.push_back(he3d);
        he2d->setHalfedge3d(he3d);
        he3d->setHalfedge2d(he2d);
        he2d=he2d->next;
    }
    for(int i =0; i<heList.size(); ++i){
        heList.at(i)->setNext(heList.at((i==heList.size()-1)? 0: i+1));
    }
    Face3d *face3d = new Face3d(heList.at(0), zOrder, faceZDistance);//MOTO NO Face2d TO Face3d NO Halfedge{2,3}d WO SOROE MASU
    face2d->setFace3d(face3d);
    face3d->setFace2d(face2d);

    emit(face3dCreated(face3d));
    // mirror
    emit(face3dCreated(face3d->createMirror()));

    return face3d;
}
