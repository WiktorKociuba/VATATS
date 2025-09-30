#pragma once
#include <QObject>

class getSectorData : public QObject{
    Q_OBJECT
public:
    struct boundaryStruct{
        double lat;
        double lon;
    };
    struct FIRData{
        QString designator;
        double minAlt;
        double maxAlt;
        double minLat;
        double minLon;
        double maxLat;
        double maxLon;
        double centerLat;
        double centerLon;
        QVector<boundaryStruct> boundary;
    };
    void getFIRSectors();
    void decodeBoundaries(QString filename);
signals:
    void finished();
};