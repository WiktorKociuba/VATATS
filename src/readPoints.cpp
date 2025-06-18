#include "readPoints.h"
#include <QObject>
#include <QSqlDatabase>
#include <QSqlQuery>
#include <QSqlError>
#include <QFile>
#include <tuple>

QVector<std::tuple<double,double,double>> readPoints::readPointsDB(QString saveName){
    QVector<std::tuple<double,double,double>> points;
    {
        QSqlDatabase db = QSqlDatabase::addDatabase("QSQLITE", "readPoints_connection");
        db.setDatabaseName(saveName);
        if(!db.open()){
            qWarning() << "Cannot open database: " << db.lastError().text();
        }
        QSqlQuery query(db);
        if(!query.exec("SELECT latitude, longitude, altitude FROM points")){
            qWarning() << "Select failed: " << query.lastError().text();
            db.close();
            QSqlDatabase::removeDatabase("readPoints_connection");
            return points;
        }
        while(query.next()){
            double latitude = query.value(0).toDouble();
            double longitude = query.value(1).toDouble();
            double altitude = query.value(2).toDouble();
            points.append(std::make_tuple(latitude,longitude,altitude));
        }
        db.close();
    }
    QSqlDatabase::removeDatabase("readPoints_connection");
    return points;
}