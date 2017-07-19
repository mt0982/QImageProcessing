#ifndef CANNYFILTER_H
#define CANNYFILTER_H

#include <QWidget>
#include <QImage>
#include <QDebug>
#include <QLabel>
#include <QtMath>
#include <gausskernel.h>

class CannyFilter : public QWidget {
    Q_OBJECT

public:
    explicit CannyFilter(QWidget *parent = nullptr);

    void setImage(QImage value);
    void processImage();

private:
    QImage image;
    QImage output;
    GaussUnsharpFilter *gaussUnsharpFilter;
    QVector<int> xSobel, ySobel;

public slots:
    void overloadImage(QImage value);

signals:
    void sendImage(QImage);
};

#endif // CANNYFILTER_H


















