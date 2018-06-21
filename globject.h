#ifndef GLOBJECT_H
#define GLOBJECT_H
#include <QObject>
#include <QColor>

class GLObject :public QObject
{
    Q_OBJECT
private:
    QColor color;
    QColor defaultColor;
    int size;
public:
    virtual void draw();
    virtual void drawWithoutColor();
    virtual void drawWithoutSize();
    virtual void drawWithoutAll();

    GLObject(QObject *parent = 0, QColor _color = QColor(0,0,0), int _size = 1);
    QColor getColor() const;
    int getSize() const;
    QColor getDefaultColor() const;

public slots:// THESE FUNCTIONS ARE CALLED DIRECTLY
    void setColor(const QColor &value);
    void setSize(int value);
    void setDefaultColor(const QColor &value);
signals:
    void colorUpdated(const QColor &value);
    void sizeUpdated(int value);

};

#endif // GLOBJECT_H
