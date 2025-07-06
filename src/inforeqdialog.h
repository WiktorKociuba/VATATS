#ifndef INFOREQDIALOG_H
#define INFOREQDIALOG_H

#include <QDialog>

namespace Ui {
class inforeqdialog;
}

class inforeqdialog : public QDialog
{
    Q_OBJECT

public:
    explicit inforeqdialog(QWidget *parent = nullptr);
    ~inforeqdialog();

private:
    Ui::inforeqdialog *ui;
    void send(QString msg);
private slots:
    void info1();
    void info2();
    void info3();
    void info4();
};

#endif // INFOREQDIALOG_H
