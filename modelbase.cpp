#include "modelbase.h"

ModelBase::ModelBase(QObject *parent)
    :QObject(parent)
{
    setVertexColor(QColor(45,45,45));//Qt::black;//QColor(199, 26, 58);//QColor(126, 194, 66);
    setHalfedgeColor(QColor(45,45,45));//Qt::black;//QColor(199, 26, 58);//QColor(45, 45, 45);//QColor(69,161,207);
    setFaceColor(Qt::white);//QColor(0,98,172));//(Qt::white);//QColor(218,91,114);//QColor(45,45,45);QColor(218,91,114);//Qt::white;//QColor(246,212,216);//QColor(191, 194, 222);
    setFlipedFaceColor(Qt::white);
    setSelectColor(QColor(156,255,156));//Qt::yellow;
    setSelectedFaceColor(QColor(220,255,220));
    setPointSize(5);
    setLineWidth(1);
}

int ModelBase::getPointSize() const
{
    return pointSize;
}

void ModelBase::setPointSize(int value)
{
    pointSize = value;

}

int ModelBase::getLineWidth() const
{
    return lineWidth;
}

void ModelBase::setLineWidth(int value)
{
    lineWidth = value;
}

QColor ModelBase::getVertexColor() const
{
    return vertexColor;
}

void ModelBase::setVertexColor(const QColor &value)
{
    vertexColor = value;
}

QColor ModelBase::getHalfedgeColor() const
{
    return halfedgeColor;
}

void ModelBase::setHalfedgeColor(const QColor &value)
{
    halfedgeColor = value;
}

QColor ModelBase::getFaceColor() const
{
    return faceColor;
}

void ModelBase::setFaceColor(const QColor &value)
{
    faceColor = value;
}

QColor ModelBase::getFlipedFaceColor() const
{
    return flipedFaceColor;
}

void ModelBase::setFlipedFaceColor(const QColor &value)
{
    flipedFaceColor = value;
}

QColor ModelBase::getSelectColor() const
{
    return selectColor;
}

void ModelBase::setSelectColor(const QColor &value)
{
    selectColor = value;
}

QColor ModelBase::getSelectedFaceColor() const
{
    return selectedFaceColor;
}

void ModelBase::setSelectedFaceColor(const QColor &value)
{
    selectedFaceColor = value;
}

