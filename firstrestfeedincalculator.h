#pragma once

#include "itypecalculator.h"

class FirstRestFeedInCalculator : public ITypeCalculator
{
    Q_OBJECT
public:
    FirstRestFeedInCalculator();
    virtual ~FirstRestFeedInCalculator() override;

    virtual double GetTotal(const UsageData& data) const override;
    virtual QWidget* CreateWidgets() override;
    virtual QString GetTitle() const override;

private:
    double mFirstKWH = 0.0f;
    double mFirstRateDollars = 0.0f;
    double mRestRateDollars = 0.0f;
    bool mAverageOverQuarters = false;
};


