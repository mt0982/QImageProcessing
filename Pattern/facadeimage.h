#ifndef FACADEIMAGE_H
#define FACADEIMAGE_H

#include <QWidget>

class FacadeImage : public QWidget {
    Q_OBJECT

protected:
    QImage image;
    QImage output;

public:
    explicit FacadeImage(QWidget *parent = nullptr);

    void setImage(const QImage &value);

signals:
    void sendImage(QImage);

public slots:
};

#endif // FACADEIMAGE_H
