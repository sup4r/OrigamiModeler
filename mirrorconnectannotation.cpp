#include "mirrorconnectannotation.h"
#include "halfedge2d.h"
#include <QOpenGLFunctions>
#include "gldrawutil.h"
#include "mirrorplane.h"
#include "halfedge3d.h"

MirrorConnectAnnotation::MirrorConnectAnnotation(QVector<Halfedge2d *> _hes, QObject *parent)
    :Annotation(parent)
{
    setHes(_hes);
//    setSize(3);
    setColor(QColor(240,240,50));
    setAnnotationName(MIRROR);
}


void MirrorConnectAnnotation::draw()
{
    glLineWidth(getSize()*2);
    glColor3d((float)getColor().red()/255, (float)getColor().green()/255, (float)getColor().blue()/255);
    //draw bold edges
    for(int i = 0; i < getHes().size(); ++i){
        Halfedge2d *he = getHes().at(i);
        glBegin(GL_LINE_STRIP);
        glVertex2d(he->getSV2D().x(), he->getSV2D().y());
        glVertex2d(he->getEV2D().x(), he->getEV2D().y());
        glEnd();
    }
    //draw arrow
    for(int i = 0; i < getHes().size(); ++i){
        Halfedge2d *he1 = getHes().at(i);
        int j = (i==getHes().size()-1)? 0 : i+1;
        Halfedge2d *he2 = getHes().at(j);
        GLDrawUtil::drawArrow2D(he1->getCenter(), he2->getCenter());
    }
}

void MirrorConnectAnnotation::debugPrint()
{

}

void MirrorConnectAnnotation::dragEvent(const QVector2D &mouseMove)
{

}

QVector2D MirrorConnectAnnotation::getHundlePosition() const
{
    return hundlePosition;
}

void MirrorConnectAnnotation::setHundlePosition(const QVector2D &value)
{
    hundlePosition = value;
}

