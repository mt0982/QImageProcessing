#include "staticfilter.h"
#include "ui_staticfilter.h"

StaticFilter::StaticFilter(QWidget *parent) : QWidget(parent), ui(new Ui::StaticFilter)
{
    ui->setupUi(this);
    ui->sbRadius->setButtonSymbols(QAbstractSpinBox::PlusMinus);
    ui->sbRadiusMax->setButtonSymbols(QAbstractSpinBox::PlusMinus);
}

StaticFilter::~StaticFilter()
{
    delete ui;
}

void StaticFilter::setImage(const QImage &value)
{
    image = value;
}

void StaticFilter::filter(int nr)
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
    sendImage(output);
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
    filter(1);
    image = output;
    filter(2);
    image = tmp;
}

void StaticFilter::on_pbClosing_clicked()
{
    QImage tmp = image;
    filter(0);
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
    int mred[256], mgreen[256], mblue[256];
    output = QImage(image.width(), image.height(), QImage::Format_RGB32);

    for (int y = 0; y < image.height(); ++y) {
        QRgb *ptr_input = (QRgb*)image.scanLine(y);
        QRgb *ptr_output = (QRgb*)output.scanLine(y);

        memset(mred, 0, sizeof(int) * 256);
        memset(mgreen, 0, sizeof(int) * 256);
        memset(mblue, 0, sizeof(int) * 256);

        for (int x = 0; x < image.width(); ++x) {

            /* Reset Radius Value */
            radius_min = ui->sbRadius->value();
            ended = false;

            /* Adaptive Median */
            while (radius_min <= radius_max) {
                med = pow(radius_min * 2 + 1, 2) / 2;

                /* If first column then fill array */
                if (!x) {
                    for (int i = -radius_min; i <= radius_min; ++i) {
                        int index = ((y + i) > image.height()) ? y - i : abs(y + i);
                        QRgb *ptr_image = (QRgb*)image.scanLine(index);

                        for (int j = -radius_min; j <= radius_min; ++j) {
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
                    int offradius_min = radius_min + 1;
                    for (int i = -radius_min; i <= radius_min; ++i) {
                        int column = ((y + i) > image.height()) ? y - i : abs(y + i);
                        int row = ((x + radius_min) > image.width()) ? x - radius_min : abs(x + radius_min);
                        ptr_off = (QRgb*)image.scanLine(column);
                        mred[qRed(ptr_off[row])]++;
                        mgreen[qGreen(ptr_off[row])]++;
                        mblue[qBlue(ptr_off[row])]++;
                    }
                    /* Remove Old */
                    for (int i = -radius_min; i <= radius_min; ++i) {
                        int column = ((y + i) > image.height()) ? y - i : abs(y + i);
                        int row = ((x + offradius_min) > image.width()) ? x - offradius_min : abs(x - offradius_min);
                        ptr_off = (QRgb*)image.scanLine(column);
                        mred[qRed(ptr_off[row])]--;
                        mgreen[qGreen(ptr_off[row])]--;
                        mblue[qBlue(ptr_off[row])]--;
                    }
                }

                /* Lambda Expression - Sort */
                auto lambda_max = [](int *arr) {
                    for (int mmax = 255; mmax >= 0; mmax--) {
                        if (arr[mmax] != 0)
                            return mmax;
                    }
                };

                auto lambda_med = [med](int *arr) {
                    int counter = 0, value = -1;
                    while (counter <= med) {
                        value++;
                        counter += arr[value];
                    } return value;
                };

                auto lambda_min = [](int *arr) {
                    for (int mmin = 0; mmin < 256; mmin++) {
                        if (arr[mmin] != 0)
                            return mmin;
                    }
                };

                /* Adaptive Median Conditions */
                if ((lambda_min(mred) < lambda_med(mred)) && (lambda_med(mred) < lambda_max(mred))) {// min < med < max
                    if ((lambda_min(mred) < qRed(ptr_input[x])) && (qRed(ptr_input[x]) < lambda_max(mred))) {//min < f(x,y) < max
                        ptr_output[x] = ptr_input[x];
                        ended = true;
                        break;
                    } else {
                        ptr_output[x] = qRgb(lambda_med(mred), lambda_med(mred), lambda_med(mred));
                        ended = true;
                        break;
                    }
                } else {
                    radius_min++;
                }
            }

            auto lambda_med = [med](int *arr) {
                int counter = 0, value = -1;
                while (counter <= med) {
                    value++;
                    counter += arr[value];
                } return value;
            };

            if (!ended) ptr_output[x] = qRgb(lambda_med(mred), lambda_med(mred), lambda_med(mred));
        }
    }

    /* Send Output */
    sendImage(output);
}


































