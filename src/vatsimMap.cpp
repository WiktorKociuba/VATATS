#include "vatsimMap.h"
#include <QApplication>
#include <QObject>
#include <QUrl>
#include <QNetworkAccessManager>
#include <QNetworkReply>
#include <QNetworkRequest>
#include <QJsonArray>
#include <QByteArray>
#include <QJsonObject>
#include <QJsonDocument>
#include <QSqlDatabase>
#include <QSqlError>
#include <QtConcurrent/QtConcurrentRun>
#include "globals.h"
#include "tracking.h"
#include "savePoints.h"

auto joinTextAtis = [](const QJsonValue& v)->QString{
    if(v.isArray()){
        QStringList lines;
        for(const QJsonValue& l : v.toArray()){
            QString s = l.toString().trimmed();
            if(!s.isEmpty()) lines << s;
        }
        return lines.join("\n");
    }
    if(v.isString()) return v.toString();
    return QString();
};

void vatsimMap::getVatsimData(){
    QUrl url("https://data.vatsim.net/v3/vatsim-data.json");
    QNetworkRequest request(url);
    request.setRawHeader("Accept", "application/json");
    QNetworkAccessManager* manager = new QNetworkAccessManager(this);
    QNetworkReply* reply = manager->get(request);
    connect(reply, &QNetworkReply::finished, this, [reply, this](){
        g_currentPilotData = {};
        g_currentATCData = {};
        g_currentAtisData = {};
        g_currentPrefileData = {};
        QByteArray response = reply->readAll();
        QJsonDocument doc = QJsonDocument::fromJson(response);
        QJsonObject obj = doc.object();
        QJsonArray pilots = obj["pilots"].toArray();
        QJsonArray controllers = obj["controllers"].toArray();
        QJsonArray atis = obj["atis"].toArray();
        QJsonArray prefiles = obj["prefiles"].toArray();
        
        for(const QJsonValue &value : pilots){
            QJsonObject pilot = value.toObject();
            QString CID = QString::number(pilot["cid"].toInt());
            QString name = pilot["name"].toString();
            QString callsign = pilot["callsign"].toString();
            QString pilotRating = QString::number(pilot["pilot_rating"].toInt());
            QString militaryRating = QString::number(pilot["military_rating"].toInt());
            double latitude = pilot["latitude"].toDouble();
            double longitude = pilot["longitude"].toDouble();
            QString altitude = QString::number(pilot["altitude"].toInt());
            QString groundSpeed = QString::number(pilot["groundspeed"].toInt());
            QString transponder = pilot["transponder"].toString();
            QString heading = QString::number(pilot["heading"].toInt());
            QString qnhmb = QString::number(pilot["qnh_mb"].toInt());
            QString logonTime = pilot["logon_time"].toString();
            QJsonObject flightplanD = pilot["flight_plan"].toObject();
            QString flightRules = flightplanD["flight_rules"].toString();
            QString aircraft = flightplanD["aircraft"].toString();
            QString aircraftShort = flightplanD["aircraft_short"].toString();
            QString departure = flightplanD["departure"].toString();
            QString arrival = flightplanD["arrival"].toString();
            QString alternate = flightplanD["alternate"].toString();
            QString deptime = flightplanD["deptime"].toString();
            QString enrouteTime = flightplanD["enroute_time"].toString();
            QString remarks = flightplanD["remarks"].toString();
            QString route = flightplanD["route"].toString();
            QString assignedTransponder = flightplanD["assigned_transponder"].toString();
            vatsimMap::flightplanData tempFlightplan = {flightRules, aircraft, aircraftShort, departure, arrival, alternate, deptime, enrouteTime, remarks, route, assignedTransponder};
            g_currentPilotData.push_back({CID,name,callsign,pilotRating,militaryRating,latitude,longitude,altitude,groundSpeed,transponder,heading,qnhmb,logonTime, tempFlightplan});
        }
        (void)QtConcurrent::run(savePoints::saveSPPos, g_currentPilotData);
        for(const QJsonValue &value : controllers){
            QJsonObject ATC = value.toObject();
            QString cid = QString::number(ATC["cid"].toInt());
            QString name = ATC["name"].toString();
            QString callsign = ATC["callsign"].toString();
            QString freq = ATC["frequency"].toString();
            QString facility = ATC["facility"].toString();
            QString rating = ATC["rating"].toString();
            QString visualRange = ATC["visual_range"].toString();
            QString textAtis = ATC["text_atis"].toString();
            QString logonTime = ATC["logon_time"].toString();
            g_currentATCData.push_back({cid,name,callsign,freq,facility,rating,visualRange,textAtis,logonTime});
        }
        for(const QJsonValue &value : atis){
            QJsonObject ATIS = value.toObject();
            QString cid = ATIS["cid"].toString();
            QString name = ATIS["name"].toString();
            QString callsign = ATIS["callsign"].toString();
            QString freq = ATIS["frequency"].toString();
            QString facility = ATIS["facility"].toString();
            QString rating = ATIS["rating"].toString();
            QString visualRange = ATIS["visual_range"].toString();
            QString atisCode = ATIS["atis_code"].toString();
            QString textAtis = joinTextAtis(ATIS["text_atis"]);
            QString logonTime = ATIS["logon_time"].toString();
            g_currentAtisData.push_back({cid,name,callsign,freq,facility,rating,visualRange,atisCode,textAtis,logonTime});
        }
        for(const QJsonValue &value : prefiles){
            QJsonObject prefile = value.toObject();
            QString cid = prefile["cid"].toString();
            QString name = prefile["name"].toString();
            QString callsign = prefile["callsign"].toString();
            QString flightRules = prefile["flight_rules"].toString();
            QString aircraft = prefile["aircraft"].toString();
            QString aircraftShort = prefile["aircraft_short"].toString();
            QString departure = prefile["departure"].toString();
            QString arrival = prefile["arrival"].toString();
            QString alternate = prefile["alternate"].toString();
            QString deptime = prefile["deptime"].toString();
            QString enrouteTime = prefile["enroute_time"].toString();
            QString remarks = prefile["remarks"].toString();
            QString route = prefile["route"].toString();
            QString assignedTransponder = prefile["assigned_transponder"].toString();
            vatsimMap::flightplanData tempFlightplan = {flightRules,aircraft,aircraftShort,departure,arrival,alternate,deptime,enrouteTime,remarks,route,assignedTransponder};
            g_currentPrefileData.push_back({cid,name,callsign,tempFlightplan});
        }
        if(!ifVatsim){
            ifVatsim = true;
            QJsonArray facilities = obj["facilities"].toArray();
            QJsonArray ratings = obj["rating"].toArray();
            QJsonArray pilotRatings = obj["pilot_ratings"].toArray();
            QJsonArray militaryRatings = obj["military_ratings"].toArray();
            for(const QJsonValue &value : facilities){
                QJsonObject facility = value.toObject();
                QString id = facility["id"].toString();
                QString shortName = facility["short_name"].toString();
                QString longName = facility["long_name"].toString();
                g_facilities.push_back({id,shortName,longName});
            }
            for(const QJsonValue &value : ratings){
                QJsonObject rating = value.toObject();
                QString id = QString::number(rating["id"].toInt());
                QString shortName = rating["short_name"].toString();
                QString longName = rating["long_name"].toString();
                g_ratings.push_back({id,shortName,longName});
            }
            for(const QJsonValue &value : pilotRatings){
                QJsonObject prating = value.toObject();
                QString id = QString::number(prating["id"].toInt());
                QString shortName = prating["short_name"].toString();
                QString longName = prating["long_name"].toString();
                g_pilotRatings.push_back({id,shortName,longName});
            }
            for(const QJsonValue &value : militaryRatings){
                QJsonObject mrating = value.toObject();
                QString id = mrating["id"].toString();
                QString shortName = mrating["short_name"].toString();
                QString longName = mrating["long_name"].toString();
                g_militaryRatings.push_back({id,shortName,longName});
            }
        }
        emit newVatsimData();
        reply->deleteLater();
    });
}


void vatsimMap::requestData(){
    connect(this, &vatsimMap::newVatsimData, this, [](){
        g_mainWindow->updateVatsimMap();
    });
    this->getVatsimData();
}
