#include "chardialog.h"
#include "ui_chardialog.h"

charDialog::charDialog(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::charDialog)
{
    ui->setupUi(this);
    connect(ui->widget,SIGNAL(characterSelected(int)),this,SLOT(mydone(int)));
}

charDialog::~charDialog()
{
    delete ui;
}

void charDialog::setFontData(unsigned char *data, int len)
{
    ui->widget->updateFont(data, len);
}

void charDialog::mydone(int n)
{
    done(n+1);
}
