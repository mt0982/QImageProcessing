#ifndef MORFOLOGICALFILTER_H
#define MORFOLOGICALFILTER_H

#include <QWidget>
#include <QDebug>

namespace Ui {
class MorfologicalFilter;
}

class MorfologicalFilter : public QWidget {
    Q_OBJECT

public:
    explicit MorfologicalFilter(QWidget *parent = 0);
    ~MorfologicalFilter();

    void setImage(const QImage &value);

private slots:
    void on_pbContourExtraction_clicked();

signals:
    void sendImage(QImage);

private:
    Ui::MorfologicalFilter *ui;

    QImage image;
    QImage output;

    void thresholding();
};

#endif // MORFOLOGICALFILTER_H






























