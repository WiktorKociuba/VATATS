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
            if(QFile::exists("VATSpy.dat"))
                QFile::remove("VATSpy.dat");
            QJsonArray assets = obj["assets"].toArray();
            QString downloadUrl, vatspyUrl;
            for(const QJsonValue& asset : assets){
                QJsonObject assetObj = asset.toObject();
                QString name = assetObj["name"].toString();
                if(name == "FIRBoundaries.dat"){
                    downloadUrl = assetObj["browser_download_url"].toString();
                }
                else if(name == "VATSpy.dat"){
                    vatspyUrl = assetObj["browser_download_url"].toString();
                }
            }
            QString firUrl = downloadUrl;
            QVector<QPair<QString,QString>> downloads;
            if(!firUrl.isEmpty())
                downloads.push_back({"FIRBoundaries.dat", firUrl});
            if(!vatspyUrl.isEmpty())
                downloads.push_back({"VATSpy.dat", vatspyUrl});
            QNetworkAccessManager* filemanager = new QNetworkAccessManager();
            int* remaining = new int(downloads.size());
            QObject::connect(filemanager, &QNetworkAccessManager::finished, [this,filemanager,remaining](QNetworkReply* fr){
                QString fname = fr->property("targetName").toString();
                if(fname.isEmpty())
                    fname = fr->url().fileName();
                if(fr->error() == QNetworkReply::NoError){
                    QByteArray data = fr->readAll();
                    if(data.isEmpty()){
                        qWarning() << "Empty file:" << fname;
                    }
                    else{
                        QFile out(fname);
                        if(out.open(QIODevice::WriteOnly)){
                            out.write(data);
                            out.close();
                            qInfo() << "saved" << fname << data.size() << "bytes";
                            if(fname == "FIRBoundaries.dat"){
                                this->decodeBoundaries(fname);
                            }
                        }
                        else{
                            qWarning() << "Cannot write:" << fname;
                        }
                    }
                }
                else{
                    qWarning() << "Download error" << fname << fr->errorString();
                }
                fr->deleteLater();
                (*remaining)--;
                if(*remaining == 0){
                    delete remaining;
                    filemanager->deleteLater();
                    emit finished();
                }
            });
        for(const auto& d : downloads){
            QNetworkRequest fReq(QUrl(d.second));
            QNetworkReply* r = filemanager->get(fReq);
            r->setProperty("targetName", d.first);
        }
    }
        reply->deleteLater();
        manager->deleteLater();
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