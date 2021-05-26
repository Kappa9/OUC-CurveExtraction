#include "extractcurvewindow.h"
#include "ui_extractcurvewindow.h"

extractcurvewindow::extractcurvewindow(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::extractcurvewindow)
{
    ui->setupUi(this);
}

extractcurvewindow::~extractcurvewindow()
{
    delete ui;
}
