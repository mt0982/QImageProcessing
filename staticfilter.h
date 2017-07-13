#ifndef STATICFILTER_H
#define STATICFILTER_H

#include <QWidget>
#include <QVector3D>
#include <QVector>

class Color {
private:
    QVector<int> red, green, blue;
    int last, mid;

public:
    Color() {}

    void add(int r, int g, int b)
    {
        red.push_back(r);
        green.push_back(g);
        blue.push_back(b);
    }

    void msort()
    {
        qSort(red.begin(), red.end());
        qSort(green.begin(), green.end());
        qSort(blue.begin(), blue.end());
        last = red.size() - 1;
        mid = red.size() / 2;
    }

    void mclear()
    {
        red.clear();
        blue.clear();
        green.clear();
    }

    QVector3D mmin() { return QVector3D(red[0], green[0], blue[0]); }
    QVector3D mmax() { return QVector3D(red[last], green[last], blue[last]); }
    QVector3D mmid() { return QVector3D(red[mid], green[mid], blue[mid]); }
};

namespace Ui {
class StaticFilter;
}

class StaticFilter : public QWidget
{
    Q_OBJECT

public:
    explicit StaticFilter(QWidget *parent = 0);
    ~StaticFilter();

    void setImage(const QImage &value);
    void filter(int nr);

signals:
    void sendImage(QImage);

private slots:
    void on_pbMinimum_clicked();
    void on_pbMaximum_clicked();

private:
    Ui::StaticFilter *ui;

    QImage image;
};

#endif // STATICFILTER_H
