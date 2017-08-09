#ifndef MORFOLOGICALFILTER_H
#define MORFOLOGICALFILTER_H

#include <QWidget>
#include <QTableWidget>
#include <QSpinBox>
#include <Pattern/facadeimage.h>

struct StructuralElement {
    QVector<int> data;

    void operator = (std::initializer_list<int> values) {
        data = values;
    }
};

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
    void on_pbSkeletonization_clicked();

private:
    Ui::MorfologicalFilter *ui;

    int **array;
    int ystart, yend, xstart, xend;
    QImage iBinary;

    void binarization();
    void erosion();
    void boundaryExtraction(const QImage &imageErosion);
    QImage thinning(QImage &input);
    void skeletonization();
};

#endif // MORFOLOGICALFILTER_H






























