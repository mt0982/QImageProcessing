#include "fftw.h"
#include "ui_fftw.h"

#define REALIS 0
#define IMAGINALIS 1

FFTW::FFTW(FacadeImage *parent): FacadeImage(parent), ui(new Ui::FFTW), specturmWindow(NULL)
{
    ui->setupUi(this);

    /* Connect, SpinBox Value Changed, Cast Is Neccessary */
    connect(ui->sbRadius, static_cast<void(QSpinBox::*)(int)>(&QSpinBox::valueChanged), [=](const int &radius) {
        ui->tableWidget->setRowCount(radius * 2 + 1);
        ui->tableWidget->setColumnCount(radius * 2 + 1);

        /* Fill Tables With 1 Values */
        for (int x = 0; x < ui->tableWidget->rowCount(); ++x) {
            for (int y = 0; y < ui->tableWidget->columnCount(); ++y) {

                QTableWidgetItem *item = new QTableWidgetItem;
                ui->tableWidget->setItem(x,y,item);
                item->setText(QString::number(1));
            }
        }
    });

    /* Visualisation Connect (ComboBox) */
    connect(ui->comboBox, static_cast<void(QComboBox::*)(int)>(&QComboBox::currentIndexChanged), [=](const int &index) {
        showSpecturm(index);
    });
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

void FFTW::swapArrays(int **mask, int **reversed, int width, int height)
{
    /* Swap Quarters I <-> III, II <-> IV */
    int yindex = 0;
    for(int y = height / 2; y < height; ++y) {
        int xindex = 0;
        for(int x = width / 2; x < width; ++x) {
            reversed[yindex][xindex] = mask[y][x];
            xindex++;
        }
        yindex++;
    }

    yindex = height / 2.0;
    for(int y = 0; y < height / 2; ++y) {
        int xindex = width/2.0;
        for(int x = 0; x < width / 2; ++x) {
            reversed[yindex][xindex] = mask[y][x];
            xindex++;
        }
        yindex++;
    }

    yindex = 0;
    for(int y = height / 2; y < height; ++y) {
        int xindex = width/2.0;
        for(int x = 0; x < width / 2; ++x) {
            reversed[yindex][xindex] = mask[y][x];
            xindex++;
        }
        yindex++;
    }

    yindex = height/2.0;
    for(int y = 0; y < height / 2; ++y) {
        int xindex=0;
        for(int x = width / 2; x < width; ++x) {
            reversed[yindex][xindex] = mask[y][x];
            xindex++;
        }
        yindex++;
    }
}

void FFTW::showSpecturm(const int operation_nr)
{
    /* Alloc */
    fftw_complex *inRed, *inGreen, *inBlue;
    fftw_complex *outRed, *outGreen, *outBlue;

    inRed = (fftw_complex*)fftw_malloc(sizeof(fftw_complex) * image.width() * image.height());
    inGreen = (fftw_complex*)fftw_malloc(sizeof(fftw_complex) * image.width() * image.height());
    inBlue = (fftw_complex*)fftw_malloc(sizeof(fftw_complex) * image.width() * image.height());
    outRed = (fftw_complex*)fftw_malloc(sizeof(fftw_complex) * image.width() * image.height());
    outGreen = (fftw_complex*)fftw_malloc(sizeof(fftw_complex) * image.width() * image.height());
    outBlue = (fftw_complex*)fftw_malloc(sizeof(fftw_complex) * image.width() * image.height());

    /* Initialize */
    QRgb *ptr_image = (QRgb*)image.bits();
    for (int i = 0; i < image.width() * image.height(); ++i) {

        inRed[i][REALIS] = qRed(ptr_image[i]);
        inRed[i][IMAGINALIS] = 0;
        inGreen[i][REALIS] = qGreen(ptr_image[i]);
        inGreen[i][IMAGINALIS] = 0;
        inBlue[i][REALIS] = qBlue(ptr_image[i]);
        inBlue[i][IMAGINALIS] = 0;
    }

    /* Plans */
    fftw_plan planRed, planBlue, planGreen;
    planRed = fftw_plan_dft_2d(image.height(), image.width(), inRed, outRed, FFTW_FORWARD, FFTW_ESTIMATE);
    planGreen = fftw_plan_dft_2d(image.height(), image.width(), inGreen, outGreen, FFTW_FORWARD, FFTW_ESTIMATE);
    planBlue = fftw_plan_dft_2d(image.height(), image.width(), inBlue, outBlue, FFTW_FORWARD, FFTW_ESTIMATE);

    fftw_execute(planRed);          fftw_destroy_plan(planRed);
    fftw_execute(planGreen);        fftw_destroy_plan(planGreen);
    fftw_execute(planBlue);         fftw_destroy_plan(planBlue);

    /* Find Max */
    double maxRed = 0, maxBlue = 0, maxGreen = 0;
    for (int i = 0; i < image.width() * image.height(); ++i) {

        if (operation_nr == 0) {            //Realis
            outRed[i][REALIS] = log(abs(outRed[i][REALIS]) + 1);
            outGreen[i][REALIS] = log(abs(outGreen[i][REALIS]) + 1);
            outBlue[i][REALIS] = log(abs(outBlue[i][REALIS]) + 1);

            maxRed = qMax(maxRed, outRed[i][REALIS]);
            maxBlue = qMax(maxRed, outGreen[i][REALIS]);
            maxGreen = qMax(maxRed, outBlue[i][REALIS]);
        }
        else if (operation_nr == 1) {       //Imaginalis
            outRed[i][IMAGINALIS] = log(abs(outRed[i][IMAGINALIS]) + 1);
            outGreen[i][IMAGINALIS] = log(abs(outGreen[i][IMAGINALIS]) + 1);
            outBlue[i][IMAGINALIS] = log(abs(outBlue[i][IMAGINALIS]) + 1);

            maxRed = qMax(maxRed, outRed[i][IMAGINALIS]);
            maxBlue = qMax(maxRed, outGreen[i][IMAGINALIS]);
            maxGreen = qMax(maxRed, outBlue[i][IMAGINALIS]);
        }
        else if (operation_nr == 2) {       //Spectrum, hypot = sqrt(a*a + b*b)
            double spectrumRed = log(hypot(outRed[i][REALIS], outRed[i][IMAGINALIS]) + 1);
            double spectrumGreen = log(hypot(outGreen[i][REALIS], outGreen[i][IMAGINALIS]) + 1);
            double spectrumBlue = log(hypot(outBlue[i][REALIS], outBlue[i][IMAGINALIS]) + 1);

            maxRed = qMax(maxRed, spectrumRed);
            maxBlue = qMax(maxRed, spectrumGreen);
            maxGreen = qMax(maxRed, spectrumBlue);
        }
        else {                              //Phase
            double phaseRed = log(abs(atan(outRed[i][IMAGINALIS] / outRed[i][REALIS])) + 1);
            double phaseGreen = log(abs(atan(outGreen[i][IMAGINALIS] / outGreen[i][REALIS])) + 1);
            double phaseBlue = log(abs(atan(outBlue[i][IMAGINALIS] / outBlue[i][REALIS])) + 1);

            maxRed = qMax(maxRed, phaseRed);
            maxBlue = qMax(maxBlue, phaseGreen);
            maxGreen = qMax(maxGreen, phaseBlue);
        }
    }

    maxRed = 255.f / maxRed;
    maxGreen = 255.f / maxGreen;
    maxBlue = 255.f / maxBlue;

    /* Output */
    output = QImage(image.width(), image.height(), QImage::Format_ARGB32);
    QRgb *ptr_output = (QRgb*)output.bits();

    for (int i = 0; i < image.width() * image.height(); ++i) {

        if (operation_nr == 0) {        //Realis
            ptr_output[i] = qRgb(outRed[i][REALIS] * maxRed,
                                 outGreen[i][REALIS] * maxGreen,
                                 outBlue[i][REALIS] * maxBlue);
        }
        else if (operation_nr == 1) {   //Imaginalis
            ptr_output[i] = qRgb(outRed[i][IMAGINALIS] * maxRed,
                                 outGreen[i][IMAGINALIS] * maxGreen,
                                 outBlue[i][IMAGINALIS] * maxBlue);
        }
        else if (operation_nr == 2) {   //Spectrum, hypot = sqrt(a*a + b*b)
            double spectrumRed = log(hypot(outRed[i][REALIS], outRed[i][IMAGINALIS]) + 1);
            double spectrumGreen = log(hypot(outGreen[i][REALIS], outGreen[i][IMAGINALIS]) + 1);
            double spectrumBlue = log(hypot(outBlue[i][REALIS], outBlue[i][IMAGINALIS]) + 1);

            ptr_output[i] = qRgb(spectrumRed * maxRed,
                                 spectrumGreen * maxGreen,
                                 spectrumBlue * maxBlue);
        }
        else {                          //Phase
            double phaseRed = log(abs(atan(outRed[i][IMAGINALIS] / outRed[i][REALIS])) + 1);
            double phaseGreen = log(abs(atan(outGreen[i][IMAGINALIS] / outGreen[i][REALIS])) + 1);
            double phaseBlue = log(abs(atan(outBlue[i][IMAGINALIS] / outBlue[i][REALIS])) + 1);

            ptr_output[i] = qRgb(phaseRed * maxRed,
                                 phaseGreen * maxGreen,
                                 phaseBlue * maxBlue);
        }
    }

    /* Swap Quarters I <-> III, II <-> IV */
    output = swap(output);

    if (specturmWindow == NULL) specturmWindow = new QLabel;
    specturmWindow->setPixmap(QPixmap::fromImage(output));
    specturmWindow->show();
}

void FFTW::convolution()
{
    int radius = ui->sbRadius->value();
    QVector<int> value;

    /* Sum of the Mask & Initialize */
    int mSum = 0;
    for (int i = 0; i < ui->tableWidget->rowCount(); ++i) {
        for (int j = 0; j < ui->tableWidget->columnCount(); ++j) {
            int arg = ui->tableWidget->item(i, j)->text().toInt();
            mSum += arg;
            value.push_back(arg);
        }
    }

    mSum = (mSum != 0) ? mSum : 1;

    /* 2D Array Works Better Than QImage, QImage produce some bugs (black screen) after few compute */
    /* Create And Fill Mask HxW */
    int **mMask = new int*[image.height()];
    int **mMaskReversed = new int*[image.height()];

    for(int i = 0; i < image.height(); i++){
        mMask[i] = new int[image.width()];
        mMaskReversed[i] = new int[image.width()];
    }

    for(int y = 0; y < image.height(); ++y){
        for(int x = 0; x < image.width(); ++x){
            mMask[y][x] = 0;
            mMaskReversed[y][x] = 0;
        }
    }

    /* Put Small Mask Into Big */
    int index = 0;
    for (int y = (image.height() / 2) - radius; y <= (image.height() / 2) + radius; ++y) {

        for (int x = (image.width() / 2) - radius; x <= (image.width() / 2) + radius; ++x) {

            mMask[y][x] = value[index];
            index++;
        }
    }

    /* Swap Quarters (Array) I <-> III, II <-> IV */
    swapArrays(mMask, mMaskReversed, image.width(), image.height());

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

        inRed[i][REALIS] = qRed(ptr_image[i]);
        inRed[i][IMAGINALIS] = 0;
        inGreen[i][REALIS] = qGreen(ptr_image[i]);
        inGreen[i][IMAGINALIS] = 0;
        inBlue[i][REALIS] = qBlue(ptr_image[i]);
        inBlue[i][IMAGINALIS] = 0;
        inMask[i][REALIS] = ((1.0 * mMaskReversed[i / image.width()][i % image.width()]) / mSum);
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
    planRed = fftw_plan_dft_2d(image.height(), image.width(), inRed, outRed, FFTW_BACKWARD, FFTW_ESTIMATE);
    planGreen = fftw_plan_dft_2d(image.height(), image.width(), inGreen, outGreen, FFTW_BACKWARD, FFTW_ESTIMATE);
    planBlue = fftw_plan_dft_2d(image.height(), image.width(), inBlue, outBlue, FFTW_BACKWARD, FFTW_ESTIMATE);

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
    convolution();
    showSpecturm(ui->comboBox->currentIndex());
}















