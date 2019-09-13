
#include "sysloggerwidget.h"

#include <QDateTime>
#include <QMouseEvent>
#include <QMenu>
#include <QClipboard>
#include <QList>
#include <QFileDialog>
#include <QMessageBox>
#include <QProgressDialog>
#include <QPainter>
#include <QStyledItemDelegate>

#include <string>
#include <boost/bind.hpp>


const std::string gProjectRevision = "$ProjectRevision: Last Checkpoint: 1.9 $";
const std::string gArticleCode     = "Article Code: 08 146 0040";


namespace
{

    struct SysLogVersion
    {
        enum Type
        {
            RFC3164,
            RFC5424
        };
    };

    SysLogVersion::Type checkSysLogVersion(uint32_t aVersionStrLength)
    {
        SysLogVersion::Type fType = SysLogVersion::RFC5424;
        if(aVersionStrLength == 3)
        {
            fType = SysLogVersion::RFC3164;
        }
        return fType;
    }


    std::vector<QString> createSevertyStringVector()
    {
        std::vector<QString> fSevertyString(8);
        fSevertyString[0] = "Emergency";
        fSevertyString[1] = "Alert";
        fSevertyString[2] = "Critical";
        fSevertyString[3] = "Error";
        fSevertyString[4] = "Warning";
        fSevertyString[5] = "Notice";
        fSevertyString[6] = "Information";
        fSevertyString[7] = "Debug";
        return fSevertyString;
    }


    std::string convertSeverityNumberToString(uint32_t aSevertyNumber)
    {
        switch(aSevertyNumber)
        {
        case 0: return "Emergency"; break;
        case 1: return "Alert"; break;
        case 2: return "Critical"; break;
        case 3: return "Error"; break;
        case 4: return "Warning"; break;
        case 5: return "Notice"; break;
        case 6: return "Information"; break;
        case 7: return "Debug"; break;
        default: return "Unknown"; break;
        }
    }



    QColor convertSeverityNumberToBackgroundColor(uint32_t aSevertyNumber)
    {
        switch(aSevertyNumber)
        {
        case 0: return Qt::darkRed; break;
        case 1: return Qt::magenta; break;
        case 2: return Qt::red; break;
        case 3: return QColor(255, 128, 0); break;
        case 4: return Qt::yellow; break;
        case 5: return Qt::cyan; break;
        case 6: return QColor(210, 210, 210); break;
        case 7: return Qt::green; break;
        default: return Qt::white; break;
        }
    }

    QColor convertSeverityNumberToForegroundColor(uint32_t aSevertyNumber)
    {
        switch(aSevertyNumber)
        {
        case 0: return Qt::white; break;
        case 1: return Qt::white; break;
        case 2: return Qt::white; break;
        case 3: return Qt::black; break;
        case 4: return Qt::black; break;
        case 5: return Qt::black; break;
        case 6: return Qt::black; break;
        case 7: return Qt::black; break;
        default: return Qt::black; break;
        }
    }

    QString convertSeverityNumberToQStyleSheetString(uint32_t aSevertyNumber)
    {
        switch(aSevertyNumber)
        {
        case 0: return QString("color: white; background-color: darkRed;"); break; 
        case 1: return QString("color: white; background-color: magenta;"); break;
        case 2: return QString("color: white; background-color: red;"); break;
        case 3: return QString("color: black; background-color: rgb(255, 128, 0);"); break;
        case 4: return QString("color: black; background-color: yellow;"); break;
        case 5: return QString("color: black; background-color: cyan;"); break;
        case 6: return QString("color: black; background-color: lightGray;"); break;
        case 7: return QString("color: black; background-color: rgb(0, 255, 0);"); break;
        default: return QString("color: black; background-color: lightGray;"); break;
        }
    }



    std::string convertFacilityNumberToString(uint32_t aSevertyNumber)
    {
        switch(aSevertyNumber)
        {
        case 0: return "kern"; break;
        case 1: return "user"; break;
        case 2: return "mail"; break;
        case 3: return "daemon"; break;
        case 4: return "auth"; break;
        case 5: return "syslog"; break;
        case 6: return "lpr"; break;
        case 7: return "news"; break;
        case 8: return "uucp"; break;
        case 9: return "-"; break;
        case 10: return "authpriv"; break;
        case 11: return "ftp"; break;
        case 12: return "-"; break;
        case 13: return "-"; break;
        case 14: return "-"; break;
        case 15: return "cron"; break;
        case 16: return "local0"; break;
        case 17: return "local1"; break;
        case 18: return "local2"; break;
        case 19: return "local3"; break;
        case 20: return "local4"; break;
        case 21: return "local5"; break;
        case 22: return "local6"; break;
        case 23: return "local7"; break;
        default: return "-"; break;
        }
    }



}



ItemList::ItemList(QObject *aParent) 
: QAbstractListModel(aParent) 
, mFontMetrics(QApplication::font())
, mIconText("Pattern Text")
{
    mIconSize.setWidth(mFontMetrics.boundingRect(mIconText).width()+2);
    mIconSize.setHeight(mFontMetrics.boundingRect(mIconText).height() );
}


int ItemList::rowCount(const QModelIndex & /*parent*/) const
{ 
    return 8; 
}


QVariant ItemList::data(const QModelIndex &index, int role) const
{
    if (!index.isValid())
    {
        return QVariant();
    }

    if(role == Qt::DecorationRole)
    {
        QString      fIconText = mIconText;
        QFont        fFont =QApplication::font();
        QFontMetrics fFontMatrix(fFont);

        QPixmap fPixmap(mIconSize.width(), mIconSize.height());
        fPixmap.fill(convertSeverityNumberToBackgroundColor(index.row()));
        QPainter fPainter;
        fPainter.begin(&fPixmap);
        fPainter.setPen(convertSeverityNumberToForegroundColor(index.row()));
        fPainter.drawText(1, mFontMetrics.ascent(), fIconText);
        fPainter.end();
        return fPixmap;
    }

    if (role == Qt::DisplayRole)
    {
        return QString("Highlighter %1").arg(index.row() + 1);
    }
    else
    {
        return QVariant();
    }
}


QSize ItemList::getIconSize()
{
    return mIconSize;
}




class BackgroundDelegate : public QStyledItemDelegate
{
public:
    explicit BackgroundDelegate(QObject *parent = 0)
        : QStyledItemDelegate(parent)
    {
    }

    void paint(QPainter *painter, const QStyleOptionViewItem &option,
               const QModelIndex &index) const
    {
        QString fText = index.model()->data(index).toString();

        QVariant background = index.data(Qt::BackgroundRole);
        QVariant fTextColor = index.data(Qt::TextColorRole);

        QColor fBackgroundColor = background.value<QBrush>().color();


        painter->fillRect(option.rect, fBackgroundColor);
        

        if (option.state & QStyle::State_Selected)
        {
            painter->fillRect(option.rect, QColor(200, 200, 255, 200));
            painter->setPen(QColor(150, 150, 255, 255));

            QRect fRect(option.rect);
            painter->drawLine(fRect.topLeft(), fRect.topRight());
            painter->drawLine(fRect.bottomLeft(), fRect.bottomRight());
        }

        if (option.state & QStyle::State_MouseOver)
        {
            QColor fBackColor(235, 235, 235, 190);
            QColor fLineColor(230, 230, 235, 255);

            if (fBackgroundColor.lightness() >= 255)
            {
                fBackColor = QColor(180, 180, 180, 190);
                fLineColor = QColor(175, 175, 180, 255);
            }

            QRect fRect(option.rect);
            painter->fillRect(fRect, fBackColor);

            painter->setPen(fLineColor);
            painter->drawLine(fRect.topLeft(), fRect.topRight());
            painter->drawLine(fRect.bottomLeft(), fRect.bottomRight());
        }





        //QStyle* fStyle = QApplication::style();
        QRect fTextRect = option.rect;
        fTextRect.setLeft(fTextRect.left() + 2);


        painter->setPen(fTextColor.value<QColor>());

        fText = elidedText(painter->fontMetrics(), fTextRect.width(), Qt::ElideRight, fText);
    painter->drawText(fTextRect, fText, option.displayAlignment);

    }
};



SysLoggerWidget::SysLoggerWidget()
: QWidget()
, mCounter(0)
, mStop(false)
, mRestoreFilter(false)
, mRestoreHighlighter(false)
{
    setupUi(this);

    setMouseTracking(true);

    //setWindowTitle(windowTitle() + "  (" + gProjectRevision.c_str() + " ; " + gArticleCode.c_str() + ")");
    setWindowTitle(windowTitle());


    mFilterSettings.resize(10);
    mHighlighterSettings.resize(8);


    mSettingsFrame->setMaximumSize(QSize(16777215, fontMetrics().height()*9));
    mPlaybackWidget->setMaximumSize(QSize(16777215, fontMetrics().height()* 6));


    ItemList* fItemList = new ItemList;
    mHighlighterComboBox->setModel(fItemList);
    mHighlighterComboBox->setIconSize(fItemList->getIconSize());
    //mHighlighterComboBox->setStyleSheet(convertSeverityNumberToQStyleSheetString(0));



    mFileLoggingModel = new QStandardItemModel(0, 10, this);
    mFileLoggingProxyModel.resize(10);
    for(int32_t i=0; i < 10; ++i)
    {
        mFileLoggingProxyModel[i] = new QSortFilterProxyModel;
        mFileLoggingProxyModel[i]->setFilterKeyColumn(i-1);
    }
    mFileLoggingProxyModel[0]->setSourceModel(mFileLoggingModel);  
    for(uint32_t i=0; i < 9; ++i)
    {
        mFileLoggingProxyModel[i+1]->setSourceModel(mFileLoggingProxyModel[i]);
    }  


    mFindLoggingModel = new QStandardItemModel(0, 10, this);
    mFindLoggingProxyModel.resize(10);
    for(int32_t i=0; i < 10; ++i)
    {
        mFindLoggingProxyModel[i] = new QSortFilterProxyModel;
        mFindLoggingProxyModel[i]->setFilterKeyColumn(i-1);
    }
    mFindLoggingProxyModel[0]->setSourceModel(mFindLoggingModel);  
    for(uint32_t i=0; i < 9; ++i)
    {
        mFindLoggingProxyModel[i+1]->setSourceModel(mFindLoggingProxyModel[i]);
    }  


    mModel = new QStandardItemModel(0, 9, this);
    mModel->setHeaderData(0, Qt::Horizontal, QObject::tr("Counter"));
    mModel->setHeaderData(1, Qt::Horizontal, QObject::tr("Date/Time UTC"));
    mModel->setHeaderData(2, Qt::Horizontal, QObject::tr("Facility"));
    mModel->setHeaderData(3, Qt::Horizontal, QObject::tr("Severity"));
    mModel->setHeaderData(4, Qt::Horizontal, QObject::tr("Host Name"));
    mModel->setHeaderData(5, Qt::Horizontal, QObject::tr("Message-ID"));
    mModel->setHeaderData(6, Qt::Horizontal, QObject::tr("Process-ID"));
    mModel->setHeaderData(7, Qt::Horizontal, QObject::tr("App-Name"));
    mModel->setHeaderData(8, Qt::Horizontal, QObject::tr("Message"));


    mProxyModel.resize(10);
    for(uint32_t i=0; i < 10; ++i)
    {
        mProxyModel[i] = new QSortFilterProxyModel;
        mProxyModel[i]->setFilterKeyColumn(i-1);
    }

    mProxyModel[0]->setSourceModel(mModel);  
    for(uint32_t i=0; i < 9; ++i)
    {
        mProxyModel[i+1]->setSourceModel(mProxyModel[i]);
    }  
    mLogTableView->setModel(mProxyModel[9]);

    mLogTableView->clearSelection();
    mLogTableView->clearFocus();
    mLogTableView->setFocusPolicy(Qt::NoFocus);
    mLogTableView->setContextMenuPolicy(Qt::DefaultContextMenu);
    mLogTableView->setSelectionMode(QAbstractItemView::NoSelection);
    mLogTableView->setEditTriggers(QAbstractItemView::NoEditTriggers);

    mLogTableView->setItemDelegate(new BackgroundDelegate(this));

    //mLogTableView->setStyleSheet("selection-background-color: rgba(80, 80, 80, 180);");
    //mLogTableView->setStyleSheet("selection-background-color: transparent;");


    QHeaderView *verticalHeader = mLogTableView->verticalHeader();
    //verticalHeader->setSectionResizeMode(QHeaderView::Fixed);
    verticalHeader->setDefaultSectionSize(fontMetrics().height() +4 );


    mLogTableView->horizontalHeader()->setDefaultAlignment(Qt::AlignLeft);
    mLogTableView->horizontalHeader()->setFixedHeight(fontMetrics().height()*1.9);

    mLogTableView->setColumnWidth(0, 65);
    mLogTableView->setColumnWidth(1, 165);
    mLogTableView->setColumnWidth(2, 80);
    mLogTableView->setColumnWidth(3, 80);
    mLogTableView->setColumnWidth(4, 80);
    mLogTableView->setColumnWidth(5, 80);
    mLogTableView->setColumnWidth(6, 80);
    mLogTableView->setColumnWidth(7, 80);
    mLogTableView->setColumnWidth(8, 850);

    mLogTableView->setRowHeight(0, 200);

    mLogTableView->sortByColumn(0);

    loadPersistentData();


    mReceiverThread = new ReceiverThread(this);

    mReceiverThread->start();

    initSocket();

    mTimer->start();

    QList<int> fSplitterSizes = mSplitter->sizes();
    fSplitterSizes[0] = 100;
    fSplitterSizes[1] = 100;
    fSplitterSizes[2] = 0;
    mSplitter->setSizes(fSplitterSizes);

}

SysLoggerWidget::~SysLoggerWidget()
{
    if(mFileStream.is_open())
    {
        mFileStream.close();
    }

    savePersitentData();

    mReceiverThread->stop();
    mReceiverThread->wait(1000);
    mReceiverThread->terminate();
}


void SysLoggerWidget::initSocket()
{
    mTimer = new QTimer(this);
    mTimer->setInterval(50);
    connect(mTimer, SIGNAL(timeout()), this, SLOT(readPendingDatagrams()));

    mContextMenu = new QMenu(this);


    QAction* fCopyAction = mContextMenu->addAction("&Copy");
        connect(fCopyAction, SIGNAL(triggered(bool)), this, SLOT(copySelectedLinesToClipboard()));
    mContextMenu->addSeparator();
    QAction* fSelectAllAction = mContextMenu->addAction("Select &All");
        connect(fSelectAllAction, SIGNAL(triggered(bool)), this, SLOT(selectAll()));

    QAction* fClearSelectionAction = mContextMenu->addAction("Clea&r Selection");
        connect(fClearSelectionAction, SIGNAL(triggered(bool)), this, SLOT(clearSelection()));

        


    connect(mFindPushButton, SIGNAL(clicked(bool)), this, SLOT(findNextMatchingFilterPattern()));

    connect(mClearPushButton, SIGNAL(clicked(bool)), this, SLOT(clear_clicked(bool)));

    connect(mStopPushButton, SIGNAL(clicked(bool)), this, SLOT(stop_clicked(bool)));

    connect(mFilterColumnComboBox, SIGNAL(currentIndexChanged(int)),
            this, SLOT(filterColumnChanged()));

    connect(mFilterPatternComboBox, SIGNAL(editTextChanged(QString)),
            this, SLOT(filterRegExpChanged()));

    connect(mCaseSensitiveFilterCheckBox, SIGNAL(toggled(bool)),
            this, SLOT(filterRegExpChanged()));

    connect(mHideCheckBox, SIGNAL(toggled(bool)),
            this, SLOT(filterRegExpChanged()));

    connect(mFilterSyntaxComboBox, SIGNAL(currentIndexChanged(int)),
            this, SLOT(filterRegExpChanged()));

    connect(mFileLoggingIsActiveCheckBox, SIGNAL(stateChanged(int)),
            this, SLOT(fileLogging_StateChanged(int)));

    connect(mUDPPortSpinBox, SIGNAL(valueChanged(int)),
            this, SLOT(UDPPortChanged(int)));


    connect(mLogTableView, SIGNAL(customContextMenuRequested(const QPoint&)), 
            this, SLOT(popUpContextMenu(const QPoint&)));

    
    connect(mToTopToolButton, SIGNAL(toggled(bool)), 
            this, SLOT(toTopToggled(bool)));

    connect(mToBottomToolButton, SIGNAL(toggled(bool)), 
            this, SLOT(toBottomToggled(bool)));




    



    connect(mLoadPlaybackFilePushButton, SIGNAL(clicked(bool)), this, SLOT(loadPlaybackFile_clicked(bool)));

    connect(mPlaybackClearPushButton, SIGNAL(clicked(bool)), this, SLOT(clearPlaybackFile_clicked(bool)));

    connect(mPlaybackProgressSlider, SIGNAL(sliderReleased()), this, SLOT(updatePlaybackContent()));

    connect(mPlaybackProgressSlider, SIGNAL(valueChanged (int)), this, SLOT(updatePlaybackStatus()));

    connect(mPlaybackProgressSlider, SIGNAL(actionTriggered (int)), this, SLOT(actionTriggerdPlaybackProgressSlider(int)));




    connect(mHighlighterComboBox, SIGNAL(currentIndexChanged(int)),
            this, SLOT(highlighterChanged(int)));

    connect(mHighlighterLineEdit, SIGNAL(textChanged(QString)),
            this, SLOT(highlighterPatternChanged()));

    connect(mInvertHighlighterCheckBox, SIGNAL(toggled(bool)),
            this, SLOT(highlighterPatternChanged()));

    connect(mCaseSensitveHighlighterCheckBox, SIGNAL(toggled(bool)),
            this, SLOT(highlighterPatternChanged()));

    connect(mCustomcheckBox, SIGNAL(toggled(bool)),
            this, SLOT(customHiglightingFlagChanged()));


}





void SysLoggerWidget::readPendingDatagrams()
{

    QByteArray fDatagram;


    bool fCanRead = true;
    for(uint32_t i=0; (i < 50)&&(fCanRead); ++i)
    {
        fCanRead = mReceiverThread->mDatagramQueue.pull(fDatagram, 10);
        if(fCanRead)
        {
            processTheDatagram(fDatagram);
        }
    }
}

void SysLoggerWidget::clear_clicked(bool)
{
    mCounter = 0;
    mModel->removeRows(0, mModel->rowCount());
}


void SysLoggerWidget::stop_clicked(bool aFlag)
{
    if(aFlag)
    {
        mLogTableView->setFocusPolicy(Qt::WheelFocus);
        mLogTableView->setSelectionMode(QAbstractItemView::MultiSelection/*QAbstractItemView::SingleSelection*/);
        mLogTableView->setContextMenuPolicy(Qt::CustomContextMenu);
        mLogTableView->setEditTriggers(QAbstractItemView::DoubleClicked | QAbstractItemView::EditKeyPressed);
        
    }
    else
    {
        mLogTableView->clearSelection();
        mLogTableView->clearFocus();
        mLogTableView->setFocusPolicy(Qt::NoFocus);
        mLogTableView->setContextMenuPolicy(Qt::DefaultContextMenu);
        mLogTableView->setSelectionMode(QAbstractItemView::NoSelection);
        mLogTableView->setEditTriggers(QAbstractItemView::NoEditTriggers);
    }

    mStop = aFlag;

    mLogTableView->setMouseTracking(mStop);
}

void SysLoggerWidget::filterColumnChanged()
{
    mRestoreFilter = true;
    int32_t fIndex = mFilterColumnComboBox->currentIndex();
    mHideCheckBox->setEnabled(fIndex > 0);
    mFilterPatternComboBox->setEditText(mFilterSettings[fIndex].mPattern);
    mFilterSyntaxComboBox->setCurrentIndex(mFilterSettings[fIndex].mSyntax);
    mCaseSensitiveFilterCheckBox->setChecked(mFilterSettings[fIndex].mCaseSensitive == Qt::CaseSensitive );
    mHideCheckBox->setChecked(mFilterSettings[fIndex].mHide);
    mRestoreFilter = false;
}

void SysLoggerWidget::filterRegExpChanged()
{
    if(!mRestoreFilter)
    {
        QRegExp::PatternSyntax syntax(static_cast<QRegExp::PatternSyntax>(mFilterSyntaxComboBox->currentIndex()));
        Qt::CaseSensitivity fCaseSensitivity =
                mCaseSensitiveFilterCheckBox->isChecked() ? Qt::CaseSensitive
                                                           : Qt::CaseInsensitive;

        bool fHide = mHideCheckBox->isChecked();

        QString fPattern = mFilterPatternComboBox->currentText();
        QRegExp fRegExp(fPattern, fCaseSensitivity, syntax);
        int32_t fIndex = mFilterColumnComboBox->currentIndex();
        mFilterSettings[fIndex].mPattern = fPattern;
        mFilterSettings[fIndex].mCaseSensitive = fCaseSensitivity;
        mFilterSettings[fIndex].mHide = fHide;
        mFilterSettings[fIndex].mSyntax =syntax;

        if(fIndex > 0)
        {
            if(fHide)
            {
                  mLogTableView->hideColumn(fIndex-1);
            }
            else
            {
                  mLogTableView->showColumn(fIndex-1);
            }
        }

        mProxyModel[fIndex]->setFilterRegExp(fRegExp);
        mFileLoggingProxyModel[fIndex]->setFilterRegExp(fRegExp);
        mFindLoggingProxyModel[fIndex]->setFilterRegExp(fRegExp);


        if(fIndex>0)
        {
            if(!fPattern.isEmpty())
            {
                QFont fFont;
                fFont.setBold(true);
                fFont.setUnderline(true);
                mModel->setHeaderData(fIndex-1, Qt::Horizontal, fFont, Qt::FontRole);      
                mModel->setHeaderData(fIndex-1, Qt::Horizontal, QBrush(Qt::red), Qt::ForegroundRole);
            }
            else
            {
                QFont fFont;
                fFont.setBold(!mFilterSettings[0].mPattern.isEmpty());
                fFont.setUnderline(false);
                mModel->setHeaderData(fIndex-1, Qt::Horizontal, fFont, Qt::FontRole);
                mModel->setHeaderData(fIndex-1, Qt::Horizontal, 
                    (!mFilterSettings[0].mPattern.isEmpty()) ? QBrush(Qt::red) : QBrush(Qt::black), Qt::ForegroundRole);
            }
        }
        else
        {
            for(uint32_t i=1; i < 10; ++i)
            {
                QFont fFont;
                if(!mFilterSettings[0].mPattern.isEmpty())
                {
                    fFont.setBold(true);
                }
                else
                {
                    fFont.setBold(!mFilterSettings[i].mPattern.isEmpty());
                }
                fFont.setUnderline(!mFilterSettings[i].mPattern.isEmpty());
                mModel->setHeaderData(i-1, Qt::Horizontal, fFont, Qt::FontRole);   
                if(!mFilterSettings[0].mPattern.isEmpty())
                {
                    mModel->setHeaderData(i-1, Qt::Horizontal, QBrush(Qt::red), Qt::ForegroundRole);
                }
                else
                {
                    mModel->setHeaderData(i-1, Qt::Horizontal, 
                        (!mFilterSettings[i].mPattern.isEmpty()) ? QBrush(Qt::red) : QBrush(Qt::black), Qt::ForegroundRole);
                }
            }
        }
    }
}


void SysLoggerWidget::fileLogging_StateChanged(int aState)
{
    if(aState == Qt::Checked)
    {
        QDateTime fCurrentDateTime = QDateTime::currentDateTimeUtc();


        QString fFileName = "messages_";
        fFileName += fCurrentDateTime.toString("yyyyMMddThhmmss.zzzZ");
      
        mCurrentFileName = fFileName.toStdString();
      
        mCurrentFileCounter = 0;
        std::string fFile = mCurrentFileName + QString("_%1.log").arg(mCurrentFileCounter, 4, 10, QLatin1Char('0')).toStdString();
        mFileStream.open(fFile.c_str(),
                         std::ios::binary | std::ios::in | std::ios::out | std::ios::trunc);  
    }
    else
    {
        mFileStream.close();
    }

}

void SysLoggerWidget::UDPPortChanged(int aUDPPortNumber)
{
    mReceiverThread->changeUDPPort(aUDPPortNumber);
}


void SysLoggerWidget::popUpContextMenu(const QPoint& aPos)
{
    mContextMenu->popup(mLogTableView->mapToGlobal(aPos));
}


void SysLoggerWidget::copySelectedLinesToClipboard()
{
    QClipboard* fClipboard = QApplication::clipboard();

    QModelIndexList fList = mLogTableView->selectionModel()->selectedIndexes();
   
    QString fClipboardText;

    for(int i=0; i < fList.size(); ++i)
    {
        QModelIndex& fModelIndex = fList[i];

        QStringList fStringList = fModelIndex.data().toStringList();
        for(int j=0; j < fStringList.size(); ++j)
        {
            fClipboardText += fStringList[j] + " "; 
        }
        if(!((i+1) % 9))
        {
            fClipboardText += "\n";
        }
    }

    fClipboard->setText(fClipboardText);
}

void SysLoggerWidget::selectAll()
{
    mLogTableView->selectAll();
}

void SysLoggerWidget::clearSelection()
{
    mLogTableView->clearSelection();
}



namespace
{
    void parseSyslogLine(
        const QString& aText,
        uint32_t& aSeverityNumber,
        uint32_t& aFacilityNumber,
        QString&       aDateTimeString,
        QString&       aHostNameString,
        QString&       aAppNameString,
        QString&       aProcessIDString,
        QString&       aMessageIDString,
        QString&       aStructuredDataString,
        QString&       aMessageString
        )
    {
    std::vector<uint32_t> fStartOfSysLogPart;
    fStartOfSysLogPart.push_back(0);
    uint32_t fIndex = 0;
    while((fIndex = aText.indexOf(" ", fIndex+1)) > 0 && (fStartOfSysLogPart.size()) < 8)
    {
        fStartOfSysLogPart.push_back(fIndex+1);
    }


    QString fFacilityPlusSeverityString = aText.mid(fStartOfSysLogPart[0]+1, (aText.indexOf(">")-1));
    int32_t fMsgStart = aText.indexOf(">")+1;
    int32_t fVersionStringLength = fStartOfSysLogPart[1]-fMsgStart-1;
    QString fVersionString = aText.mid(fMsgStart, fStartOfSysLogPart[1]-fStartOfSysLogPart[0]);

    uint32_t fFacilityPlusSeverityNumber =  fFacilityPlusSeverityString.toUInt();
    aSeverityNumber = fFacilityPlusSeverityNumber % 8;
    aFacilityNumber = fFacilityPlusSeverityNumber / 8;

    if(fVersionStringLength == 1)
    {
        aDateTimeString = aText.mid(fStartOfSysLogPart[1], fStartOfSysLogPart[2]-fStartOfSysLogPart[1]);
        aHostNameString = aText.mid(fStartOfSysLogPart[2], fStartOfSysLogPart[3]-fStartOfSysLogPart[2]);
        aAppNameString = aText.mid(fStartOfSysLogPart[3], fStartOfSysLogPart[4]-fStartOfSysLogPart[3]);
        aProcessIDString = aText.mid(fStartOfSysLogPart[4], fStartOfSysLogPart[5]-fStartOfSysLogPart[4]);
        aMessageIDString = aText.mid(fStartOfSysLogPart[5], fStartOfSysLogPart[6]-fStartOfSysLogPart[5]);

        if(fStartOfSysLogPart.size() > 7)
        {
            aStructuredDataString = aText.mid(fStartOfSysLogPart[6], fStartOfSysLogPart[7]-fStartOfSysLogPart[6]);

            int32_t fLineBreakIndex = aText.indexOf("\n", fStartOfSysLogPart[7]);
            if(fLineBreakIndex >=0)
            {
                aMessageString = aText.mid(fStartOfSysLogPart[7], fLineBreakIndex-fStartOfSysLogPart[7]);
            }
            else
            {
                aMessageString = aText.mid(fStartOfSysLogPart[7], aText.size()-1);
            }
        }
        else
        {
            aStructuredDataString = aText.mid(fStartOfSysLogPart[6], aText.size()-1);
        }
    } 
    else
    {
        uint32_t fOffset = 0;
        if(aText.at(fMsgStart).isSpace())
        {
            ++fMsgStart; 
            fOffset +=2;
        }
        if(aText.at(fStartOfSysLogPart[1]).isSpace())
        {
            ++fOffset;
        }

        if(fStartOfSysLogPart[4+fOffset]-fStartOfSysLogPart[3+fOffset]-1 == 4)
        {    
            QString fYear = aText.mid(fStartOfSysLogPart[3+fOffset], 4);
            bool fOk;
            fYear.toULong(&fOk);
            if(fOk)
            {
                ++fOffset;
            }
        }


        aDateTimeString = aText.mid(fMsgStart, fStartOfSysLogPart[3+fOffset]-fMsgStart);
        aHostNameString = aText.mid(fStartOfSysLogPart[3+fOffset], fStartOfSysLogPart[4+fOffset]-fStartOfSysLogPart[3+fOffset]);

        int32_t fLineBreakIndex = aText.indexOf("\n", fStartOfSysLogPart[4+fOffset]);
        if(fLineBreakIndex >=0)
        {
            aMessageString = aText.mid(fStartOfSysLogPart[4+fOffset], fLineBreakIndex-fStartOfSysLogPart[4+fOffset]);
        }
        else
        {
            aMessageString = aText.mid(fStartOfSysLogPart[4+fOffset], aText.size()-1);
        }

        
        //Optional Tag field
        int32_t fTagMessageLimiter = 0;
        if((fTagMessageLimiter = aMessageString.indexOf(":", 1)) > 0)
        {
            QString fTagString = aMessageString.mid(0, fTagMessageLimiter);
            
            int32_t fStartPID = fTagString.indexOf("[", 1);
            int32_t fStopPID = fTagString.indexOf("]", 1);
            if(fStartPID > 0 && fStopPID > 0)
            {
                aAppNameString = fTagString.mid(0, fStartPID);
                aProcessIDString = fTagString.mid(fStartPID+1, fStopPID-fStartPID-1);    
            }
            else
            {
                aAppNameString = fTagString;
            }
            
        

            if(fLineBreakIndex >=0)
            {
                aMessageString = aMessageString.mid(fTagMessageLimiter+1, fLineBreakIndex-(fTagMessageLimiter+1));
            }
            else
            {
                aMessageString = aMessageString.mid(fTagMessageLimiter+1, aMessageString.size()-1);
            }


        }
    }


    }
}

void SysLoggerWidget::processTheDatagram(QByteArray& aDatagram)
{
    QString fText = aDatagram.data();


    uint32_t fSeverityNumber;
    uint32_t fFacilityNumber;
    QString fDateTimeString;
    QString fHostNameString;
    QString fAppNameString;
    QString fProcessIDString;
    QString fMessageIDString;
    QString fStructuredDataString;
    QString fMessageString;

    parseSyslogLine(fText, fSeverityNumber, fFacilityNumber, 
        fDateTimeString, fHostNameString, fAppNameString, fProcessIDString, fMessageIDString, fStructuredDataString, fMessageString);



    QList<QStandardItem*> fItemList;


    fItemList.append(new QStandardItem(QString("%1").arg(mCounter, 9, 10, QLatin1Char('0'))));   //Counter
    fItemList.append(new QStandardItem(fDateTimeString));   //Date/Time
    fItemList.append(new QStandardItem( convertFacilityNumberToString(fFacilityNumber).c_str() )); //Facility
    fItemList.append(new QStandardItem( convertSeverityNumberToString(fSeverityNumber).c_str())); //Severity
    fItemList.append(new QStandardItem(fHostNameString)); //Host Name
    fItemList.append(new QStandardItem(fMessageIDString)); //Message-ID
    fItemList.append(new QStandardItem(fProcessIDString)); //Process-ID
    fItemList.append(new QStandardItem(fAppNameString)); //App-Name
    fItemList.append(new QStandardItem(fMessageString)); //Message

    QColor fBColor;
    QColor fFColor;

    if(mCustomcheckBox->isChecked())
    {
        fBColor = convertSeverityNumberToBackgroundColor(9); //Use default layout 
        fFColor = convertSeverityNumberToForegroundColor(9); //Use default layout 
        
        updateOneLineCostumHighlighting(fBColor, fFColor, 0,
            [fItemList](int /*aRow*/, int aCol){ return fItemList[aCol]; } );
    }
    else
    {
        fBColor = convertSeverityNumberToBackgroundColor(fSeverityNumber);
        fFColor = convertSeverityNumberToForegroundColor(fSeverityNumber);
    }

    for(int32_t i=0; i < fItemList.size(); ++i)
    {
        fItemList[i]->setBackground(QBrush(fBColor));
        fItemList[i]->setForeground(QBrush(fFColor));
    }


    if(!mStop)
    {        
        mModel->appendRow(fItemList);
        ++mCounter;
        if(mModel->rowCount() > mOutputLengthSpinBox->value())
        {
            mModel->removeRow(0);
        }
 
        if(mToTopToolButton->isChecked())
        {
            mLogTableView->scrollToTop();
        }
        if(mToBottomToolButton->isChecked())
        {
            mLogTableView->scrollToBottom();
        }
        
    }

    if(mFileLoggingIsActiveCheckBox->checkState() == Qt::Checked)
    {
        bool fFilterFlag = (mFileLoggingUseFilterCheckBox->checkState() == Qt::Checked);

        if(fFilterFlag)
        {
            QList<QStandardItem*> fFileItemList;

            fFileItemList.append(new QStandardItem(QString("%1").arg(mCounter, 9, 10, QLatin1Char('0'))));   //Counter
            fFileItemList.append(new QStandardItem(fDateTimeString));   //Date/Time
            fFileItemList.append(new QStandardItem( convertFacilityNumberToString(fFacilityNumber).c_str() )); //Facility
            fFileItemList.append(new QStandardItem( convertSeverityNumberToString(fSeverityNumber).c_str())); //Severity
            fFileItemList.append(new QStandardItem(fHostNameString)); //Host Name
            fFileItemList.append(new QStandardItem(fMessageIDString)); //Message-ID
            fFileItemList.append(new QStandardItem(fProcessIDString)); //Process-ID
            fFileItemList.append(new QStandardItem(fAppNameString)); //App-Name
            fFileItemList.append(new QStandardItem(fMessageString)); //Message

            mFileLoggingModel->removeRow(0);
            mFileLoggingModel->appendRow(fFileItemList);  

              
        }

        if((!fFilterFlag) || (mFileLoggingProxyModel[mFileLoggingProxyModel.size()-1]->hasChildren()))
        {
            uint32_t fCurrentPos = mFileStream.tellg();
            mFileStream.seekg (0, mFileStream.end);
            uint32_t fLength = mFileStream.tellg();
            mFileStream.seekg (fCurrentPos, mFileStream.beg);

            static const uint32_t fMaxSizeOfFile = 100*1024*1024;
            if(fLength > fMaxSizeOfFile)
            {
                mFileStream.close();
                ++mCurrentFileCounter;
                mFileStream.open((mCurrentFileName + QString("_%1.log").arg(mCurrentFileCounter, 4, 10, QLatin1Char('0')).toStdString()).c_str(),
                        std::ios::binary | std::ios::in | std::ios::out | std::ios::trunc);  
            }

            if(fText.indexOf('\n', fText.size()-2) < 0)
            {
                mFileStream << fText.toStdString() << std::endl;
            }
            else
            {
                mFileStream << fText.toStdString();
            }
            mFileStream.flush();
        }
    }
}



void SysLoggerWidget::savePersitentData()
{
    std::fstream fFileStream;
    fFileStream.open("persistent.dat", std::ios::binary | std::ios::out );
    if(fFileStream.is_open())
    {
        auto fIter= mFilterSettings.begin();
        for(;fIter != mFilterSettings.end();++fIter)
        {
            uint32_t fPatternSize = (*fIter).mPattern.length()+1;
            fFileStream.write(reinterpret_cast<char*>(&fPatternSize), sizeof(uint32_t));
            fFileStream.write((*fIter).mPattern.toStdString().c_str(), fPatternSize);
            fFileStream.write(reinterpret_cast<char*>(&(*fIter).mCaseSensitive), sizeof(uint32_t));
            fFileStream.write(reinterpret_cast<char*>(&(*fIter).mHide), sizeof(uint32_t));
            fFileStream.write(reinterpret_cast<char*>(&(*fIter).mSyntax), sizeof(uint32_t));
        }

        fFileStream.close();
    }


}

void SysLoggerWidget::loadPersistentData()
{
    std::fstream fFileStream;
    fFileStream.open("persistent.dat", std::ios::binary | std::ios::in );
    if(fFileStream.is_open())
    {
        auto fIter= mFilterSettings.begin();
        for(;fIter != mFilterSettings.end(); ++fIter)
        {
            uint32_t fPatternSize = 0;
            fFileStream.read(reinterpret_cast<char*>(&fPatternSize), sizeof(uint32_t));
            std::vector<char> fBuffer(fPatternSize);
            fFileStream.read(reinterpret_cast<char*>(fBuffer.data()), fPatternSize);
            (*fIter).mPattern = &fBuffer[0];
            fFileStream.read((char*)(&(*fIter).mCaseSensitive), sizeof(uint32_t));
            fFileStream.read((char*)(&(*fIter).mHide), sizeof(uint32_t));
            fFileStream.read((char*)(&(*fIter).mSyntax), sizeof(uint32_t));
        }
        fFileStream.close();
    }


    for(uint32_t i=0; i< 10; ++i)
    {
        const FilterSettings fSettings = mFilterSettings[i];
        bool fHide = fSettings.mHide;
        QString fPattern = fSettings.mPattern;
        
        if(i>0)
        {
            if(fHide)
            {
                mLogTableView->hideColumn(i-1);
            }
            else
            {
                mLogTableView->showColumn(i-1);
            }
        }
        QRegExp fRegExp(fSettings.mPattern, fSettings.mCaseSensitive, fSettings.mSyntax);
        mProxyModel[i]->setFilterRegExp(fRegExp);
        mFileLoggingProxyModel[i]->setFilterRegExp(fRegExp);
        mFindLoggingProxyModel[i]->setFilterRegExp(fRegExp);

        if(fPattern.length())
        {
            QFont fFont;
            fFont.setBold(true);
            fFont.setUnderline(true);
            mModel->setHeaderData(i, Qt::Horizontal, fFont, Qt::FontRole);
            mModel->setHeaderData(i, Qt::Horizontal, QBrush(Qt::red), Qt::ForegroundRole);
        }
        else
        {
            QFont fFont;
            fFont.setBold(false);
            fFont.setUnderline(false);
            mModel->setHeaderData(i, Qt::Horizontal, fFont, Qt::FontRole);
            mModel->setHeaderData(i, Qt::Horizontal, QBrush(Qt::black), Qt::ForegroundRole);
        }

    }

    filterColumnChanged();
    filterRegExpChanged();

}



void SysLoggerWidget::loadPlaybackFile_clicked(bool)
{
    QString fFileName = QFileDialog::getOpenFileName(this, tr("Open File"), "", tr("Files (*.*)"));

    if(!fFileName.isEmpty())
    {

        if(mPlaybackFileStream.is_open())
        {
            mPlaybackFileStream.close();
        }

        mPlaybackFileStream.open(fFileName.toStdString().c_str(), std::ios::binary | std::ios::in );

        if(mPlaybackFileStream.is_open())
        {
            mPlaybackFileNameLineEdit->setText(fFileName);


            /*
            mPlaybackFileStream.unsetf(std::ios_base::skipws);

            
            mPlaybackNumberOfLines = std::count(
                std::istream_iterator<char>(mPlaybackFileStream),
                std::istream_iterator<char>(), 
                '\n');
            */



            int64_t fCurrentPos = mPlaybackFileStream.tellg();
            mPlaybackFileStream.seekg (0, mPlaybackFileStream.end);
            mPlaybackFileSize = mPlaybackFileStream.tellg();
            mPlaybackFileStream.seekg (fCurrentPos, mFileStream.beg);
   


            mPlaybackNumberOfLines = 0;           
            
            size_t fLength = mPlaybackFileSize;
            static const size_t fBufferSize = 4096;
            char fBuffer[fBufferSize];

            while(!mPlaybackFileStream.eof() && fLength > 0)
            {
                size_t fReadBufferSize = fBufferSize;
                if(fLength < fBufferSize)
                {
                    fReadBufferSize = fLength;
                }
                mPlaybackFileStream.read(&fBuffer[0], fReadBufferSize);
                
                fLength -= fReadBufferSize;
                for(uint64_t i=0; i < fReadBufferSize; ++i)
                {
                    if(fBuffer[i] == '\n')
                    {
                        ++mPlaybackNumberOfLines;
                    }    
                }
            }          
            


            mPlaybackProgressLabel->setText(QString("[0[0;%1]%1]").arg(mPlaybackNumberOfLines-1));

            mPlaybackProgressSlider->setRange(0, mPlaybackNumberOfLines-1);

            updatePlaybackContent();
        }
    }   
}


void SysLoggerWidget::clearPlaybackFile_clicked(bool)
{
    mPlaybackProgressSlider->setRange(0, 99);
    mPlaybackProgressSlider->setValue(0);
    mPlaybackFileNameLineEdit->setText("");
    mPlaybackProgressLabel->setText("[0[0;0]0]");
    mPlaybackFileStream.close();

    mCounter = 0;
    mModel->removeRows(0, mModel->rowCount());
}


void SysLoggerWidget::updatePlaybackContent(int aValueChange)
{
    if(mPlaybackFileStream.is_open())
    {
        mModel->removeRows(0, mModel->rowCount());

        mPlaybackFileStream.clear();
        mPlaybackFileStream.seekg (0, mPlaybackFileStream.beg);


        
        std::string fTmpLine;
   

        // Find the Correct Start Line
        int32_t fLineCounter = 0;
        int32_t fCurrentLine = mPlaybackProgressSlider->value()+aValueChange;

        size_t fLength = mPlaybackFileSize;
        static const int32_t fBufferSize = 4096;
        static const int32_t fMaxLineLength = 512;
        char fBuffer[fBufferSize];

        while(mPlaybackFileStream.good() && fLength > 0 && (fLineCounter<fCurrentLine))
        {
            size_t fReadBufferSize = fBufferSize;
            if(fLineCounter<fCurrentLine-fBufferSize)
            {
                if(fLineCounter< (fCurrentLine-fMaxLineLength))
                {
                    fReadBufferSize = fMaxLineLength;
                }

                mPlaybackFileStream.read(&fBuffer[0], fReadBufferSize);             
                fLength -= fReadBufferSize;
                for(uint64_t i=0; i < fReadBufferSize; ++i)
                {
                    if(fBuffer[i] == '\n')
                    {
                        ++fLineCounter;
                    }    
                }
            }
            else
            {
                std::getline(mPlaybackFileStream, fTmpLine);
                ++fLineCounter; 
            }
        }
        

        //Read data 
        /*
        uint32_t fNumberOfReadLines = mOutputLengthSpinBox->value();
        while((mPlaybackFileStream.good()) && (fNumberOfReadLines > 0))
        {
            std::getline(mPlaybackFileStream, fTmpLine);
            if(mPlaybackFileStream.good())
            {
                mCounter = fLineCounter++;
                processTheDatagram(QByteArray(fTmpLine.c_str(), fTmpLine.length()));
                --fNumberOfReadLines;
            }
        }  
        */
        

        
        fLength = mPlaybackFileSize-mPlaybackFileStream.tellg();
        size_t fFileJumpBack = 0;
        uint32_t fNumberOfReadLines = mOutputLengthSpinBox->value();
        while((mPlaybackFileStream.good()) && (fLength > 0) && (fNumberOfReadLines > 0))
        {
            size_t fFilePos = mPlaybackFileStream.tellg(); 
            mPlaybackFileStream.seekg (fFilePos-fFileJumpBack, mPlaybackFileStream.beg);
            int32_t fReadBufferSize = fBufferSize;
            if(fLength < fBufferSize)
            {
                fReadBufferSize = fLength;
            }
            mPlaybackFileStream.read(&fBuffer[0], fReadBufferSize);
            fLength -= fReadBufferSize;
            size_t fStartIndex = 0;
            size_t fEndIndex = 0;
            for(int64_t i=0; (i < fReadBufferSize) && (fNumberOfReadLines > 0); ++i)
            {
                if(fBuffer[i] == '\n')
                {
                    fEndIndex = i;
                    mCounter = fLineCounter++;
                    QByteArray fBA(&fBuffer[fStartIndex], fEndIndex-fStartIndex+1);
                    processTheDatagram(fBA);
                    fStartIndex = fEndIndex+1;
                    --fNumberOfReadLines;
                }    
            }
            fFileJumpBack = fBufferSize - fStartIndex;
            if(fLength > 0)
            {
                fLength += fFileJumpBack;
            }
        }
        
    }

    updatePlaybackStatus();
}

void SysLoggerWidget::updatePlaybackStatus()
{
    if(mPlaybackFileStream.is_open())
    {
        uint32_t fLineCounter = mPlaybackProgressSlider->value();

        uint32_t fStartLine = fLineCounter;
        uint32_t fEndLine = fLineCounter+(mOutputLengthSpinBox->value()-1);
        if(fEndLine > mPlaybackNumberOfLines)
        {
            fEndLine = mPlaybackNumberOfLines-1;
        }
        mPlaybackProgressLabel->setText(QString("[0[%1;%2]%3]").arg(fStartLine).arg(fEndLine).arg(mPlaybackNumberOfLines-1));
    }
}


void SysLoggerWidget::actionTriggerdPlaybackProgressSlider(int aAction)
{
    if( aAction == QAbstractSlider::SliderSingleStepAdd)
    {
        updatePlaybackContent(mPlaybackProgressSlider->singleStep());
    }
    if( aAction == QAbstractSlider::SliderSingleStepSub)
    {
        updatePlaybackContent(-mPlaybackProgressSlider->singleStep());
    }
    if( aAction == QAbstractSlider::SliderPageStepAdd)
    {
        updatePlaybackContent(mPlaybackProgressSlider->pageStep());
    }
    if( aAction == QAbstractSlider::SliderPageStepSub)
    {
        updatePlaybackContent(-mPlaybackProgressSlider->pageStep());
    }
}



void  SysLoggerWidget::findNextMatchingFilterPattern()
{

    if(mPlaybackFileStream.is_open())
    {
        QProgressDialog fProgress("Find Filter Pattern...", "Abort", 0, mPlaybackNumberOfLines, this);
        fProgress.setWindowModality(Qt::WindowModal);


        bool fFindLine = false;
        uint64_t fSearchedLineNumber = 0;


        mModel->removeRows(0, mModel->rowCount());

        mPlaybackFileStream.clear();
        mPlaybackFileStream.seekg (0, mPlaybackFileStream.beg);

        
        std::string fTmpLine;
   
        // Find the Correct Start Line
        int32_t fLineCounter = 0;
        int32_t fCurrentLine = mPlaybackProgressSlider->value()+1;

        size_t fLength = mPlaybackFileSize;
        static const int32_t fBufferSize = 4096;
        static const int32_t fMaxLineLength = 512;
        char fBuffer[fBufferSize];

        while(mPlaybackFileStream.good() && fLength > 0 && (fLineCounter<fCurrentLine))
        {
            size_t fReadBufferSize = fBufferSize;
            if(fLineCounter<fCurrentLine-fBufferSize)
            {
                if(fLineCounter< (fCurrentLine-fMaxLineLength))
                {
                    fReadBufferSize = fMaxLineLength;
                }

                mPlaybackFileStream.read(&fBuffer[0], fReadBufferSize);             
                fLength -= fReadBufferSize;
                for(uint64_t i=0; i < fReadBufferSize; ++i)
                {
                    if(fBuffer[i] == '\n')
                    {
                        ++fLineCounter;
                    }    
                }
            }
            else
            {
                std::getline(mPlaybackFileStream, fTmpLine);
                ++fLineCounter; 
            }
        }
     
        fLength = mPlaybackFileSize-mPlaybackFileStream.tellg();
        size_t fFileJumpBack = 0;
        uint32_t fNumberOfProcessedLines = fCurrentLine;
        uint32_t fNumberOfReadLines = mPlaybackNumberOfLines-fCurrentLine;
        while((!fProgress.wasCanceled())&&(!fFindLine)&&(mPlaybackFileStream.good()) && (fLength > 0) && (fNumberOfReadLines > 0))
        {
            size_t fFilePos = mPlaybackFileStream.tellg(); 
            mPlaybackFileStream.seekg (fFilePos-fFileJumpBack, mPlaybackFileStream.beg);
            int32_t fReadBufferSize = fBufferSize;
            if(fLength < fBufferSize)
            {
                fReadBufferSize = fLength;
            }
            mPlaybackFileStream.read(&fBuffer[0], fReadBufferSize);
            fLength -= fReadBufferSize;
            size_t fStartIndex = 0;
            size_t fEndIndex = 0;
            for(int64_t i=0; (!fProgress.wasCanceled()) && (!fFindLine) && (i < fReadBufferSize) && (fNumberOfReadLines > 0); ++i)
            {
                if(fBuffer[i] == '\n')
                {
                    fEndIndex = i;

                    fSearchedLineNumber=fLineCounter++;
                    QString fText = QByteArray(&fBuffer[fStartIndex], fEndIndex-fStartIndex+1).data();

//--------------------
                    uint32_t fSeverityNumber;
                    uint32_t fFacilityNumber;
                    QString fDateTimeString;
                    QString fHostNameString;
                    QString fAppNameString;
                    QString fProcessIDString;
                    QString fMessageIDString;
                    QString fStructuredDataString;
                    QString fMessageString;

                    parseSyslogLine(fText, fSeverityNumber, fFacilityNumber, 
                        fDateTimeString, fHostNameString, fAppNameString, fProcessIDString, fMessageIDString, fStructuredDataString, fMessageString);


                    QList<QStandardItem*> fFindItemList;

                    fFindItemList.append(new QStandardItem(QString("%1").arg(mCounter, 9, 10, QLatin1Char('0'))));   //Counter
                    fFindItemList.append(new QStandardItem(fDateTimeString));   //Date/Time
                    fFindItemList.append(new QStandardItem( convertFacilityNumberToString(fFacilityNumber).c_str() )); //Facility
                    fFindItemList.append(new QStandardItem( convertSeverityNumberToString(fSeverityNumber).c_str())); //Severity
                    fFindItemList.append(new QStandardItem(fHostNameString)); //Host Name
                    fFindItemList.append(new QStandardItem(fMessageIDString)); //Message-ID
                    fFindItemList.append(new QStandardItem(fProcessIDString)); //Process-ID
                    fFindItemList.append(new QStandardItem(fAppNameString)); //App-Name
                    fFindItemList.append(new QStandardItem(fMessageString)); //Message

                    mFindLoggingModel->removeRow(0);
                    mFindLoggingModel->appendRow(fFindItemList); 
//-------------------
                    fFindLine = mFindLoggingProxyModel[mFindLoggingProxyModel.size()-1]->hasChildren();
   
                    fStartIndex = fEndIndex+1;
                    --fNumberOfReadLines;
                    ++fNumberOfProcessedLines;
                    fProgress.setValue(fNumberOfProcessedLines);


                }    
            }
            fFileJumpBack = fBufferSize - fStartIndex;
            if(fLength > 0)
            {
                fLength += fFileJumpBack;
            }
        }

        if(fFindLine)
        {
            mPlaybackProgressSlider->setValue(fSearchedLineNumber);
            updatePlaybackContent();
        }
        else
        {
            QMessageBox::information(0, "Find", "The Filter pattern could not be found.");
        }
    }
}



void SysLoggerWidget::highlighterChanged(int aIndex)
{
    mRestoreHighlighter = true;
    //mHighlighterComboBox->setStyleSheet(convertSeverityNumberToQStyleSheetString(aIndex));
    mHighlighterLineEdit->setText(mHighlighterSettings[aIndex].mPattern);
    mInvertHighlighterCheckBox->setChecked(mHighlighterSettings[aIndex].mInvert);
    mCaseSensitveHighlighterCheckBox->setChecked(mHighlighterSettings[aIndex].mCaseSensitive == Qt::CaseSensitive);
    mRestoreHighlighter = false;
}


void SysLoggerWidget::highlighterPatternChanged()
{
    if(!mRestoreHighlighter)
    {
        int fCurrentIndex = mHighlighterComboBox->currentIndex();
        QString fPattern = mHighlighterLineEdit->text();
        mHighlighterSettings[fCurrentIndex].mPattern = fPattern;
        mHighlighterSettings[fCurrentIndex].mInvert = mInvertHighlighterCheckBox->isChecked();
        mHighlighterSettings[fCurrentIndex].mCaseSensitive = 
            mCaseSensitveHighlighterCheckBox->isChecked() ? Qt::CaseSensitive : Qt::CaseInsensitive;

        if(mCustomcheckBox->isChecked())
        {
            updateCostumHighlighting();
        }
    }
}


void SysLoggerWidget::customHiglightingFlagChanged()
{
    if(mCustomcheckBox->isChecked())
    {
        updateCostumHighlighting();       
    }
    else
    {
        std::vector<QString>  fServityStringVector =  createSevertyStringVector();


        QColor fDefaultBColor = convertSeverityNumberToBackgroundColor(9); //Use default layout
        QColor fDefaultFColor = convertSeverityNumberToForegroundColor(9); //Use default layout

        for(int row=0; row < mModel->rowCount(); ++row)
        {
            QColor fBColor = fDefaultBColor; 
            QColor fFColor = fDefaultFColor; 

            for(size_t fHighlighterIdx = 0; fHighlighterIdx < fServityStringVector.size(); ++fHighlighterIdx) 
            {
                int col = 3;
             
                QStandardItem* fItem = mModel->item(row, col);
                QString fItemText = fItem->text();

                if(fItemText.indexOf(fServityStringVector[fHighlighterIdx]) >= 0)
                {
                    fBColor = convertSeverityNumberToBackgroundColor(fHighlighterIdx); //Use default layout
                    fFColor = convertSeverityNumberToForegroundColor(fHighlighterIdx); //Use default layout
                    break;
                }      
            }
            for(int col=0; col < mModel->columnCount(); ++col)
            {
                QStandardItem* fItem = mModel->item(row, col);

                fItem->setBackground(QBrush(fBColor));
                fItem->setForeground(QBrush(fFColor));   
            }
        }
    }
}



void SysLoggerWidget::resetHighlighting()
{
    QColor fBColor = convertSeverityNumberToBackgroundColor(9); //Use default layout
    QColor fFColor = convertSeverityNumberToForegroundColor(9); //Use default layout
    for(int row=0; row < mModel->rowCount(); ++row)
    {
        for(int col=0; col < mModel->columnCount(); ++col)
        {
            QStandardItem* fItem = mModel->item(row, col);
            fItem->setBackground(QBrush(fBColor));
            fItem->setForeground(QBrush(fFColor));
        }
    }
}



void SysLoggerWidget::updateOneLineCostumHighlighting(QColor& aBColor, QColor& aFColor, int aRow, boost::function<QStandardItem*(int, int)> aItemFunc)
{
    QColor fDefaultBColor = convertSeverityNumberToBackgroundColor(9); //Use default layout
    QColor fDefaultFColor = convertSeverityNumberToForegroundColor(9); //Use default layout

    QColor fBColor = fDefaultBColor; 
    QColor fFColor = fDefaultFColor; 
    QColor fBLastColor = fDefaultBColor; 
    QColor fFLastColor = fDefaultFColor; 

    for(size_t fHighlighterIdx = 0; fHighlighterIdx < mHighlighterSettings.size(); ++fHighlighterIdx) 
    {
        bool fInvert = mHighlighterSettings[fHighlighterIdx].mInvert;
                
        if(!fInvert)
        {
            fBColor = fBLastColor;  
            fFColor = fFLastColor; 
        }
        else
        {
            fBColor = convertSeverityNumberToBackgroundColor(fHighlighterIdx); 
            fFColor = convertSeverityNumberToForegroundColor(fHighlighterIdx);
        } 
             
        for(int col=0; col < mModel->columnCount(); ++col)
        {
            QString fItemText = aItemFunc(aRow, col)->text();

            bool fPatternMatches = ((!mHighlighterSettings[fHighlighterIdx].mPattern.isEmpty()) 
                                   && (fItemText.indexOf(mHighlighterSettings[fHighlighterIdx].mPattern, 0, mHighlighterSettings[fHighlighterIdx].mCaseSensitive) >= 0));
            if(!fInvert && fPatternMatches)
            {
                fBColor = convertSeverityNumberToBackgroundColor(fHighlighterIdx); 
                fFColor = convertSeverityNumberToForegroundColor(fHighlighterIdx);
                break;
            }
            if(fInvert && fPatternMatches)
            {
                fBColor = fBLastColor; 
                fFColor = fFLastColor;
                break;
            }
        } 
        fBLastColor = fBColor;
        fFLastColor = fFColor;
    }

    aBColor = fBColor;
    aFColor = fFColor;
}


void SysLoggerWidget::updateCostumHighlighting()
{
    QColor fDefaultBColor = convertSeverityNumberToBackgroundColor(9); //Use default layout
    QColor fDefaultFColor = convertSeverityNumberToForegroundColor(9); //Use default layout
    QColor fBColor = fDefaultBColor; 
    QColor fFColor = fDefaultFColor; 

    for(int row=0; row < mModel->rowCount(); ++row)
    {
        updateOneLineCostumHighlighting(fBColor, fFColor, row, [this](int aRow, int aCol){ return mModel->item(aRow, aCol); });

        for(int col=0; col < mModel->columnCount(); ++col)
        {
            QStandardItem* fItem = mModel->item(row, col);

            fItem->setBackground(QBrush(fBColor));
            fItem->setForeground(QBrush(fFColor));   
        }
    }
}


void SysLoggerWidget::toTopToggled(bool aChecked)
{
    if(aChecked)
    {
        mToBottomToolButton->setChecked(false);
        mLogTableView->scrollToTop();
    }
}
    
void SysLoggerWidget::toBottomToggled(bool aChecked)
{
    if(aChecked)
    {
        mToTopToolButton->setChecked(false);
        mLogTableView->scrollToBottom();
    }
}





