#include "mainwindow.h"
#include <QAction>
#include <QFileDialog>
#include <QMenuBar>
MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
{

}

MainWindow::~MainWindow()
{


}
/*protected*/
void MainWindow::closeEvent(QCloseEvent *)
{
    emit(closed());
}



