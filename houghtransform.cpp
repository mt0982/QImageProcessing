#include "houghtransform.h"
#include "ui_houghtransform.h"

HoughTransform::HoughTransform(FacadeImage *parent) : FacadeImage(parent), ui(new Ui::HoughTransform)
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
    QImage iOutput(image);
    canny->setImage(image);
    canny->processImage(3, 100, 150);

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

                    int r = (x - image.width() / 2) * cos((m * M_PI) / 180) + (y - image.height() / 2)
                                  * sin((m * M_PI) / 180);

                    quint8 *ptr_iaccumulator = iAccumulator.scanLine(r + height);
                    ptr_iaccumulator[m] += (ptr_iaccumulator[m] + 1 < 255) ? 1 : 0;
                    accumulator[r + height][m]++;
                }
            }
        }
    }
    /* */
    QPainter iPainter(&iOutput);
    iPainter.setPen(QPen(QColor(200, 0, 0, 100), 1));

    for (int y = 0; y < iAccumulator.height(); ++y) {

        for (int x = 0; x < iAccumulator.width(); ++x) {

            if (accumulator[y][x] > 175) {

                int mmax = accumulator[y][x];
                for (int lx = -4; lx <= 4; lx++) {

                    for (int ly = -4; ly <= 4; ly++) {

                        if ((x + lx > 0) && (x + lx < iAccumulator.width())) {
                            if ((y + ly > 0) && (y + ly < iAccumulator.height())) {

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

//    int maxima = 0;
//    QVector<QVector3D> sortedArray;
//    for (int r = 0; r < iAccumulator.height(); ++r) {
//        for (int m = 0; m < iAccumulator.width(); ++m) {
//            if (accumulator[r][m] > maxima)
//                maxima = accumulator[r][m];

//            int count = accumulator[r][m];
//            sortedArray.push_back(QVector3D(count, r, m));
//        }
//    }

//    std::sort(sortedArray.begin(), sortedArray.end(), [](const QVector3D &a, const QVector3D &b)->bool{
//        return a.x() > b.x();
//    });

//    qDebug() << maxima;

//    /* */
//    QPainter iPainter(&iOutput);
//    iPainter.setPen(QPen(QColor(200, 0, 0, 100), 1));

//    for (int i = 0; i < 100; i++) {

//        float rho = sortedArray[i][1], theta = sortedArray[i][2] - 90;
//        double a = cos(theta), b = sin(theta);
//        double x0 = a*rho, y0 = b*rho;

//        int p1x = round(x0 + 1000 * (-b));
//        int p1y = round(y0 + 1000 * (a));
//        int p2x = round(x0 - 1000 * (-b));
//        int p2y = round(y0 - 1000 * (a));
//        iPainter.drawLine(p1x, p1y, p2x, p2y);

//        qDebug() << p1x << p1y << p2x << p2y;
//    }

    iAccumulator = iAccumulator.scaled(image.width(), image.height(), Qt::IgnoreAspectRatio, Qt::SmoothTransformation);
    sendImage(iAccumulator);

    sendImage(iOutput);
    //sendImage(image);
}



















