#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QtXml/QDomDocument>
#include <QErrorMessage>
#include <QMediaPlayer>
#include <QVideoWidget>

class QAbstractItemModel;
class QModelIndex;

namespace Ui {
class MainWindow;
}

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    explicit MainWindow(QWidget *parent = 0);
    ~MainWindow();

private:
    QString getConfigName() const;

private slots:
    void on_pushButtonOpen_clicked();
    void on_tableView_doubleClicked(const QModelIndex &index);
    void on_pushButtonSelect_clicked();


private:
    Ui::MainWindow *ui;
    QErrorMessage mErrorMsgDlg;
    QString mFileOpenPath;
    QMediaPlayer mPlayer;
    QVideoWidget mVideo;
    QAbstractItemModel* mListModel;
    QList<bool> mIsRadio;
};

#endif // MAINWINDOW_H
