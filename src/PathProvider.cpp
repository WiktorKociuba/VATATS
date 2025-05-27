#include "PathProvider.h"
#include <iostream>

QVariantList PathProvider::getPath() const {
    QVariantList path;
    for (const auto& pt : points)
        path << QVariantList{pt.first, pt.second};
    return path;
}
void PathProvider::setPoints(const QVector<QPair<double,double>>& pts){
    points = pts;
    emit pathChanged();
}