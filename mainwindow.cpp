#include "mainwindow.h"
#include "ui_mainwindow.h"

#include "usagedata.h"

#include <QFileDialog>
#include <QBarSeries>
#include <QBarSet>
#include <QBarCategoryAxis>
#include <QStackedBarSeries>
#include <QValueAxis>

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , mUI(new Ui::MainWindow)
{
    mUI->setupUi(this);

    connect(mUI->action_Open, &QAction::triggered, this, &MainWindow::OpenFile);

    LoadData("C:/Users/user/Documents/build-Electricity-Desktop_Qt_6_4_2_MinGW_64_bit-Debug/2273487111_01Feb2022_02Feb2023_27Feb2023_ACTEWAGL_SUMMARY.csv");

    mUI->graphicsView->setChart(&mChart);


}

MainWindow::~MainWindow()
{
    delete mUI;
}

void MainWindow::OpenFile()
{
    const QString openName = QFileDialog::getOpenFileName(this, "Open Data file", QString(), "CSV Files (*.csv)");
    if (openName.isEmpty())
    {
        return;
    }

    LoadData(openName);

}

void MainWindow::LoadData(const QString& path)
{
    mUsageData.reset(new UsageData(path));

    mChart.removeAllSeries();

    QStackedBarSeries* usageSeries = new QStackedBarSeries();

    mChart.addSeries(usageSeries);

    QBarSet* feedIn = new QBarSet("Feed In");
    QBarSet* peak = new QBarSet("Peak");
    QBarSet* shoulder = new QBarSet("Shoulder");
    QBarSet* offPeak = new QBarSet("Off Peak");

    feedIn->setColor(QColor(255, 245, 0)); // Bright Yellow

    double maxUsage = 0.0f;
    double maxFeedIn = 0.0f;
    for (uint32_t m = 0; m < 12; ++m)
    {
        const UsageData::Stats& monthStat = mUsageData->GetMonth(m);
        *feedIn << -monthStat.mFeedIn;
        *peak << monthStat.mPeak;
        *shoulder << monthStat.mShoulder;
        *offPeak << monthStat.mOffPeak;

        maxUsage = qMax(maxUsage, monthStat.mPeak + monthStat.mShoulder +  monthStat.mOffPeak);
        maxFeedIn = qMax(maxFeedIn, monthStat.mFeedIn);
    }

    usageSeries->append(feedIn);
    usageSeries->append(peak);
    usageSeries->append(shoulder);
    usageSeries->append(offPeak);

    for (QBarSet* barSet : usageSeries->barSets())
    {
        barSet->setSelectedColor(barSet->brush().color().darker());
    }

    mChart.setAnimationOptions(QChart::SeriesAnimations);

    // categories are months
    QStringList categories;
    categories << "Jan" << "Feb" << "Mar" << "Apr" << "May" << "Jun" << "Jul" << "Aug" << "Sep" << "Oct" << "Nov" << "Dec";

    QBarCategoryAxis* axisX = new QBarCategoryAxis();
    axisX->append(categories);
    mChart.addAxis(axisX, Qt::AlignBottom);

    usageSeries->attachAxis(axisX);

    QValueAxis *axisY = new QValueAxis();
    axisY->setRange(-maxFeedIn, maxUsage);
    axisY->applyNiceNumbers();

    mChart.addAxis(axisY, Qt::AlignLeft);
    usageSeries->attachAxis(axisY);

    mChart.legend()->setVisible(true);
    mChart.legend()->setAlignment(Qt::AlignBottom);

    mUI->txtPeakUsage->setText(QString("%1").arg(mUsageData->GetTotal().mPeak));
    mUI->txtShoulderUsage->setText(QString("%1").arg(mUsageData->GetTotal().mShoulder));
    mUI->txtOffPeakUsage->setText(QString("%1").arg(mUsageData->GetTotal().mOffPeak));
    mUI->txtSolarFeedIn->setText(QString("%1").arg(mUsageData->GetTotal().mFeedIn));

    /*
    connect(usageSeries, &QAbstractBarSeries::clicked, this, [](int index, QBarSet *set)
    {
        //set->toggleSelection({index});
        //set->sees
    });
    */
}

