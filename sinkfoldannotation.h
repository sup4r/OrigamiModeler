#ifndef SINKFOLDANNOTATION_H
#define SINKFOLDANNOTATION_H
#include "annotation.h"
class Halfedge2d;

class SinkFoldAnnotation : public Annotation
{
    Q_OBJECT
public:
    SinkFoldAnnotation(Halfedge2d* he, QObject *parent = 0);
    ~SinkFoldAnnotation();
    Halfedge2d *getHe() const;
    void setHe(Halfedge2d *value);
    void draw();
private:
    Halfedge2d *he;
signals:
    void deleted(Halfedge2d*);
};

#endif // SINKFOLDANNOTATION_H
