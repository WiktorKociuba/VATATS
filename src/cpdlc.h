#pragma once
#include <QObject>

class cpdlc : public QObject{
    Q_OBJECT
public:
    explicit cpdlc(QObject* parent = nullptr);
    Q_INVOKABLE void getVatsimCallsign(QString id);
    Q_INVOKABLE void testConnection(QString secret, QString callsign);
    Q_INVOKABLE void sendMessage(QString secret, QString callsign, QString station, QString type, QString packet);
    Q_INVOKABLE void pollMessages(QString secret, QString callsign);
    struct hoppieMessage{
        QString station;
        QString type;
        QString messageID;
        QString requestID;
        QString responseType;
        QString packet;
    };
    static void connectToNetwork();
    QTimer* pollTimer;
    void stopPolling();
private:
    void startPolling();
    void receiveMessages();
signals:
    void vatsimCallsignFound(const QString& callsign);
    void testResult(const bool result);
    void messageResult(const bool result);
    void pollResult(const QVector<cpdlc::hoppieMessage> messages);
    void logonAccepted();
};