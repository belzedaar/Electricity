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
    double feedIn = 0.0f;
    for (uint32_t i = 0; i < 365; ++i)
    {
        const UsageData::Stats& dayStats = data.Getday(i);
        if (dayStats.mFeedIn > mFirstKWH)
        {
            feedIn += mFirstKWH * mFirstRateDollars;
            feedIn += (dayStats.mFeedIn - mFirstKWH) * mRestRateDollars;
        }
        else
        {
            feedIn += dayStats.mFeedIn * mFirstRateDollars;
        }
    }

    // we return calculaed feed in values as negative
    return -feedIn;
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
