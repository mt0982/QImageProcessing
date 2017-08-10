#include "fftw.h"
#include "ui_fftw.h"

FFTW::FFTW(FacadeImage *parent): FacadeImage(parent), ui(new Ui::FFTW)
{
    ui->setupUi(this);
}

FFTW::~FFTW()
{
    delete ui;
}
