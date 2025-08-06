#include "chartfox.h"
#include <QApplication>
#include <QByteArray>
#include <QCryptographicHash>
#include <QRandomGenerator>
#include <QString>
#include <QDesktopServices>
#include <QUrl>
#include <QTcpServer>
#include <QTcpSocket>
#include <QRegularExpression>
#include <QUrlQuery>
#include <QNetworkAccessManager>
#include <QNetworkRequest>
#include <QNetworkReply>
#include <QJsonDocument>
#include <QJsonObject>
#include <QJsonArray>
#include "globals.h"
#include "savePoints.h"
#include "readPoints.h"
#include <QDateTime>
#include "tracking.h"

QString codeVerifier;

void chartfox::retrieveToken(){
    QString expireDatestr = readPoints::getSettingsData(5);
    if(expireDatestr.isEmpty())
        return;
    qint64 expireDate = expireDatestr.toLongLong();
    tracking* trackingChartfox = new tracking(g_mainWindow);
    if(expireDate < QDateTime::currentSecsSinceEpoch()){
        g_chartfoxToken = readPoints::getSettingsData(3);
        trackingChartfox->disableChartfoxAuthorize();
    }
    else{
        this->refreshToken();
    }
}

QString chartfox::generateCodeVerifier(int length){
    const char charset[] = "ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz0123456789-._~";
    QString code;
    for(int i = 0; i < length; i++){
        int rchar = QRandomGenerator::global()->bounded(sizeof(charset)-1);
        code += charset[rchar];
    }
    codeVerifier = code;
    return code;
}

QString chartfox::generateCodeChallenge(const QString& code){
    QByteArray hash = QCryptographicHash::hash(code.toUtf8(), QCryptographicHash::Sha256);
    QByteArray base64 = hash.toBase64(QByteArray::Base64UrlEncoding | QByteArray::OmitTrailingEquals);
    return QString::fromUtf8(base64);
}

void chartfox::authorizeChartfox(){
    QString codeChallenge = this->generateCodeChallenge(this->generateCodeVerifier(64));
    QString url = QString("https://api.chartfox.org/oauth/authorize?response_type=code&client_id=9f72eedf-dde5-44fd-992f-fccd8bc27fa4&redirect_uri=http://localhost:5555/callback&scope=airports:view+charts:index+charts:view+charts:view_source_url+charts:geos+charts:files&code_challenge=%1&code_challenge_method=S256").arg(codeChallenge);
    QDesktopServices::openUrl(QUrl(url));
    this->startOAuthServer();
}

void chartfox::startOAuthServer(){
    oauthServer = new QTcpServer(this);
    connect(oauthServer, &QTcpServer::newConnection, this, [this](){
        QTcpSocket* socket = oauthServer->nextPendingConnection();
        if(!socket)
            return;
        if(!socket->waitForReadyRead(30000)){
            socket->close();
            socket->deleteLater();
            return;
        }
        QByteArray request = socket->readAll();

        QRegularExpression re("GET /callback\\?code=([^& ]+)");
        QRegularExpressionMatch match = re.match(QString::fromUtf8(request));
        QString code;
        if(match.hasMatch()){
            code = match.captured(1);
        }
        QByteArray response = "HTTP/1.1 200 OK\r\nContent-Type: text/html\r\n\r\n"
        "<html><body>Authorization complete. You may close this window</body>";
        socket->write(response);
        socket->waitForBytesWritten(1000);
        socket->close();
        socket->deleteLater();
        if(!code.isEmpty()){
            exchangeForToken(code);
        }
        oauthServer->close();
    });
    if(!oauthServer->listen(QHostAddress::LocalHost, 5555)){
        qWarning() << "Failed to start OAuth server:" << oauthServer->errorString();
    }
}

void chartfox::exchangeForToken(const QString& code){
    QUrl tokenUrl("https://api.chartfox.org/oauth/token");
    QNetworkRequest request(tokenUrl);
    request.setHeader(QNetworkRequest::ContentTypeHeader, "application/x-www-form-urlencoded");
    QUrlQuery params;
    params.addQueryItem("grant_type", "authorization_code");
    params.addQueryItem("client_id", "9f72eedf-dde5-44fd-992f-fccd8bc27fa4");
    params.addQueryItem("code", code);
    params.addQueryItem("redirect_uri", "http://localhost:5555/callback");
    params.addQueryItem("code_verifier", codeVerifier);
    QNetworkAccessManager* manager = new QNetworkAccessManager(this);
    QNetworkReply* reply = manager->post(request, params.toString(QUrl::FullyEncoded).toUtf8());
    connect(reply, &QNetworkReply::finished, this, [reply, this](){
        if(reply->error() == QNetworkReply::NoError){
            QByteArray response = reply->readAll();
            QJsonDocument doc = QJsonDocument::fromJson(response);
            QJsonObject obj = doc.object();
            QString accessToken = obj["access_token"].toString();
            QString refreshToken = obj["refresh_token"].toString();
            qint64 expireDate = QDateTime::currentSecsSinceEpoch() + obj["expires_in"].toInt();
            savePoints::saveSettings(3, accessToken);
            savePoints::saveSettings(4, refreshToken);
            savePoints::saveSettings(5, QString::number(expireDate));
            g_chartfoxToken = accessToken;
            emit chartfoxAuthorized();
        }
        reply->deleteLater();
    });
}

void chartfox::refreshToken(){
    QUrl url("https://api.chartfox.org/oauth/token");
    QNetworkRequest request(url);
    request.setHeader(QNetworkRequest::ContentTypeHeader, "application/x-www-form-urlencoded");

    QUrlQuery params;
    params.addQueryItem("grant_type", "refresh_token");
    params.addQueryItem("client_id", "9f55210b-28e4-4d0a-9129-5e8d0f8c93d5");
    params.addQueryItem("refresh_token", readPoints::getSettingsData(4));

    QNetworkAccessManager* manager = new QNetworkAccessManager(this);
    QNetworkReply* reply = manager->post(request, params.toString(QUrl::FullyEncoded).toUtf8());

    connect(reply, &QNetworkReply::finished, this, [reply, this](){
        if(reply->error() == QNetworkReply::NoError){
            QByteArray response = reply->readAll();
            QJsonDocument doc = QJsonDocument::fromJson(response);
            QJsonObject obj = doc.object();
            QString newAccessToken = obj["access_token"].toString();
            QString newRefreshToken = obj["refresh_token"].toString();
            qint64 expireDate = QDateTime::currentSecsSinceEpoch() + obj["expires_in"].toInt();
            g_chartfoxToken = newAccessToken;
            savePoints::saveSettings(3, newAccessToken);
            savePoints::saveSettings(4, newRefreshToken);
            savePoints::saveSettings(5, QString::number(expireDate));
        }
    });
}

void chartfox::getChartsForAirport(const QString& icao){
    QUrl url(QString("https://api.chartfox.org/v2/airports/%1/charts").arg(icao));
    QNetworkRequest request(url);
    request.setRawHeader("Authorization", QString("Bearer %1").arg(g_chartfoxToken).toUtf8());
    QNetworkAccessManager* manager = new QNetworkAccessManager(this);
    QNetworkReply* reply = manager->get(request);
    connect(reply, &QNetworkReply::finished, this, [reply, this](){
        if(reply->error() == QNetworkReply::NoError){
            QByteArray response = reply->readAll();
            QJsonDocument doc = QJsonDocument::fromJson(response);
            QJsonObject obj = doc.object();
            QJsonArray data = obj["data"].toArray();
            int totalCharts = data.size();
            auto sharedResult = std::make_shared<QVector<chartfox::chartData>>();
            auto completed = std::make_shared<int>(0);
            for(const QJsonValue& dataVal : data){
                QJsonObject chart = dataVal.toObject();
                QString chartName = chart["name"].toString();
                QString chartId = chart["id"].toString();
                QString chartType = chart["type_key"].toString();
                QUrl url(QString("https://api.chartfox.org/v2/charts/%1").arg(chartId));
                QNetworkRequest chartRequest(url);
                chartRequest.setRawHeader("Authorization", QString("Bearer %1").arg(g_chartfoxToken).toUtf8());
                QNetworkAccessManager* chartManager = new QNetworkAccessManager(this);
                QNetworkReply* chartReply = chartManager->get(chartRequest);
                connect(chartReply, &QNetworkReply::finished, this, [=](){
                    QString source;
                    if(chartReply->error() == QNetworkReply::NoError){
                        QByteArray response = chartReply->readAll();
                        QJsonDocument doc = QJsonDocument::fromJson(response);
                        QJsonObject obj = doc.object();
                        source = obj["source_url"].toString();
                    }
                    sharedResult->push_back({chartName, chartId, chartType, source});
                    chartReply->deleteLater();
                    (*completed)++;
                    if(*completed == totalCharts){
                        emit chartsFound(*sharedResult);
                    }
                });
            }
        }
        else
            emit chartsFound({});
        reply->deleteLater();
    });
}

void chartfox::getCharts(const QString& icao){
    connect(this, &chartfox::chartsFound, this, [this](QVector<chartfox::chartData> charts){
        g_currentCharts.clear();
        g_currentCharts = charts;
        emit updateCharts();
    });
    this->getChartsForAirport(icao);
}
