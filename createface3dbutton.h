#ifndef CREATEFACE3DBUTTON_H
#define CREATEFACE3DBUTTON_H

#include <QPushButton>
class Face2d;

class CreateFace3dButton : public QPushButton
{
    Q_OBJECT
public:
    CreateFace3dButton(Face2d *_face2d, QWidget *parent = 0);
signals:
    void myClicked(Face2d *, const int);
private slots:
    void buttonClicked();
public slots:
    void setLastHeight(int _lastHeight);
private:
    Face2d *face2d;
    int lastHeight;
};

#endif // CREATEFACE3DBUTTON_H
