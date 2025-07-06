#include "logon.h"
#include "ui_logon.h"
#include "globals.h"
#include "tracking.h"

logon::logon(QWidget *parent)
    : QDialog(parent)
    , ui(new Ui::logon)
{
    ui->setupUi(this);

    connect(ui->logonPB, &QPushButton::clicked, this, &logon::onLogonClicked);
}

logon::~logon()
{
    delete ui;
}

void logon::onLogonClicked(){
    QString station = ui->logonLine->text();
    QString packet = QString("/data2/%1//Y/REQUEST LOGON").arg(QString::number(messageId));
    cpdlc* myCpdlcLogon = new cpdlc(g_mainWindow);
    QObject::connect(myCpdlcLogon, &cpdlc::messageResult, [](const bool result){
        if(result){
            g_messages.push_front({g_callsign, "cpdlc", QString::number(messageId), "", "Y", "REQUEST LOGON"});
            messageId++;
        }
    });
    QObject::connect(myCpdlcLogon, &cpdlc::logonAccepted, [station](){
        g_currentStation = station;
        g_mainWindow->updateCurrentStation();
        g_mainWindow->logonConfirmed();
    });
    myCpdlcLogon->sendMessage(g_hoppieSecret, g_callsign, station, "cpdlc", packet);
    this->accept();
}