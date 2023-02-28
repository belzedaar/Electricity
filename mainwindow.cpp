#include "mainwindow.h"
#include "ui_mainwindow.h"

#include "usagedata.h"
#include "flatrateusagecalculator.h"
#include "timeofusecalculator.h"
#include "flatratefeedincalculator.h"
#include "firstrestfeedincalculator.h"

#include <QFileDialog>
#include <QBarSeries>
#include <QBarSet>
#include <QBarCategoryAxis>
#include <QStackedBarSeries>
#include <QValueAxis>
#include <QStandardItem>

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , mUI(new Ui::MainWindow)
{
    mUI->setupUi(this);

    mUI->graphicsView->setChart(&mChart);


    connect(mUI->btnRecordResult, &QPushButton::clicked, this, &MainWindow::RecordResult);

    mResults.setHorizontalHeaderLabels(QStringList() << "Name" << "Rate" << "Feed In" << "Total Cost");

    mUI->tblResults->setModel(&mResults);

    connect(mUI->action_Open, &QAction::triggered, this, &MainWindow::OpenFile);

    QList<QPair<QString, QString>> peaks;
    peaks << qMakePair("07:00", "09:00");
    peaks << qMakePair("17:00", "20:00");
    SetupDataModel(mPeakTimes, peaks);

    QList<QPair<QString, QString>> shoulder;
    shoulder << qMakePair("09:00", "17:00");
    shoulder << qMakePair("20:00", "22:00");
    SetupDataModel(mShoulderTimes, shoulder);

    mUI->tblPeak->setModel(&mPeakTimes);
    mUI->tblShoulder->setModel(&mShoulderTimes);

    connect(&mPeakTimes, &QStandardItemModel::dataChanged, this, &MainWindow::Reload);
    connect(&mShoulderTimes, &QStandardItemModel::dataChanged, this, &MainWindow::Reload);

    mCostCalculators << new FlatRateUsageCalculator();
    mCostCalculators << new TimeOfUseCalculator();

    SetupCalculators(mUI->tabGridCost, mCostCalculators);

    mFeedInCalculators << new FlatRateFeedInCalculator();
    mFeedInCalculators << new FirstRestFeedInCalculator();
    SetupCalculators(mUI->tabFeedIn, mFeedInCalculators);

    connect(mUI->tabGridCost, &QTabWidget::currentChanged, this, &MainWindow::Calculate);
    connect(mUI->tabFeedIn, &QTabWidget::currentChanged, this, &MainWindow::Calculate);

    connect(mUI->txtAccessFee, &QLineEdit::editingFinished, this, &MainWindow::Calculate);
    connect(mUI->txtBonus, &QLineEdit::editingFinished, this, &MainWindow::Calculate);

    LoadData("C:/Users/user/Documents/build-Electricity-Desktop_Qt_6_4_2_MinGW_64_bit-Debug/2273487111_01Feb2022_02Feb2023_27Feb2023_ACTEWAGL_SUMMARY.csv");
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

void MainWindow::Reload()
{
    if (mFileName.isEmpty())
    {
        return;
    }
    LoadData(mFileName);
}

void MainWindow::Calculate()
{
    double cost = 0.0f;

    cost += mUI->txtAccessFee->text().toDouble() * 365;

    for (ITypeCalculator* c : mCostCalculators)
    {
        if (c->GetTitle() == mUI->tabGridCost->tabText(mUI->tabGridCost->currentIndex()))
        {
            cost += c->GetTotal(*mUsageData);
            mCostString = c->GetDescription();
        }
    }

    double credits = 0.0f;

    credits -= mUI->txtBonus->text().toDouble();

    for (ITypeCalculator* c : mFeedInCalculators)
    {
        if (c->GetTitle() == mUI->tabFeedIn->tabText(mUI->tabFeedIn->currentIndex()))
        {
            // this is expected to be negative
            credits += c->GetTotal(*mUsageData);
            mFeedInString = c->GetDescription();
        }
    }

    mUI->txtTotalCost->setText(QString("%1").arg(cost));
    mUI->txtTotalCredits->setText(QString("%1").arg(credits));
    mGrandTotalString = QString("%1").arg(cost + credits);
    mUI->txtGrandTotal->setText(mGrandTotalString);
}

void MainWindow::RecordResult()
{
    QList<QStandardItem*> items;
    items << new QStandardItem(mUI->txtPlanName->text());
    items << new QStandardItem(mCostString);
    items << new QStandardItem(mFeedInString);
    items << new QStandardItem(mGrandTotalString);
    mResults.appendRow(items);
}

void MainWindow::LoadData(const QString& path)
{
    mFileName = path;

    QList<QPair<QTime, QTime>> peakTimes;
    QList<QPair<QTime, QTime>> shoulderTimes;

    GetModelData(mPeakTimes, peakTimes);
    GetModelData(mShoulderTimes, shoulderTimes);

    mUsageData.reset(new UsageData(path, peakTimes, shoulderTimes));

    mChart.removeAllSeries();
    for (QAbstractAxis* axis : mChart.axes())
    {
        mChart.removeAxis(axis);
    }
    QStackedBarSeries* usageSeries = new QStackedBarSeries();       
    mChart.addSeries(usageSeries);

    QBarSet* feedIn = new QBarSet("Feed In");
    QBarSet* peak = new QBarSet("Peak");
    QBarSet* shoulder = new QBarSet("Shoulder");
    QBarSet* offPeak = new QBarSet("Off Peak");

    // colors match the website
    feedIn->setColor(QColor(255, 198, 0));
    shoulder->setColor(QColor(60, 130, 122));
    peak->setColor(QColor(6, 77, 66));
    offPeak->setColor(QColor(191, 215, 216));
    feedIn->setPen(QPen(Qt::PenStyle::NoPen));
    shoulder->setPen(QPen(Qt::PenStyle::NoPen));
    peak->setPen(QPen(Qt::PenStyle::NoPen));
    offPeak->setPen(QPen(Qt::PenStyle::NoPen));

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
    double range = qMax(maxFeedIn, maxUsage);
    //axisY->setRange(-maxFeedIn, maxUsage);
    axisY->setRange(-range, range);

    mChart.addAxis(axisY, Qt::AlignLeft);
    usageSeries->attachAxis(axisY);

    mChart.legend()->setVisible(true);
    mChart.legend()->setAlignment(Qt::AlignBottom);

    const UsageData::Stats& total = mUsageData->GetTotal();

    mUI->txtPeakUsage->setText(QString("%1").arg(total.mPeak));
    mUI->txtShoulderUsage->setText(QString("%1").arg(total.mShoulder));
    mUI->txtOffPeakUsage->setText(QString("%1").arg(total.mOffPeak));
    mUI->txtSolarFeedIn->setText(QString("%1").arg(total.mFeedIn));
    mUI->txtTotal->setText(QString("%1").arg(total.mPeak + total.mShoulder + total.mOffPeak));

    Calculate();
}

void MainWindow::SetupDataModel(QStandardItemModel &model, QList<QPair<QString, QString> > &times)
{
    model.setHorizontalHeaderLabels(QStringList() << "Start" << "End");

    for (const QPair<QString, QString>& startAndEnd : times)
    {
        QList<QStandardItem*> items;
        QStandardItem* start = new QStandardItem();
        start->setData(QTime::fromString(startAndEnd.first, "HH:mm"), Qt::DisplayRole);
        QStandardItem* end = new QStandardItem();
        end->setData(QTime::fromString(startAndEnd.second, "HH:mm"), Qt::DisplayRole);
        items << start;
        items << end;
        model.appendRow(items);
    }
}

void MainWindow::GetModelData(const QStandardItemModel& model, QList<QPair<QTime, QTime>>& outTimes) const
{
    for (int32_t r = 0; r < model.rowCount(); ++r)
    {
        outTimes << qMakePair(model.data(model.index(r, 0)).toTime(),
                              model.data(model.index(r, 1)).toTime());
    }
}

void MainWindow::SetupCalculators(QTabWidget *tabWidget, QList<ITypeCalculator *> &calculators)
{
    for (ITypeCalculator* calculator : calculators)
    {
        QWidget* widget = calculator->CreateWidgets();
        tabWidget->addTab(widget, calculator->GetTitle());
        connect(calculator, &ITypeCalculator::CaculationsChanged, this, &MainWindow::Calculate);
    }
}

