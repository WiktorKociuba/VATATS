#pragma once
#include <QObject>

class getSectorData : public QObject{
    Q_OBJECT
public:
    static void getFIRSectors();
};