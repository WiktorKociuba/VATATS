#ifndef TRACKING_H
#define TRACKING_H

#include <QWidget>

namespace Ui {
class tracking;
}

class tracking : public QWidget
{
    Q_OBJECT

public:
    explicit tracking(QWidget *parent = nullptr);
    ~tracking();
    Ui::tracking *ui;
};

#endif // TRACKING_H
