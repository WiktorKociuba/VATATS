#ifndef TELEXDIALOG_H
#define TELEXDIALOG_H

#include <QDialog>

namespace Ui {
class telexdialog;
}

class telexdialog : public QDialog
{
    Q_OBJECT

public:
    explicit telexdialog(QWidget *parent = nullptr);
    ~telexdialog();

private:
    Ui::telexdialog *ui;
private slots:
    void onSendClicked();
};

#endif // TELEXDIALOG_H
