#ifndef COMMITMESSAGE_H
#define COMMITMESSAGE_H

#include <QDialog>

namespace Ui {
class CommitMessage;
}

class CommitMessage : public QDialog
{
    Q_OBJECT

public:
    explicit CommitMessage(QWidget *parent = 0);
    ~CommitMessage();

    QString getMessageText();
    bool    getAutoStage();
private slots:

private:
    Ui::CommitMessage *ui;
};

#endif // COMMITMESSAGE_H
