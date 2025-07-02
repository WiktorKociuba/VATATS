#include <QApplication>
#include <QObject>
#include "globals.h"

tracking* g_mainWindow = nullptr;
bridgeToMSFS* g_bridgeToMSFSInstance = nullptr;

int main(int argc, char** argv){
    QApplication app(argc,argv);
    ifConnected = false;
    g_mainWindow = new tracking();
    g_bridgeToMSFSInstance = new bridgeToMSFS();
    g_mainWindow->show();
    return app.exec();
}