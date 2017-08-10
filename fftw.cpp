#include "fftw.h"
#include "ui_fftw.h"

#define REALIS 0
#define IMAGINALIS 1

FFTW::FFTW(FacadeImage *parent): FacadeImage(parent), ui(new Ui::FFTW)
{
    ui->setupUi(this);
}

FFTW::~FFTW()
{
    delete ui;
}

void FFTW::spectrum()
{
    fftw_complex *input_red, *input_green, *input_blue;
    fftw_complex *output_red, *output_green, *output_blue;

    /* Alloc FFTW */
    input_red = (fftw_complex*)fftw_malloc(sizeof(fftw_complex) * image.width() * image.height());
    input_green = (fftw_complex*)fftw_malloc(sizeof(fftw_complex) * image.width() * image.height());
    input_blue = (fftw_complex*)fftw_malloc(sizeof(fftw_complex) * image.width() * image.height());
    output_red = (fftw_complex*)fftw_malloc(sizeof(fftw_complex) * image.width() * image.height());
    output_green = (fftw_complex*)fftw_malloc(sizeof(fftw_complex) * image.width() * image.height());
    output_blue = (fftw_complex*)fftw_malloc(sizeof(fftw_complex) * image.width() * image.height());

    /* Init FFTW */
    QRgb *pixel = (QRgb*)image.bits();
    for(int i = 0; i < image.width() * image.height(); i++) {

        input_red[i][REALIS] = qRed(pixel[i]);
        input_red[i][IMAGINALIS] = 0;
        input_green[i][REALIS] = qGreen(pixel[i]);
        input_green[i][IMAGINALIS] = 0;
        input_blue[i][REALIS] = qBlue(pixel[i]);
        input_blue[i][IMAGINALIS] = 0;
    }

    fftw_plan plan_red, plan_green, plan_blue;
    plan_red = fftw_plan_dft_2d(image.height(), image.width(), input_red, output_red, FFTW_FORWARD, FFTW_ESTIMATE);
    plan_green = fftw_plan_dft_2d(image.height(), image.width(), input_green, output_green, FFTW_FORWARD, FFTW_ESTIMATE);
    plan_blue = fftw_plan_dft_2d(image.height(), image.width(), input_blue, output_blue, FFTW_FORWARD, FFTW_ESTIMATE);

    fftw_execute(plan_red);
    fftw_execute(plan_green);
    fftw_execute(plan_blue);

    /* Visualisation */
    output = QImage(image.width(), image.height(), QImage::Format_RGB32);
    QRgb *ptr_output = (QRgb*)output.bits();

    /* Find Max */
    QColor maximumFromRealis;
    for (int i = 0; i < image.width() * image.height(); ++i) {

        QColor crealis = QColor(log(abs(output_red[i][REALIS]) + 1),
                                log(abs(output_green[i][REALIS]) + 1),
                                log(abs(output_blue[i][REALIS]) + 1));

        //qDebug() << color;

        maximumFromRealis = QColor(qMax(maximumFromRealis.red(), crealis.red()),
                                   qMax(maximumFromRealis.green(), crealis.green()),
                                   qMax(maximumFromRealis.blue(), crealis.blue()));
    }

    for (int i = 0; i < image.width() * image.height(); ++i) {

        QColor crealis = QColor(log(abs(output_red[i][REALIS]) + 1) * maximumFromRealis.red(),
                                log(abs(output_green[i][REALIS]) + 1) * maximumFromRealis.green(),
                                log(abs(output_blue[i][REALIS]) + 1) * maximumFromRealis.blue());

        ptr_output[i] = qRgb(crealis.red(), crealis.green(), crealis.blue());
    }


    sendImage(output);
}

void FFTW::on_pbCalculate_clicked()
{
    spectrum();
}















