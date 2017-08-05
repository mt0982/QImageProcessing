#ifndef HOUGHTRANSFORM_H
#define HOUGHTRANSFORM_H

#include <QWidget>
#include "Pattern/facadeimage.h"

namespace Ui {
class HoughTransform;
}

class HoughTransform : public FacadeImage {
    Q_OBJECT

public:
    explicit HoughTransform(FacadeImage *parent = 0);
    ~HoughTransform();

private:
    Ui::HoughTransform *ui;
};

#endif // HOUGHTRANSFORM_H
