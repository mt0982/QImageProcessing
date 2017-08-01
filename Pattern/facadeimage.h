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
    virtual ~FacadeImage();

    virtual void setImage(const QImage &value);

signals:
    void sendImage(QImage);
};

#endif // FACADEIMAGE_H
