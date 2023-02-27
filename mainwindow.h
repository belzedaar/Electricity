#pragma once

#include <QMainWindow>

#include <QScopedPointer>
#include <QChart>

namespace Ui { class MainWindow; }

class UsageData;


class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    MainWindow(QWidget *parent = nullptr);
    ~MainWindow();
private slots:
    void OpenFile();

private:
    void LoadData(const QString& path);

    Ui::MainWindow *mUI;

    QScopedPointer<UsageData> mUsageData;

    QChart mChart;
};

