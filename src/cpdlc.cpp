#include "cpdlc.h"
#include <QNetworkAccessManager>
#include <QNetworkReply>
#include <QJsonDocument>
#include <QJsonObject>
#include <QJsonArray>
#include <QRegularExpression>
#include "globals.h"

cpdlc::cpdlc(QObject* parent) : QObject(parent) {}

void cpdlc::getVatsimCallsign(QString id){
    QNetworkAccessManager* manager = new QNetworkAccessManager(this);
    QString url = QString("https://api.vatsim.net/v2/members/%1/status").arg(id);
    QNetworkReply* reply = manager->get(QNetworkRequest(QUrl(url)));
    connect(reply, &QNetworkReply::finished, this, [this, reply](){
        QString callsign;
        if(reply->error() == QNetworkReply::NoError){
            QByteArray data = reply->readAll();
            QJsonDocument doc = QJsonDocument::fromJson(data);
            QJsonObject obj = doc.object();
            if(obj.contains("callsign")){
                callsign = obj["callsign"].toString();
            }
        }
        emit vatsimCallsignFound(callsign);
        reply->deleteLater();
    });
}

void cpdlc::testConnection(QString secret, QString callsign){
    QNetworkAccessManager* manager = new QNetworkAccessManager(this);
    QString url = QString("http://www.hoppie.nl/acars/system/connect.html?logon=%1&from=%2&to=SERVER&type=ping").arg(secret,callsign);
    QNetworkReply* reply = manager->get(QNetworkRequest(QUrl(url)));
    connect(reply, &QNetworkReply::finished, this, [this, reply](){
        bool result = false;
        if(reply->error() == QNetworkReply::NoError){
            QByteArray response = reply->readAll();
            QString responseStr = QString::fromUtf8(response).trimmed();
            qDebug() << "Ping:" << responseStr;
            if(responseStr.contains("ok")){
                result = true;
            }
        }
        emit testResult(result);
        reply->deleteLater();
    });
}

void cpdlc::sendMessage(QString secret, QString callsign, QString station, QString type, QString packet){
    QNetworkAccessManager* manager = new QNetworkAccessManager(this);
    QString url = QString("http://www.hoppie.nl/acars/system/connect.html?logon=%1&from=%2&to=%3&type=%4&packet=%5&msgno=%6").arg(secret,callsign,station,type,packet,QString::number(messageId));
    QNetworkReply* reply = manager->get(QNetworkRequest(QUrl(url)));
    connect(reply, &QNetworkReply::finished, this, [this, reply](){
        bool result = false;
        if(reply->error() == QNetworkReply::NoError){
            QByteArray response = reply->readAll();
            QString responseStr = QString::fromUtf8(response).trimmed();
            if(responseStr.contains("ok")){
                result = true;
            }
        }
        emit messageResult(result);
        reply->deleteLater();
    });
}

void cpdlc::pollMessages(QString secret, QString callsign){
    QNetworkAccessManager* manager = new QNetworkAccessManager(this);
    QString url = QString("http://www.hoppie.nl/acars/system/connect.html?logon=%1&from=%2&to=SERVER&type=poll").arg(secret,callsign);
    QNetworkReply* reply = manager->get(QNetworkRequest(QUrl(url)));
    connect(reply, &QNetworkReply::finished, this, [this, reply](){
        QVector<cpdlc::hoppieMessage> result = {};
        if(reply->error() == QNetworkReply::NoError){
            QByteArray response = reply->readAll();
            QString responseStr = QString::fromUtf8(response).trimmed();
            if(responseStr.contains("ok")){
                QRegularExpression re(R"(\{([^\}])\})");
                auto it = re.globalMatch(responseStr);
                while(it.hasNext()){
                    auto match = it.next();
                    QString msg = match.captured(1).trimmed();
                    QRegularExpression innerRe(R"(\[[^\]]+\]\s+(\w+)\s+\{\/data2\/([^\/]+)\/([^\/]+)\/([^\/]+)\/([^\/]+)\})");
                    auto iM = innerRe.match(msg);
                    if(iM.hasMatch()){
                        result.push_back({iM.captured(1),iM.captured(2),iM.captured(3),iM.captured(4),iM.captured(5),iM.captured(6)});
                    }
                }
            }
        }
        emit pollResult(result);
        reply->deleteLater();
    });
}

