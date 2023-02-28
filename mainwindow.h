#pragma once

#include <QMainWindow>

#include <QScopedPointer>
#include <QChart>
#include <QStandardItemModel>

namespace Ui { class MainWindow; }

class UsageData;
class ITypeCalculator;

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    MainWindow(QWidget *parent = nullptr);
    ~MainWindow();
private slots:
    void OpenFile();
    void Reload();
    void Calculate();
    void RecordResult();

private:
    void LoadData(const QString& path);

    void SetupDataModel(QStandardItemModel& model, QList<QPair<QString, QString>>& times);
    void GetModelData(const QStandardItemModel& model, QList<QPair<QTime, QTime>>& outTimes) const;
    void SetupCalculators(QTabWidget* tabWidget, QList<ITypeCalculator*>& calculators);

    Ui::MainWindow *mUI;

    QScopedPointer<UsageData> mUsageData;

    QChart mChart;

    QStandardItemModel mPeakTimes;
    QStandardItemModel mShoulderTimes;

    QStandardItemModel mResults;

    QString mFileName;

    QList<ITypeCalculator*> mCostCalculators;
    QList<ITypeCalculator*> mFeedInCalculators;

    QString mCostString;
    QString mFeedInString;
    QString mGrandTotalString;
};

