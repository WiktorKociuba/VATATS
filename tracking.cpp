#include "tracking.h"
#include "ui_tracking.h"
#include <QWebEngineView>

tracking::tracking(QWidget *parent)
    : QWidget(parent)
    , ui(new Ui::tracking)
{
    ui->setupUi(this);
}

tracking::~tracking()
{
    delete ui;
}
