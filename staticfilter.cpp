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

void StaticFilter::filter(int nr)
{
    /* Parameters */
    int radius = ui->sbRadius->value();
    QImage output(image.width(), image.height(), QImage::Format_RGB32);
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
                ptr_copy[x] = qRgb(color.mmid().x(), color.mmid().y(), color.mmid().z());
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























