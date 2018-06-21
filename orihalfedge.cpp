#include "orihalfedge.h"
#include <QDebug>
#include "subface.h"

OriHalfedge::OriHalfedge(OriVertex *v, SubFace *sf)
{
    vertex = v;
    pair = next = prev = NULL;
    edge = NULL;
    subface = sf;
}
