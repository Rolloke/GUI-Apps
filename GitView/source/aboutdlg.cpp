#include "aboutdlg.h"
#include "ui_aboutdlg.h"

AboutDlg::AboutDlg(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::AboutDlg)
{
    ui->setupUi(this);
    if (parent)
    {
        setWindowTitle(tr("About ") + parent->windowTitle());
        ui->labelTitle->setText(parent->windowTitle());
    }
    ui->textBased_on->setText(ui->textBased_on->text()+qVersion());
    ui->textVersion->setText(QApplication::applicationVersion());

    ui->textBuildDate->setText(__DATE__ + tr(", ") + __TIME__);
}

AboutDlg::~AboutDlg()
{
    delete ui;
}
