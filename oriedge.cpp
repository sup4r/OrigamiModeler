#include "oriedge.h"
#include <QDebug>
#include "orivertex.h"
#include "orihalfedge.h"

OriEdge::OriEdge(OriVertex *_sv, OriVertex *_ev, bool isCut)
{
    sv = _sv; ev = _ev;
    left = right = NULL;
    setCutType(isCut);
}

void OriEdge::debugPrint()
{

}

OriVertex *OriEdge::getOppositeVertex(OriVertex *v)
{
    return (sv==v)?ev :sv;
}

void OriEdge::setCutType(bool _isCut)
{
    isCut = _isCut;
}

bool OriEdge::getCutType()
{
    return isCut;
}
