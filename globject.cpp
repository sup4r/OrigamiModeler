#include "globject.h"
#include <QOpenGLFunctions>



GLObject::GLObject(QObject *parent, QColor _color, int _size)
    :QObject(parent)
{
    setColor(_color);
    setSize(_size);
    setDefaultColor(_color);
}

QColor GLObject::getColor() const
{
    return color;
}

int GLObject::getSize() const
{
    return size;
}

void GLObject::setColor(const QColor &value)
{
    if(color!=value){
        color = value;
        emit(colorUpdated(color));
    }
}


void GLObject::setSize(int value)
{
    if(size!=value){
        size = value;
        emit(sizeUpdated(size));
    }
}

QColor GLObject::getDefaultColor() const
{
    return defaultColor;
}

void GLObject::setDefaultColor(const QColor &value)
{
    defaultColor = value;
}

void GLObject::draw()
{

}

void GLObject::drawWithoutColor()
{

}

void GLObject::drawWithoutSize()
{

}

void GLObject::drawWithoutAll()
{

}
