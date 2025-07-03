#pragma once
#include <QObject>
#include <tuple>

class readPoints : public QObject{
    Q_OBJECT
public:
    static QVector<std::tuple<double, double, double>> readPointsDB(QString dbName);
    static QVector<QString> getLastSaveName();
    static int getFlightTime();
    static QVector<int> getPointsTime();
};