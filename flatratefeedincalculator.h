#pragma once

#include "itypecalculator.h"

class FlatRateFeedInCalculator : public ITypeCalculator
{
    Q_OBJECT
public:
    FlatRateFeedInCalculator();
    virtual ~FlatRateFeedInCalculator() override;

    virtual double GetTotal(const UsageData& data) const override;
    virtual QWidget* CreateWidgets() override;
    virtual QString GetTitle() const override;

private:
    double mFlatRateDollars = 0.0f;
};


