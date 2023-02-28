#include "FirstRestFeedInCalculator.h"

#include <QWidget>
#include <QFormLayout>
#include <QLineEdit>
#include <QLabel>
#include <QCheckBox>

FirstRestFeedInCalculator::FirstRestFeedInCalculator() = default;

FirstRestFeedInCalculator::~FirstRestFeedInCalculator() = default;

double FirstRestFeedInCalculator::GetTotal(const UsageData& data) const
{
    double feedInCredit = 0.0f;
    if (mAverageOverQuarters)
    {
        double feedInPerQuater[4] = { 0 };

        for (uint32_t m = 0; m < 11; ++m)
        {
            const UsageData::Stats& monthStats = data.GetMonth(m);
            const uint32_t quaterIndex = m / 3; // 3 months per quater
            feedInPerQuater[quaterIndex] += monthStats.mFeedIn;
        }

        for (double feedOneQuater : feedInPerQuater)
        {
            double averagePerDay = feedOneQuater / 91.25;

            double creditPerDay = 0.0f;
            if (averagePerDay> mFirstKWH)
            {
                creditPerDay += mFirstKWH * mFirstRateDollars;
                creditPerDay += (averagePerDay - mFirstKWH) * mRestRateDollars;
            }
            else
            {
                creditPerDay += averagePerDay * mFirstRateDollars;
            }

            feedInCredit += creditPerDay * 91.25;
        }
    }
    else
    {
        for (uint32_t i = 0; i < 365; ++i)
        {
            const UsageData::Stats& dayStats = data.Getday(i);
            if (dayStats.mFeedIn > mFirstKWH)
            {
                feedInCredit += mFirstKWH * mFirstRateDollars;
                feedInCredit += (dayStats.mFeedIn - mFirstKWH) * mRestRateDollars;
            }
            else
            {
                feedInCredit += dayStats.mFeedIn * mFirstRateDollars;
            }
        }
    }
    // we return calculaed feed in values as negative
    return -feedInCredit;
}

QWidget* FirstRestFeedInCalculator::CreateWidgets()
{
    QWidget* widget = new QWidget();

    QFormLayout* layout = new QFormLayout(widget);
    layout->setHorizontalSpacing(15);

    {
        QLineEdit* feedInFirstRate = new QLineEdit();
        layout->addRow("Feed In First Rate:", feedInFirstRate);

        connect(feedInFirstRate, &QLineEdit::editingFinished, this, [this, feedInFirstRate]()
        {
            mFirstRateDollars =  feedInFirstRate->text().toDouble();
            emit CaculationsChanged();
        });
    }

    {
        QLineEdit* feedInFirstKWH = new QLineEdit();
        layout->addRow("Feed In First KwH:", feedInFirstKWH);

        connect(feedInFirstKWH, &QLineEdit::editingFinished, this, [this, feedInFirstKWH]()
        {
            mFirstKWH =  feedInFirstKWH->text().toDouble();
            emit CaculationsChanged();
        });
    }

    {
        QLineEdit* feedInRestRate = new QLineEdit();
        layout->addRow("Feed Rest Rate:", feedInRestRate);

        connect(feedInRestRate, &QLineEdit::editingFinished, this, [this, feedInRestRate]()
        {
            mRestRateDollars =  feedInRestRate->text().toDouble();
            emit CaculationsChanged();
        });
    }

    {
        QCheckBox* averageOverQuaters = new QCheckBox();
        layout->addRow("Average Over Quarters", averageOverQuaters);

        connect(averageOverQuaters, &QCheckBox::toggled, this, [this, averageOverQuaters]()
        {
            mAverageOverQuarters =  averageOverQuaters->isChecked();
            emit CaculationsChanged();
        });
    }

    return widget;
}

QString FirstRestFeedInCalculator::GetTitle() const
{
    return "First/Rest Feed In";
}
