#ifndef SUBFACEBUILDER_H
#define SUBFACEBUILDER_H
#include "subface.h"
#include "face3d.h"
#include "oriline.h"
#include <QVector>
#include <QVector2D>
class SubFace;
class OriLine;
class OriVertex;
class SubFaceBuilder
{
public:
    static QVector<SubFace*> build(const  QVector<Face3d*> faces);
    static QVector<OriLine*> getOriLines(const QVector<Face3d*>faces);
private:
    static void addOriLine(OriLine* inputLine, QVector<OriLine*>&lines);
    static void cleanDuplicatedOriLines(QVector<OriLine*>&lines);
    static QVector<SubFace*> buildSubfaceFromOriLines(QVector<OriLine*>&lines);
    static bool isSameLineSegment(OriLine *l0, OriLine *l1);
    static QVector2D *getCrossPoint(OriLine *l0, OriLine *l1);
    static double distancePointToSegment(QVector2D p, OriLine *line);
    static bool compQVector2DX(const QVector2D p0, const QVector2D p1);
    static bool compQVector2DY(const QVector2D p0, const QVector2D p1);
    static OriVertex *addAndGetVertexfromVVec(QVector2D p, QVector<OriVertex*>&vertices);
    static void deleteOverlappSubFace(QVector<SubFace*>&subfaces);
};

#endif // SUBFACEBUILDER_H
