#include "cpdlcrequests.h"
#include "ui_cpdlcrequests.h"
#include "globals.h"

cpdlcrequests::cpdlcrequests(QWidget *parent)
    : QDialog(parent)
    , ui(new Ui::cpdlcrequests)
{
    ui->setupUi(this);

    connect(ui->comboBox, QOverload<int>::of(&QComboBox::currentIndexChanged),
            ui->stackedWidget, &QStackedWidget::setCurrentIndex);

    //climb
    connect(ui->climbToSend, &QPushButton::clicked, this, &cpdlcrequests::climb1);
    connect(ui->atClimbPB, &QPushButton::clicked, this, &cpdlcrequests::climb2);

    //descent
    connect(ui->descentPB, &QPushButton::clicked, this, &cpdlcrequests::descent1);
    connect(ui->atDesPB, &QPushButton::clicked, this, &cpdlcrequests::descent2);

    //extra
    connect(ui->extra1PB, &QPushButton::clicked, this, &cpdlcrequests::extra1);
    connect(ui->extra2PB, &QPushButton::clicked, this, &cpdlcrequests::extra2);
    connect(ui->extra3PB, &QPushButton::clicked, this, &cpdlcrequests::extra3);
    connect(ui->extra4PB, &QPushButton::clicked, this, &cpdlcrequests::extra4);

    //free
    connect(ui->freePB, &QPushButton::clicked, this, &cpdlcrequests::free1);

    //negotiation
    connect(ui->nego1PB, &QPushButton::clicked, this, &cpdlcrequests::nego1);
    connect(ui->nego2PB, &QPushButton::clicked, this, &cpdlcrequests::nego2);
    connect(ui->nego3PB, &QPushButton::clicked, this, &cpdlcrequests::nego3);
    connect(ui->nego4PB, &QPushButton::clicked, this, &cpdlcrequests::nego4);
    connect(ui->nego5PB, &QPushButton::clicked, this, &cpdlcrequests::nego5);

    //offset
    connect(ui->off1PB, &QPushButton::clicked, this, &cpdlcrequests::offset1);
    connect(ui->off2PB, &QPushButton::clicked, this, &cpdlcrequests::offset2);

    //route
    connect(ui->route1PB, &QPushButton::clicked, this, &cpdlcrequests::route1);
    connect(ui->route2PB, &QPushButton::clicked, this, &cpdlcrequests::route2);
    connect(ui->route3PB, &QPushButton::clicked, this, &cpdlcrequests::route3);
    connect(ui->route4PB, &QPushButton::clicked, this, &cpdlcrequests::route4);
    connect(ui->route5PB, &QPushButton::clicked, this, &cpdlcrequests::route5);
    connect(ui->route6PB, &QPushButton::clicked, this, &cpdlcrequests::route6);

    //speed
    connect(ui->speedPB, &QPushButton::clicked, this, &cpdlcrequests::speed1);
}

cpdlcrequests::~cpdlcrequests()
{
    delete ui;
}

void cpdlcrequests::sendCPDLC(QString msg){
    cpdlc* myCpdlcRequest = new cpdlc(g_mainWindow);
    QString packet = QString("/data2/%1//Y/%2").arg(QString::number(messageId),msg);
    QObject::connect(myCpdlcRequest, &cpdlc::messageResult, [packet,msg](const bool result){
        if(result){
            g_messages.push_front({g_callsign, "cpdlc", QString::number(messageId), "", "Y", msg});
            messageId++;
        }
    });
    myCpdlcRequest->sendMessage(g_hoppieSecret, g_callsign, g_currentStation, "cpdlc", packet);
    this->accept();
}

void cpdlcrequests::climb1(){
    QString arg1 = ui->climbToLine->text();
    QString msg = QString("REQUEST CLIMB TO @%1@").arg(arg1);
    this->sendCPDLC(msg);
}

void cpdlcrequests::climb2(){
    QString arg1 = ui->atClimbWay->text(), arg2 = ui->atClimbAlt->text();
    QString msg = QString("AT @%1@ REQUEST CLIMB TO @%2@").arg(arg1,arg2);
    this->sendCPDLC(msg);
}

void cpdlcrequests::descent1(){
    QString arg1 = ui->descentLine->text();
    QString msg = QString("REQUEST DESCENT TO @%1@").arg(arg1);
    this->sendCPDLC(msg);
}

void cpdlcrequests::descent2(){
    QString arg1 = ui->atDesWp->text(), arg2 = ui->atDesAlt->text();
    QString msg = QString("AT @%1@ REQUEST DESCENT TO @%2@").arg(arg1,arg2);
    this->sendCPDLC(msg);
}

void cpdlcrequests::extra1(){
    QString arg1 = ui->extra1line1->text(), arg2 = ui->extra1line2->text();
    QString msg = QString("WE CAN ACCEPT @%1@ AT @%2@").arg(arg1,arg2);
    this->sendCPDLC(msg);
}

void cpdlcrequests::extra2(){
    QString arg1 = ui->extra2line->text();
    QString msg = QString("WE CANNOT ACCEPT @%1@").arg(arg1);
    this->sendCPDLC(msg);
}

void cpdlcrequests::extra3(){
    QString arg1 = ui->extra3line->text();
    QString msg = QString("WHEN CAN WE EXPECT CLIMB TO @%1@").arg(arg1);
    this->sendCPDLC(msg);
}

void cpdlcrequests::extra4(){
    QString arg1 = ui->extra4line->text();
    QString msg = QString("WHEN CAN WE EXPECT DESCENT TO @%1%@").arg(arg1);
    this->sendCPDLC(msg);
}

void cpdlcrequests::free1(){
    QString msg = ui->freeTextEdit->toPlainText();
    this->sendCPDLC(msg);
}

void cpdlcrequests::nego1(){
    QString arg1 = ui->nego1line->text();
    QString msg = QString("WHEN CAN WE EXPECT @%1@").arg(arg1);
    this->sendCPDLC(msg);
}

void cpdlcrequests::nego2(){
    QString msg = "WHEN CAN WE EXPECT BACK ON ROUTE";
    this->sendCPDLC(msg);
}

void cpdlcrequests::nego3(){
    QString msg = "WHEN CAN WE EXPECT LOWER ALTITUDE";
    this->sendCPDLC(msg);
}

void cpdlcrequests::nego4(){
    QString msg = "WHEN CAN WE EXPECT HIGHER ALTITUDE";
    this->sendCPDLC(msg);
}

void cpdlcrequests::nego5(){
    QString arg1 = ui->nego5line->text();
    QString msg = QString("WHEN CAN WE EXPECT CRUISE CLIMB TO @%1@").arg(arg1);
    this->sendCPDLC(msg);
}

void cpdlcrequests::offset1(){
    QString arg1 = ui->off1line1->text(), arg2 = ui->off1line2->text();
    QString msg = QString("REQUEST OFFSET @%1@ @%2@ OF ROUTE").arg(arg1,arg2);
    this->sendCPDLC(msg);
}

void cpdlcrequests::offset2(){
    QString arg1 = ui->off2line1->text(), arg2 = ui->off2line2->text(), arg3 = ui->off2line3->text();
    QString msg = QString("AT @%1@ REQUEST OFFSET @%2@ @%3@ OF ROUTE").arg(arg1,arg2,arg3);
    this->sendCPDLC(msg);
}

void cpdlcrequests::route1(){
    QString arg1 = ui->route1line->text();
    QString msg = QString("REQUEST DIRECT TO @%1@").arg(arg1);
    this->sendCPDLC(msg);
}

void cpdlcrequests::route2(){
    QString arg1 = ui->route2line->text();
    QString msg = QString("REQUEST @%1@").arg(arg1);
    this->sendCPDLC(msg);
}

void cpdlcrequests::route3(){
    QString arg1 = ui->route3line1->text(), arg2 = ui->route3line2->text();
    QString msg = QString("REQUEST WEATHER DEVIATION TO @%1@ VIA @%2@").arg(arg1,arg2);
    this->sendCPDLC(msg);
}

void cpdlcrequests::route4(){
    QString arg1 = ui->route4line1->text(), arg2 = ui->route4line2->text();
    QString msg = QString("REQUEST WEATHER DEVIATION UP TO @%1@ @%2@ OF ROUTE").arg(arg1,arg2);
    this->sendCPDLC(msg);
}

void cpdlcrequests::route5(){
    QString arg1 = ui->route5line->text();
    QString msg = QString("REQUEST HEADING @%1@").arg(arg1);
    this->sendCPDLC(msg);
}

void cpdlcrequests::route6(){
    QString arg1 = ui->route6line->text();
    QString msg = QString("REQUEST GROUND TRACK @%1@").arg(arg1);
    this->sendCPDLC(msg);
}

void cpdlcrequests::speed1(){
    QString arg1 = ui->speedLine->text();
    QString msg = QString("REQUEST @%1@").arg(arg1);
    this->sendCPDLC(msg);
}

void cpdlcrequests::voice1(){
    QString msg = "REQUEST VOICE CONTACT";
    this->sendCPDLC(msg);
}