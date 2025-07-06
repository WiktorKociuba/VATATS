#include "cpdlc.h"
#include <QNetworkAccessManager>
#include <QNetworkReply>
#include <QJsonDocument>
#include <QJsonObject>
#include <QJsonArray>
#include <QRegularExpression>
#include <QTimer>
#include <QRandomGenerator>
#include "globals.h"
#include "tracking.h"

cpdlc::cpdlc(QObject* parent) : QObject(parent) {}
static bool ifConnectedHoppie = false;

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
    connect(reply, &QNetworkReply::finished, this, [this, reply, type](){
        bool result = false;
        if(reply->error() == QNetworkReply::NoError){
            QByteArray response = reply->readAll();
            QString responseStr = QString::fromUtf8(response).trimmed();
            if(responseStr.contains("ok")){
                result = true;
                if(type == "inforeq"){
                    QRegularExpression re(R"(\{([^\}]*)\})");
                    auto it = re.globalMatch(responseStr);
                    if(it.hasNext()){
                        auto match = it.next();
                        QString temp = match.captured(1).trimmed();
                        temp+="}";
                        QRegularExpression innerRe(R"((\S+)\s+(\w+)\s+\{([^\}]*)\})");
                        auto iM = innerRe.match(temp);
                        if(iM.hasMatch()){
                            g_messages.push_back({iM.captured(1).trimmed(),iM.captured(2).trimmed(),QString(""),QString(""),QString(""),iM.captured(3).trimmed()});
                            static_cast<tracking*>(g_mainWindow)->updateMessageList();
                        }
                    }
                }
            }
        }
        emit messageResult(result);
        if(!ifConnectedHoppie){
            this->startPolling();
            ifConnectedHoppie = true;
        }
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
                QRegularExpression re(R"(\{([^\}]*)\})");
                auto it = re.globalMatch(responseStr);
                while(it.hasNext()){
                    auto match = it.next();
                    hoppieMessage temp = {};
                    QString msg = match.captured(1).trimmed();
                    msg += "}";
                    QRegularExpression innerRe(R"((\S+)\s+(\w+)\s+\{([^\}]*)\})");
                    auto iM = innerRe.match(msg);
                    if(iM.hasMatch()){
                        temp.station = iM.captured(1).trimmed();
                        temp.type = iM.captured(2).trimmed();
                        QString packet = iM.captured(3).trimmed();
                        if(temp.type == "cpdlc"){
                            QRegularExpression cpdlcRe(R"(\/data2\/([^\/]*)\/([^\/]*)\/([^\/]*)\/(.*))");
                            auto cpdlcCap = cpdlcRe.match(packet);
                            temp.messageID = cpdlcCap.captured(1).trimmed();
                            temp.requestID = cpdlcCap.captured(2).trimmed();
                            temp.responseType = cpdlcCap.captured(3).trimmed();
                            temp.packet = cpdlcCap.captured(4).trimmed();
                            if(temp.packet == "LOGON ACCEPTED"){
                                emit logonAccepted();
                            }
                            else if(temp.packet.contains("HANDOVER")){
                                g_currentStation = temp.packet.right(4);
                                g_mainWindow->updateCurrentStation();
                            }
                            else if(temp.packet.contains("LOGOFF")){
                                g_currentStation = "";
                                g_mainWindow->logoffATC();
                            }
                        }
                        else if(temp.type == "telex"){
                            temp.packet = packet;
                        }
                        result.push_front(temp);
                    }
                }
            }
        }
        emit pollResult(result);
        reply->deleteLater();
    });
}

void cpdlc::connectToNetwork(){
    cpdlc* myCpdlc = new cpdlc(g_mainWindow);
    QObject::connect(myCpdlc, &cpdlc::vatsimCallsignFound, [myCpdlc](const QString& callsign){
        g_callsign = callsign;
        static_cast<tracking*>(g_mainWindow)->updateCallsignLabel(g_callsign);
        myCpdlc->testConnection(g_hoppieSecret, g_callsign);
    });
    QObject::connect(myCpdlc, &cpdlc::testResult, [myCpdlc](const bool result){
        static_cast<tracking*>(g_mainWindow)->connectedHoppie();
        ifConnectedHoppie = false;
    });
    myCpdlc->getVatsimCallsign(g_vatsimCID);
}

void cpdlc::startPolling(){
    pollTimer = new QTimer(this);
    pollTimer->setInterval(QRandomGenerator::global()->bounded(45,76)*1000);
    connect(pollTimer, &QTimer::timeout, this, [this](){
        this->pollMessages(g_hoppieSecret, g_callsign);
        pollTimer->setInterval(QRandomGenerator::global()->bounded(45,76)*1000);
    });
    connect(this, &cpdlc::pollResult, this, [this](const QVector<cpdlc::hoppieMessage>& messages){
        for(const auto& msg : messages){
            g_messages.push_front(msg);
        }
        static_cast<tracking*>(g_mainWindow)->updateMessageList();
    });
    pollTimer->start();
}

void cpdlc::stopPolling(){
    if(pollTimer)
        pollTimer->stop();
}