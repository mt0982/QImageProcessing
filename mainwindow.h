#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QImage>
#include <QPixmap>
#include <QDebug>
#include "gausskernel.h"

namespace Ui {
class MainWindow;
}

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    explicit MainWindow(QWidget *parent = 0);
    ~MainWindow();

private slots:
    void on_actionGauss_triggered();
    void setImage(QImage image);

private:
    Ui::MainWindow *ui;

    GaussKernel *gaussKernel;
};

#endif // MAINWINDOW_H

















