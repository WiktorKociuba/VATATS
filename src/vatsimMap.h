#pragma once
#include <QObject>

class vatsimMap : public QObject{
    Q_OBJECT
public:
    void getVatsimData();
    struct pilotData{
        QString CID;
        QString name;
        QString callsign;
        QString pilotRating;
        QString militaryRating;
        QString latitude;
        QString longitude;
        QString altitude;
        QString groundSpeed;
        QString transponder;
        QString heading;
        QString qnhmb;
    };
    void requestData();
signals:
    void newVatsimData();
};