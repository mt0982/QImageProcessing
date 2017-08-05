#include "houghtransform.h"
#include "ui_houghtransform.h"

HoughTransform::HoughTransform(FacadeImage *parent) : FacadeImage(parent), ui(new Ui::HoughTransform)
{
    ui->setupUi(this);
}

HoughTransform::~HoughTransform()
{
    delete ui;
}
