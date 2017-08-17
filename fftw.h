#ifndef FFTW_H
#define FFTW_H

#include <QWidget>
#include <QLabel>
#include <fftw3.h>
#include "Pattern/facadeimage.h"

namespace Ui {
class FFTW;
}

class FFTW : public FacadeImage {
    Q_OBJECT

public:
    explicit FFTW(FacadeImage *parent = 0);
    ~FFTW();

private slots:
    void on_pbCalculate_clicked();

private:
    Ui::FFTW *ui;

    QLabel *specturmWindow;

    void convolution();
    QImage swap(QImage &input);
    void swapArrays(int **mask, int **reversed, int width, int height);
    void showSpecturm(const int operation_nr);
};

#endif // FFTW_H





























