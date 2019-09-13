#include "mainwindow.h"
#include "ui_mainwindow.h"

#include <QFile>
#include <QDir>
#include <QUrl>
#include <QStandardItemModel>
#include <QAbstractItemModel>
#include <QDesktopServices>
#include <QUrl>
#include <vector>
#include <string>
#include <string.h>

QDomNode findElement(const QDomNode& aParent, const QString & aName);
std::vector<std::string> SplitPath(const std::string &path);
QString toDate(const QString &aDate);

enum eTable
{
    eFile, ePath, eExtension, eAdded, eModified, eVisited, eLast
};

static const QString gRecentFilePath("/.local/share/recently-used.xbel");

MainWindow::MainWindow(QWidget *parent) :
  QMainWindow(parent)
, ui(new Ui::MainWindow)
, mDoc("Recent")
{
    ui->setupUi(this);

    QFile file(QDir::homePath() + gRecentFilePath);

    bool fResult = false;
    if (file.open(QIODevice::ReadOnly))
    {
        fResult = mDoc.setContent(&file);
        file.close();
    }
    QStringList fSectionNames = { tr("File"), tr("Path"), tr("Type"), tr("added"), tr("modified"), tr("visited")};
    mListModel = new QStandardItemModel(0, eLast, this);
    for (int fSection = 0; fSection<eLast; ++fSection)
    {
        mListModel->setHeaderData(fSection, Qt::Horizontal, fSectionNames[fSection]);
        ui->comboBoxSearchColumn->addItem(fSectionNames[fSection], fSection);
    }
    ui->comboBoxSearchColumn->setCurrentIndex(ePath);
    ui->tableView->setModel(mListModel);

    if (fResult)
    {
        int fRow = 0;
        QDomNode fNode = mDoc.firstChild();
        for (; !fNode.isNull(); fNode = fNode.nextSibling())
        {
            QDomNode fElement = findElement(fNode, "bookmark");
            for (; !fElement.isNull(); fElement = fElement.nextSibling())
            {
                std::vector<std::string> fFile = SplitPath(fElement.attributes().namedItem("href").nodeValue().toStdString());
                mListModel->insertRows(fRow, 1, QModelIndex());
                mListModel->setData(mListModel->index(fRow, eFile, QModelIndex()), QString(fFile[eFile].c_str()));
                mListModel->setData(mListModel->index(fRow, ePath, QModelIndex()), QString(fFile[ePath].c_str()));
                mListModel->setData(mListModel->index(fRow, eExtension, QModelIndex()), QString(fFile[eExtension].c_str()));
                mListModel->setData(mListModel->index(fRow, eAdded, QModelIndex()), toDate(fElement.attributes().namedItem("added").nodeValue()));
                mListModel->setData(mListModel->index(fRow, eModified, QModelIndex()), toDate(fElement.attributes().namedItem("modified").nodeValue()));
                mListModel->setData(mListModel->index(fRow, eVisited, QModelIndex()), toDate(fElement.attributes().namedItem("visited").nodeValue()));
                mListModel->setData(mListModel->index(fRow, eLast, QModelIndex()), fElement.attributes().namedItem("href").nodeValue());
                fElement.setNodeValue(QString::number(fRow));
                ++fRow;
            }
        }
    }
}

MainWindow::~MainWindow()
{
    delete ui;
}


QDomNode findElement(const QDomNode& aParent, const QString & aName)
{
    QDomNode fReturn;
    QStringList fNameList = aName.split('/');
    QDomNode fParent = aParent;
    bool bFound = false;
    for (int i=0; i<fNameList.size(); ++i)
    {
        QString sattribute, svalue, ssearch(fNameList.at(i));
        int nAttribute  = (int)aName.indexOf("#"); // attribute is recognized by '#'
        if (nAttribute != -1)
        {
            int nEqual = (int)aName.indexOf("=");  // the value is separated by '='
            if (nEqual != -1)
            {
                sattribute = aName.mid(nAttribute+1, nEqual-nAttribute-1);
                svalue     = aName.mid(nEqual+1, aName.length()-nEqual);
                ssearch    = aName.mid(0, nAttribute);
                nAttribute = 1;                 // use also attribute
            }
            else
            {
                nAttribute = 0;
            }
        }
        else
        {
            nAttribute = 0;
        }

        bFound = false;
        for(QDomNode n = fParent.firstChild(); !bFound && !n.isNull(); n = n.nextSibling())
        {
            if (n.nodeName() == ssearch)          // found name
            {
                if (nAttribute == 1)            // use attribute?
                {                               // compare the attributes value
                    QDomNode fattr = n.attributes().namedItem(sattribute);
                    if (!fattr.isNull() && svalue == fattr.nodeValue())
                    {
                        fParent = n;
                        bFound = true;
                    }
                }
                else
                {
                    fParent = n;
                    bFound = true;
                }
            }
            else                                // search recursively
            {
                QDomNode node = findElement(n, aName);
                if (!node.isNull())
                {
                    fParent = node;
                    bFound = true;
                }
            }
        }

        if (bFound && i == fNameList.size()-1)
        {
            fReturn = fParent;
        }
    }
    return fReturn;
}



std::vector<std::string> SplitPath(const std::string &path)
{
    std::string delims("/\\");
    std::vector<std::string> elements;
    int pos = path.find_last_of(delims);
    int posfile = path.find("file://");
    posfile = (posfile != -1) ? posfile + 7 : 0;
    if (pos != -1)
    {
        elements.push_back(path.substr(pos + 1));
        elements.push_back(path.substr(posfile, pos-posfile));
        pos = elements[0].find_last_of('.');
        if (pos != -1)
        {
            elements.push_back(elements[0].substr(pos+1));
        }
    }
    while (elements.size() < 3)
    {
        elements.push_back("");
    }
    return elements;
}

QString toDate(const QString &aDate)
{
    // source:      2017-03-29T19:55:26Z
    // destination:19:55:26,29.03.2017
    int year(0), month(0), day(0), hour(0), minute(0), second(0);
    sscanf(aDate.toStdString().c_str(), "%04d-%02d-%02dT%02d:%02d:%02dZ", &year, &month, &day, &hour, &minute, &second);
    return QString("%1:%2:%3 %4.%5.%6").arg(hour, 2).arg(minute, 2).arg(second, 2).arg(day, 2).arg(month, 2).arg(year, 4);
}

void MainWindow::on_pushButtonSave_clicked()
{
    QString xml = mDoc.toString();
    QFile file(QDir::homePath() + gRecentFilePath);
    QDir::setCurrent(QDir::homePath());
    file.open(QFile::WriteOnly|QFile::Text);
    file.write(xml.toStdString().c_str());
    file.close();
}

void MainWindow::on_pushButtonDeleteSelected_clicked()
{
    QDomNode fElement;
    QDomNode fNode = mDoc.firstChild();
    for (; !fNode.isNull(); fNode = fNode.nextSibling())
    {
        fElement = findElement(fNode, "bookmark");
        if (!fElement.isNull())
        {
            break;
        }
    }
    std::vector<QDomNode> fNodelist;
    std::vector<int>      fRowlist;
    int fRow = 0;
    QModelIndexList indexes = ui->tableView->selectionModel()->selection().indexes();
    do
    {
        fRow = fElement.nodeValue().toInt();
        for (auto index : indexes)
        {
            if (index.row() == fRow)
            {
                fNodelist.push_back(fElement);
                fRowlist.push_back(fRow);
                break;
            }
        }
    }
    while (!(fElement = fElement.nextSibling()).isNull());

    ui->tableView->clearSelection();

    for (auto item : fNodelist)
    {
        item.parentNode().removeChild(item);
    }

    for (auto row = fRowlist.rbegin(); row != fRowlist.rend(); ++row)
    {
        mListModel->removeRow(*row);
    }
}

void MainWindow::on_tableView_doubleClicked(const QModelIndex &index)
{
    QString fFile = "gnome-open \"" + mListModel->data(mListModel->index(index.row(), ePath)).toString() + "/" +
            mListModel->data(mListModel->index(index.row(), eFile)).toString() + "\"";

    QUrl fUrl(fFile);
    fFile = fUrl.path();

    int fError = ::system(fFile.toStdString().c_str());
    if (fError)
    {
        mErrorMsgDlg.showMessage(QString(strerror(fError)));
        mErrorMsgDlg.show();
    }
}

void MainWindow::on_pushButtonSelect_clicked()
{
    QString fSelect = ui->lineEditSelection->text();
    int fRows = mListModel->rowCount();
    int fSection = ui->comboBoxSearchColumn->currentIndex();
    for (int fRow=0; fRow < fRows; ++fRow)
    {
        if (mListModel->data(mListModel->index(fRow, fSection)).toString().indexOf(fSelect) != -1)
        {
            ui->tableView->selectionModel()->select(mListModel->index(fRow, fSection), QItemSelectionModel::Select);
        }
    }
}
