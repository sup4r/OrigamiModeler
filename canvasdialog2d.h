#ifndef CANVASDIALOG2D_H
#define CANVASDIALOG2D_H
#include <QDialog>
class Canvas2d;
class QVBoxLayout;
class QLabel;
class QRadioButton;
class QSlider;
class QSpinBox;
#include <QWidget>
class CanvasDialog2d : public QDialog
{
    Q_OBJECT
public:
    CanvasDialog2d(Canvas2d *c, QWidget *parent = 0);

    Canvas2d *getCanvas() const;
    void setCanvas(Canvas2d *value);

private:
    Canvas2d *canvas;
    void makeConnection();

    QVBoxLayout *mainLayout;

    /* mirror rotate */
    QSlider *rotateSlider;
    QSpinBox *rotateSpinBox;
    /* mirror scale */
    QSlider *scaleSlider;
    QSpinBox *scaleSpinBox;
private slots:
public slots:

};

#endif // CANVASDIALOG2D_H
