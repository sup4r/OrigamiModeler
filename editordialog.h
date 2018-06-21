#ifndef EDITORDIALOG_H
#define EDITORDIALOG_H
#include <QDialog>


class Face2d;
class Face3d;
class QHBoxLayout;
class QVBoxLayout;
class QSpinBox;
class QSlider;
class EditORDialog : public QDialog

{
    Q_OBJECT
public:
    EditORDialog(QWidget *parent = 0);

private:
    QVBoxLayout *mainLayout;
    QSpinBox *faceDistanceSpinBox;
    QSlider *faceDistanceSlider;
    int faceDistance;
    int cnt;

signals:
    void face3dCreated(Face3d*);

private slots:
    void setFaceDistance(int value);
public slots:
    void createHBox(Face2d* face2d);

private:
    Face3d *createFace3d(Face2d *face2d, const int zOrder, const int faceZDistance);
    void resize();
    Face3d *createFace3d_old(Face2d *face2d, const int zOrder, const int faceZDistance);
};

#endif // EDITORDIALOG_H
