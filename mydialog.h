#ifndef MYDIALOG_H
#define MYDIALOG_H

#include <QDialog>

class QLabel;
class QPushButton;
class QVBoxLayout;
class Face2d;
class Face3d;
class CreateFace3dButton;

class MyDialog: public QDialog
{
    Q_OBJECT

public:
    MyDialog(QWidget *parent = 0);
private:
    void resize();
    QPushButton *button;
    QVBoxLayout *layout;
signals:
    void face3dCreated(Face3d *);
    void lastHeightChanged(const int);
private slots:
    void buttonClicked();
public slots:
    void createColumn(Face2d *face2d);
    CreateFace3dButton *createCreateFace3dButton(Face2d *face2d);
    Face3d *createFace3d(Face2d *face2d, const int z);


};

#endif // MYDIALOG_H
