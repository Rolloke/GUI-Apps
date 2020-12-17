
#ifndef SYSLOGGERWIDGET_H
#define SYSLOGGERWIDGET_H

#include "ui_sysloggerwidget.h"

#include "receiverthread.h"

#include <QWidget>
#include <QSortFilterProxyModel>
#include <QStandardItemModel>
#include <QTimer>

#include <fstream>
#include <cstdint>
#include <vector>
#include <functional>

#include <cstdint>






struct FilterSettings
{
    QString                    mPattern;
    Qt::CaseSensitivity        mCaseSensitive;
    bool                       mHide;
    QRegExp::PatternSyntax     mSyntax;

    FilterSettings()
    : mPattern()
    , mCaseSensitive(Qt::CaseInsensitive)
    , mHide(false)
    , mSyntax(QRegExp::RegExp)
    {}
};

struct HighlighterSettings
{
    QString mPattern;
    Qt::CaseSensitivity  mCaseSensitive;
    bool mInvert;
     

    HighlighterSettings()
    : mPattern()
    , mCaseSensitive(Qt::CaseInsensitive)
    , mInvert(false)
    {}
};





class ItemList : public QAbstractListModel
{
   Q_OBJECT

   QSize            mIconSize;
   QFontMetrics     mFontMetrics;
   QString          mIconText;

public:
   ItemList(QObject *parent = 0);

   int rowCount(const QModelIndex &parent = QModelIndex()) const; 
   QVariant data(const QModelIndex &index, int role) const; 

   QSize getIconSize();


};


class SysLoggerWidget : public QWidget , public Ui::SysLoggerWidget
{
    Q_OBJECT

private:
    QStandardItemModel*                  mModel;
    std::vector<QSortFilterProxyModel*>  mProxyModel;

    QStandardItemModel*                  mFileLoggingModel;
    std::vector<QSortFilterProxyModel*>  mFileLoggingProxyModel;

    QStandardItemModel*                  mFindLoggingModel;
    std::vector<QSortFilterProxyModel*>  mFindLoggingProxyModel;

    std::vector<FilterSettings>          mFilterSettings;

    std::vector<HighlighterSettings>     mHighlighterSettings;



    ReceiverThread*         mReceiverThread;


    std::fstream            mFileStream;

    std::string             mCurrentFileName;
    std::uint32_t           mCurrentFileCounter;
    std::uint32_t           mCounter;

    bool                    mStop;

    bool                    mRestoreFilter;
    bool                    mRestoreHighlighter;

    QTimer*                 mTimer;
    QMenu*                  mContextMenu;




    std::fstream            mPlaybackFileStream;
    std::uint64_t           mPlaybackFileSize;
    std::uint64_t           mPlaybackNumberOfLines;
    std::uint64_t           mPlaybackFilePos;

    struct AutoScrollDirection
    {
        enum Type
        {
            Undefined,
            ToTop,
            ToBottom
        };
    };

    AutoScrollDirection::Type mAutoScrollDirection;

public:
    SysLoggerWidget();
    ~SysLoggerWidget();

    void initSocket();
    void processTheDatagram(QByteArray& datagram);

public Q_SLOTS:
    void readPendingDatagrams();
    void clear_clicked(bool);
    void stop_clicked(bool);
    void filterColumnChanged();
    void filterRegExpChanged();
    void fileLogging_StateChanged(int);
    void protocolChanged(int);
    void portChanged(int);
    void addressChanged(const QString&);
    void popUpContextMenu(const QPoint&);
    void copySelectedLinesToClipboard();
    void selectAll();
    void clearSelection();

    void loadPlaybackFile_clicked(bool);
    void clearPlaybackFile_clicked(bool);

    void updatePlaybackContent(int aValueChange = 0);

    void updatePlaybackStatus();

    void actionTriggerdPlaybackProgressSlider(int);

    void findNextMatchingFilterPattern();

    void highlighterChanged(int);
    void highlighterPatternChanged();

    void usedHighlighterChanged(int);

    void toTopToggled(bool);
    void toBottomToggled(bool);

private:
    void savePersitentData();
    void loadPersistentData();


    void resetHighlighting();

    void updateOneLineCostumHighlighting(QColor&, QColor&, int, std::function<QStandardItem*(int, int)>, bool);
    void updateDefaultHighlighting();
    void updateCostumHighlighting(bool);

};




#endif
