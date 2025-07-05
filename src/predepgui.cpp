#include "predepgui.h"
#include "ui_predepgui.h"
#include "cpdlc.h"
#include "globals.h"
#include <QObject>

predepgui::predepgui(QWidget *parent)
    : QDialog(parent)
    , ui(new Ui::predepgui)
{
    ui->setupUi(this);

    connect(ui->sendPB, &QPushButton::clicked, this, &predepgui::onSendClicked);
}

predepgui::~predepgui()
{
    delete ui;
}

void predepgui::onSendClicked(){
    QString station = ui->stationLine->text().trimmed();
    QString callsign = ui->callsignLine->text().trimmed();
    QString acft = ui->acftLine->text().trimmed();
    QString from = ui->originLine->text().trimmed();
    QString dest = ui->destLine->text().trimmed();
    QString stand = ui->standLine->text().trimmed();
    QString atis = ui->atisLine->text().trimmed();
    QString packet = QString("REQUEST PREDEP CLEARANCE @%1@ @%2@ TO @%3@ AT @%4@ STAND @%5@ ATIS @%6@").arg(callsign,acft,dest,from,stand,atis);
    cpdlc* myCpdlc = new cpdlc(g_mainWindow);
    QObject::connect(myCpdlc, &cpdlc::messageResult, [myCpdlc,packet](const bool result){
        g_messages.push_back(packet);
        static_cast<tracking*>(g_mainWindow)->updateMessageList();
    });
    myCpdlc->sendMessage(g_hoppieSecret, g_callsign, station, "telex", packet);
}