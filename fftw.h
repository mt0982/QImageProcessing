#ifndef FFTW_H
#define FFTW_H

#include <QWidget>
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

    void spectrum();
};

#endif // FFTW_H





























