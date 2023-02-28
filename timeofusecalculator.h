#pragma once

#include "itypecalculator.h"

class TimeOfUseCalculator : public ITypeCalculator
{
    Q_OBJECT
public:
    TimeOfUseCalculator();
    virtual ~TimeOfUseCalculator() override;

    virtual double GetTotal(const UsageData& data) const override;
    virtual QWidget* CreateWidgets() override;
    virtual QString GetTitle() const override;
    virtual QString GetDescription() const override;

private:
    double mPeakCost = 0.0f;
    double mShoulderCost = 0.0f;
    double mOffPeakCost = 0.0f;
};


