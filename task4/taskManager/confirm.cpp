#include "confirm.h"
#include "ui_confirm.h"

Confirm::Confirm(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::Confirm)
{
    ui->setupUi(this);
    connect(ui->buttonBox, SIGNAL(accepted()), \
            this, SLOT(Ok()));
    connect(ui->buttonBox, SIGNAL(rejected()), \
            this, SLOT(Cancel()));
}

Confirm::~Confirm()
{
    delete ui;
}

void Confirm::Ok()
{
    this->hide();
    system("poweroff");
}

void Confirm::Cancel()
{
    this->hide();
}
