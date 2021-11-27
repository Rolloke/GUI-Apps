#include "mergedialog.h"
#include "actions.h"
#include "helper.h"
#include "ui_mergedialog.h"

// TODO: merge command mit Dialog validieren
// git merge --abort
// git merge --continue
// git merge -s <resolve|recursive <ours|theirs|patience> > obsolete
// git mergetool
// Preview anzeigen ...
// git log --merge -p <path>

MergeDialog::MergeDialog(QWidget *parent) :
    QDialog(parent)
  , ui(new Ui::MergeDialog)
  , mAction("Merge", parent)
  , mStrategy(Strategy::Patience)
{
    ui->setupUi(this);

    QObject::connect(ui->groupBoxMergeStrategy, SIGNAL(clicked(int)), this, SLOT(on_clickedMergeStrategy(int)));
    ui->groupBoxMergeStrategy->setCurrentIndex(mStrategy);


    mAction.setStatusTip("git log");
    mAction.setToolTip("git merge command");

    QString result_string;
    int result = execute("git mergetool --tool-help", result_string);
    if (result == NoError)
    {
        auto result_list = result_string.split("\n");
        for (int i=1; i<result_list.size(); ++i)
        {
            if (result_list[i].size())
            {
                ui->comboGitMergeTool->addItem(result_list[i].trimmed());
            }
            else
            {
                break;
            }
        }
    }

    QVariantList fList;
    fList.append(QVariant(ActionList::sNoCustomCommandMessageBox));
    for (int i = ActionList::Data::PostCmdAction; i<ActionList::Data::ListSize; ++i)
    {
        fList.append(QVariant());
    }
    fList[ActionList::Data::Cmd] = QVariant(git::Cmd::NonGitCommands-1);
    mAction.setData(fList);
}

QAction* MergeDialog::getAction()
{
    return &mAction;
}

void MergeDialog::update()
{
    ui->textFileName->setText(mGitFilePath);
}

void MergeDialog::setCommand(const QString &command, bool trigger)
{
    mAction.setStatusTip(command);
    if (trigger)
    {
        mAction.trigger();
    }
}

QString MergeDialog::getMergeCommand()
{
    QString command;
    if (ui->comboGitMergeTool->currentIndex() == Tool::Automatic)
    {
        command = "merge ";
        switch (mStrategy)
        {
            case Strategy::Ours:      command += "-s ours ";      break;
            case Strategy::Theirs:    command += "-s theirs ";     break;
            case Strategy::Patience:  command += "-s recursive -Xdiff-algorithm=patience ";  break;
            case Strategy::Minimal:   command += "-s recursive -Xdiff-algorithm=minimal ";   break;
            case Strategy::Histogram: command += "-s recursive -Xdiff-algorithm=histogram "; break;
            case Strategy::Myers:     command += "-s recursive -Xdiff-algorithm=myers ";     break;
        }
        command += "%1 %2";
    }
    else
    {
        command = "mergetool %1 --no-prompt %2";
    }
    return command;
}

MergeDialog::~MergeDialog()
{
    delete ui;
}

void MergeDialog::on_btnMergePreview_clicked()
{
    setCommand(tr("git log -- %1").arg(getMergeCommand()));
}

void MergeDialog::on_bntMergeAbort_clicked()
{
    setCommand("git merge --abort");
}

void MergeDialog::on_btnMergeContinue_clicked()
{
    setCommand("git merge --continue");
}

void MergeDialog::on_btnMergeNow_clicked()
{
    setCommand(tr("git %1").arg(getMergeCommand()));
}

void MergeDialog::showEvent(QShowEvent * /* event */)
{
    update();
}

void MergeDialog::on_clickedMergeStrategy(int index)
{
    mStrategy = static_cast<Strategy::e>(index);
}
