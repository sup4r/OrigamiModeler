#ifndef ORIEDGE_H
#define ORIEDGE_H
class OriVertex;
class OriHalfedge;

class OriEdge
{
public:
    OriEdge(OriVertex *_sv, OriVertex *_ev, bool isCut = true);
    OriVertex *sv, *ev;
    OriHalfedge *left, *right;
    void debugPrint();
    OriVertex *getOppositeVertex(OriVertex *v);
    void setCutType(bool _isCut);
    bool getCutType();

    //debug
    int itmp;
private:
    bool isCut;
};

#endif // ORIEDGE_H
