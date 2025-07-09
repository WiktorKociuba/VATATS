#pragma once
#include <QObject>

class vatsimMap : public QObject{
    Q_OBJECT
public:
    void getVatsimData();
    struct flightplanData{
        QString flightRules;
        QString aircraft;
        QString aircraftShort;
        QString departure;
        QString arrival;
        QString alternate;
        QString deptime;
        QString enrouteTime;
        QString remarks;
        QString route;
        QString assignedTransponder;
    };
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
        QString logonTime;
        vatsimMap::flightplanData Flightplan;
    };
    struct ATCData{
        QString cid;
        QString name;
        QString callsign;
        QString freq;
        QString facility;
        QString rating;
        QString visualRange;
        QString textAtis;
        QString logonTime;
    };
    struct atisData{
        QString cid;
        QString name;
        QString callsign;
        QString freq;
        QString facility;
        QString rating;
        QString visualRange;
        QString atisCode;
        QString textAtis;
        QString logonTime;
    };
    struct prefileData{
        QString cid;
        QString name;
        QString callsign;
        vatsimMap::flightplanData flightplan;
    };
    struct extraData{
        QString id;
        QString shortName;
        QString longName;
    };
    void requestData();
signals:
    void newVatsimData();
};