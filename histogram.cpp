#include "histogram.h"
#include "ui_histogram.h"

Histogram::Histogram(QWidget *parent) : QWidget(parent), ui(new Ui::Histogram)
{
    ui->setupUi(this);
}

Histogram::~Histogram()
{
    delete ui;
}
