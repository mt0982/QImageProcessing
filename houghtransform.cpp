#include "houghtransform.h"
#include "ui_houghtransform.h"

HoughTransform::HoughTransform(FacadeImage *parent) : FacadeImage(parent), ui(new Ui::HoughTransform),
    windowAccumulator(NULL), flag(false)
{
    ui->setupUi(this);

    canny = new Canny;
    connect(canny, &Canny::sendImage, [this](const QImage &value) {
        image = value;
    });
}

HoughTransform::~HoughTransform()
{
    delete ui;
}

void HoughTransform::on_pbCalculate_clicked()
{
    /* Detect Edges - Canny */
    if (!flag) {
        iOriginal = image;
        canny->setImage(image);
        canny->processImage(3, 100, 150);
        flag = true;
    }

    QImage iOutput(iOriginal);

    /* Accumulato */
    int height = floor(sqrt(pow(image.width(), 2) + pow(image.height(), 2)));       //rows = max(diagonal)
    QImage iAccumulator(180, 2 * height, QImage::Format_Grayscale8);                //-R < height < R, R = diagonal
    iAccumulator.fill(0);

    int accumulator[iAccumulator.height()][iAccumulator.width()];
    memset(accumulator, 0, sizeof(accumulator));

    for (int y = 0; y < image.height(); ++y) {

        quint8 *ptr_image = image.scanLine(y);

        for (int x = 0; x < image.width(); ++x) {

            if (ptr_image[x] != 0) {
                for (int m = 0; m < 180; m++) {

                    float angle = (m * M_PI) / 180;
                    float r = (x - image.width() / 2) * cos(angle) + (y - image.height() / 2) * sin(angle);

                    quint8 *ptr_iaccumulator = iAccumulator.scanLine(r + height);
                    ptr_iaccumulator[m] += (ptr_iaccumulator[m] + 1 < 255) ? 1 : 0;
                    accumulator[(int)round(r + height)][m]++;
                }
            }
        }
    }

    /* Normalize */
    int mmax = 0;
    for (int y = 0; y < 2 * height; ++y) {
        for (int x = 0; x < 180; ++x) {
            if (accumulator[y][x] > mmax)
                mmax = accumulator[y][x];
        }
    }

    float norm = 255.0f / mmax;
    QImage iAccumulatoNormalized(iAccumulator);
    iAccumulatoNormalized.fill(0);

    for (int y = 0; y < height * 2; ++y) {
        quint8 *ptr_iaccumulator = iAccumulatoNormalized.scanLine(y);
        for (int x = 0; x < 180; ++x) {
            ptr_iaccumulator[x] = accumulator[y][x] * norm;
        }
    }

    /* Draw Lines */
    QPainter iPainter(&iOutput);
    iPainter.setPen(QPen(QColor(200, 0, 0, 100), 1));

    for (int y = 0; y < iAccumulator.height(); ++y) {

        for (int x = 0; x < iAccumulator.width(); ++x) {

            if (accumulator[y][x] > ui->sbThreshold->value()) {

                int mmax = accumulator[y][x];
                for (int lx = -4; lx <= 4; lx++) {

                    for (int ly = -4; ly <= 4; ly++) {

                        if ((x + lx >= 0) && (x + lx < iAccumulator.width())) {
                            if ((y + ly >= 0) && (y + ly < iAccumulator.height())) {

                                if (accumulator[y + ly][x + lx] > mmax) {
                                    mmax = accumulator[y + ly][x + lx];
                                    lx = ly = 5;
                                }

                            }
                        }
                    }
                }

                if (accumulator[y][x] > mmax)     //maxima was found
                    continue;

                int x1, y1, x2, y2;
                x1 = y1 = x2 = y2 = 0;

                if(x >= 45 && x <= 135) {

                    x1 = 0;
                    y1 = ((y - (iAccumulator.height() / 2)) - (x1 - (image.width() / 2)) * cos((x * M_PI) / 180)) /
                            sin((x * M_PI) / 180) + (image.height() / 2);
                    x2 = image.width();
                    y2 = ((y - (iAccumulator.height() / 2)) - (x2 - (image.width() / 2)) * cos((x * M_PI) / 180)) /
                            sin((x * M_PI) / 180) + (image.height() / 2);
                }
                else {

                    y1 = 0;
                    x1 = ((y - (iAccumulator.height() / 2)) - (y1 - (image.height() / 2)) * sin((x * M_PI) / 180)) /
                            cos((x * M_PI) / 180) + (image.width() / 2);
                    y2 = image.height();
                    x2 = ((y - (iAccumulator.height() / 2)) - (y2 - (image.height() / 2)) * sin((x * M_PI) / 180)) /
                            cos((x * M_PI) / 180) + (image.width() / 2);
                }

                iPainter.drawLine(x1, y1, x2, y2);
            }
        }
    }

    iAccumulator = iAccumulator.scaled(image.width(), image.height(), Qt::IgnoreAspectRatio, Qt::SmoothTransformation);
    iAccumulatoNormalized = iAccumulatoNormalized.scaled(image.width(), image.height(),
                                                         Qt::IgnoreAspectRatio, Qt::SmoothTransformation);
    //sendImage(iAccumulatoNormalized);
    //sendImage(iAccumulator);
    sendImage(iOutput);
    //sendImage(image);

    if(!windowAccumulator) windowAccumulator = new QLabel;
    windowAccumulator->setPixmap(QPixmap::fromImage(iAccumulatoNormalized));
    windowAccumulator->show();
}

void HoughTransform::setImage(const QImage &value)
{
    /* Flag - Calculate Canny Only Once Per New Image */
    image = value;
    flag = false;
}













