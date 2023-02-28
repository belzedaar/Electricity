#include "FlatRateFeedInCalculator.h"

#include <QWidget>
#include <QFormLayout>
#include <QLineEdit>
#include <QLabel>

FlatRateFeedInCalculator::FlatRateFeedInCalculator() = default;

FlatRateFeedInCalculator::~FlatRateFeedInCalculator() = default;

double FlatRateFeedInCalculator::GetTotal(const UsageData& data) const
{

    const double totalFeedIn= data.GetTotal().mFeedIn;
    return -mFlatRateDollars * totalFeedIn;
}

QWidget* FlatRateFeedInCalculator::CreateWidgets()
{
    QWidget* widget = new QWidget();

    QFormLayout* layout = new QFormLayout(widget);
    layout->setHorizontalSpacing(15);

    QLineEdit* lineEdit = new QLineEdit();
    layout->addRow("Feed In Rate:", lineEdit);

    connect(lineEdit, &QLineEdit::editingFinished, this, [this, lineEdit]()
    {
        mFlatRateDollars =  lineEdit->text().toDouble();
        emit CaculationsChanged();
    });

    return widget;
}

QString FlatRateFeedInCalculator::GetTitle() const
{
    return "Flat Rate Feed In";
}
