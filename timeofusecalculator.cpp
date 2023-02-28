#include "TimeOfUseCalculator.h"

#include <QWidget>
#include <QFormLayout>
#include <QLineEdit>
#include <QLabel>

TimeOfUseCalculator::TimeOfUseCalculator() = default;

TimeOfUseCalculator::~TimeOfUseCalculator() = default;

double TimeOfUseCalculator::GetTotal(const UsageData& data) const
{
    double cost = 0.0f;
    cost += mPeakCost * data.GetTotal().mPeak;
    cost += mShoulderCost * data.GetTotal().mShoulder;
    cost += mOffPeakCost * data.GetTotal().mOffPeak;

    return cost;
}

QWidget* TimeOfUseCalculator::CreateWidgets()
{
    QWidget* widget = new QWidget();

    QFormLayout* layout = new QFormLayout(widget);
    layout->setHorizontalSpacing(15);

    {
        QLineEdit* peak = new QLineEdit();
        layout->addRow("Peak:", peak);
        connect(peak, &QLineEdit::editingFinished, this, [this, peak]()
        {
            mPeakCost =  peak->text().toDouble();
            emit CaculationsChanged();
        });
    }
    {
        QLineEdit* shoulder = new QLineEdit();
        layout->addRow("Shoulder:", shoulder);
        connect(shoulder, &QLineEdit::editingFinished, this, [this, shoulder]()
        {
            mShoulderCost =  shoulder->text().toDouble();
            emit CaculationsChanged();
        });
    }
    {
        QLineEdit* offPeak = new QLineEdit();
        layout->addRow("Off Peak:", offPeak);
        connect(offPeak, &QLineEdit::editingFinished, this, [this, offPeak]()
        {
            mOffPeakCost =  offPeak->text().toDouble();
            emit CaculationsChanged();
        });
    }

    return widget;
}

QString TimeOfUseCalculator::GetTitle() const
{
    return "Time of Use";
}

QString TimeOfUseCalculator::GetDescription() const
{
  return QString("Peak $%1 Shoulder $%2 Off Peak $%3 ").arg(mPeakCost).arg(mShoulderCost).arg(mOffPeakCost);
}
