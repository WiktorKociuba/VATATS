#include "readPoints.h"
#include <QObject>
#include <QSqlDatabase>
#include <QSqlQuery>
#include <QSqlError>
#include <QFile>
#include <tuple>
#include "bridgeToMSFS.h"
#include "globals.h"

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

QVector<QString> readPoints::getLastSaveName(){
    QVector<QString> lastName;
    {
        QSqlDatabase db = QSqlDatabase::addDatabase("QSQLITE", "getlastsave_connection");
        db.setDatabaseName("saves.sqlite");
        if(!db.open()){
            qWarning() << "Cannot open database: " << db.lastError().text();
            return lastName;
        }
        QSqlQuery query(db);
        if(query.exec("SELECT saveName FROM saves ORDER BY id DESC")){
            while(query.next()){
                lastName.push_back(query.value(0).toString());
            }
        }
        else{
            qWarning() << "Select failed: " << query.lastError().text();
        }
        db.close();
        QSqlDatabase::removeDatabase("getlastsave_connection");
        return lastName;
    }
}

int readPoints::getFlightTime(){
    QVector<int> times;
    {
        QSqlDatabase db = QSqlDatabase::addDatabase("QSQLITE", "flighttime_connection");
        db.setDatabaseName(bridgeToMSFS::saveName);
        if(!db.open()){
            qWarning() << "Cannot open the database: " << db.lastError().text();
        }
        QSqlQuery query(db);
        if(query.exec("SELECT time FROM startTimes ORDER BY id")){
            while(query.next()){
                times.push_back(query.value(0).toInt());
            }
        }
        else{
            qWarning() << "Select failed: " << query.lastError().text();
        }
        db.close();
    }
    QSqlDatabase::removeDatabase("flighttime_connection");
    if(times.size() > 1){
        return times[1]-times[0];
    }
    else{
        return -1;
    }
}

QVector<int> readPoints::getPointsTime(){
    QVector<int> res;
    {
        QSqlDatabase db = QSqlDatabase::addDatabase("QSQLITE", "getPointsTime_connection");
        db.setDatabaseName(bridgeToMSFS::saveName);
        if(!db.open()){
            qWarning() << "Cannot open database: " << db.lastError().text();
        }
        QSqlQuery query;
        if(query.exec("SELECT time FROM times ORDER BY id")){
            while(query.next()){
                res.push_back(query.value(0).toInt());
            }
        }
        else{
            qWarning() << "Query failed: " << query.lastError().text();
        }
        db.close();
    }
    QSqlDatabase::removeDatabase("getPointsTime_connection");
    return res;
}

void readPoints::getHoppieVatsim(){
    if(!QFile::exists("settings.sqlite")){
        return;
    }
    {
        QSqlDatabase db = QSqlDatabase::addDatabase("QSQLITE", "hoppievatsim_connection");
        db.setDatabaseName("settings.sqlite");
        if(!db.open()){
            qWarning() << "Cannot open database" << db.lastError().text();
        }
        QSqlQuery query(db);
        if(query.exec("SELECT * FROM settings WHERE id = 1")){
            if(query.next()){
                g_vatsimCID = query.value("value").toString();
            }
        }
        else{
            qWarning() << "Select failed" << query.lastError().text();
        }
        if(query.exec("SELECT * FROM settings WHERE id = 2")){
            if(query.next()){
                g_hoppieSecret = query.value("value").toString();
            }
        }
        else{
            qWarning() << "Select failed" << query.lastError().text();
        }
        db.close();
    }
    QSqlDatabase::removeDatabase("hoppievatsim_connection");
}

QString readPoints::getSettingsData(int id){
    if(!QFile::exists("settings.sqlite")){
        return "";
    }
    QString result;
    {
        QSqlDatabase db = QSqlDatabase::addDatabase("QSQLITE", "settings_connection");
        db.setDatabaseName("settings.sqlite");
        if(!db.open()){
            qWarning() << "Cannot open database" << db.lastError().text();
        }
        QSqlQuery query(db);
        query.prepare("SELECT * FROM settings WHERE id = :id");
        query.bindValue(":id", id);
        if(query.exec()){
            if(query.next()){
                result = query.value("value").toString();
            }
        }
        else{
            qWarning() << "Select failed" << query.lastError().text();
        }
        db.close();
    }
    QSqlDatabase::removeDatabase("settings_connection");
    return result;
}