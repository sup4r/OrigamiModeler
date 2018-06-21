
#include <QCheckBox>
#include <QHBoxLayout>
#include <QLineEdit>
#include <QPushButton>
#include <QtGui>
#include <QLabel>
#include <QVector>

#include "mydialog.h"

#include "face2d.h"
#include "halfedge2d.h"
#include "vertex2d.h"

#include "face3d.h"
#include "halfedge3d.h"
#include "vertex3d.h"

#include "createface3dbutton.h"

MyDialog::MyDialog(QWidget *parent)
    :QDialog(parent)
{
    layout = new QVBoxLayout;

//    button = new QPushButton("Add");
//    layout->addWidget(button);
//    connect(button, SIGNAL(clicked()), this, SLOT(buttonClicked()));

    setLayout(layout);
    setGeometry(100, 620, 100, 100);
   // resize();
}

void MyDialog::resize()
{
    setFixedHeight(sizeHint(). height());
    setFixedWidth(sizeHint().width());
}

void MyDialog::buttonClicked()
{
    QPushButton *newButton = new QPushButton("Added");
    layout->addWidget(newButton);

}

void MyDialog::createColumn(Face2d *face2d)
{
    face2d;
}

CreateFace3dButton *MyDialog::createCreateFace3dButton(Face2d *face2d)
{
    CreateFace3dButton *button = new CreateFace3dButton(face2d);
    button->setText("face");
    //button->setGeometry(0, 0, 30, 80);
    layout->addWidget(button);
    connect(button, SIGNAL(myClicked(Face2d*, const int)), this, SLOT(createFace3d(Face2d*, const int)));
    connect(this, SIGNAL(lastHeightChanged(const int)), button, SLOT(setLastHeight(const int)));
    return button;
}

Face3d *MyDialog::createFace3d(Face2d *face2d, const int z)
{
    QVector<Vertex3d *> vList;
    Halfedge2d *he2d = face2d->halfedge;
    do{
        Vertex2d *v2d = he2d->vertex;
        vList.push_back(new Vertex3d(v2d->x, v2d->y, z));
        he2d = he2d->next;
    }while(he2d != face2d->halfedge);

    QVector<Halfedge3d *>heList;
    for(int i = 0; i<vList.size(); ++i){
        heList.push_back(new Halfedge3d(vList.at(i), vList.at((i==vList.size()-1)? 0: i+1)));
    }
    for(int i =0; i<heList.size(); ++i){
        heList.at(i)->setNext(heList.at((i==heList.size()-1)? 0: i+1));
    }
    Face3d *face3d = new Face3d(heList.at(0));//MOTO NO Face2d TO Face3d NO Halfedge{2,3}d WO SOROE MASU

    emit(face3dCreated(face3d));
    emit(lastHeightChanged(z+1));

    return face3d;
}
