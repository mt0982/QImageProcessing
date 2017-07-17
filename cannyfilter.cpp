#include "cannyfilter.h"

CannyFilter::CannyFilter(QWidget *parent) : QWidget(parent)
{
    gaussUnsharpFilter = new GaussUnsharpFilter;
    connect(gaussUnsharpFilter, SIGNAL(sendImage(QImage)), this, SLOT(overloadImage(QImage)));
}

void CannyFilter::setImage(QImage value)
{
    image = value;
    gaussUnsharpFilter->setImage(value);
}

void CannyFilter::processImage()
{
    output = QImage(image.width(), image.height(), QImage::Format_RGB32);

    /* Preprocessing - Gauss */
    gaussUnsharpFilter->gaussianFilterFastCanny(2, 3);

    sendImage(image);
}

void CannyFilter::overloadImage(QImage value)
{
    image = value;
}
