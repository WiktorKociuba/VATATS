#ifndef CPDLCREQUESTS_H
#define CPDLCREQUESTS_H

#include <QDialog>

namespace Ui {
class cpdlcrequests;
}

class cpdlcrequests : public QDialog
{
    Q_OBJECT

public:
    explicit cpdlcrequests(QWidget *parent = nullptr);
    ~cpdlcrequests();

private:
    Ui::cpdlcrequests *ui;
    void sendCPDLC(QString msg);
private slots:
    void climb1();
    void climb2();
    void descent1();
    void descent2();
    void extra1();
    void extra2();
    void extra3();
    void extra4();
    void free1();
    void nego1();
    void nego2();
    void nego3();
    void nego4();
    void nego5();
    void offset1();
    void offset2();
    void route1();
    void route2();
    void route3();
    void route4();
    void route5();
    void route6();
    void speed1();
    void voice1();
};

#endif // CPDLCREQUESTS_H
