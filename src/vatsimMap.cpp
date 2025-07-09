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
#include "globals.h"

void vatsimMap::getVatsimData(){
    QUrl url("https://data.vatsim.net/v3/vatsim-data.json");
    QNetworkRequest request(url);
    request.setRawHeader("Accept", "application/json");
    QNetworkAccessManager* manager = new QNetworkAccessManager(this);
    QNetworkReply* reply = manager->get(request);
    connect(reply, &QNetworkReply::finished, this, [reply, this](){
        QByteArray response = reply->readAll();
        QJsonDocument doc = QJsonDocument::fromJson(response);
        QJsonObject obj = doc.object();
        QJsonArray pilots = obj["pilots"].toArray();
        QJsonArray controllers = obj["controllers"].toArray();
        QJsonArray atis = obj["atis"].toArray();
        for(const QJsonValue &value : pilots){
            QJsonObject pilot = value.toObject();
            QString CID = pilot["cid"].toString();
            QString name = pilot["name"].toString();
            QString callsign = pilot["callsign"].toString();
            QString pilotRating = pilot["pilot_rating"].toString();
            QString militaryRating = pilot["military_rating"].toString();
            QString latitude = pilot["latitude"].toString();
            QString longitude = pilot["longitude"].toString();
            QString altitude = pilot["altitude"].toString();
            QString groundSpeed = pilot["groundspeed"].toString();
            QString transponder = pilot["transponder"].toString();
            QString heading = pilot["heading"].toString();
            QString qnhmb = pilot["qnh_mb"].toString();
            g_currentPilotData.push_back({CID,name,callsign,pilotRating,militaryRating,latitude,longitude,altitude,groundSpeed,transponder,heading,qnhmb});
        }
        emit newVatsimData();
        reply->deleteLater();
    });
}

void vatsimMap::requestData(){
    connect(this, &vatsimMap::newVatsimData, this, [](){
        for(auto pilot : g_currentPilotData){
            qDebug() << pilot.callsign;
        }
    });
    this->getVatsimData();
}