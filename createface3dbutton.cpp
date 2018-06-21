#include "createface3dbutton.h"

CreateFace3dButton::CreateFace3dButton(Face2d *_face2d, QWidget *parent)
    :QPushButton(parent)
{
    face2d = _face2d;
    setLastHeight(0);
    connect(this, SIGNAL(clicked()), this, SLOT(buttonClicked()));
}

void CreateFace3dButton::buttonClicked()
{
    emit(myClicked(face2d, lastHeight));
}

void CreateFace3dButton::setLastHeight(const int _lastHeight)
{
    lastHeight = _lastHeight;
}
