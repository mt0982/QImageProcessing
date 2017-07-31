#include "staticfilter.h"
#include "ui_staticfilter.h"

StaticFilter::StaticFilter(QWidget *parent) : QWidget(parent), ui(new Ui::StaticFilter)
{
    ui->setupUi(this);
}

StaticFilter::~StaticFilter()
{
    delete ui;
}

void StaticFilter::setImage(const QImage &value)
{
    image = value;
}

void StaticFilter::filter(int nr, bool send)
{
    /* Parameters */
    int radius = ui->sbRadius->value();
    output = QImage(image.width(), image.height(), QImage::Format_RGB32);
    Color color;

    for (int y = 0; y < image.height(); ++y) {
        QRgb *ptr_copy = (QRgb*)output.scanLine(y);

        for (int x = 0; x < image.width(); ++x) {

            for (int i = -radius; i <= radius; ++i) {
                int index = ((y+i) > image.height()) ? y-i : abs(y+i);
                QRgb *ptr_image = (QRgb*)image.scanLine(index);

                for (int j = -radius; j <= radius; ++j) {
                    index = ((x+j) > image.width()) ? x-j : abs(x+j);
                    int red = qRed(ptr_image[index]);
                    int green = qGreen(ptr_image[index]);
                    int blue = qBlue(ptr_image[index]);
                    color.add(red, green, blue);
                }
            }

            color.msort();
            switch (nr) {
            case 0:
                ptr_copy[x] = qRgb(color.mmin().x(), color.mmin().y(), color.mmin().z());
                break;
            case 1:
                ptr_copy[x] = qRgb(color.mmax().x(), color.mmax().y(), color.mmax().z());
                break;
            case 2:
                ptr_copy[x] = qRgb(color.mmed().x(), color.mmed().y(), color.mmed().z());
                break;
            default:
                break;
            }
            color.mclear();
        }
    }

    /* Send Output */
    if (send) sendImage(output);
}

void StaticFilter::on_pbMinimum_clicked()
{
    filter(0);
}

void StaticFilter::on_pbMaximum_clicked()
{
    filter(1);
}

void StaticFilter::on_pushButton_3_clicked()
{
    filter(2);
}

void StaticFilter::on_pbOpening_clicked()
{
    QImage tmp = image;
    filter(1, false);
    image = output;
    filter(0);
    image = tmp;
}

void StaticFilter::on_pbClosing_clicked()
{
    QImage tmp = image;
    filter(0, false);
    image = output;
    filter(1);
    image = tmp;
}

void StaticFilter::on_pbFastMedian_clicked()
{
    int radius = ui->sbRadius->value();
    int mid = pow(radius * 2 + 1, 2) / 2;
    int mred[256], mgreen[256], mblue[256];
    output = QImage(image.width(), image.height(), QImage::Format_RGB32);

    for (int y = 0; y < image.height(); ++y) {
        QRgb *ptr_output = (QRgb*)output.scanLine(y);

        memset(mred, 0, sizeof(int) * 255);
        memset(mgreen, 0, sizeof(int) * 255);
        memset(mblue, 0, sizeof(int) * 255);

        for (int x = 0; x < image.width(); ++x) {

            /* If first column then fill array */
            if (!x) {
                for (int i = -radius; i <= radius; ++i) {
                    int index = ((y + i) > image.height()) ? y - i : abs(y + i);
                    QRgb *ptr_image = (QRgb*)image.scanLine(index);

                    for (int j = -radius; j <= radius; ++j) {
                        index = ((x + j) > image.width()) ? x - j : abs(x + j);
                        int red = qRed(ptr_image[index]);
                        int green = qGreen(ptr_image[index]);
                        int blue = qBlue(ptr_image[index]);

                        mred[red]++;
                        mgreen[green]++;
                        mblue[blue]++;
                    }
                }
            }
            else {
                /* Add New */
                QRgb *ptr_off;
                int offradius = radius + 1;
                for (int i = -radius; i <= radius; ++i) {
                    int column = ((y + i) > image.height()) ? y - i : abs(y + i);
                    int row = ((x + radius) > image.width()) ? x - radius : abs(x + radius);
                    ptr_off = (QRgb*)image.scanLine(column);
                    mred[qRed(ptr_off[row])]++;
                    mgreen[qGreen(ptr_off[row])]++;
                    mblue[qBlue(ptr_off[row])]++;
                }
                /* Remove Old */
                for (int i = -radius; i <= radius; ++i) {
                    int column = ((y + i) > image.height()) ? y - i : abs(y + i);
                    int row = ((x + offradius) > image.width()) ? x - offradius : abs(x - offradius);
                    ptr_off = (QRgb*)image.scanLine(column);
                    mred[qRed(ptr_off[row])]--;
                    mgreen[qGreen(ptr_off[row])]--;
                    mblue[qBlue(ptr_off[row])]--;
                }
            }

            /* Lambda Expression - Sort */
            auto msort = [mid](int *arr) {
                int counter = 0, value = -1;
                while (counter <= mid) {
                    value++;
                    counter += arr[value];
                }
                return value;
            };

            ptr_output[x] = qRgb(msort(mred), msort(mgreen), msort(mblue));
        }
    }

    /* Send Output */
    sendImage(output);
}

void StaticFilter::on_pbAdaptiveMedian_clicked()
{
    int radius_min = ui->sbRadius->value();
    int radius_max = ui->sbRadiusMax->value(), med;
    bool ended;
    QVector<int> mvec;
    output = QImage(image.width(), image.height(), QImage::Format_RGB32);

    for (int y = 0; y < image.height(); ++y) {
        QRgb *ptr_input = (QRgb*)image.scanLine(y);
        QRgb *ptr_output = (QRgb*)output.scanLine(y);

        /* RED */
        for (int x = 0; x < image.width(); ++x) {

            /* Reset Radius Value */
            radius_min = ui->sbRadius->value();
            ended = false;

            /* Adaptive Median */
            while (radius_min <= radius_max) {
                med = pow(radius_min * 2 + 1, 2) / 2;
                mvec.clear();

                for (int i = -radius_min; i <= radius_min; ++i) {
                    int index = ((y + i) > image.height()) ? y - i : abs(y + i);
                    QRgb *ptr_image = (QRgb*)image.scanLine(index);

                    for (int j = -radius_min; j <= radius_min; ++j) {
                        index = ((x + j) > image.width()) ? x - j : abs(x + j);
                        int red = qRed(ptr_image[index]);

                        mvec.push_back(red);
                    }
                }

               std::sort(mvec.begin(), mvec.end());
               int mmin = mvec[0];
               int mmed = mvec[med];
               int mmax = mvec[mvec.size() - 1];

               int A1 = mmed - mmin;
               int A2 = mmed - mmax;

               if (A1 > 0 && A2 < 0) {

                   int B1 = qRed(ptr_input[x]) - mmin;
                   int B2 = qRed(ptr_input[x]) - mmax;

                   if (B1 > 0 && B2 < 0) {
                       ptr_output[x] += qRgb(qRed(ptr_input[x]), 0, 0);
                       ended = true;
                       break;
                   } else {
                       ptr_output[x] += qRgb(mmed, 0, 0);
                       ended = true;
                       break;
                   }
               }

               radius_min++;
            }

            std::sort(mvec.begin(), mvec.end());
            int mmed = mvec[med];
            if (!ended) ptr_output[x] += qRgb(mmed, 0, 0);
        }

        /* GREEN */
        for (int x = 0; x < image.width(); ++x) {

            /* Reset Radius Value */
            radius_min = ui->sbRadius->value();
            ended = false;

            /* Adaptive Median */
            while (radius_min <= radius_max) {
                med = pow(radius_min * 2 + 1, 2) / 2;
                mvec.clear();

                for (int i = -radius_min; i <= radius_min; ++i) {
                    int index = ((y + i) > image.height()) ? y - i : abs(y + i);
                    QRgb *ptr_image = (QRgb*)image.scanLine(index);

                    for (int j = -radius_min; j <= radius_min; ++j) {
                        index = ((x + j) > image.width()) ? x - j : abs(x + j);
                        int green = qGreen(ptr_image[index]);

                        mvec.push_back(green);
                    }
                }

               std::sort(mvec.begin(), mvec.end());
               int mmin = mvec[0];
               int mmed = mvec[med];
               int mmax = mvec[mvec.size() - 1];

               int A1 = mmed - mmin;
               int A2 = mmed - mmax;

               if (A1 > 0 && A2 < 0) {

                   int B1 = qGreen(ptr_input[x]) - mmin;
                   int B2 = qGreen(ptr_input[x]) - mmax;

                   if (B1 > 0 && B2 < 0) {
                       ptr_output[x] += qRgb(0, qGreen(ptr_input[x]), 0);
                       ended = true;
                       break;
                   } else {
                       ptr_output[x] += qRgb(0, mmed, 0);
                       ended = true;
                       break;
                   }
               }

               radius_min++;
            }

            std::sort(mvec.begin(), mvec.end());
            int mmed = mvec[med];
            if (!ended) ptr_output[x] += qRgb(0, mmed, 0);
        }

        /* BLUE */
        for (int x = 0; x < image.width(); ++x) {

            /* Reset Radius Value */
            radius_min = ui->sbRadius->value();
            ended = false;

            /* Adaptive Median */
            while (radius_min <= radius_max) {
                med = pow(radius_min * 2 + 1, 2) / 2;
                mvec.clear();

                for (int i = -radius_min; i <= radius_min; ++i) {
                    int index = ((y + i) > image.height()) ? y - i : abs(y + i);
                    QRgb *ptr_image = (QRgb*)image.scanLine(index);

                    for (int j = -radius_min; j <= radius_min; ++j) {
                        index = ((x + j) > image.width()) ? x - j : abs(x + j);
                        int blue = qBlue(ptr_image[index]);

                        mvec.push_back(blue);
                    }
                }

               std::sort(mvec.begin(), mvec.end());
               int mmin = mvec[0];
               int mmed = mvec[med];
               int mmax = mvec[mvec.size() - 1];

               int A1 = mmed - mmin;
               int A2 = mmed - mmax;

               if (A1 > 0 && A2 < 0) {

                   int B1 = qBlue(ptr_input[x]) - mmin;
                   int B2 = qBlue(ptr_input[x]) - mmax;

                   if (B1 > 0 && B2 < 0) {
                       ptr_output[x] += qRgb(0, 0, qBlue(ptr_input[x]));
                       ended = true;
                       break;
                   } else {
                       ptr_output[x] += qRgb(0, 0, mmed);
                       ended = true;
                       break;
                   }
               }

               radius_min++;
            }

            std::sort(mvec.begin(), mvec.end());
            int mmed = mvec[med];
            if (!ended) ptr_output[x] += qRgb(0, 0, mmed);
        }
    }

    /* Send Output */
    sendImage(output);
}

void StaticFilter::on_pushButton_clicked()
{
    int radius = ui->sbRadius->value();
    double sigma_distance = ui->sbDistance->value();
    double sigma_intensity = ui->sbIntensity->value();
    output = QImage(image.width(), image.height(), QImage::Format_RGB32);

    double distance, intensity_r, intensity_g, intensity_b;
    double norm_r, norm_g, norm_b, mred, mgreen, mblue;

    /* Circural - values to remove */
    QVector<int> index3x3 = {0, 2, 6, 8};
    QVector<int> index5x5 = {0, 1, 3, 4, 5, 9, 15, 19, 20, 21, 23, 24};
    QVector<int> index7x7 = {0, 1, 2, 4, 5, 6, 7, 8, 12, 13, 14, 20, 28, 34, 35, 36, 40, 41, 42, 43, 44, 46, 47, 48};
    QVector<int> index9x9 = {0, 2, 6, 8};
    QVector<int> index11x11 = {0, 2, 6, 8};

    for (int y = 0; y < image.height(); ++y) {
        QRgb *ptr_input = (QRgb*)image.scanLine(y);
        QRgb *ptr_output = (QRgb*)output.scanLine(y);

        for (int x = 0; x < image.width(); ++x) {

            norm_r = norm_g = norm_b = mred = mgreen = mblue = 0;
            QGenericMatrix<7, 7, float> matrix;

            for (int i = -radius; i <= radius; ++i) {
                int yindex = ((y + i) > image.height()) ? y - i : abs(y + i);
                QRgb *ptr_kernel = (QRgb*)image.scanLine(yindex);

                for (int j = -radius; j <= radius; ++j) {
                    int xindex = ((x + j) > image.width()) ? x - j : abs(x + j);

                    int circularIndex = (i + radius) * (radius * 2 + 1) + (j + radius);

                    switch (radius) {
                    case 1:
                        distance = (!index3x3.contains(circularIndex)) ?
                                        exp(-0.5 * (pow(x - (xindex), 2) + pow(y - (yindex), 2)) / pow(sigma_distance, 2)) : 0;
                        break;
                    case 2:
                        distance = (!index5x5.contains(circularIndex)) ?
                                        exp(-0.5 * (pow(x - (xindex), 2) + pow(y - (yindex), 2)) / pow(sigma_distance, 2)) : 0;
                        break;
                    case 3:
                        distance = (!index7x7.contains(circularIndex)) ?
                                        exp(-0.5 * (pow(x - (xindex), 2) + pow(y - (yindex), 2)) / pow(sigma_distance, 2)) : 0;
                        break;
                    default:
                        break;
                    }

                    intensity_r = exp(-0.5*(pow(qRed(ptr_kernel[xindex]) - qRed(ptr_input[x]), 2) / pow(sigma_intensity, 2)));
                    intensity_g = exp(-0.5*(pow(qGreen(ptr_kernel[xindex]) - qGreen(ptr_input[x]), 2) / pow(sigma_intensity, 2)));
                    intensity_b = exp(-0.5*(pow(qBlue(ptr_kernel[xindex]) - qBlue(ptr_input[x]), 2) / pow(sigma_intensity, 2)));

                    norm_r += (distance * intensity_r);
                    norm_g += (distance * intensity_g);
                    norm_b += (distance * intensity_b);

                    mred += (qRed(ptr_kernel[xindex]) * distance * intensity_r);
                    mgreen += (qGreen(ptr_kernel[xindex]) * distance * intensity_g);
                    mblue += (qBlue(ptr_kernel[xindex]) * distance * intensity_b);

                    /* Gauss Kernel */
                    matrix.data()[circularIndex] = distance;
                }
            }

            qDebug() << matrix; return;

            ptr_output[x] = qRgb(mred / norm_r, mgreen / norm_g, mblue / norm_b);
        }
    }

    /* Send Output */
    sendImage(output);
}


































