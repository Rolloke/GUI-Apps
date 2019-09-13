#include "lptporttestdlg.h"
#include "ui_lptporttestdlg.h"
#include "LPTPort.h"
#include "PlotterDriver.h"

#include <errno.h>

LPTPortTestDlg::LPTPortTestDlg(PlotterDriver*apPD, QWidget *parent) :
    QDialog(parent),
    ui(new Ui::LPTPortTestDlg)
{
    ui->setupUi(this);
    mPort = &apPD->getLPT();

    QString fS;
    ui->labelAddress->setText("");
    ui->labelPortState->setText(strerror(errno));
}

LPTPortTestDlg::~LPTPortTestDlg()
{
    delete ui;
}

void LPTPortTestDlg::on_buttonReadData_clicked()
{
    Bits fbits = mPort->getBits(LPTPort::data);
    ui->ckData_1->setChecked((fbits.bits & (1<<0)) != 0);
    ui->ckData_1->setChecked((fbits.bits & (1<<1)) != 0);
    ui->ckData_1->setChecked((fbits.bits & (1<<2)) != 0);
    ui->ckData_1->setChecked((fbits.bits & (1<<3)) != 0);
    ui->ckData_1->setChecked((fbits.bits & (1<<4)) != 0);
    ui->ckData_1->setChecked((fbits.bits & (1<<5)) != 0);
    ui->ckData_1->setChecked((fbits.bits & (1<<6)) != 0);
    ui->ckData_1->setChecked((fbits.bits & (1<<7)) != 0);
    ui->labelPortState->setText(strerror(errno));
}

void LPTPortTestDlg::on_buttonSetData_clicked()
{
    Bits fbits;
    fbits.bits  = ui->ckData_1->isChecked();
    fbits.bits |= ui->ckData_2->isChecked() << 1;
    fbits.bits |= ui->ckData_3->isChecked() << 2;
    fbits.bits |= ui->ckData_4->isChecked() << 3;
    fbits.bits |= ui->ckData_5->isChecked() << 4;
    fbits.bits |= ui->ckData_6->isChecked() << 5;
    fbits.bits |= ui->ckData_7->isChecked() << 6;
    fbits.bits |= ui->ckData_8->isChecked() << 7;
    mPort->setBits(LPTPort::data, fbits);
    ui->labelPortState->setText(strerror(errno));
}

void LPTPortTestDlg::on_buttonReadStatus_clicked()
{
    Bits fbits = mPort->getBits(LPTPort::status);
    ui->chStatus1->setChecked((fbits.bits & (1<<0)) != 0);
    ui->chStatus2->setChecked((fbits.bits & (1<<1)) != 0);
    ui->chStatus3->setChecked((fbits.bits & (1<<2)) != 0);
    ui->chStatus4->setChecked((fbits.bits & (1<<3)) != 0);
    ui->chStatus5->setChecked((fbits.bits & (1<<4)) != 0);
    ui->chStatus6->setChecked((fbits.bits & (1<<5)) != 0);
    ui->chStatus7->setChecked((fbits.bits & (1<<6)) != 0);
    ui->chStatus8->setChecked((fbits.bits & (1<<7)) != 0);
    ui->labelPortState->setText(strerror(errno));
}

void LPTPortTestDlg::on_buttonReadControl_clicked()
{
    Bits fbits = mPort->getBits(LPTPort::control);
    ui->ckControl1->setChecked((fbits.bits & (1<<0)) != 0);
    ui->ckControl2->setChecked((fbits.bits & (1<<1)) != 0);
    ui->ckControl3->setChecked((fbits.bits & (1<<2)) != 0);
    ui->ckControl4->setChecked((fbits.bits & (1<<3)) != 0);
    ui->ckControl5->setChecked((fbits.bits & (1<<4)) != 0);
    ui->ckControl6->setChecked((fbits.bits & (1<<5)) != 0);
    ui->ckControl7->setChecked((fbits.bits & (1<<6)) != 0);
    ui->ckControl8->setChecked((fbits.bits & (1<<7)) != 0);
    ui->labelPortState->setText(strerror(errno));
}

void LPTPortTestDlg::on_buttonSetControl_clicked()
{
    Bits fbits;
    fbits.bits  = ui->ckControl1->isChecked();
    fbits.bits |= ui->ckControl2->isChecked() << 1;
    fbits.bits |= ui->ckControl3->isChecked() << 2;
    fbits.bits |= ui->ckControl4->isChecked() << 3;
    fbits.bits |= ui->ckControl5->isChecked() << 4;
    fbits.bits |= ui->ckControl6->isChecked() << 5;
    fbits.bits |= ui->ckControl7->isChecked() << 6;
    fbits.bits |= ui->ckControl8->isChecked() << 7;
    mPort->setBits(LPTPort::control, fbits);
    ui->labelPortState->setText(strerror(errno));
}

