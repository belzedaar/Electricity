#include "usagedata.h"

#include <QDate>
#include <QTime>
#include <QFile>

UsageData::UsageData(const QString& path)
{
    QFile file(path);
    if (file.open(QFile::ReadOnly | QFile::Text) == false)
    {
        return;
    }

    bool atData = false;
    while (!file.atEnd())
    {
        const QByteArray line = file.readLine();
        QString str(line);
        if (str.endsWith('\n'))
        {
            str.chop(1);
        }
        if (str.isEmpty())
        {
           continue;
        }
        if (str.contains("NMI"))
        {
            atData = true;
            continue;
        }
        if (atData == false)
        {
            continue;
        }

        ParseLine(str);
    }

    for (const Stats& stats : mPerMonth)
    {
        mTotals.mPeak += stats.mPeak;
        mTotals.mShoulder += stats.mShoulder;
        mTotals.mOffPeak += stats.mOffPeak;
        mTotals.mFeedIn += stats.mFeedIn;
    }
}

void UsageData::ParseLine(const QString& line)
{
    const QStringList parts = line.split(',');
    if (parts.size() != 11)
    {
        return;
    }

    const QString& dateString = parts[6];
    const QDate date = QDate::fromString(dateString, "dd/MM/yyyy");

    // date returns month in the 1-12 range
    const int32_t monthIndex = date.month() - 1;
    const int32_t dayOfYearIndex = date.dayOfYear() - 1;

    Stats& monthStats = mPerMonth[monthIndex];
    Stats& dayStats = mPerDay[dayOfYearIndex];

    const QString& timeString = parts[7];
    const QTime time = QTime::fromString(timeString, "HH:mm");

    const QString& type = parts[4];
    const QString& kwh = parts[9];
    const double value = kwh.toDouble();
    if (type == "Solar")
    {
        monthStats.mFeedIn += value;
        dayStats.mFeedIn += value;
    }
    else
    {
        // This is inefficient working out the peak/off peak twice
        double* monthTarget = GetSection(monthStats, time);
        *monthTarget += value;

        double* dayTarget = GetSection(dayStats, time);
        *dayTarget += value;
    }
}

double* UsageData::GetSection(Stats& stats, const QTime& time) const
{
    static const QTime kMorningPeakStart = QTime::fromString("07:00", "HH:mm");
    static const QTime kMorningPeakEnd = QTime::fromString("09:00", "HH:mm");
    if ((time >= kMorningPeakStart) && (time < kMorningPeakEnd))
    {
        return &stats.mPeak;
    }

    static const QTime kEveningPeakStart = QTime::fromString("17:00", "HH:mm");
    static const QTime kEveningPeakEnd = QTime::fromString("20:00", "HH:mm");
    if ((time >= kEveningPeakStart) && (time < kEveningPeakEnd))
    {
        return &stats.mPeak;
    }


    static const QTime kDayShoulderStart = QTime::fromString("09:00", "HH:mm");
    static const QTime kDayShoulderEnd = QTime::fromString("17:00", "HH:mm");
    if ((time >= kDayShoulderStart) && (time < kDayShoulderEnd))
    {
        return &stats.mShoulder;
    }

    static const QTime kEveningShoulderStart = QTime::fromString("20:00", "HH:mm");
    static const QTime kEveningShoulderEnd = QTime::fromString("22:00", "HH:mm");
    if ((time >= kEveningShoulderStart) && (time < kEveningShoulderEnd))
    {
        return &stats.mShoulder;
    }

    return &stats.mOffPeak;
}
