#include "facadeimage.h"

FacadeImage::FacadeImage(QWidget *parent) : QWidget(parent)
{

}

void FacadeImage::setImage(const QImage &value)
{
    image = value;
}
