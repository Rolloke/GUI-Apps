#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QtXml/QDomDocument>
#include <QErrorMessage>
#include <QMediaPlayer>
#include <QVideoWidget>
#include <QNetworkAccessManager>
#include <QStandardItemModel>
#include <QLabel>


class QModelIndex;
class QWebEngineView;
class QItemSelection;

class CheckboxItemModel : public QStandardItemModel
{
public:
    CheckboxItemModel(int rows, int columns, QObject *parent = nullptr);

    QVariant data(const QModelIndex &index, int role = Qt::DisplayRole) const override;

    void setCheckedColumn(int checked);
private:
    int mChecked;
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
    QString get_item_name(int row) const;
    void select_index(int select);
    void display_play_status();
    void keyPressEvent(QKeyEvent *) override;

private slots:
    void menu_file_open();
    void menu_file_save_as_favorites();
    void menu_file_update_favorites();
    void menu_file_upload_favorites();
    void menu_file_download_favorites();
    void menu_edit_copy_url();
    void menu_edit_copy_thumb();
    void menu_edit_open_media_player();
    void menu_option_media_player_command();
    void menu_option_edit_upload_command();
    void menu_option_edit_download_command();
    void menu_help_about();
    void menu_help_info();
    void on_tableView_doubleClicked(const QModelIndex &index);
    void on_checkBoxSelectAll_clicked(bool checked);
    void on_tableView_clicked(const QModelIndex &index);
    void onReplyFinished();
    void on_sliderVolume_valueChanged(int value);
    void on_pushButtonStart_clicked();
    void on_pushButtonPause_clicked();
    void on_pushButtonStop_clicked();
    void on_pushButtonSelect_clicked();
    void on_pushButtonFind_clicked();
    void on_lineEditSelection_textChanged(const QString &arg1);
    void show_media_player_error(QMediaPlayer::Error error);
    void table_selectionChanged(const QItemSelection & selected, const QItemSelection &);

private:

    Ui::MainWindow *    ui;
    QErrorMessage       mErrorMsgDlg;
    QVideoWidget        mVideo;
    QMediaPlayer        mPlayer;
    CheckboxItemModel*  mListModel;
    QLabel*             m_play_status;
    QLabel*             m_play_name;
    QNetworkAccessManager mNetManager;
    QSharedPointer<QWebEngineView> mWebEngineView;
    int                 mCurrentRowIndex;
    int                 mCurrentPlayIndex;
    QList<int>          mHiddenColumns;
    QString             mFileOpenPath;
    QString             mFavoritesOpenPath;
    QString             mMediaPlayerCommand;

    QString             mOpenFileAtStart;
    QString             mOpenFileCmdLine;
    QString             mUploadFavoriteCommand;
    QString             mDownloadFavoriteCommand;
    int                 mFindStartRow;
    bool                mShowIcon;
    int                 mChecked;
};

#endif // MAINWINDOW_H
