#ifndef MORFOLOGICALFILTER_H
#define MORFOLOGICALFILTER_H

#include <QWidget>
#include <QDebug>
#include <Pattern/facadeimage.h>

namespace Ui {
class MorfologicalFilter;
}

class MorfologicalFilter : public FacadeImage {
    Q_OBJECT

public:
    explicit MorfologicalFilter(FacadeImage *parent = 0);
    ~MorfologicalFilter();

private slots:
    void on_pbContourExtraction_clicked();

private:
    Ui::MorfologicalFilter *ui;

    void thresholding();
};

#endif // MORFOLOGICALFILTER_H






























