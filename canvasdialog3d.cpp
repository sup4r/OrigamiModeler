#include "canvasdialog3d.h"

#include <QBoxLayout>
#include <QLabel>
#include <QRadioButton>


CanvasDialog3d::CanvasDialog3d(QWidget *parent)
    :QDialog(parent)
{
    mainLayout = new QVBoxLayout();
    label = new QLabel("Selection Mode:");
    allButton = new QRadioButton("All");
    vertexButton = new QRadioButton("Vertex");
    halfedgeButton = new QRadioButton("Edge");
    faceButton = new QRadioButton("Face");
    mainLayout->addWidget(label);
    mainLayout->addWidget(allButton);
    mainLayout->addWidget(vertexButton);
    mainLayout->addWidget(halfedgeButton);
    mainLayout->addWidget(faceButton);

    connect(allButton, SIGNAL(clicked(bool)), this, SIGNAL(allChecked()));
    connect(vertexButton, SIGNAL(clicked(bool)), this, SIGNAL(vertexChecked()));
    connect(halfedgeButton, SIGNAL(clicked(bool)), this, SIGNAL(halfedgeChecked()));
    connect(faceButton, SIGNAL(clicked(bool)), this, SIGNAL(faceChecked()));
    setLayout(mainLayout);
    //show();

}

void CanvasDialog3d::init()
{
    allButton->click();
}
