#include "annotation.h"
#include <QDebug>

void Annotation::draw()
{
//    qDebug()<<"Annotation";
}

void Annotation::debugPrint()
{
    qDebug()<<"Annotation";
}

void Annotation::dragEvent(const QVector2D &mouseMove)
{
    //    qDebug()<<"Annotation::dragEvent()";
}

void Annotation::calcZOrder()
{

}

Annotation::Annotation(QObject *parent)
    : GLObject(parent)
{

}

void Annotation::addHes(Halfedge2d *he)
{
    hes.push_back(he);
}

int Annotation::getHesNum()
{
    return hes.size();
}

QVector<Halfedge2d *> Annotation::getHes() const
{
    return hes;
}

void Annotation::setHes(const QVector<Halfedge2d *> &value)
{
    hes = value;
}

int Annotation::getAnnotationName() const
{
    return annotationName;
}

void Annotation::setAnnotationName(int value)
{
    annotationName = (enum AnnotationName)value;
}
