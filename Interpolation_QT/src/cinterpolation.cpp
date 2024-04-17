#include "cinterpolation.h"
#include "ui_cinterpolation.h"

#define MIN(AA,BB) ((AA) > (BB)) ? (BB) : (AA)

CInterpolation::CInterpolation(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::CInterpolation)
    , m_Epsilon(1e-20)
{
    ui->setupUi(this);
    Init();
}

CInterpolation::~CInterpolation()
{
    delete ui;
}

/*
    member
*/
void CInterpolation::Init()
{
    InitMember();
    InitGraph();
    UpdateGraph();
}

void CInterpolation::InitMember()
{
    std::vector<double> Table_X;
    std::vector<double> Table_Y;

    std::vector<double> GraphKey_Graph0_cubic_spline;
    std::vector<double> GraphData_Graph0_cubic_spline;
    std::vector<double> GraphKey_Graph0_monotonic_cubic_Hermite_spline;
    std::vector<double> GraphData_Graph0_monotonic_cubic_Hermite_spline;

    double Test[101] =
    {
          0,
         88,101,109,116,122,127,131,135,138,141,
        144,147,149,152,154,156,158,160,162,164,
        166,167,169,170,172,173,175,176,177,179,
        180,181,183,184,185,186,187,188,189,190,
        192,193,194,195,196,197,198,199,200,201,
        201,202,203,204,205,206,207,208,209,210,
        211,212,213,214,215,216,216,217,218,219,
        220,221,222,223,224,225,226,227,228,229,
        230,230,231,232,233,234,234,235,236,237,
        237,238,239,239,240,241,241,242,242,243
    };

    for(int i = 0; i < 101; i++)
    {
        Table_X.push_back(i);
        Table_Y.push_back(Test[i]);
    }

    cubic_spline(&Table_X, &Table_Y, &GraphKey_Graph0_cubic_spline, &GraphData_Graph0_cubic_spline);

    monotonic_cubic_Hermite_spline(Table_X, Table_Y, &GraphKey_Graph0_monotonic_cubic_Hermite_spline, &GraphData_Graph0_monotonic_cubic_Hermite_spline);

    m_GraphKey_Graph0_cubic_spline = QVector<double>(GraphKey_Graph0_cubic_spline.begin(), GraphKey_Graph0_cubic_spline.end());
    m_GraphData_Graph0_cubic_spline = QVector<double>(GraphData_Graph0_cubic_spline.begin(), GraphData_Graph0_cubic_spline.end());
    m_GraphKey_Graph0_monotonic_cubic_Hermite_spline = QVector<double>(GraphKey_Graph0_monotonic_cubic_Hermite_spline.begin(), GraphKey_Graph0_monotonic_cubic_Hermite_spline.end());
    m_GraphData_Graph0_monotonic_cubic_Hermite_spline = QVector<double>(GraphData_Graph0_monotonic_cubic_Hermite_spline.begin(), GraphData_Graph0_monotonic_cubic_Hermite_spline.end());
}

void CInterpolation::InitGraph()
{
    // Graph1
    QCPItemText *textLabel = new QCPItemText(ui->qcustomplotwidget_Graph0);
    textLabel->setPositionAlignment(Qt::AlignTop|Qt::AlignLeft);
    textLabel->position->setType(QCPItemPosition::ptAxisRectRatio);
    textLabel->position->setCoords(0.01, 0.01); // place position at center/top of axis rect
    textLabel->setText("");
    textLabel->setColor(Qt::white);
    textLabel->setFont(QFont(font().family(), 16)); // make font a bit larger
    textLabel->setPen(QPen(Qt::black)); // show black border around text

    //ui->qcustomplotwidget_Graph0->axisRect()->setRangeDrag(Qt::Vertical);
    ui->qcustomplotwidget_Graph0->setInteractions(QCP::iRangeDrag|QCP::iRangeZoom|QCP::iSelectAxes|QCP::iSelectPlottables);
    ui->qcustomplotwidget_Graph0->setBackground(QBrush(Qt::black));
    ui->qcustomplotwidget_Graph0->legend->setTextColor(QColor(255, 255, 255));
    ui->qcustomplotwidget_Graph0->legend->setBrush(QBrush(Qt::black));
    ui->qcustomplotwidget_Graph0->xAxis->setVisible(true);
    ui->qcustomplotwidget_Graph0->yAxis->setVisible(true);
    ui->qcustomplotwidget_Graph0->xAxis->setLabelColor(Qt::white);
    ui->qcustomplotwidget_Graph0->xAxis->setBasePen(QPen(Qt::white));
    ui->qcustomplotwidget_Graph0->xAxis->setTickLabelColor(Qt::white);
    ui->qcustomplotwidget_Graph0->addGraph(ui->qcustomplotwidget_Graph0->xAxis, ui->qcustomplotwidget_Graph0->yAxis);
    ui->qcustomplotwidget_Graph0->addGraph(ui->qcustomplotwidget_Graph0->xAxis, ui->qcustomplotwidget_Graph0->yAxis);
    ui->qcustomplotwidget_Graph0->graph(0)->setScatterStyle((QCPScatterStyle(QCPScatterStyle::ssCircle, 5)));
    ui->qcustomplotwidget_Graph0->graph(0)->setPen(QColor(255, 255, 0));
    ui->qcustomplotwidget_Graph0->graph(0)->setName(QString("cubic_spline"));
    ui->qcustomplotwidget_Graph0->graph(0)->setSelectable(QCP::SelectionType::stNone);
    ui->qcustomplotwidget_Graph0->graph(1)->setScatterStyle((QCPScatterStyle(QCPScatterStyle::ssCircle, 5)));
    ui->qcustomplotwidget_Graph0->graph(1)->setPen(QColor(0, 255, 255));
    ui->qcustomplotwidget_Graph0->graph(1)->setName(QString("monotonic_cubic_Hermite_spline"));
    ui->qcustomplotwidget_Graph0->graph(1)->setSelectable(QCP::SelectionType::stNone);
    ui->qcustomplotwidget_Graph0->yAxis->setLabel("y");
    ui->qcustomplotwidget_Graph0->yAxis->setLabelColor(Qt::white);
    ui->qcustomplotwidget_Graph0->yAxis->setBasePen(QPen(Qt::white));
    ui->qcustomplotwidget_Graph0->yAxis->setTickLabelColor(Qt::white);
    ui->qcustomplotwidget_Graph0->yAxis->setRange(0, 100);
    ui->qcustomplotwidget_Graph0->legend->setVisible(true);
    ui->qcustomplotwidget_Graph0->xAxis->setLabel("x");
    ui->qcustomplotwidget_Graph0->xAxis->setRange(0, 100);
    ui->qcustomplotwidget_Graph0->xAxis->ticker()->setTickCount(5);
    ui->qcustomplotwidget_Graph0->replot();
}

void CInterpolation::UpdateGraph()
{
    ui->qcustomplotwidget_Graph0->graph(0)->setData(m_GraphKey_Graph0_cubic_spline, m_GraphData_Graph0_cubic_spline);
    ui->qcustomplotwidget_Graph0->graph(1)->setData(m_GraphKey_Graph0_monotonic_cubic_Hermite_spline, m_GraphData_Graph0_monotonic_cubic_Hermite_spline);

    ui->qcustomplotwidget_Graph0->xAxis->rescale();
    ui->qcustomplotwidget_Graph0->yAxis->rescale();

    ui->qcustomplotwidget_Graph0->replot();
}

bool CInterpolation::cubic_spline(std::vector<double>* x_series, std::vector<double>* y_series, std::vector<double> *destX, std::vector<double>* destY)
{
    int n = MIN((int)x_series->size()-1, (int)y_series->size()-1);

    // Step 1.
    double *h = new double[n+1];
    double *alpha = new double[n+1];
    int i = 0;

    for(i = 0; i<=n-1; i++){
        h[i] = (*x_series)[i+1] - (*x_series)[i];
    }

    // Step 2.
    for(i = 1; i<=n-1;i++){
        alpha[i]= 3*((*y_series)[i+1]-(*y_series)[i])/h[i]-3*((*y_series)[i]-(*y_series)[i-1])/h[i-1];
    }

    // Step 3.
    double *l = new double[n+1];
    double *u = new double[n+1];
    double *z = new double[n+1];
    double *c = new double[n+1];
    double *b = new double[n+1];
    double *d = new double[n+1];

    l[0] = 1; u[0] = 0; z[0] = 0;

    // Step 4.
    for(i = 1; i<=n-1; i++){
        l[i] = 2*((*x_series)[i+1] - (*x_series)[i-1]) - h[i-1]*u[i-1];
        u[i] = h[i]/l[i];
        z[i] = (alpha[i] - h[i-1]*z[i-1]) / l[i];
    }

    // Step 5.
    l[n] = 1;     z[n] = 0;     c[n] = 0;

    // Step 6.
    for(i = n-1; i>=0; i--){
        c[i] = z[i] - u[i]*c[i+1];
        b[i] = ((*y_series)[i+1] - (*y_series)[i])/h[i] - h[i]*(c[i+1] + 2*c[i])/3;
        d[i] = (c[i+1] - c[i]) / (3*h[i]);
    }

    for(i = 0; i<=n-1;i++){
        double x = (*x_series)[i];
        double inc = ((*x_series)[i+1] - (*x_series)[i])*0.1;
        for(; x < (*x_series)[i+1]; x+=inc){
            double x_offset = x - (*x_series)[i];
            double Sx = (*y_series)[i] + b[i]*x_offset + c[i]*x_offset*x_offset + d[i]*x_offset*x_offset*x_offset;
            if(destX != NULL){
                destX->push_back(x);
            }
            if(destY != NULL){
                destY->push_back(Sx);
            }
        }
    }

    delete [] h;
    delete [] alpha;
    delete [] l;
    delete [] u;
    delete [] z;
    delete [] c;
    delete [] b;
    delete [] d;

    return true;
}

bool CInterpolation::monotonic_cubic_Hermite_spline(const std::vector<double> x_src, const std::vector<double> y_src, std::vector<double>* destX, std::vector<double>* destY)
{
    // 0-based index 사용.
    int n = (int)x_src.size();
    int k = 0;
    double *m = new double[n];

    m[0] = (y_src[1] - y_src[0])/(x_src[1] - x_src[0]);
    m[n-1] = (y_src[n-1] - y_src[n-2])/(x_src[n-1]-x_src[n-2]);

    for(k = 1; k<n-1; k++){
        m[k] = (y_src[k] - y_src[k-1])/(2*(x_src[k] - x_src[k-1])) + (y_src[k+1] - y_src[k])/(2*(x_src[k+1]-x_src[k]));
    }

    for(k = 0; k<n-1; k++){
        double delta_k = (y_src[k+1]-y_src[k])/(x_src[k+1]-x_src[k]);

        if(fabs(delta_k) <= m_Epsilon){
            m[k] = m[k+1] = 0;
        }
        else{
            double ak = m[k]/delta_k;
            double bk = m[k+1]/delta_k;

            if(ak*ak + bk*bk > 9){
                m[k] = 3/(sqrt(ak*ak+bk*bk))*ak*delta_k;
                m[k+1] = 3/(sqrt(ak*ak+bk*bk))*bk*delta_k;
            }
        }
    }

    for(k = 0; k<n-1; k++){
        double cur_x = (double)((int)(0.5 + x_src[k]));
        double next_x = (double)((int)(x_src[k+1]));
        double cur_y = y_src[k];
        double next_y = y_src[k+1];
        double h = next_x - cur_x;
        double x = 0;
        double inc = (next_x - cur_x)*0.1;
        for(x = cur_x; x<next_x; x+=inc){
            double t = (x-cur_x)/h;
            if(destX != NULL){
                destX->push_back(x);
            }
            double y = cur_y*h00(t) + h*m[k]*h10(t) + next_y*h01(t) + h*m[k+1]*h11(t);
            destY->push_back(y);
        }
    }

    delete[] m;

    return true;
}

double CInterpolation::h00(double t)
{
    return 2*t*t*t - 3*t*t +1;
}

double CInterpolation::h10(double t)
{
    return t*(1-t)*(1-t);
}

double CInterpolation::h01(double t)
{
    return t*t*(3-2*t);
}

double CInterpolation::h11(double t)
{
    return t*t*(t-1);
}
