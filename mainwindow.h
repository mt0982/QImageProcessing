#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QImage>
#include <QPixmap>
#include <QDebug>
#include <QLabel>
#include "gausskernel.h"
#include "staticfilter.h"
#include "morfologicalfilter.h"
#include "canny.h"
#include "houghtransform.h"
#include "fftw.h"

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
    void setImage(const QImage &image);
    void on_actionStatic_Min_Max_Median_triggered();
    void on_actionCanny_triggered();
    void on_actionMorfological_triggered();
    void on_actionHough_triggered();
    void on_actionFFTW_Convolution_triggered();

private:
    Ui::MainWindow *ui;

    GaussUnsharpFilter *gaussUnsharpFilter;
    StaticFilter *staticFilter;
    Canny *cannyFilter;
    MorfologicalFilter *morfologicalFilter;
    HoughTransform *houghTransform;
    FFTW *fftwConvolution;
    QLabel *outputWindow;
};

#endif // MAINWINDOW_H

















