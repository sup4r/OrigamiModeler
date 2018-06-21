#ifndef MODELBASE_H
#define MODELBASE_H
#include <QObject>
#include <QColor>
class ModelBase :public QObject
{
    Q_OBJECT
public:
    ModelBase(QObject *parent = 0);

    int getPointSize() const;
    void setPointSize(int value);

    int getLineWidth() const;
    void setLineWidth(int value);

    QColor getVertexColor() const;
    void setVertexColor(const QColor &value);

    QColor getHalfedgeColor() const;
    void setHalfedgeColor(const QColor &value);

    QColor getFaceColor() const;
    void setFaceColor(const QColor &value);

    QColor getFlipedFaceColor() const;
    void setFlipedFaceColor(const QColor &value);

    QColor getSelectColor() const;
    void setSelectColor(const QColor &value);

    QColor getSelectedFaceColor() const;
    void setSelectedFaceColor(const QColor &value);


private:
    int pointSize;
    int lineWidth;
    QColor vertexColor;
    QColor halfedgeColor;
    QColor faceColor;
    QColor flipedFaceColor;
    QColor selectColor;
    QColor selectedFaceColor;
protected:

};

#endif // MODELBASE_H
