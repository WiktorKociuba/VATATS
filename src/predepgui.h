#ifndef PREDEPGUI_H
#define PREDEPGUI_H

#include <QDialog>

namespace Ui {
class predepgui;
}

class predepgui : public QDialog
{
    Q_OBJECT

public:
    explicit predepgui(QWidget *parent = nullptr);
    ~predepgui();

private:
    Ui::predepgui *ui;
private slots:
    void onSendClicked();
};

#endif // PREDEPGUI_H
