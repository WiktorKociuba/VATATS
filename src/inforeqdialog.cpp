#include "inforeqdialog.h"
#include "ui_inforeqdialog.h"
#include "cpdlc.h"
#include "globals.h"

inforeqdialog::inforeqdialog(QWidget *parent)
    : QDialog(parent)
    , ui(new Ui::inforeqdialog)
{
    ui->setupUi(this);
    connect(ui->info1PB, &QPushButton::clicked, this, &inforeqdialog::info1);
    connect(ui->info2PB, &QPushButton::clicked, this, &inforeqdialog::info2);
    connect(ui->info3PB, &QPushButton::clicked, this, &inforeqdialog::info3);
    connect(ui->info4PB, &QPushButton::clicked, this, &inforeqdialog::info4);
}

inforeqdialog::~inforeqdialog()
{
    delete ui;
}

void inforeqdialog::send(QString msg){
    cpdlc* cpdlcInforeq = new cpdlc(g_mainWindow);
    cpdlcInforeq->sendMessage(g_hoppieSecret, g_callsign, "ACARS", "inforeq", msg);
    this->accept();
}

void inforeqdialog::info1(){
    QString msg = QString("METAR %1").arg(ui->inforeqLine->text());
    this->send(msg);
}

void inforeqdialog::info2(){
    QString msg = QString("TAF %1").arg(ui->inforeqLine->text());
    this->send(msg);
}

void inforeqdialog::info3(){
    QString msg = QString("SHORTTAF %1").arg(ui->inforeqLine->text());
    this->send(msg);
}

void inforeqdialog::info4(){
    QString msg = QString("VATATIS %1").arg(ui->inforeqLine->text());
    this->send(msg);
}