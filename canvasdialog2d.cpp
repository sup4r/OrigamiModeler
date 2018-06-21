#include "canvasdialog2d.h"
#include "canvas2d.h"
#include <QSlider>
#include <QSpinBox>
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QLabel>
CanvasDialog2d::CanvasDialog2d(Canvas2d *c, QWidget *parent)
    :QDialog(parent)
{
    setCanvas(c);

    mainLayout = new QVBoxLayout(this);
    setLayout(mainLayout);

    rotateSlider = new QSlider(Qt::Horizontal,this);
    rotateSlider->setRange(-180, 180);
    rotateSlider->setFixedWidth(100);
    rotateSpinBox = new QSpinBox(this);
    rotateSpinBox->setRange(-180,180);
    QHBoxLayout *hLayout = new QHBoxLayout;
    hLayout->addWidget(new QLabel("rotate: "));
    hLayout->addWidget(rotateSpinBox);
    hLayout->addWidget(rotateSlider);
    mainLayout->addLayout(hLayout);

    scaleSlider = new QSlider(Qt::Horizontal, this);
    scaleSlider->setRange(0,100);
    scaleSlider->setFixedWidth(100);
    scaleSpinBox = new QSpinBox(this);
    scaleSpinBox->setRange(0, 100);
    hLayout = new QHBoxLayout;
    hLayout->addWidget(new QLabel("scale: "));
    hLayout->addWidget(scaleSpinBox);
    hLayout->addWidget(scaleSlider);
    mainLayout->addLayout(hLayout);

    makeConnection();

    rotateSlider->setValue(30);
    scaleSlider->setValue(10);

    setGeometry(320, 750, 100, 100);
}

Canvas2d *CanvasDialog2d::getCanvas() const
{
    return canvas;
}

void CanvasDialog2d::setCanvas(Canvas2d *value)
{
    canvas = value;
}

void CanvasDialog2d::makeConnection()
{
    connect(rotateSlider, SIGNAL(valueChanged(int)), rotateSpinBox, SLOT(setValue(int)));
    connect(rotateSpinBox, SIGNAL(valueChanged(int)), rotateSlider, SLOT(setValue(int)));
    connect(rotateSlider, SIGNAL(valueChanged(int)), canvas, SLOT(rotateSelectedMirror(int)));

    connect(scaleSlider, SIGNAL(valueChanged(int)), scaleSpinBox, SLOT(setValue(int)));
    connect(scaleSpinBox, SIGNAL(valueChanged(int)), scaleSlider, SLOT(setValue(int)));
    connect(scaleSlider, SIGNAL(valueChanged(int)), canvas, SLOT(scaleSelectedMirror(int)));
}

