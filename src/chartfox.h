#pragma once
#include <QObject>
#include <QTcpServer>

class chartfox : public QObject{
    Q_OBJECT
public:
    QString generateCodeVerifier(int length);
    QString generateCodeChallenge(const QString& code);
    void authorizeChartfox();
    void startOAuthServer();
    void exchangeForToken(const QString& code);
    void getChartsForAirport(const QString& icao);
private:
    QTcpServer* oauthServer = nullptr;
};
