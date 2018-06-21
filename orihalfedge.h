#ifndef ORIHALFEDGE_H
#define ORIHALFEDGE_H
class OriVertex;
class OriEdge;
class SubFace;
class OriHalfedge
{
public:
    OriHalfedge(OriVertex *v, SubFace *sf);
    OriHalfedge *next, *prev, *pair;
    OriVertex *vertex;
    OriEdge *edge;
    SubFace *subface;
};

#endif // ORIHALFEDGE_H
