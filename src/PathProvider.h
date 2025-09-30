#pragma once
#include <QObject>
#include <QJSValue>
#include <QVector>
#include <QVariantList>
#include <QSqlDatabase>

class PathProvider : public QObject {
    Q_OBJECT
public:
    explicit PathProvider(QObject *parent = nullptr) : QObject(parent) {}
    Q_INVOKABLE QVariantList getPath() const;
    Q_INVOKABLE QVariantList getPointTimes() const;
    Q_INVOKABLE QVariantList getAltitude() const;
    Q_INVOKABLE QVariantList getVatsimPlanes() const;
    Q_INVOKABLE QVariantList getCachedLoc(const QString& callsign) const;
    void setPoints(const QVector<QPair<double,double>>& pts);
    Q_INVOKABLE QVariantList getVatsimControllers() const;
    Q_INVOKABLE QVariantList getFirBounds(QString fir, QString cid) const;
    Q_INVOKABLE QVariantList getVatsimAtis() const;
    Q_INVOKABLE QVariantMap getAirportPos(const QString& icao) const;
    static void createFIRConnection();
signals:
    void pathChanged();
    void atcPosReceived(QString cid, QVariantMap position);
private:
    QVector<QPair<double,double>> points;
    static bool connectionOpen;
    static QSqlDatabase db;
    static QStringList firTables;
    static bool airportCacheLoaded;
    static QHash<QString,QPair<double,double>> airportPosCache;
    static void loadAirportCache();
};
