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
};

#endif // CPDLCREQUESTS_H
