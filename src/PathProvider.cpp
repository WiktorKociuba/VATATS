#include "PathProvider.h"
#include <iostream>
#include "globals.h"
#include "vatsimMap.h"

QVariantList PathProvider::getPath() const {
    QVariantList path;
    for (const auto& pt : points)
        path << QVariantList{pt.first, pt.second};
    return path;
}
QVariantList PathProvider::getPointTimes() const {
    QVariantList times;
    for(int t = 0; t < pointTimes.size(); t++){
        times << pointTimes[t];
    }
    return times;
}
QVariantList PathProvider::getAltitude() const {
    QVariantList altitude;
    for(double a : bridgeToMSFS::altitudeData)
        altitude << a;
    return altitude;
}
QVariantList PathProvider::getVatsimPlanes() const {
    QVariantList planes;
    for(const auto& pilot : g_currentPilotData){
        QVariantMap plane;
        plane["lat"] = pilot.latitude;
        plane["lon"] = pilot.longitude;
        plane["callsign"] = pilot.callsign;
        planes.append(plane);
    }
    return planes;
}
void PathProvider::setPoints(const QVector<QPair<double,double>>& pts){
    points = pts;
    emit pathChanged();
}