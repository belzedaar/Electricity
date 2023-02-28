#pragma once

#include "itypecalculator.h"

class FlatRateUsageCalculator : public ITypeCalculator
{    
    Q_OBJECT
public:
    FlatRateUsageCalculator();
    virtual ~FlatRateUsageCalculator() override;

    virtual double GetTotal(const UsageData& data) const override;
    virtual QWidget* CreateWidgets() override;
    virtual QString GetTitle() const override;
    virtual QString GetDescription() const override;

private:
    double mFixedRateDollars = 0.0f;
};


