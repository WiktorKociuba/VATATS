#include <QApplication>
#include "flightTrackingPage.h"

int main(int argc, char** argv){
    QApplication app(argc,argv);
    flightTrackingPage::startTracking();
    return app.exec();
}