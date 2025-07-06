#ifndef LOGON_H
#define LOGON_H

#include <QDialog>

namespace Ui {
class logon;
}

class logon : public QDialog
{
    Q_OBJECT

public:
    explicit logon(QWidget *parent = nullptr);
    ~logon();

private:
    Ui::logon *ui;
private slots:
    void onLogonClicked();
};

#endif // LOGON_H
