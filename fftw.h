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

private:
    Ui::FFTW *ui;
};

#endif // FFTW_H
