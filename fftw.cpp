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

QImage FFTW::swap(QImage &input)
{
    /* Swap Quarters I <-> III, II <-> IV */
    for (int y = 0; y < input.height() / 2; ++y) {
        QRgb *first = (QRgb*)input.scanLine(y);
        QRgb *second = (QRgb*)input.scanLine(y + input.height() / 2);

        for (int x = 0; x < input.width() / 2; ++x) {

            qSwap(first[x], second[x + input.width() / 2]);
            qSwap(first[x + input.width() / 2], second[x]);
        }
    }

    return input;
}

void FFTW::forward()
{
    int mSum = 25;

    int radius = 2;
    QVector<int> value = {1,1,1,1,1,
                         1,1,1,1,1,
                         1,1,1,1,1,
                         1,1,1,1,1,
                         1,1,1,1,1};

    /* Create And Fill Mask HxW */
    QImage mMask = QImage(image.width(), image.height(), QImage::Format_RGB32);

    /* Put Small Mask Into Big (WxH) */
    int index = 0;
    for (int y = (image.height() / 2) - radius; y <= (image.height() / 2) + radius; ++y) {

        QRgb *ptr_mask = (QRgb*)mMask.scanLine(y);

        for (int x = (image.width() / 2) - radius; x <= (image.width() / 2) + radius; ++x) {

            ptr_mask[x] = value[index];
            index++;
        }
    }

    /* Swap Quarters I <-> III, II <-> IV */
    mMask = swap(mMask);

    /* Alloc */
    fftw_complex *inRed, *inGreen, *inBlue, *inMask;
    fftw_complex *outRed, *outGreen, *outBlue, *outMask;

    inRed = (fftw_complex*)fftw_malloc(sizeof(fftw_complex) * image.width() * image.height());
    inGreen = (fftw_complex*)fftw_malloc(sizeof(fftw_complex) * image.width() * image.height());
    inBlue = (fftw_complex*)fftw_malloc(sizeof(fftw_complex) * image.width() * image.height());
    inMask = (fftw_complex*)fftw_malloc(sizeof(fftw_complex) * image.width() * image.height());
    outRed = (fftw_complex*)fftw_malloc(sizeof(fftw_complex) * image.width() * image.height());
    outGreen = (fftw_complex*)fftw_malloc(sizeof(fftw_complex) * image.width() * image.height());
    outBlue = (fftw_complex*)fftw_malloc(sizeof(fftw_complex) * image.width() * image.height());
    outMask = (fftw_complex*)fftw_malloc(sizeof(fftw_complex) * image.width() * image.height());

    /* Initialize */
    QRgb *ptr_image = (QRgb*)image.bits();
    for (int i = 0; i < image.width() * image.height(); ++i) {

        QRgb *ptr_mask = (QRgb*)mMask.scanLine(i/image.width());

        inRed[i][REALIS] = qRed(ptr_image[i]);
        inRed[i][IMAGINALIS] = 0;
        inGreen[i][REALIS] = qGreen(ptr_image[i]);
        inGreen[i][IMAGINALIS] = 0;
        inBlue[i][REALIS] = qBlue(ptr_image[i]);
        inBlue[i][IMAGINALIS] = 0;
        inMask[i][REALIS] = ((1.0 * ptr_mask[i%image.width()]) / mSum);
        inMask[i][IMAGINALIS] = 0;
    }

    /* Plans */
    fftw_plan planRed, planBlue, planGreen, planMask;
    planRed = fftw_plan_dft_2d(image.height(), image.width(), inRed, outRed, FFTW_FORWARD, FFTW_ESTIMATE);
    planGreen = fftw_plan_dft_2d(image.height(), image.width(), inGreen, outGreen, FFTW_FORWARD, FFTW_ESTIMATE);
    planBlue = fftw_plan_dft_2d(image.height(), image.width(), inBlue, outBlue, FFTW_FORWARD, FFTW_ESTIMATE);
    planMask = fftw_plan_dft_2d(image.height(), image.width(), inMask, outMask, FFTW_FORWARD, FFTW_ESTIMATE);

    fftw_execute(planRed);          fftw_destroy_plan(planRed);
    fftw_execute(planGreen);        fftw_destroy_plan(planGreen);
    fftw_execute(planBlue);         fftw_destroy_plan(planBlue);
    fftw_execute(planMask);         fftw_destroy_plan(planMask);

    /* Calculate New Values */
    for(int i = 0; i < image.width() * image.height(); i++) {
        inRed[i][REALIS] = outRed[i][REALIS] * outMask[i][REALIS] - outRed[i][IMAGINALIS] * outMask[i][IMAGINALIS];
        inRed[i][IMAGINALIS] = outRed[i][IMAGINALIS] * outMask[i][REALIS] + outRed[i][REALIS] * outMask[i][IMAGINALIS];
        inGreen[i][REALIS] = outGreen[i][REALIS] * outMask[i][REALIS] - outGreen[i][IMAGINALIS] * outMask[i][IMAGINALIS];
        inGreen[i][IMAGINALIS] = outGreen[i][IMAGINALIS] * outMask[i][REALIS] + outGreen[i][REALIS] * outMask[i][IMAGINALIS];
        inBlue[i][REALIS] = outBlue[i][REALIS] * outMask[i][REALIS] - outBlue[i][IMAGINALIS] * outMask[i][IMAGINALIS];
        inBlue[i][IMAGINALIS] = outBlue[i][IMAGINALIS] * outMask[i][REALIS] + outBlue[i][REALIS] * outMask[i][IMAGINALIS];
    }

    /* Backward */
    planRed = fftw_plan_dft_2d(image.height(), image.width(), inRed, outRed, FFTW_FORWARD, FFTW_ESTIMATE);
    planGreen = fftw_plan_dft_2d(image.height(), image.width(), inGreen, outGreen, FFTW_FORWARD, FFTW_ESTIMATE);
    planBlue = fftw_plan_dft_2d(image.height(), image.width(), inBlue, outBlue, FFTW_FORWARD, FFTW_ESTIMATE);

    fftw_execute(planRed);          fftw_destroy_plan(planRed);
    fftw_execute(planGreen);        fftw_destroy_plan(planGreen);
    fftw_execute(planBlue);         fftw_destroy_plan(planBlue);

    /* Output */
    output = QImage(image.width(), image.height(), QImage::Format_ARGB32);
    QRgb *ptr_output = (QRgb*)output.bits();

    for(int i = 0; i < image.width() * image.height(); i++) {

        ptr_output[i] = qRgb(qBound(0, (int)(outRed[i][REALIS] / (image.width() * image.height())), 255),
                             qBound(0, (int)(outGreen[i][REALIS] / (image.width() * image.height())), 255),
                             qBound(0, (int)(outBlue[i][REALIS] / (image.width() * image.height())), 255));
    }

    sendImage(output);
}

void FFTW::on_pbCalculate_clicked()
{
    forward();
}















