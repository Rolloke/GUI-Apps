#include "commitmessage.h"
#include "ui_commitmessage.h"

CommitMessage::CommitMessage(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::CommitMessage)
{
    ui->setupUi(this);
}

CommitMessage::~CommitMessage()
{
    delete ui;
}


QString CommitMessage::getMessageText()
{
    return ui->textMessageEdit->toPlainText();
}

bool    CommitMessage::getAutoStage()
{
    return ui->ckAutoStage->isChecked();
}

bool    CommitMessage::getAndPush()
{
    return ui->ckCommitAndPush->isChecked();
}
