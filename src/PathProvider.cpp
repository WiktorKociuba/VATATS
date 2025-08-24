#include "PathProvider.h"
#include <iostream>
#include <QDebug>
#include <QSqlDatabase>
#include <QSqlError>
#include <QSqlQuery>
#include "globals.h"
#include "vatsimMap.h"

QVariantList PathProvider::getPath() const {
    QVariantList path;
    for (const auto& pt : points)
        path << QVariantList{pt.first, pt.second};
    return path;
}
QVariantList PathProvider::getPointTimes() const {
    QVariantList times;
    for(int t = 0; t < pointTimes.size(); t++){
        times << pointTimes[t];
    }
    return times;
}
QVariantList PathProvider::getAltitude() const {
    QVariantList altitude;
    for(double a : bridgeToMSFS::altitudeData)
        altitude << a;
    return altitude;
}
QVariantList PathProvider::getVatsimPlanes() const {
    QVariantList planes;
    for(const auto& pilot : g_currentPilotData){
        QVariantMap plane;
        plane["lat"] = pilot.latitude;
        plane["lon"] = pilot.longitude;
        plane["callsign"] = pilot.callsign;
        plane["groundspeed"] = pilot.groundSpeed;
        plane["heading"] = pilot.heading;
        plane["altitude"] = pilot.altitude;
        plane["cid"] = pilot.CID;
        plane["name"] = pilot.name;
        for(auto rating : g_pilotRatings){
            if(rating.id == pilot.pilotRating)
                plane["rating"] = rating.shortName;
        }
        plane["mrating"] = pilot.militaryRating;
        plane["trans"] = pilot.transponder;
        plane["dep"] = pilot.Flightplan.departure;
        plane["arr"] = pilot.Flightplan.arrival;
        plane["route"] = pilot.Flightplan.route;
        plane["acft"] = pilot.Flightplan.aircraftShort;
        plane["deptime"] = pilot.Flightplan.deptime;
        plane["remarks"] = pilot.Flightplan.remarks;
        plane["EET"] = pilot.Flightplan.enrouteTime;
        planes.append(plane);
    }
    return planes;
}
void PathProvider::setPoints(const QVector<QPair<double,double>>& pts){
    points = pts;
    emit pathChanged();
}
Q_INVOKABLE QVariantList PathProvider::getCachedLoc(const QString& callsign) const{
    QVariantList cachedLoc;
    QSqlDatabase db = QSqlDatabase::addDatabase("QSQLITE", "cache_connection");
    {
        db.setDatabaseName("tempVatsim.sqlite");
        if(!db.open()){
            qWarning() << "Cannot open database" << db.lastError().text();
        }
        QSqlQuery query(db);
        query.prepare(QString("SELECT lat, lon FROM \"%1\"").arg(callsign));
        if(query.exec()){
            while(query.next()){
                QVariantMap point;
                point["lat"] = query.value(0).toDouble();
                point["lon"] = query.value(1).toDouble();
                cachedLoc << point;
            }
        }
        else{
            qWarning() << "Cannot execute query" << db.lastError().text();
        }
        db.close();
    }
    QSqlDatabase::removeDatabase("cache_connection");
    return cachedLoc;
}

Q_INVOKABLE QVariantList PathProvider::getVatsimControllers() const {
    QVariantList controllers;
    for(const auto& curATC : g_currentATCData){
        QVariantMap ATC;
        ATC["cid"] = curATC.cid;
        ATC["name"] = curATC.name;
        ATC["callsign"] = curATC.callsign;
        ATC["frequency"] = curATC.freq;
        ATC["facility"] = curATC.facility;
        ATC["rating"] = curATC.rating;
        ATC["range"] = curATC.visualRange;
        ATC["textAtis"] = curATC.textAtis;
        ATC["logonTime"] = curATC.logonTime;
        controllers.append(ATC);
    }
    return controllers;
}