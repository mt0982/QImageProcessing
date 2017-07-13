#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QImage>
#include <QPixmap>
#include <QDebug>
#include "gausskernel.h"
#include "staticfilter.h"

namespace Ui {
class MainWindow;
}

class MainWindow : public QMainWindow {
    Q_OBJECT

public:
    explicit MainWindow(QWidget *parent = 0);
    ~MainWindow();

private slots:
    void on_actionGauss_triggered();
    void setImage(QImage image);
    void on_actionStatic_Min_Max_Median_triggered();

private:
    Ui::MainWindow *ui;

    GaussKernel *gaussKernel;
    StaticFilter *staticFilter;
};

#endif // MAINWINDOW_H

















