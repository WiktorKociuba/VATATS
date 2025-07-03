#include "savePoints.h"
#include <QSqlDatabase>
#include <QSqlQuery>
#include <QSqlError>
#include <QFile>
#include <QDateTime>
#include "bridgeToMSFS.h"
#include <ctime>
#include "globals.h"

void savePoints::sendPointsToSQL(const QString& dbFile, double latitude, double longitude, double altitude){
    {
        QSqlDatabase db = QSqlDatabase::addDatabase("QSQLITE", "points_connection");
        db.setDatabaseName(dbFile);
        if(!db.open()){
            qWarning() << "Cannot open database: " << db.lastError().text();
        }
        QSqlQuery query(db);
        if(!query.exec("CREATE TABLE IF NOT EXISTS points (id INTEGER PRIMARY KEY AUTOINCREMENT, latitude REAL, longitude REAL, altitude REAL)")){
            qWarning() << "Create table failed: " << query.lastError().text();
            db.close();
            return;
        }
        query.prepare("INSERT INTO points (latitude, longitude, altitude) VALUES (?, ?, ?)");
        query.addBindValue(latitude);
        query.addBindValue(longitude);
        query.addBindValue(altitude);
        if(!query.exec()){
            qWarning() << "Insert failed" << query.lastError().text();
            return;
        }
        db.close();
    }
    QSqlDatabase::removeDatabase("points_connection");
}

void savePoints::createNewSave(){
    bridgeToMSFS::saveName = QString("save%1.sqlite").arg(QDateTime::currentDateTime().toString("yyyyMMddHHmmss"));
    {
        QSqlDatabase db = QSqlDatabase::addDatabase("QSQLITE", "saveindex_connection");
        db.setDatabaseName("saves.sqlite");
        if(!db.open()){
            qWarning() << "Cannot open database: " << db.lastError().text();
        }
        QSqlQuery query(db);
        if(!query.exec("CREATE TABLE IF NOT EXISTS saves (id INTEGER PRIMARY KEY AUTOINCREMENT, saveName TEXT)")){
            qWarning() << "Create table failed: " << query.lastError().text();
        }
        query.prepare("INSERT INTO saves (saveName) VALUES (?)");
        query.addBindValue(bridgeToMSFS::saveName);
        if(!query.exec()){
            qWarning() << "Insert failed: " << query.lastError().text();
        }
        db.close();
    }
    QSqlDatabase::removeDatabase("saveindex_connection");
}

void savePoints::saveTakeoffTime(){
    {
        QSqlDatabase db = QSqlDatabase::addDatabase("QSQLITE", "takeofftime_connection");
        db.setDatabaseName(bridgeToMSFS::saveName);
        if(!db.open()){
            qWarning() << "Cannot open database: " << db.lastError().text();
        }
        QSqlQuery query(db);
        if(!query.exec("CREATE TABLE IF NOT EXISTS startTimes (id INTEGER PRIMARY KEY AUTOINCREMENT, time INTEGER)")){
            qWarning() << "Create table failed: " << query.lastError().text();
        }
        query.prepare("INSERT INTO startTimes (time) VALUES (?)");
        query.addBindValue(std::time(nullptr));
        if(!query.exec()){
            qWarning() << "Insert failed: " << query.lastError().text();
        }
        db.close();
    }
    QSqlDatabase::removeDatabase("takeofftime_connection");
}

void savePoints::saveLandingTime(){
    {
        QSqlDatabase db = QSqlDatabase::addDatabase("QSQLITE", "landingtime_connection");
        db.setDatabaseName(bridgeToMSFS::saveName);
        if(!db.open()){
            qWarning() << "Cannot open the database: " << db.lastError().text();
        }
        QSqlQuery query(db);
        query.prepare("INSERT INTO startTimes (time) VALUES (?)");
        query.addBindValue(std::time(nullptr));
        if(!query.exec()){
            qWarning() << "Cannot insert value: " << query.lastError().text();
        }
        db.close();
    }
    QSqlDatabase::removeDatabase("landingtime_connection");
}

void savePoints::removeLastPoint(const QString& dbFile){
    {
        QSqlDatabase db = QSqlDatabase::addDatabase("QSQLITE", "removePoint_connection");
        db.setDatabaseName(dbFile);
        if(!db.open()){
            qWarning() << "Cannot open database: " << db.lastError().text();
            return;
        }
        QSqlQuery query(db);
        if(query.exec("SELECT id FROM points ORDER BY id DESC LIMIT 1")){
            if(query.next()){
                int lastId = query.value(0).toInt();
                QSqlQuery delQuery(db);
                delQuery.prepare("DELETE FROM points WHERE id = ?");
                delQuery.addBindValue(lastId);
                if(!delQuery.exec()){
                    qWarning() << "Delete failed: " << delQuery.lastError().text();
                }
            }
            else{
                qWarning() << "Select failed: " << query.lastError().text();
            }
        }
        db.close();
    }
    QSqlDatabase::removeDatabase("removePoints_connection");
}

void savePoints::savePointsTime(){
    {
        QSqlDatabase db = QSqlDatabase::addDatabase("QSQLITE", "savePointsTime_connection");
        db.setDatabaseName(bridgeToMSFS::saveName);
        if(!db.open()){
            qWarning() << "Cannot open database" << db.lastError().text();
        }
        QSqlQuery query(db);
        if(!query.exec("CREATE TABLE IF NOT EXISTS times (id INTEGER PRIMARY KEY AUTOINCREMENT, time INTEGER)")){
            qWarning() << "Cannot create table" << query.lastError().text();
        }
        query.prepare("INSERT INTO times (time) VALUES (?)");
        query.addBindValue(std::time(nullptr));
        pointTimes.push_back(std::time(nullptr));
        qDebug() << pointTimes.size();
        if(!query.exec()){
            qWarning() << "Cannot insert time: " << query.lastError().text();
        }
        db.close();
    }
    QSqlDatabase::removeDatabase("savePointsTime_connection");
}