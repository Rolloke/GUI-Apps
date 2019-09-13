#include "gcodecontroldlg.h"
#include "ui_gcodecontroldlg.h"

CodeTestDlg::CodeTestDlg(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::CodeTestDlg)
{
    ui->setupUi(this);
}

CodeTestDlg::~CodeTestDlg()
{
    delete ui;
}

void CodeTestDlg::on_BtnDisplay_clicked()
{
    testParser(ui->comboBox->currentIndex(), ui->plainTextEdit->toPlainText());
}

void CodeTestDlg::setText(const QString& aText)
{
    ui->plainTextEdit->setPlainText(aText);
}

QString CodeTestDlg::getText() const
{
     return ui->plainTextEdit->toPlainText();
}
