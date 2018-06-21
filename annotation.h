#ifndef ANNOTATION_H
#define ANNOTATION_H


#include "globject.h"
#include <QVector2d>
#include <QVector>
#include "annotationname.h"

class Halfedge2d;

class Annotation : public GLObject
{
    Q_OBJECT
public:
    virtual void draw();
    virtual void debugPrint();
    virtual void dragEvent(const QVector2D &mouseMove);
    virtual void calcZOrder();
    explicit Annotation(QObject *parent = 0);

    void addHes(Halfedge2d *he);
    int getHesNum();

    QVector<Halfedge2d *> getHes() const;
    void setHes(const QVector<Halfedge2d *> &value);

    int getAnnotationName() const;
    void setAnnotationName(int value);

private:
    QVector<Halfedge2d*> hes;
    enum AnnotationName annotationName;

signals:

public slots:

};

#endif // ANNOTATION_H
