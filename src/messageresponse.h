#ifndef MESSAGERESPONSE_H
#define MESSAGERESPONSE_H

#include <QDialog>
#include "globals.h"

namespace Ui {
class messageResponse;
}

class messageResponse : public QDialog
{
    Q_OBJECT

public:
    explicit messageResponse(QWidget *parent = nullptr);
    ~messageResponse();
    static void response(QString requestID, QString station, QString response);
    void setupWindow(cpdlc::hoppieMessage message);
private:
    Ui::messageResponse *ui;
    static cpdlc::hoppieMessage currentMessage;
private slots:
    void onPosResClicked();
    void onNegResClicked();
};

#endif // MESSAGERESPONSE_H
