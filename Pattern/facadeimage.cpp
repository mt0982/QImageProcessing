#include "facadeimage.h"

FacadeImage::FacadeImage(QWidget *parent) : QWidget(parent)
{

}

FacadeImage::~FacadeImage()
{

}

void FacadeImage::setImage(const QImage &value)
{
    image = value;
}
