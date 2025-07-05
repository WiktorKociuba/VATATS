#include "cpdlcrequests.h"
#include "ui_cpdlcrequests.h"

cpdlcrequests::cpdlcrequests(QWidget *parent)
    : QDialog(parent)
    , ui(new Ui::cpdlcrequests)
{
    ui->setupUi(this);

    connect(ui->comboBox, QOverload<int>::of(&QComboBox::currentIndexChanged),
            ui->stackedWidget, &QStackedWidget::setCurrentIndex);
}

cpdlcrequests::~cpdlcrequests()
{
    delete ui;
}
