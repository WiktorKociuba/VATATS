#pragma once
#include <QObject>
#include <QSqlDatabase>
#include "vatsimMap.h"

class savePoints : public QObject {
    Q_OBJECT
public:
    static void sendPointsToSQL(const QString& dbFile, double latitude, double longitude, double altitude);
    static void createNewSave();
    static void saveTakeoffTime();
    static void saveLandingTime();
    static void removeLastPoint(const QString& dbFile);
    static void savePointsTime();
    static void saveVatsimCid();
    static void saveHoppieSecret();
    static void saveSettings(int id, QString data);
    static void saveSPPos(QVector<vatsimMap::pilotData> l_currentPilotData);
};