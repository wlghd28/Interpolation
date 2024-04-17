#ifndef CINTERPOLATION_H
#define CINTERPOLATION_H

#include <QMainWindow>
#include <QDesktopWidget>

#include "qcustomplot.h"

QT_BEGIN_NAMESPACE
namespace Ui { class CInterpolation; }
QT_END_NAMESPACE

class CInterpolation : public QMainWindow
{
    Q_OBJECT

public:
    CInterpolation(QWidget *parent = nullptr);
    ~CInterpolation();

private:
    Ui::CInterpolation *ui;


public:
    double m_Epsilon;

    QVector<double> m_GraphKey_Graph0_cubic_spline;
    QVector<double> m_GraphData_Graph0_cubic_spline;
    QVector<double> m_GraphKey_Graph0_monotonic_cubic_Hermite_spline;
    QVector<double> m_GraphData_Graph0_monotonic_cubic_Hermite_spline;

public:
    void Init();
    void InitMember();
    void InitGraph();
    void UpdateGraph();

    bool cubic_spline(std::vector<double>* x_series, std::vector<double>* y_series, std::vector<double> *destX, std::vector<double>* destY);
    bool monotonic_cubic_Hermite_spline(const std::vector<double> x_src, const std::vector<double> y_src, std::vector<double>* destX, std::vector<double>* destY);
    double h00(double t);
    double h10(double t);
    double h01(double t);
    double h11(double t);
};
#endif // CINTERPOLATION_H
