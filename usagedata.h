#pragma once

#include <stdint.h>

class QString;
class QTime;

class UsageData
{
public:
    explicit UsageData(const QString& path);

    class Stats
    {
    public:
        double mPeak = 0.0f;
        double mShoulder = 0.0f;
        double mOffPeak = 0.0f;
        double mFeedIn = 0.0f;
    };

    const Stats& GetTotal() const
    {
        return mTotals;
    }

    const Stats& GetMonth(uint32_t monthIndex) const
    {
        return mPerMonth[monthIndex];
    }

    const Stats& Getday(uint32_t dayIndex) const
    {
        return mPerDay[dayIndex];
    }

private:
    void ParseLine(const QString& line);
    double* GetSection(Stats& stats, const QTime& time) const;

    Stats mTotals;
    Stats mPerDay[366];
    Stats mPerMonth[12];

};


