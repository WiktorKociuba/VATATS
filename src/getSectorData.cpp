#include "getSectorData.h"
#include <QNetworkAccessManager>
#include <QNetworkRequest>
#include <QNetworkReply>
#include <QJsonDocument>
#include <QJsonObject>
#include <QJsonArray>
#include <QFile>
#include <QDebug>

void getSectorData::getFIRSectors(){
    QNetworkAccessManager* manager = new QNetworkAccessManager();
    QUrl gitVerApi("https://api.github.com/repos/vatsimnetwork/vatspy-data-project/releases/latest");
    QNetworkRequest request(gitVerApi);

    QObject::connect(manager, &QNetworkAccessManager::finished, [manager](QNetworkReply* reply){
        if(reply->url().toString().contains("/releases/latest")){
            QJsonDocument doc = QJsonDocument::fromJson(reply->readAll());
            QJsonObject obj = doc.object();
            QString airacVer = obj["name"].toString();
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
                QObject::connect(manager, &QNetworkAccessManager::finished, [manager](QNetworkReply* dataReply){
                    if(dataReply->url().toString().contains("FIRBoundaries.dat")){
                        QByteArray data = dataReply->readAll();
                        QFile file("FIRBoundaries.dat");
                        if(file.open(QIODevice::WriteOnly)){
                            file.write(data);
                            file.close();
                        }
                        dataReply->deleteLater();
                        manager->deleteLater();
                    }
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