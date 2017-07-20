#include "cannyfilter.h"

CannyFilter::CannyFilter(QWidget *parent) : QWidget(parent)
{
    gaussUnsharpFilter = new GaussUnsharpFilter;
    connect(gaussUnsharpFilter, SIGNAL(sendImage(QImage)), this, SLOT(overloadImage(QImage)));

    xSobel = {1,  2,  1,
              0,  0,  0,
             -1,- 2, -1};

    ySobel = {-1,  0,  1,
              -2,  0,  2,
              -1,  0,  1};
}

void CannyFilter::setImage(QImage value)
{
    image = value;
    gaussUnsharpFilter->setImage(value);
}

void CannyFilter::processImage()
{
    int radius = 1, sum_rv, sum_gv, sum_bv, sum_rh, sum_gh, sum_bh, mask_index;
    QImage output_suppression = QImage(image.width(), image.height(), QImage::Format_RGB32);
    QImage output_horizontal = QImage(image.width(), image.height(), QImage::Format_RGB32);
    QImage output_vertical = QImage(image.width(), image.height(), QImage::Format_RGB32);
    QImage output_magnitude = QImage(image.width(), image.height(), QImage::Format_RGB32);
    QImage output_direction = QImage(image.width(), image.height(), QImage::Format_RGB32);
    QImage output_canny = QImage(image.width(), image.height(), QImage::Format_RGB32);

    /* 1. Preprocessing - Gauss */
    gaussUnsharpFilter->gaussianFilterFastCanny(2, 1);

    /* 2. Calculating Gradients */
    for (int y = 0; y < image.height(); ++y) {
        QRgb *ptr_horizontal = (QRgb*)output_horizontal.scanLine(y);
        QRgb *ptr_vertical = (QRgb*)output_vertical.scanLine(y);
        QRgb *ptr_magnitude = (QRgb*)output_magnitude.scanLine(y);
        QRgb *ptr_direction = (QRgb*)output_direction.scanLine(y);

        for (int x = 0; x < image.width(); ++x) {

            sum_rv = sum_gv = sum_bv = sum_rh = sum_bh = sum_gh = mask_index = 0;

            for (int i = -radius; i <= radius; ++i) {
                int yindex = ((y + i) > image.height()) ? y - i : abs(y + i);
                QRgb *ptr_image = (QRgb*)image.scanLine(yindex);

                for (int j = -radius; j <= radius; ++j) {
                    int xindex = ((x + j) > image.width()) ? x - j : abs(x + j);

                    int red = qRed(ptr_image[xindex]);
                    int green = qGreen(ptr_image[xindex]);
                    int blue = qBlue(ptr_image[xindex]);

                    /* 2. Horizontal - fx */
                    sum_rh += (red * xSobel[mask_index]);
                    sum_gh += (green * xSobel[mask_index]);
                    sum_bh += (blue * xSobel[mask_index]);

                    /* 2. Vertical - fy */
                    sum_rv += (red * ySobel[mask_index]);
                    sum_gv += (green * ySobel[mask_index]);
                    sum_bv += (blue * ySobel[mask_index]);

                    mask_index++;

                }
            }

            sum_rv = (sum_rv > 255) ? 255 : ((sum_rv < 0) ? 0 : sum_rv);
            sum_gv = (sum_gv > 255) ? 255 : ((sum_gv < 0) ? 0 : sum_gv);
            sum_bv = (sum_bv > 255) ? 255 : ((sum_bv < 0) ? 0 : sum_bv);

            sum_rh = (sum_rh > 255) ? 255 : ((sum_rh < 0) ? 0 : sum_rh);
            sum_gh = (sum_gh > 255) ? 255 : ((sum_gh < 0) ? 0 : sum_gh);
            sum_bh = (sum_bh > 255) ? 255 : ((sum_bh < 0) ? 0 : sum_bh);

            ptr_horizontal[x] = qRgb(sum_rv, sum_gv, sum_bv);
            ptr_vertical[x] = qRgb(sum_rh, sum_gh, sum_bh);

            /* 3. Gradient, Direction */
            int magnitude = sqrt(pow(qGray(ptr_vertical[x]), 2) + pow(qGray(ptr_horizontal[x]), 2));
            //float direction = (fmod(atan2(qGray(ptr_horizontal[x]), qGray(ptr_vertical[x])) + M_PI, M_PI) / M_PI) * 8;

            ptr_magnitude[x] = qRgb(magnitude, magnitude, magnitude);
            //ptr_direction[x] = qRgb(direction, direction, direction);
        }
    }

    /* 4. Nonmaximum Supression */
    for (int y = 0; y < image.height(); ++y) {
        QRgb *ptr_suppression = (QRgb*)output_suppression.scanLine(y);
        QRgb *ptr_horizontal = (QRgb*)output_horizontal.scanLine(y);
        QRgb *ptr_vertical = (QRgb*)output_vertical.scanLine(y);
        QRgb *ptr_magnitude = (QRgb*)output_magnitude.scanLine(y);
        QRgb *ptr_direction = (QRgb*)output_direction.scanLine(y);

        for (int x = 0; x < image.width(); ++x) {

            float direction = atan2(qGray(ptr_horizontal[x]), qGray(ptr_vertical[x])) * 180 / M_PI;

            if (direction > 67.5 && direction <= 112.5) {      //0 deg
                if (qGray(ptr_magnitude[x]) > qGray(ptr_magnitude[x - 1]) &&
                        qGray(ptr_magnitude[x]) > qGray(ptr_magnitude[x + 1])) {
                    ptr_suppression[x] = ptr_magnitude[x];
                    ptr_direction[x] = qRgb(0,0,0);
                }
            }
            else if (direction > 22.5 && direction <= 67.5) { //45 deg
                QRgb *ptr_down = (QRgb*)output_magnitude.scanLine(y - 1);
                QRgb *ptr_up = (QRgb*)output_magnitude.scanLine(y + 1);
                if (qGray(ptr_magnitude[x]) > qGray(ptr_down[x - 1]) && qGray(ptr_magnitude[x]) > qGray(ptr_up[x + 1])) {
                    ptr_suppression[x] = ptr_magnitude[x];
                    ptr_direction[x] = qRgb(45, 45, 45);
                }
            }
            else if (direction > 112.5 && direction <= 157.5){ //135 deg
                QRgb *ptr_down = (QRgb*)output_magnitude.scanLine(y - 1);
                QRgb *ptr_up = (QRgb*)output_magnitude.scanLine(y + 1);
                if (qGray(ptr_magnitude[x]) > qGray(ptr_down[x + 1]) && qGray(ptr_magnitude[x]) > qGray(ptr_up[x - 1])) {
                    ptr_suppression[x] = ptr_magnitude[x];
                    ptr_direction[x] = qRgb(135, 135, 135);
                }
            }
            else { //90 deg
                QRgb *ptr_down = (QRgb*)output_magnitude.scanLine(y - 1);
                QRgb *ptr_up = (QRgb*)output_magnitude.scanLine(y + 1);
                if (qGray(ptr_magnitude[x]) > qGray(ptr_down[x]) && qGray(ptr_magnitude[x]) > qGray(ptr_up[x])) {
                    ptr_suppression[x] = ptr_magnitude[x];
                    ptr_direction[x] = qRgb(90, 90, 90);
                }
            }
        }
    }

    /* 5. Thresholding with Hysterysis */
    int tmin = 15, tmax = 150;
    QQueue<QPoint> mqueue;

    for (int y = 0; y < image.height(); ++y) {
        QRgb *ptr_suppression = (QRgb*)output_suppression.scanLine(y);
        QRgb *ptr_canny = (QRgb*)output_canny.scanLine(y);

        for (int x = 0; x < image.width(); ++x) {

            if (qGray(ptr_suppression[x]) > tmax) ptr_canny[x] = qRgb(255, 255, 255);
            else if (qGray(ptr_suppression[x]) < tmin) ptr_canny[x] = qRgb(0, 0, 0);
            else {
                //ptr_canny[x] = qRgb(0, 255, 0);
                mqueue.push_back(QPoint(x,y));

                while (!mqueue.isEmpty()) {

                    QPoint index = mqueue.front();
                    mqueue.pop_front();

                    /* For Each Neighbor */
                    int sum = 0;
                    for (int i = -radius; i <= radius; ++i) {
                        int yindex = ((index.y() + i) > image.height()) ? index.y() - i : abs(index.y() + i);
                        QRgb *ptr_neighbor = (QRgb*)output_magnitude.scanLine(yindex);

                        for (int j = -radius; j <= radius; ++j) {
                            int xindex = ((index.x() + j) > image.width()) ? index.x() - j : abs(index.x() + j);

                            if (i != 0 && j != 0) {
                                sum += qGray(ptr_neighbor[xindex]);
//                                if (qGray(ptr_neighbor[xindex]) > tmin && qGray(ptr_canny[xindex]) == 0) {
//                                    ptr_canny[xindex] = qRgb(255, 255, 255);
//                                    mqueue.push_back(QPoint(xindex, yindex));
//                                }
                            }

                            if (i == radius && j == radius) {
                                if ((sum / 8) > tmin && qGray(ptr_canny[xindex]) == 0) {
                                    ptr_canny[x] = qRgb(255, 255, 255);
                                    mqueue.push_back(QPoint(xindex, yindex));
                                }
                            }
                        }
                    }
                }
            }

        }
    }

//    QLabel *windowA = new QLabel;
//    windowA->setPixmap(QPixmap::fromImage(output_horizontal));
//    windowA->show();

//    QLabel *windowB = new QLabel;
//    windowB->setPixmap(QPixmap::fromImage(output_vertical));
//    windowB->show();

    //sendImage(output_suppression);
    sendImage(output_canny);
}

void CannyFilter::overloadImage(QImage value)
{
    image = value;
}



































