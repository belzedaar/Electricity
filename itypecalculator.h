#pragma once

#include "usagedata.h"

#include <QObject>

class QWidget;

class ITypeCalculator : public QObject
{
    Q_OBJECT
public:
    ITypeCalculator();
    virtual ~ITypeCalculator();

    virtual double GetTotal(const UsageData& data) const = 0;
    virtual QWidget* CreateWidgets() = 0;
    virtual QString GetTitle() const = 0;
signals:
    void CaculationsChanged();
};


