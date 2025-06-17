#include "savePoints.h"
#include <QSqlDatabase>
#include <QSqlQuery>
#include <QSqlError>
#include <QFile>

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