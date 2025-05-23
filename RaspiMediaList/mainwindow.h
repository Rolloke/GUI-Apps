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
#include <QMap>

class QModelIndex;
class QWebEngineView;
class QItemSelection;
class QSystemTrayIcon;

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

protected:
    bool  event(QEvent*) override;
    void timerEvent(QTimerEvent*);

private:
    enum class meta { tracks, meta };
    void open_file(const QString& file_name);
    QString getConfigName() const;
    QString get_item_name(int row) const;
    void select_index(int select);
    void display_play_status();
    void keyPressEvent(QKeyEvent *) override;
    void changeEvent(QEvent *event) override;
    void closeEvent(QCloseEvent *event) override;
    void add_button_to_menue(QMenu*menu, QPushButton* button);
    void update_command_states();
    void handle_end_of_media();

private slots:
    void menu_file_open();
    void menu_folder_open();
    void menu_file_save_as_favorites();
    void menu_file_update_favorites();
    void menu_file_upload_favorites();
    void menu_file_download_favorites();
    void menu_file_download_kodi_raw_file();
    void menu_edit_copy_url();
    void menu_edit_copy_thumb();
    void menu_edit_open_media_player();
    void menu_option_media_player_command();
    void menu_option_edit_upload_command();
    void menu_option_edit_download_command();
    void menu_option_edit_download_m3u_file();
    void menu_option_show_tray_icon(bool show);
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
    void on_pushButtonNext_released();
    void on_pushButtonPrevious_released();
    void on_lineEditSelection_textChanged(const QString &arg1);
    void show_media_player_error(QMediaPlayer::Error error, const QString& sError = {});
    void table_selectionChanged(const QItemSelection & selected, const QItemSelection &);
    void media_status_changed(const QMediaPlayer::MediaStatus& status);
    void tableView_clicked(const QModelIndex &index, bool called_by_function=false);

#if QT_VERSION >= QT_VERSION_CHECK(6, 0, 0)
    void metaDataChanged();
    void tracksChanged();
#endif
    void metaDataChanged(const QString&key, const QVariant&value);
    void traymenu_hide_window();
    void traymenu_show_window();
    void onDownloadFinished();
    bool set_media_info_to_item(int row);
    void generate_media_file_tray_message();
    void update_duration_info(bool position);

private:
    void call_metaDataChanged(meta what);

    Ui::MainWindow *    ui;
    QErrorMessage       mErrorMsgDlg;
    QVideoWidget        mVideo;
    QMediaPlayer        mPlayer;
    CheckboxItemModel*  mListModel;
    QLabel*             m_play_status {nullptr};
    QLabel*             m_play_name {nullptr};
    QSystemTrayIcon*    m_tray_message {nullptr};
    QNetworkAccessManager mNetManager;
    QSharedPointer<QWebEngineView> mWebEngineView;
    int                 mCurrentRowIndex {0};
    int                 mCurrentPlayIndex {0};
    QList<int>          mHiddenColumns;
    QString             mFileOpenPath;
    QString             mFileOpenFolderPath;
    QString             mFavoritesOpenPath;
    QString             mMediaPlayerCommand;
    QString             mOpenFileAtStart;
    QString             mOpenFileCmdLine;
    QString             mUploadFavoriteCommand;
    QString             mDownloadFavoriteCommand;
    QString             mDownloadKodiRawFilePath;
    QMap<QString, QVariant> mCurrentMetainfo;
    int                 mFindStartRow {0};
    bool                mShowIcon {false};
    int                 mChecked {0};
    bool                m_media_folder_mode {false};
    int                 mTimerID {0};
};

#endif // MAINWINDOW_H
