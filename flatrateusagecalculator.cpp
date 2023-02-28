#include "flatrateusagecalculator.h"

#include <QWidget>
#include <QFormLayout>
#include <QLineEdit>
#include <QLabel>

FlatRateUsageCalculator::FlatRateUsageCalculator() = default;

FlatRateUsageCalculator::~FlatRateUsageCalculator() = default;

double FlatRateUsageCalculator::GetTotal(const UsageData& data) const
{

    const double totalUsage = data.GetTotal().mPeak +
            data.GetTotal().mShoulder +
            data.GetTotal().mOffPeak;

    return mFixedRateDollars * totalUsage;
}

QWidget* FlatRateUsageCalculator::CreateWidgets()
{
    QWidget* widget = new QWidget();

    QFormLayout* layout = new QFormLayout(widget);
    layout->setHorizontalSpacing(15);

    QLineEdit* lineEdit = new QLineEdit();
    layout->addRow("Cost:", lineEdit);

    connect(lineEdit, &QLineEdit::editingFinished, this, [this, lineEdit]()
    {
        mFixedRateDollars =  lineEdit->text().toDouble();
        emit CaculationsChanged();
    });

    return widget;
}

QString FlatRateUsageCalculator::GetTitle() const
{
    return "Fixed Rate";
}

QString FlatRateUsageCalculator::GetDescription() const
{
    return QString("$%1 per kwh").arg(mFixedRateDollars);
}
