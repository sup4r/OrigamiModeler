#ifndef MIRRORCONNECTANNOTATION_H
#define MIRRORCONNECTANNOTATION_H

#include "annotation.h"
#include <QVector>
#include <QVector2D>

class Halfedge2d;
class MirrorPlane;
//this annotation must not be added to models like other annotations;

class MirrorConnectAnnotation : public Annotation
{
    Q_OBJECT

public:
    MirrorConnectAnnotation(QVector<Halfedge2d*> _hes, QObject *parent = 0);
    void draw();
    void debugPrint();
    void dragEvent(const QVector2D &mouseMove);

    QVector2D getHundlePosition() const;
    void setHundlePosition(const QVector2D &value);

private:
    QVector<Halfedge2d*> hes;
    QVector2D hundlePosition;

};

#endif // MIRRORCONNECTANNOTATION_H
