#pragma once
#include <QObject>
#include <QTcpServer>
#include <QString>

class chartfox : public QObject{
    Q_OBJECT
public:
    QString generateCodeVerifier(int length);
    QString generateCodeChallenge(const QString& code);
    void authorizeChartfox();
    void startOAuthServer();
    void exchangeForToken(const QString& code);
    void getChartsForAirport(const QString& icao);
    struct chartData{
        QString name;
        QString id;
        QString type;
        QString source;
    };
    void getChartSource(const QString& id);
    void getCharts(const QString& icao);
    void refreshToken();
    void retrieveToken();
private:
    QTcpServer* oauthServer = nullptr;
signals:
    void sourceUrlFound(QString sourceUrlFound);
    void chartsFound(QVector<chartfox::chartData> charts);
    void updateCharts();
    void chartfoxAuthorized();
};
