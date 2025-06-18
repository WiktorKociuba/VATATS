#pragma once
#include <QObject>
#include <tuple>

class readPoints : public QObject{
    Q_OBJECT
public:
    QVector<std::tuple<double, double, double>> readPointsDB(QString dbName);
};