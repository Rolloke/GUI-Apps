#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QtXml/QDomDocument>
#include <QErrorMessage>
#include <QMediaPlayer>
#include <QVideoWidget>
#include <QNetworkAccessManager>
#include <QStandardItemModel>


class QModelIndex;

class CheckboxItemModel : public QStandardItemModel
{
public:
    CheckboxItemModel(int rows, int columns, QObject *parent = nullptr);

    QVariant data(const QModelIndex &index, int role = Qt::DisplayRole) const override;

};

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
    void open_file(const QString& file_name);
    QString getConfigName() const;

private slots:
    void on_pushButtonOpen_clicked();
    void on_tableView_doubleClicked(const QModelIndex &index);
    void on_pushButtonSelect_clicked();
    void on_tableView_clicked(const QModelIndex &index);
    void onReplyFinished();
    void on_sliderVolume_valueChanged(int value);
    void on_pushButtonStart_clicked();
    void on_pushButtonStop_clicked();
    void on_checkBoxSelectAll_clicked(bool checked);
    void on_pushButtonSaveAsFavorites_clicked();
    void on_pushButtonFind_clicked();
    void on_lineEditSelection_textChanged(const QString &arg1);
    void show_media_player_error(QMediaPlayer::Error error);

private:

    Ui::MainWindow *ui;
    QErrorMessage mErrorMsgDlg;
    QString mFileOpenPath;
    QMediaPlayer mPlayer;
    QVideoWidget mVideo;
    CheckboxItemModel* mListModel;
    QNetworkAccessManager mNetManager;
    QString mCurrentUrl;
    QString mCurrentDestination;
    QList<int> mHiddenColumns;
    int        mFindStartRow;
};

#endif // MAINWINDOW_H
