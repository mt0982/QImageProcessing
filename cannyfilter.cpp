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
    int tmin = 15, tmax = 150;
    QImage output_suppression = QImage(image.width(), image.height(), QImage::Format_RGB32);
    QImage output_horizontal = QImage(image.width(), image.height(), QImage::Format_RGB32);
    QImage output_vertical = QImage(image.width(), image.height(), QImage::Format_RGB32);
    QImage output_magnitude = QImage(image.width(), image.height(), QImage::Format_RGB32);
    QImage output_direction = QImage(image.width(), image.height(), QImage::Format_RGB32);
    QImage output_canny = QImage(image.width(), image.height(), QImage::Format_RGB32);

    /* 1. Preprocessing - Gauss */
    gaussUnsharpFilter->gaussianFilterFastCanny(2, 140);

    /* 2. Calculating Gradients (Magnitude) & Direction */
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

            /* 2. Gradient, Direction */
            int magnitude = sqrt(pow(qGray(ptr_vertical[x]), 2) + pow(qGray(ptr_horizontal[x]), 2));
            float direction = atan2(qGray(ptr_horizontal[x]), qGray(ptr_vertical[x])) * 180 / M_PI;

            ptr_magnitude[x] = qRgb(magnitude, magnitude, magnitude);
            ptr_direction[x] = qRgb(direction, direction, direction);
        }
    }

    /* 3. Nonmaximum Supression */
    for (int y = 0; y < image.height(); ++y) {
        QRgb *ptr_suppression = (QRgb*)output_suppression.scanLine(y);
        QRgb *ptr_horizontal = (QRgb*)output_horizontal.scanLine(y);
        QRgb *ptr_vertical = (QRgb*)output_vertical.scanLine(y);
        QRgb *ptr_magnitude = (QRgb*)output_magnitude.scanLine(y);
        QRgb *ptr_direction = (QRgb*)output_direction.scanLine(y);

        for (int x = 0; x < image.width(); ++x) {

            float direction = qGray(ptr_direction[x]);
            while(direction < 0) direction += 180;

            if (pow(qGray(ptr_horizontal[x]),2) + pow(qGray(ptr_vertical[x]),2) < tmax) continue;

            if (direction > 67.5 && direction <= 112.5) {      //90 deg
                if (qGray(ptr_magnitude[x]) > qGray(ptr_magnitude[x - 1]) &&
                        qGray(ptr_magnitude[x]) > qGray(ptr_magnitude[x + 1])) {
                    ptr_suppression[x] = qRgb(255, 255, 255); //ptr_magnitude[x];
                    ptr_direction[x] = qRgb(90, 90, 90);
                }
            }
            else if (direction > 22.5 && direction <= 67.5) { //45 deg
                QRgb *ptr_down = (QRgb*)output_magnitude.scanLine(y - 1);
                QRgb *ptr_up = (QRgb*)output_magnitude.scanLine(y + 1);
                if (qGray(ptr_magnitude[x]) > qGray(ptr_down[x - 1]) && qGray(ptr_magnitude[x]) > qGray(ptr_up[x + 1])) {
                    ptr_suppression[x] = qRgb(255, 255, 255); //ptr_magnitude[x];
                    ptr_direction[x] = qRgb(45, 45, 45);
                }
            }
            else if (direction > 112.5 && direction <= 157.5){ //135 deg
                QRgb *ptr_down = (QRgb*)output_magnitude.scanLine(y - 1);
                QRgb *ptr_up = (QRgb*)output_magnitude.scanLine(y + 1);
                if (qGray(ptr_magnitude[x]) > qGray(ptr_down[x + 1]) && qGray(ptr_magnitude[x]) > qGray(ptr_up[x - 1])) {
                    ptr_suppression[x] = qRgb(255, 255, 255); //ptr_magnitude[x];
                    ptr_direction[x] = qRgb(135, 135, 135);
                }
            }
            else if ((direction > 157.5 && direction <= 180) || (direction > 0 || direction <= 22.5)) { //0 deg
                QRgb *ptr_down = (QRgb*)output_magnitude.scanLine(y - 1);
                QRgb *ptr_up = (QRgb*)output_magnitude.scanLine(y + 1);
                if (qGray(ptr_magnitude[x]) > qGray(ptr_down[x]) && qGray(ptr_magnitude[x]) > qGray(ptr_up[x])) {
                    ptr_suppression[x] = qRgb(255, 255, 255); //ptr_magnitude[x];
                    ptr_direction[x] = qRgb(200, 200, 200);
                }
            }
        }
    }

    /* 4. Thresholding with Hysterysis */
    bool flag = true;
    while (flag) {

        flag = false;

        for (int y = 0; y < image.height(); ++y) {
            QRgb *ptr_suppression = (QRgb*)output_suppression.scanLine(y);
            QRgb *ptr_magnitude = (QRgb*)output_magnitude.scanLine(y);
            QRgb *ptr_direction = (QRgb*)output_direction.scanLine(y);

            for (int x = 0; x < image.width(); ++x) {
                QRgb *ptr_magDown = (QRgb*)output_magnitude.scanLine(y - 1);
                QRgb *ptr_magUp = (QRgb*)output_magnitude.scanLine(y + 1);
                QRgb *ptr_supDown = (QRgb*)output_suppression.scanLine(y - 1);
                QRgb *ptr_supUp = (QRgb*)output_suppression.scanLine(y + 1);
                QRgb *ptr_dirDown = (QRgb*)output_direction.scanLine(y - 1);
                QRgb *ptr_dirUp = (QRgb*)output_direction.scanLine(y + 1);
                QRgb *ptr_magDown2 = (QRgb*)output_magnitude.scanLine(y - 2);
                QRgb *ptr_magUp2 = (QRgb*)output_magnitude.scanLine(y + 2);

                if (qGray(ptr_suppression[x]) == 255) {

                    ptr_suppression[x] = qRgb(64, 64, 64);
                    float direction = qGray(ptr_direction[x]);

                    if (direction > 112.5 && direction <= 157.5) {

                        /* Left-Bottom */
                        if (qGray(ptr_magDown[x - 1]) >= tmin &&
                                qGray(ptr_supDown[x - 1]) != 64 &&
                                qGray(ptr_dirDown[x - 1]) > 112.5 &&
                                qGray(ptr_dirDown[x - 1]) <= 157.5 &&
                                qGray(ptr_magDown[x - 1]) > qGray(ptr_magDown2[x]) &&
                                qGray(ptr_magDown[x - 1]) > qGray(ptr_magnitude[x - 2]))
                        {
                            ptr_supDown[x - 1] =  qRgb(255, 0, 0);
                            flag = true;
                        }

                        /* Right-Top */
                        if (qGray(ptr_magUp[x + 1]) >= tmin &&
                                qGray(ptr_supUp[x + 1]) != 64 &&
                                qGray(ptr_dirUp[x + 1]) > 112.5 &&
                                qGray(ptr_dirUp[x + 1]) <= 157.5 &&
                                qGray(ptr_magUp[x + 1]) > qGray(ptr_magUp2[x]) &&
                                qGray(ptr_magUp[x + 1]) > qGray(ptr_magnitude[x + 2]))
                        {
                            ptr_supUp[x + 1] =  qRgb(255, 0, 0);
                            flag = true;
                        }
                    }
                    else if (direction > 67.5 && direction <= 112.5) {

                        /* Left */
                        if (qGray(ptr_magnitude[x - 1]) >= tmin &&
                                qGray(ptr_suppression[x - 1]) != 64 &&
                                qGray(ptr_direction[x - 1]) > 67.5 &&
                                qGray(ptr_direction[x - 1]) <= 112.5 &&
                                qGray(ptr_magnitude[x - 1]) > qGray(ptr_magDown[x - 1]) &&
                                qGray(ptr_magnitude[x - 1]) > qGray(ptr_magUp[x - 1]))
                        {
                            ptr_suppression[x - 1] =  qRgb(255, 0, 0);
                            flag = true;
                        }

                        /* Right */
                        if (qGray(ptr_magnitude[x + 1]) >= tmin &&
                                qGray(ptr_suppression[x + 1]) != 64 &&
                                qGray(ptr_direction[x + 1]) > 67.5 &&
                                qGray(ptr_direction[x + 1]) <= 112.5 &&
                                qGray(ptr_magnitude[x + 1]) > qGray(ptr_magUp[x + 1]) &&
                                qGray(ptr_magnitude[x + 1]) > qGray(ptr_magDown[x + 1]))
                        {
                            ptr_suppression[x + 1] =  qRgb(255, 0, 0);
                            flag = true;
                        }
                    }
                    else if(direction > 22.5 && direction <= 67.5) {

                        /* Right-Bottom */
                        if (qGray(ptr_magDown[x + 1]) >= tmin &&
                                qGray(ptr_supDown[x + 1]) != 64 &&
                                qGray(ptr_dirDown[x + 1]) > 22.5 &&
                                qGray(ptr_dirDown[x + 1]) <= 67.5 &&
                                qGray(ptr_magDown[x + 1]) > qGray(ptr_magnitude[x + 2]) &&
                                qGray(ptr_magDown[x + 1]) > qGray(ptr_magDown2[x]))
                        {
                            ptr_supDown[x + 1] =  qRgb(255, 0, 0);
                            flag = true;
                        }

                        /* Left-Top */
                        if (qGray(ptr_magUp[x - 1]) >= tmin &&
                                qGray(ptr_supUp[x - 1]) != 64 &&
                                qGray(ptr_dirUp[x - 1]) > 22.5 &&
                                qGray(ptr_dirUp[x - 1]) <= 67.5 &&
                                qGray(ptr_magUp[x - 1]) > qGray(ptr_magnitude[x - 2]) &&
                                qGray(ptr_magUp[x - 1]) > qGray(ptr_magUp2[x]))
                        {
                            ptr_supUp[x - 1] =  qRgb(255, 0, 0);
                            flag = true;
                        }
                    }
                    else {

                        /* Bottom */
                        if (qGray(ptr_magDown[x]) >= tmin &&
                                qGray(ptr_supDown[x]) != 64 &&
                                qGray(ptr_dirDown[x]) < 22.5 &&
                                qGray(ptr_dirDown[x]) >= 157.5 &&
                                qGray(ptr_magDown[x]) > qGray(ptr_magDown[x + 1]) &&
                                qGray(ptr_magDown[x]) > qGray(ptr_magDown[x - 1]))
                        {
                            ptr_supDown[x] =  qRgb(255, 0, 0);
                            flag = true;
                        }

                        /* Top */
                        if (qGray(ptr_magUp[x]) >= tmin &&
                                qGray(ptr_supUp[x]) != 64 &&
                                qGray(ptr_dirUp[x]) < 22.5 &&
                                qGray(ptr_dirUp[x]) >= 157.5 &&
                                qGray(ptr_magUp[x]) > qGray(ptr_magUp[x + 1]) &&
                                qGray(ptr_magUp[x]) > qGray(ptr_magUp[x - 1]))
                        {
                            ptr_supUp[x] =  qRgb(255, 0, 0);
                            flag = true;
                        }
                    }
                }
            }
        }
    }

    QRgb *ptr_thresholding = (QRgb*)output_suppression.bits();
    QRgb *ptr_canny = (QRgb*)output_canny.bits();
    for (int i = 0; i < image.width() * image.height(); ++i) {
        if (qRed(ptr_thresholding[i]) == 64)
            ptr_canny[i] = qRgb(255, 255, 255);
    }

//    QLabel *windowA = new QLabel;
//    windowA->setPixmap(QPixmap::fromImage(output_horizontal));
//    windowA->show();

//    QLabel *windowB = new QLabel;
//    windowB->setPixmap(QPixmap::fromImage(output_vertical));
//    windowB->show();

    //sendImage(output_suppression);
    sendImage(output_suppression);
}

void CannyFilter::overloadImage(QImage value)
{
    image = value;
}



































