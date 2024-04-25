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

    std::vector<double> m_Table_X;  // X table 값은 반드시 오름차순이어야 한다.
    std::vector<double> m_Table_Y;

    QVector<double> m_GraphKey_Graph0_nearest_neighbor;
    QVector<double> m_GraphData_Graph0_nearest_neighbor;
    QVector<double> m_GraphKey_Graph0_linear;
    QVector<double> m_GraphData_Graph0_linear;
    QVector<double> m_GraphKey_Graph0_cubic_spline;
    QVector<double> m_GraphData_Graph0_cubic_spline;
    QVector<double> m_GraphKey_Graph0_monotonic_cubic_Hermite_spline;
    QVector<double> m_GraphData_Graph0_monotonic_cubic_Hermite_spline;

public:
    void Init();
    void InitMember();
    void InitGraph();
    void UpdateGraph();

    // x_series, x_src 배열의 값은 반드시 오름차순이어야 한다.
    // Nearest neighbor
    bool nearest_neighbor_polynomial_coefficients(std::vector<double>* x_series, std::vector<double>* y_series, std::vector<double>* poly_coeff);
    double cal_nearest_neighbor(double x, std::vector<double>* x_series, std::vector<double>* poly_coeff);

    // Linear
    bool linear_polynomial_coefficients(std::vector<double>* x_series, std::vector<double>* y_series, std::vector<double>* poly_coeff);
    double cal_linear(double x, std::vector<double>* x_series, std::vector<double>* poly_coeff);

    // Cubic spline
    bool cubic_spline(std::vector<double>* x_series, std::vector<double>* y_series, std::vector<double> *destX, std::vector<double>* destY);
    bool monotonic_cubic_Hermite_spline(const std::vector<double> x_src, const std::vector<double> y_src, std::vector<double>* destX, std::vector<double>* destY);
    bool cubic_spline_polynomial_coefficients(std::vector<double>* x_series, std::vector<double>* y_series, std::vector<double>* poly_coeff);
    bool monotonic_cubic_Hermite_spline_polynomial_coefficients(const std::vector<double> x_src, const std::vector<double> y_src, std::vector<double>* poly_coeff);
    double cal_cubic_spline(double x, std::vector<double>* x_series, std::vector<double>* poly_coeff);
    double cal_monotonic_cubic_Hermite_spline(double x, std::vector<double>* x_series, std::vector<double>* poly_coeff);
    double h00(double t);
    double h10(double t);
    double h01(double t);
    double h11(double t);
};
#endif // CINTERPOLATION_H
