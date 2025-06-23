#pragma once
#include <QObject>

class savePoints : public QObject {
    Q_OBJECT
public:
    static void sendPointsToSQL(const QString& dbFile, double latitude, double lonfgitude, double altitude);
};