#ifndef CANVASDIALOG3D_H
#define CANVASDIALOG3D_H
#include <QDialog>

class QVBoxLayout;
class QLabel;
class QRadioButton;

class CanvasDialog3d :public QDialog
{
    Q_OBJECT
public:
    CanvasDialog3d(QWidget *parent = 0);
    void init();
private:
    QVBoxLayout *mainLayout;
    QLabel *label;
    QRadioButton *allButton;
    QRadioButton *vertexButton;
    QRadioButton *halfedgeButton;
    QRadioButton *faceButton;

signals:
    void allChecked();
    void vertexChecked();
    void halfedgeChecked();
    void faceChecked();

private slots:

public slots:
};

#endif // CANVASDIALOG3D_H
