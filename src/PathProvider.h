#pragma once
#include <QObject>
#include <QVector>
#include <QVariantList>

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
signals:
    void pathChanged();
private:
    QVector<QPair<double,double>> points;
};