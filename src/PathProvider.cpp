#include "PathProvider.h"
#include <iostream>
#include <QDebug>
#include <QSqlDatabase>
#include <QSqlError>
#include <QSqlQuery>
#include <QUrl>
#include <QNetworkAccessManager>
#include <QNetworkReply>
#include <QNetworkRequest>
#include <QByteArray>
#include "globals.h"
#include "vatsimMap.h"
#include "readPoints.h"

bool PathProvider::connectionOpen = false;
QSqlDatabase PathProvider::db;

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

void PathProvider::createFIRConnection(){
    db = QSqlDatabase::addDatabase("QSQLITE", "getfir_connection");
    connectionOpen = true;
}

Q_INVOKABLE QVariantList PathProvider::getFirBounds(QString fir, QString cid) const{
    QVariantList bounds;
    if(!connectionOpen){
        PathProvider pp;
        pp.createFIRConnection();
    }
    {
        db.setDatabaseName("FIRData.sqlite");
        if(!db.open()){
            qWarning() << "cannot open db" << db.lastError().text();
        }
        QSqlQuery query(db);
        query.prepare("SELECT 1 FROM sqlite_master WHERE type='table' AND name=:tableName");
        query.bindValue(":tableName", fir);
        if((query.exec() && query.next())){
            query.prepare(QString("SELECT data FROM \"%1\" WHERE id = :id LIMIT 1").arg(fir+"_META"));
            query.bindValue(":id", 7);
            QVariantMap center;
            if(query.exec() && query.next()){
                center["lat"] = query.value(0).toDouble();
            }
            query.prepare(QString("SELECT data FROM \"%1\" WHERE id = :id LIMIT 1").arg(fir+"_META"));
            query.bindValue(":id", 8);
            if(query.exec() && query.next()){
                center["lon"] = query.value(0).toDouble();
            }
            bounds << center;
            query.prepare(QString("SELECT lat, lon FROM \"%1\"").arg(fir));
            query.exec();
            qDebug() << "here";
            while(query.next()){
                QVariantMap point;
                point["lat"] = query.value(0).toDouble();
                point["lon"] = query.value(1).toDouble();
                bounds << point;
            }
        }
        /*
        Index	Field	Example Value	Description
        0	FIR ID	ADR	The FIR identifier (e.g., "ADR")
        1	Min Altitude	0	Minimum altitude (usually FL, feet, or 0)
        2	Unknown	0	(Unused/unknown, often 0)
        3	Max Altitude	250	Maximum altitude (usually FL or feet)
        4	Min Lat	39.651856	Minimum latitude of bounding box
        5	Min Lon	13.0	Minimum longitude of bounding box
        6	Max Lat	46.874747	Maximum latitude of bounding box
        7	Max Lon	23.026053	Maximum longitude of bounding box
        8	Center Lat	42.9	Center latitude of FIR
        9	Center Lon	16.3	Center longitude of FIR
        */
        else{
            QUrl slurpapi(QString("https://slurper.vatsim.net/users/info?cid=%1").arg(cid));
            QNetworkRequest request(slurpapi);
            request.setRawHeader("Accept", "text/plain");
            QNetworkAccessManager* manager = new QNetworkAccessManager();
            QNetworkReply* reply = manager->get(request);
            connect(reply, &QNetworkReply::finished, this, [reply, this](){
                QByteArray raw = reply->readAll();
                QString info = QString::fromUtf8(raw);
                QStringList parts = info.split(',');
                QString lat = parts.value(5);
                QString lon = parts.value(6);
                //QVector<QString> FIRNames = readPoints::getFIRMetaList();
            });
        }
        db.close();
    }
    return bounds;
}