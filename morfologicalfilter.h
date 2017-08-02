#ifndef MORFOLOGICALFILTER_H
#define MORFOLOGICALFILTER_H

#include <QWidget>
#include <QDebug>
#include <QTableWidget>
#include <QSpinBox>
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
    void on_sbRadius_valueChanged(int arg1);
    void on_tableWidget_cellClicked(int row, int column);

private:
    Ui::MorfologicalFilter *ui;

    int **array;

    void thresholding();
    int ystart, yend, xstart, xend;
};

#endif // MORFOLOGICALFILTER_H






























