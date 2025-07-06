#include "telexdialog.h"
#include "ui_telexdialog.h"
#include "cpdlc.h"
#include "globals.h"

telexdialog::telexdialog(QWidget *parent)
    : QDialog(parent)
    , ui(new Ui::telexdialog)
{
    ui->setupUi(this);

    connect(ui->telexPB, &QPushButton::clicked, this, &telexdialog::onSendClicked);
}

telexdialog::~telexdialog()
{
    delete ui;
}

void telexdialog::onSendClicked(){
    QString msg = ui->telexEdit->toPlainText(), rec = ui->toLine->text();
    cpdlc* cpdlcTelex = new cpdlc(g_mainWindow);
    connect(cpdlcTelex, &cpdlc::messageResult, [msg](const bool result){
        if(result){
            g_messages.push_front({g_callsign, "telex", "", "", "", msg});
        }
    });
    cpdlcTelex->sendMessage(g_hoppieSecret, g_callsign, rec, "telex", msg);
    this->accept();
}