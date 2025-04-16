#ifndef MERGEDIALOG_H
#define MERGEDIALOG_H

#include <QDialog>
#include <QAction>

namespace Ui {
class MergeDialog;
}

class MergeDialog : public QDialog
{
    Q_OBJECT

public:
    explicit MergeDialog(QWidget *parent = 0);
    ~MergeDialog();

    QString mGitFilePath;
    QAction* getAction();
    void     update();
protected:

    void showEvent(QShowEvent *event) override;

private:
    void setCommand(const QString& command, bool trigger=true);
    QString getMergeCommand();

private slots:
    void on_btnMergePreview_clicked();
    void on_bntMergeAbort_clicked();
    void on_btnMergeContinue_clicked();
    void on_btnMergeNow_clicked();
    void on_clickedMergeStrategy(int index);

private:
    struct Strategy
    { enum e
        {
            Ours,
            Theirs,
            Patience,
            Minimal,
            Histogram,
            Myers
        };
    };
    struct Tool
    { enum e
        {
            Automatic,
            Default
        };
    };

    Ui::MergeDialog *ui;
    QAction mAction;
    Strategy::e mStrategy;
};

#endif // MERGEDIALOG_H
