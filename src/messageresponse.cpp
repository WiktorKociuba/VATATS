#include "messageresponse.h"
#include "ui_messageresponse.h"
#include "globals.h"

cpdlc::hoppieMessage messageResponse::currentMessage;

messageResponse::messageResponse(QWidget *parent)
    : QDialog(parent)
    , ui(new Ui::messageResponse)
{
    ui->setupUi(this);

    connect(ui->posRes, &QPushButton::clicked, this, &messageResponse::onPosResClicked);
    connect(ui->negRes, &QPushButton::clicked, this, &messageResponse::onNegResClicked);
}

messageResponse::~messageResponse()
{
    delete ui;
}

void messageResponse::response(QString requestID, QString station, QString response){
    QString packet = QString("/data2/%1/%2/N/%3").arg(QString::number(messageId),requestID,response);
    messageId++;
    QObject::connect(myCpdlc, &cpdlc::messageResult, [packet, requestID](const bool result){
        g_messages.push_front({g_callsign,"cpdlc",QString::number(messageId),requestID,"NE",packet});
        static_cast<tracking*>(g_mainWindow)->updateMessageList();
    });
    myCpdlc->sendMessage(g_hoppieSecret, g_callsign, station, "cpdlc", packet);
}

void messageResponse::setupWindow(cpdlc::hoppieMessage message){
    messageResponse::currentMessage = message;
    if(message.responseType == "WU"){
        ui->posRes->setEnabled(true);
        ui->posRes->setEnabled(true);
        ui->posRes->setText("WILCO");
        ui->negRes->setText("UNABLE");
    }
    else if(message.responseType == "AN"){
        ui->posRes->setEnabled(true);
        ui->posRes->setEnabled(true);
        ui->posRes->setText("AFFIRM");
        ui->negRes->setText("NEGATIVE");
    }
    else if(message.responseType == "R"){
        ui->posRes->setText("ROGER");
        ui->posRes->setEnabled(true);
        ui->negRes->setEnabled(false);
    }
    else{
        ui->posRes->setEnabled(false);
        ui->posRes->setEnabled(false);
    }
    ui->messageBrowser->setText(message.packet);
    ui->fromLabel->setText("FROM: " + message.station);
}

void messageResponse::onPosResClicked(){
    QString response;
    if(messageResponse::currentMessage.responseType == "WU"){
        response = "WILCO";
    }
    else if(messageResponse::currentMessage.responseType == "AN"){
        response = "AFFIRM";
    }
    else if(messageResponse::currentMessage.responseType == "R"){
        response = "ROGER";
    }
    messageResponse::response(messageResponse::currentMessage.messageID, messageResponse::currentMessage.station,response);
    this->accept();
}

void messageResponse::onNegResClicked(){
    QString response;
    if(messageResponse::currentMessage.responseType == "WU"){
        response = "UNABLE";
    }
    else if(messageResponse::currentMessage.responseType == "AN"){
        response = "NEGATIVE";
    }
    messageResponse::response(messageResponse::currentMessage.messageID, messageResponse::currentMessage.station, response);
    this->accept();
}