#include "getSectorData.h"
#include <QNetworkAccessManager>
#include <QNetworkRequest>
#include <QNetworkReply>
#include <QJsonDocument>
#include <QJsonObject>
#include <QJsonArray>
#include <QFile>
#include <QDebug>
#include <QTextStream>
#include <QPair>
#include <QVector>
#include <QSqlDatabase>
#include <QSqlQuery>
#include <QSqlError>
#include "savePoints.h"
#include "readPoints.h"

void getSectorData::getFIRSectors(){
    QNetworkAccessManager* manager = new QNetworkAccessManager();
    QUrl gitVerApi("https://api.github.com/repos/vatsimnetwork/vatspy-data-project/releases/latest");
    QNetworkRequest request(gitVerApi);
    QObject::connect(manager, &QNetworkAccessManager::finished, [manager, this](QNetworkReply* reply){
        if(reply->url().toString().contains("/releases/latest")){
            QJsonDocument doc = QJsonDocument::fromJson(reply->readAll());
            QJsonObject obj = doc.object();
            QString airacVer = obj["name"].toString();
            if(readPoints::getSettingsData(69) == airacVer)
                return;
            savePoints::saveSettings(69, airacVer);
            if(QFile::exists("FIRBoundaries.dat"))
                QFile::remove("FIRBoundaries.dat");
            if(QFile::exists("FIRData.sqlite"))
                QFile::remove("FIRData.sqlite");
            QJsonArray assets = obj["assets"].toArray();
            QString downloadUrl;
            for(const QJsonValue& asset : assets){
                QJsonObject assetObj = asset.toObject();
                if(assetObj["name"].toString() == "FIRBoundaries.dat"){
                    downloadUrl = assetObj["browser_download_url"].toString();
                    break;
                }
            }
            if(!downloadUrl.isEmpty()){
                QUrl dataUrl(downloadUrl);
                QNetworkRequest dataReq(dataUrl);
                QObject::connect(manager, &QNetworkAccessManager::finished, [manager,this](QNetworkReply* dataReply){
                    if(dataReply->url().toString().contains("FIRBoundaries.dat")){
                        QByteArray data = dataReply->readAll();
                        QFile file("FIRBoundaries.dat");
                        if(file.open(QIODevice::WriteOnly)){
                            file.write(data);
                            file.close();
                            this->decodeBoundaries("FIRBoundaries.dat");
                        }
                        dataReply->deleteLater();
                        manager->deleteLater();
                    }
                    emit finished();
                });
                manager->get(dataReq);
            }
            else{
                qWarning() << "FIRBoundaries.dat not found";
                manager->deleteLater();
            }
        }
        reply->deleteLater();
    });
    manager->get(request);
}

void getSectorData::decodeBoundaries(QString filename){
    QFile firfile(filename);
    QVector<FIRData> firs;
    if(!firfile.open(QIODevice::ReadOnly | QIODevice::Text))
        return;
    QTextStream stream(&firfile);
    FIRData currentfir;
    bool ifBound = false;
    while(!stream.atEnd()){
        QString line = stream.readLine().trimmed();
        if(line.isEmpty()) continue;
        QStringList parts = line.split('|');
        if(parts.size() > 2){
            if(ifBound)
                firs.append(currentfir);
            currentfir = FIRData();
            currentfir.designator = parts[0];
            currentfir.minAlt = parts[1].toDouble();
            currentfir.maxAlt = parts[3].toDouble();
            currentfir.minLon = parts[5].toDouble();
            currentfir.minLat = parts[4].toDouble();
            currentfir.maxLat = parts[6].toDouble();
            currentfir.maxLon = parts[7].toDouble();
            currentfir.centerLat = parts[8].toDouble();
            currentfir.centerLon = parts[9].toDouble();
            currentfir.boundary.clear();
            ifBound = true;
        }
        else if(parts.size() == 2 && ifBound){
            currentfir.boundary.append({parts[0].toDouble(),parts[1].toDouble()});
        }
    }
    if(ifBound)
        firs.append(currentfir);
    {
        QSqlDatabase db = QSqlDatabase::addDatabase("QSQLITE", "fir_connection");
        db.setDatabaseName("FIRData.sqlite");
        if(!db.open()){
            qWarning() << "Cannot open database" << db.lastError().text();
        }
        for(auto fir : firs){
            QSqlQuery query(db);
            query.prepare(QString("CREATE TABLE IF NOT EXISTS \"%1\" (id INTEGER PRIMARY KEY AUTOINCREMENT, lat DOUBLE, lon DOUBLE)").arg(fir.designator));
            if(!query.exec()){
                qWarning() << "Cannot create table" << db.lastError().text();
            }
            query.prepare(QString("CREATE TABLE IF NOT EXISTS \"%1\" (id INTEGER PRIMARY KEY AUTOINCREMENT, data DOUBLE)").arg(fir.designator + "_META"));
            if(!query.exec()){
                qWarning() << "Cannot create table" << db.lastError().text();
            }
            QString queryContent = QString("INSERT INTO \"%1\" (data) VALUES (%2)").arg(fir.designator+"_META").arg(fir.minAlt);
            if(!query.exec(queryContent)){
                qWarning() << "Cannot insert" << db.lastError().text();
            }
            queryContent = QString("INSERT INTO \"%1\" (data) VALUES (%2)").arg(fir.designator+"_META").arg(fir.maxAlt);
            if(!query.exec(queryContent)){
                qWarning() << "Cannot insert" << db.lastError().text();
            }
            queryContent = QString("INSERT INTO \"%1\" (data) VALUES (%2)").arg(fir.designator+"_META").arg(fir.minLat);
            if(!query.exec(queryContent)){
                qWarning() << "Cannot insert" << db.lastError().text();
            }
            queryContent = QString("INSERT INTO \"%1\" (data) VALUES (%2)").arg(fir.designator+"_META").arg(fir.minLon);
            if(!query.exec(queryContent)){
                qWarning() << "Cannot insert" << db.lastError().text();
            }
            queryContent = QString("INSERT INTO \"%1\" (data) VALUES (%2)").arg(fir.designator+"_META").arg(fir.maxLat);
            if(!query.exec(queryContent)){
                qWarning() << "Cannot insert" << db.lastError().text();
            }
            queryContent = QString("INSERT INTO \"%1\" (data) VALUES (%2)").arg(fir.designator+"_META").arg(fir.maxLon);
            if(!query.exec(queryContent)){
                qWarning() << "Cannot insert" << db.lastError().text();
            }
            queryContent = QString("INSERT INTO \"%1\" (data) VALUES (%2)").arg(fir.designator+"_META").arg(fir.centerLat);
            if(!query.exec(queryContent)){
                qWarning() << "Cannot insert" << db.lastError().text();
            }
            queryContent = QString("INSERT INTO \"%1\" (data) VALUES (%2)").arg(fir.designator+"_META").arg(fir.centerLon);
            if(!query.exec(queryContent)){
                qWarning() << "Cannot insert" << db.lastError().text();
            }
            for(auto bound : fir.boundary){
                queryContent = QString("INSERT INTO \"%1\" (lat, lon) VALUES (%2, %3)").arg(fir.designator).arg(bound.lat).arg(bound.lon);
                if(!query.exec(queryContent)){
                    qWarning() << "Cannot insert" << db.lastError().text();
                }
            }
        }
        db.close();
    }
    QSqlDatabase::removeDatabase("fir_connection");
}